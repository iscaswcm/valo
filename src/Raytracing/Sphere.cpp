// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Sphere.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"

using namespace Raycer;

void Sphere::initialize()
{
}

void Sphere::intersect(Ray& ray) const
{
	Vector3 originToCenter = position - ray.origin;
	double originToSphereDistance2 = originToCenter.lengthSquared();
	double radius2 = radius * radius;

	// ray origin inside the sphere
	if (originToSphereDistance2 < radius2)
		return;

	double ta = originToCenter.dot(ray.direction);

	// sphere is behind the ray
	if (ta < 0.0)
		return;

	double sphereToRayDistance2 = originToSphereDistance2 - (ta * ta);

	// ray misses the sphere
	if (sphereToRayDistance2 > radius2)
		return;

	double tb = sqrt(radius2 - sphereToRayDistance2);
	double t = ta - tb;

	// there was another intersection closer to camera
	if (t > ray.intersection.distance)
		return;

	ray.intersection.wasFound = true;
	ray.intersection.distance = t;
	ray.intersection.position = ray.origin + (t * ray.direction);
	ray.intersection.normal = (ray.intersection.position - position).normalized();
	ray.intersection.texcoord = Vector2(0.0, 0.0);
	ray.intersection.materialId = materialId;
}
