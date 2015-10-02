// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/ToneMappers/LinearToneMapper.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"
#include "Math/Color.h"

using namespace Raycer;

void LinearToneMapper::apply(const Scene& scene, Image& image)
{
	std::vector<Color>& pixelData = image.getPixelData();
	double invGamma = 1.0 / scene.toneMapper.gamma;

	#pragma omp parallel for
	for (int ti = 0; ti < int(pixelData.size()); ++ti)
	{
		size_t i = size_t(ti);

		pixelData[i] *= pow(2.0, scene.toneMapper.exposure);
		pixelData[i].a = 1.0;

		if (scene.toneMapper.applyGamma)
			pixelData[i] = Color::pow(pixelData[i], invGamma);

		if (scene.toneMapper.shouldClamp)
			pixelData[i].clamp();
	}
}
