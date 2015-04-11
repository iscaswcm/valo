// Copyright � 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "CpuRaytracing/Primitive.h"
#include "CpuRaytracing/Material.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Mesh : public Primitive
	{
	public:

		Mesh();
		
		void intersect(Ray& ray) const;

		Vector3 position = Vector3(0.0, 0.0, 0.0);
		Material material;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(position),
				CEREAL_NVP(material));
		}
	};
}