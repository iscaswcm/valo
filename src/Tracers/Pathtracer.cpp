﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Tracers/Pathtracer.h"
#include "Scenes/Scene.h"
#include "Tracing/Ray.h"
#include "Tracing/Intersection.h"

using namespace Raycer;

Color Pathtracer::trace(const Scene& scene, const Ray& ray, std::mt19937& generator, const std::atomic<bool>& interrupted)
{
	assert(scene.general.pathSampleCount >= 1);

	Color sampledPixelColor;

	for (uint64_t i = 0; i < scene.general.pathSampleCount; ++i)
		sampledPixelColor += traceRecursive(scene, ray, 0, generator, interrupted);

	return sampledPixelColor / double(scene.general.pathSampleCount);
}

Color Pathtracer::traceRecursive(const Scene& scene, const Ray& ray, uint64_t iteration, std::mt19937& generator, const std::atomic<bool>& interrupted)
{
	if (interrupted)
		return Color::BLACK;

	if (iteration >= scene.general.maxPathLength)
		return Color::BLACK;

	Intersection intersection;
	scene.intersect(ray, intersection);

	if (!intersection.wasFound)
		return Color::BLACK;

	Material* material = intersection.material;

	if (material->emissive)
	{
		Color emittance = material->emittance;

		if (material->emittanceMapTexture != nullptr)
			emittance = material->emittanceMapTexture->getColor(intersection.texcoord, intersection.position) * material->emittanceMapTexture->intensity;

		return emittance;
	}

	Sampler* sampler = samplers[SamplerType::RANDOM].get();
	Vector3 newDirection = sampler->getHemisphereSample(intersection.onb, 1.0, 0, 0, 1, 1, 0, generator);

	Ray newRay;
	newRay.origin = intersection.position + newDirection * scene.general.rayStartOffset;
	newRay.direction = newDirection;
	newRay.precalculate();

	Color reflectance = material->diffuseReflectance;

	if (material->diffuseMapTexture != nullptr)
		reflectance = material->diffuseMapTexture->getColor(intersection.texcoord, intersection.position) * material->diffuseMapTexture->intensity;

	double alpha = std::abs(newDirection.dot(intersection.normal));
	Color brdf = 2.0 * reflectance * alpha;
	Color reflected = traceRecursive(scene, newRay, iteration + 1, generator, interrupted);

	return brdf * reflected;
}
