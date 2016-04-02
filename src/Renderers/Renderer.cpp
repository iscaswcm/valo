// Copyright © 2016 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Precompiled.h"

#include "tinyformat/tinyformat.h"

#include "Core/Film.h"
#include "Core/Scene.h"
#include "Renderers/Renderer.h"
#include "Utils/Settings.h"

using namespace Raycer;

void Renderer::initialize(const Settings& settings)
{
	type = static_cast<RendererType>(settings.renderer.type);
	cpuRenderer.maxThreadCount = settings.general.maxCpuThreadCount;
	imageAutoWrite = settings.renderer.imageAutoWrite;
	imageAutoWriteInterval = settings.renderer.imageAutoWriteInterval;
	imageAutoWriteMaxNumber = settings.renderer.imageAutoWriteMaxNumber;
	imageAutoWriteFileName = settings.renderer.imageAutoWriteFileName;

	cpuRenderer.initialize();
	cudaRenderer.initialize();
}

void Renderer::render(RenderJob& job)
{
	Scene& scene = *job.scene->getPtr();
	Film& film = *job.film->getPtr();

	imageAutoWriteTimer.restart();

	for (uint32_t i = 0; i < scene.renderer.pixelSamples && !job.interrupted; ++i)
	{
		switch (type)
		{
			case RendererType::CPU: cpuRenderer.render(job, filtering); break;
			case RendererType::CUDA: cudaRenderer.render(job, filtering); break;
			default: break;
		}

		++film.pixelSamples;

		if (imageAutoWrite && imageAutoWriteTimer.getElapsedSeconds() > imageAutoWriteInterval)
		{
			film.normalize(type);
			film.tonemap(scene.tonemapper, type);
			film.getTonemappedImage().download();
			film.getTonemappedImage().save(tfm::format(imageAutoWriteFileName.c_str(), imageAutoWriteNumber), false);

			if (++imageAutoWriteNumber > imageAutoWriteMaxNumber)
				imageAutoWriteNumber = 1;

			imageAutoWriteTimer.restart();
		}
	}
}

std::string Renderer::getName() const
{
	switch (type)
	{
		case RendererType::CPU: return "cpu";
		case RendererType::CUDA: return "cuda";
		default: return "unknown";
	}
}
