// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"

#include "States/TestState.h"
#include "Utils/Log.h"
#include "App/App.h"
#include "Framebuffer/Framebuffer.h"
#include "Raytracing/Raytracer.h"
#include "Raytracing/Sphere.h"
#include "Raytracing/Plane.h"

using namespace Raycer;

TestState::TestState(BaseLog& baseLog, App& app_, Framebuffer& framebuffer_, Settings& settings_) : app(app_), framebuffer(framebuffer_), settings(settings_)
{
	log = baseLog.getNamedLog("TestState");
}

void TestState::initialize()
{
	Material m1;
	m1.color = Color(119, 158, 203);
	m1.diffuseReflectivity = 1.0;
	m1.specularReflectivity = 1.0;
	m1.rayReflectivity = 1.0;
	m1.shininess = 8.0;

	Sphere* s1 = new Sphere();
	s1->position = Vector3(0.0, 1.0, -5.0);
	s1->setRadius(1.0);
	s1->material = m1;

	m1.color = Color(255, 179, 71);

	Sphere* s2 = new Sphere();
	s2->position = Vector3(-1.0, 2.0, -5.0);
	s2->setRadius(1.0);
	s2->material = m1;

	m1.color = Color(119, 190, 119);

	Sphere* s3 = new Sphere();
	s3->position = Vector3(1.0, 2.0, -5.0);
	s3->setRadius(1.0);
	s3->material = m1;

	m1.color = Color(112, 128, 144);
	m1.specularReflectivity = 0.0;

	Plane* p1 = new Plane();
	p1->position = Vector3(0.0, 0.0, 0.0);
	p1->normal = Vector3(0.0, 1.0, 0.0);
	p1->material = m1;

	Light l1;
	l1.position = Vector3(0.0, 10.0, -5.0);
	l1.diffuseColor = Color(0.5, 0.5, 0.5);
	l1.specularColor = Color(0.8, 0.8, 0.8);

	Light l2;
	l2.position = Vector3(0.0, 4.0, 10.0);
	l2.diffuseColor = Color(0.5, 0.5, 0.5);
	l2.specularColor = Color(0.8, 0.8, 0.8);

	Light l3;
	l3.position = Vector3(0.0, -10.0, -5.0);
	l3.diffuseColor = Color(0.5, 0.5, 0.5);
	l3.specularColor = Color(0.5, 0.5, 0.5);

	scene.camera.position = Vector3(0.0, 1.0, 0.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);
	scene.camera.initialize(&app, &settings);
	scene.camera.setFov(settings.window.fov);
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());

	scene.primitives.push_back(std::shared_ptr<Primitive>(s1));
	scene.primitives.push_back(std::shared_ptr<Primitive>(s2));
	scene.primitives.push_back(std::shared_ptr<Primitive>(s3));
	scene.primitives.push_back(std::shared_ptr<Primitive>(p1));

	scene.lights.push_back(l1);
	scene.lights.push_back(l2);
	scene.lights.push_back(l3);
}

void TestState::pause()
{

}

void TestState::resume()
{

}

void TestState::shutdown()
{

}

void TestState::update(double timeStep)
{
	scene.camera.update(timeStep);
}

void TestState::render(double timeStep, double interpolation)
{
	(void)timeStep;
	(void)interpolation;

	Raytracer::raytrace(framebuffer, scene);
}

void TestState::windowResized(int width, int height)
{
	(void)width;
	(void)height;
}

void TestState::framebufferResized(int width, int height)
{
	scene.camera.setImagePlaneSize(width, height);
}
