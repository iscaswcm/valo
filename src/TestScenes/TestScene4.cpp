﻿// Copyright © 2016 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Precompiled.h"

#include "TestScenes/TestScene.h"
#include "Tracing/Scene.h"

using namespace Raycer;

// CONFERENCE //

Scene TestScene::create4()
{
	Scene scene;

	scene.general.tracerType = TracerType::PREVIEW;

	scene.bvhInfo.bvhType = BVHType::BVH4;
	scene.bvhInfo.maxLeafSize = 4;

	scene.camera.position = Vector3(-4.6401f, 0.4618f, 2.7327f);
	scene.camera.orientation = EulerAngle(-13.3503f, -56.3473f, 0.0000f);
	scene.camera.fov = 65.0f;

	ModelLoaderInfo model;
	model.modelFilePath = "data/models/conference/conference.obj";

	scene.models.push_back(model);

	return scene;
}
