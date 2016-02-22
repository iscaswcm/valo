﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "cereal/cereal.hpp"

#include "Tracing/Camera.h"
#include "Tracing/Triangle.h"
#include "BVH/BVH.h"
#include "BVH/BVH1.h"
#include "BVH/BVH4.h"
#include "Textures/ColorTexture.h"
#include "Textures/ColorGradientTexture.h"
#include "Textures/CheckerTexture.h"
#include "Textures/ImageTexture.h"
#include "Textures/PerlinNoiseTexture.h"
#include "Lights/AmbientLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Lights/AreaPointLight.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseSpecularMaterial.h"
#include "Tracers/Tracer.h"
#include "Samplers/Sampler.h"
#include "Filters/Filter.h"
#include "Tonemappers/Tonemapper.h"
#include "Rendering/Color.h"
#include "Rendering/ImagePool.h"
#include "Utils/ModelLoader.h"

namespace Raycer
{
	class Scene
	{
	public:

		static Scene loadFromFile(const std::string& fileName);
		static Scene loadFromJsonString(const std::string& text);
		static Scene loadFromXmlString(const std::string& text);

		void saveToFile(const std::string& fileName) const;
		std::string getJsonString() const;
		std::string getXmlString() const;

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection) const;

		Camera camera;

		struct General
		{
			TracerType tracerType = TracerType::RAY;
			float rayMinDistance = 0.0001f;
			Color backgroundColor = Color(0.0f, 0.0f, 0.0f);
			Color offLensColor = Color(0.0f, 0.0f, 0.0f);
			bool enableNormalMapping = true;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(tracerType),
					CEREAL_NVP(rayMinDistance),
					CEREAL_NVP(backgroundColor),
					CEREAL_NVP(offLensColor),
					CEREAL_NVP(enableNormalMapping));
			}

		} general;

		struct Raytracing
		{
			uint64_t maxIterationDepth = 3;
			uint64_t multiSampleCountSqrt = 1;
			uint64_t cameraSampleCountSqrt = 1;
			SamplerType multiSamplerType = SamplerType::CMJ;
			FilterType multiSamplerFilterType = FilterType::MITCHELL;
			SamplerType cameraSamplerType = SamplerType::CMJ;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(maxIterationDepth),
					CEREAL_NVP(multiSampleCountSqrt),
					CEREAL_NVP(cameraSampleCountSqrt),
					CEREAL_NVP(multiSamplerType),
					CEREAL_NVP(multiSamplerFilterType),
					CEREAL_NVP(cameraSamplerType));
			}

		} raytracing;

		struct Pathtracing
		{
			uint64_t pixelSampleCount = 1;
			uint64_t minPathLength = 3;
			float terminationProbability = 0.5f;
			bool enableMultiSampling = false;
			bool enableCameraSampling = false;
			bool enableDirectLighting = true;
			bool enableIndirectLighting = true;
			FilterType multiSamplerFilterType = FilterType::MITCHELL;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(pixelSampleCount),
					CEREAL_NVP(minPathLength),
					CEREAL_NVP(terminationProbability),
					CEREAL_NVP(enableMultiSampling),
					CEREAL_NVP(enableCameraSampling),
					CEREAL_NVP(enableDirectLighting),
					CEREAL_NVP(enableIndirectLighting),
					CEREAL_NVP(multiSamplerFilterType));
			}

		} pathtracing;

		struct Tonemapping
		{
			TonemapperType type = TonemapperType::LINEAR;
			bool applyGamma = true;
			bool shouldClamp = true;
			float gamma = 2.2f;
			float exposure = 0.0f;
			float key = 0.18f;
			bool enableAveraging = true;
			float averagingAlpha = 0.1f;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(type),
					CEREAL_NVP(applyGamma),
					CEREAL_NVP(shouldClamp),
					CEREAL_NVP(gamma),
					CEREAL_NVP(exposure),
					CEREAL_NVP(key),
					CEREAL_NVP(enableAveraging),
					CEREAL_NVP(averagingAlpha));
			}

		} tonemapping;

		struct Lights
		{
			std::vector<AmbientLight> ambientLights;
			std::vector<DirectionalLight> directionalLights;
			std::vector<PointLight> pointLights;
			std::vector<AreaPointLight> areaPointLights;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(ambientLights),
					CEREAL_NVP(directionalLights),
					CEREAL_NVP(pointLights),
					CEREAL_NVP(areaPointLights));
			}

		} lights;

		struct Textures
		{
			std::vector<ColorTexture> colorTextures;
			std::vector<ColorGradientTexture> colorGradientTextures;
			std::vector<CheckerTexture> checkerTextures;
			std::vector<ImageTexture> imageTextures;
			std::vector<PerlinNoiseTexture> perlinNoiseTextures;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(colorTextures),
					CEREAL_NVP(colorGradientTextures),
					CEREAL_NVP(checkerTextures),
					CEREAL_NVP(imageTextures),
					CEREAL_NVP(perlinNoiseTextures));
			}

		} textures;

		struct Materials
		{
			std::vector<DiffuseMaterial> diffuseMaterials;
			std::vector<DiffuseSpecularMaterial> diffuseSpecularMaterials;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(diffuseMaterials),
					CEREAL_NVP(diffuseSpecularMaterials));
			}

		} materials;

		BVHType bvhType = BVHType::BVH1;
		BVHBuildInfo bvhBuildInfo;
		BVH* bvh = nullptr;

		std::vector<ModelLoaderInfo> models;
		std::vector<Triangle> triangles;
		std::vector<Triangle*> emissiveTriangles;
		ImagePool imagePool;
		BVH1 bvh1;
		BVH4 bvh4;

		std::vector<Light*> lightsList;
		std::vector<Texture*> texturesList;
		std::vector<Material*> materialsList;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(camera),
				CEREAL_NVP(general),
				CEREAL_NVP(raytracing),
				CEREAL_NVP(pathtracing),
				CEREAL_NVP(tonemapping),
				CEREAL_NVP(lights),
				CEREAL_NVP(textures),
				CEREAL_NVP(materials),
				CEREAL_NVP(bvhType),
				CEREAL_NVP(bvhBuildInfo),
				CEREAL_NVP(models),
				CEREAL_NVP(triangles),
				CEREAL_NVP(imagePool),
				CEREAL_NVP(bvh1),
				CEREAL_NVP(bvh4));
		}
	};
}