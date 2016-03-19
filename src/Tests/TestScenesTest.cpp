// Copyright © 2016 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Core/Precompiled.h"

#ifdef RUN_UNIT_TESTS

#include "catch/catch.hpp"

#include "Core/Scene.h"

using namespace Raycer;

TEST_CASE("TestScenes functionality", "[testscenes]")
{
	std::vector<Scene> scenes;

	scenes.push_back(Scene::createTestScene1());
	scenes.push_back(Scene::createTestScene2());
	scenes.push_back(Scene::createTestScene3());

	uint64_t sceneCount = 0;

	for (const Scene& scene : scenes)
	{
		sceneCount++;

		scene.saveToFile(tfm::format("scene%d.json", sceneCount));
		scene.saveToFile(tfm::format("scene%d.xml", sceneCount));
		scene.saveToFile(tfm::format("scene%d.bin", sceneCount));
	}
}

#endif
