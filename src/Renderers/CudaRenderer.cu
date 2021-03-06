// Copyright © 2016 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Precompiled.h"

#ifdef USE_CUDA
#include <device_launch_parameters.h>
#endif

#include "Core/Common.h"
#include "Core/Film.h"
#include "Core/Ray.h"
#include "Core/Scene.h"
#include "Core/Intersection.h"
#include "Renderers/CudaRenderer.h"
#include "Renderers/Renderer.h"
#include "Utils/CudaUtils.h"
#include "Utils/Settings.h"
#include "App.h"

using namespace Valo;

CudaRenderer::CudaRenderer() : sceneAlloc(true), filmAlloc(true), randomStatesAlloc(false)
{
}

void CudaRenderer::initialize()
{
	sceneAlloc.resize(1);
	filmAlloc.resize(1);
}

void CudaRenderer::resize(uint32_t width, uint32_t height)
{
	std::vector<RandomGeneratorState> randomStates(width * height);

	std::random_device rd;
	std::mt19937_64 generator(rd());

	for (RandomGeneratorState& randomState : randomStates)
	{
		randomState.state = generator();
		randomState.inc = generator();
	}

	randomStatesAlloc.resize(width * height);
	randomStatesAlloc.write(randomStates.data(), width * height);
}

#ifdef USE_CUDA

__global__ void renderKernel(const Scene& scene, Film& film, RandomGeneratorState* randomStates, bool filtering, uint32_t pixelSamples)
{
	uint32_t x = threadIdx.x + blockIdx.x * blockDim.x;
	uint32_t y = threadIdx.y + blockIdx.y * blockDim.y;
	uint32_t index = y * film.getWidth() + x;

	if (x >= film.getWidth() || y >= film.getHeight())
		return;

	Random random(randomStates[index]);

	for (uint32_t i = 0; i < pixelSamples; ++i)
	{
		Vector2 pixel = Vector2(x, y);
		float filterWeight = 1.0f;

		if (filtering && scene.renderer.filtering)
		{
			Vector2 offset = (random.getVector2() - Vector2(0.5f, 0.5f)) * 2.0f * scene.renderer.filter.getRadius();
			filterWeight = scene.renderer.filter.getWeight(offset);
			pixel += offset;
		}

		CameraRay cameraRay = scene.camera.getRay(pixel, random);
		cameraRay.ray.isPrimaryRay = true;

		if (cameraRay.offLens)
		{
			film.addSample(x, y, scene.general.offLensColor, filterWeight);
			randomStates[index] = random.getState();
			return;
		}

		Intersection intersection;

		if (!scene.intersect(cameraRay.ray, intersection))
		{
			film.addSample(x, y, scene.general.backgroundColor, filterWeight);
			randomStates[index] = random.getState();
			return;
		}

		if (intersection.hasColor)
		{
			film.addSample(x, y, intersection.color, filterWeight);
			randomStates[index] = random.getState();
			return;
		}

		scene.calculateNormalMapping(intersection);

		if (scene.general.normalVisualization)
		{
			film.addSample(x, y, Color::fromNormal(intersection.normal), filterWeight);
			randomStates[index] = random.getState();
			return;
		}

		Color color = scene.integrator.calculateLight(scene, intersection, cameraRay.ray, random);

		if (scene.volume.enabled)
		{
			VolumeEffect volumeEffect = Integrator::calculateVolumeEffect(scene, cameraRay.ray.origin, intersection.position, random);
			color = color * volumeEffect.transmittance + volumeEffect.emittance;
		}

		if (!color.isNegative() && !color.isNan())
			film.addSample(x, y, color * cameraRay.brightness, filterWeight);
	}

	randomStates[index] = random.getState();
}

void CudaRenderer::render(RenderJob& job, bool filtering)
{
	Scene& scene = *job.scene;
	Film& film = *job.film;
	Settings& settings = App::getSettings();

	sceneAlloc.write(&scene, 1);
	filmAlloc.write(&film, 1);

	dim3 dimBlock(16, 16);
	dim3 dimGrid;

	dimGrid.x = (film.getWidth() + dimBlock.x - 1) / dimBlock.x;
	dimGrid.y = (film.getHeight() + dimBlock.y - 1) / dimBlock.y;

	renderKernel<<<dimGrid, dimBlock>>>(*sceneAlloc.getDevicePtr(), *filmAlloc.getDevicePtr(), randomStatesAlloc.getDevicePtr(), filtering, settings.renderer.pixelSamples);
	CudaUtils::checkError(cudaPeekAtLastError(), "Could not launch render kernel");
	CudaUtils::checkError(cudaDeviceSynchronize(), "Could not execute render kernel");

	job.totalSampleCount += film.getWidth() * film.getHeight() * settings.renderer.pixelSamples;
}

#else

void CudaRenderer::render(RenderJob& job, bool filtering)
{
	(void)filtering;
	job.film->clear(RendererType::CPU);
}

#endif
