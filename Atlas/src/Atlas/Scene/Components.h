#pragma once

#include "Atlas/Core/UUID.h"
#include "Atlas/Core/Buffer.h"
#include "Atlas/Scene/SceneCamera.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/SubTexture2D.h"
#include "Atlas/Renderer/Light.h"
#include "Atlas/Renderer/Material.h"
#include "Atlas/Renderer/Mesh.h"
#include "Atlas/Renderer/PostProcessor.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Atlas
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::vec4 GetDirection() const
		{
			return glm::toMat4(glm::quat(Rotation)) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
		}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct PostProcessorComponent
	{
		PostProcessor::PostProcessingEffect Effect;

		PostProcessorComponent() = default;
		PostProcessorComponent(const PostProcessorComponent&) = default;
	};

	struct SpriteRendererComponent
	{
		enum RenderType
		{
			Square,
			Circle
		};

		// Common
		RenderType Type = Square;
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		// Square-specific
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;
		bool SpriteSheet = false;
		Ref<SubTexture2D> SubTexture;
		glm::vec2 SubTextureCoords = { 0, 0 };
		glm::vec2 SubTextureCellSize = { 32, 32 };
		glm::vec2 SubTextureSpriteSize = { 1, 1 };

		// Circle specific
		float Thickness = 1.0f;
		float Fade = 0.005f;

		void ResetTextureValues()
		{
			Texture = nullptr;
			TilingFactor = 1.0f;
			SpriteSheet = false;
			SubTexture = nullptr;
			SubTextureCoords = { 0, 0 };
			SubTextureCellSize = { 32, 32 };
			SubTextureSpriteSize = { 1, 1 };
		}

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct MeshComponent
	{
		Ref<Mesh> Mesh;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const Ref<Atlas::Mesh>& mesh)
			: Mesh(mesh) {}
	};

	struct MaterialComponent
	{
		Ref<Material> Material;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent(const Ref<Atlas::Material>& material)
			: Material(material) {}
	};

	struct LightSourceComponent
	{
		Ref<Light> Light;

		LightSourceComponent() = default;
		LightSourceComponent(const LightSourceComponent&) = default;
		LightSourceComponent(const Ref<Atlas::Light>& light)
			: Light(light) {}
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, CameraComponent, PostProcessorComponent,
		SpriteRendererComponent, MeshComponent, MaterialComponent,
		LightSourceComponent>;
}