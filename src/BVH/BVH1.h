﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "cereal/cereal.hpp"

#include "BVH/BVH.h"
#include "Tracing/Aabb.h"

namespace Raycer
{
	class BVH1 : public BVH
	{
	public:

		void build(std::vector<Triangle>& triangles, uint64_t maxLeafSize) override;
		bool intersect(const Scene& scene, const Ray& ray, Intersection& intersection) const override;

	private:

		std::vector<BVHNode> nodes;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(nodes));
		}
	};
}
