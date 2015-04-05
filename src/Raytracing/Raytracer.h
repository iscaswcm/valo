// Copyright � 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Framebuffer;
	struct Scene;
	struct Ray;

	class Raytracer
	{
	public:

		static void raytrace(const Framebuffer& framebuffer, const Scene& scene);
		static void shootRay(Ray& ray, const Scene& scene);

		static const int MAX_REFLECTIONS = 1;
	};
}
