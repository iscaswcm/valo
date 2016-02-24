// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Precompiled.h"

#include "BVH/BVH.h"
#include "BVH/BVH1.h"
#include "BVH/BVH4.h"
#include "Tracing/Triangle.h"

using namespace Raycer;

std::unique_ptr<BVH> BVH::getBVH(BVHType type)
{
	switch (type)
	{
		case BVHType::BVH1: return std::make_unique<BVH1>();
		case BVHType::BVH4: return std::make_unique<BVH4>();
		//case BVHType::BVH8: return std::make_unique<BVH1>();
		//case BVHType::SBVH1: return std::make_unique<BVH1>();
		default: throw std::runtime_error("Unknown BVH type");
	}
}

void BVH::sortTriangles(std::vector<Triangle>& triangles, std::array<std::vector<Triangle*>, 3>& trianglePtrs)
{
	uint64_t triangleCount = triangles.size();

	trianglePtrs[0].resize(triangleCount);

	for (uint64_t i = 0; i < triangleCount; ++i)
		trianglePtrs[0][i] = &triangles[i];

	trianglePtrs[1] = trianglePtrs[0];
	trianglePtrs[2] = trianglePtrs[0];

	for (uint64_t i = 0; i < 3; ++i)
	{
		concurrency::parallel_sort(trianglePtrs[i].begin(), trianglePtrs[i].end(), [i](const Triangle* t1, const Triangle* t2)
		{
			return (&t1->center.x)[i] < (&t2->center.x)[i];
		});
	}
}

BVHSplitOutput BVH::calculateSplit(const BVHSplitInput& input)
{
	assert(input.end > input.start);

	BVHSplitOutput output;
	float lowestScore = std::numeric_limits<float>::max();

	for (uint64_t axis = 0; axis <= 2; ++axis)
	{
		concurrency::parallel_sort(input.trianglePtrs->begin() + input.start, input.trianglePtrs->begin() + input.end, [axis](const Triangle* t1, const Triangle* t2)
		{
			return (&t1->center.x)[axis] < (&t2->center.x)[axis];
		});

		output.rightAABB = AABB();
		uint64_t rightCount = 0;

		for (int64_t i = input.end - 1; i >= int64_t(input.start); --i)
		{
			output.rightAABB.expand((*input.trianglePtrs)[i]->aabb);
			rightCount++;

			(*input.rightScores)[i] = output.rightAABB.getSurfaceArea() * float(rightCount);
		}

		output.leftAABB = AABB();
		uint64_t leftCount = 0;

		for (uint64_t i = input.start; i < input.end; ++i)
		{
			output.leftAABB.expand((*input.trianglePtrs)[i]->aabb);
			leftCount++;

			float score = output.leftAABB.getSurfaceArea() * float(leftCount);
			bool isLast = (i + 1 == input.end);

			if (!isLast)
				score += (*input.rightScores)[i + 1];

			if (score < lowestScore)
			{
				output.index = isLast ? i : i + 1;
				output.axis = axis;
				lowestScore = score;
			}
		}
	}

	if (output.axis != 2)
	{
		concurrency::parallel_sort(input.trianglePtrs->begin() + input.start, input.trianglePtrs->begin() + input.end, [output](const Triangle* t1, const Triangle* t2)
		{
			return (&t1->center.x)[output.axis] < (&t2->center.x)[output.axis];
		});
	}

	if (output.index <= input.start || output.index >= input.end)
	{
		output.index = input.start + (input.end - input.start) / 2;
		output.axis = 0;
		output.failed = true;

		// TODO fix AABBs
	}

	return output;
}
