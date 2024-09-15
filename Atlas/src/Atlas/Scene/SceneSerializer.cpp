#include "atlaspch.h"
#include "Atlas/Scene/SceneSerializer.h"

#include "Atlas/Scene/Entity.h"
#include "Atlas/Scene/Components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}
namespace Atlas
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		ATLAS_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Type" << YAML::Value << spriteRendererComponent.Type;
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

			if (spriteRendererComponent.Texture)
			{
				out << YAML::Key << "TexturePath" << YAML::Value << spriteRendererComponent.Texture->GetPath().string();
			}

			out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.TilingFactor;
			out << YAML::Key << "SpriteSheet" << YAML::Value << spriteRendererComponent.SpriteSheet;
			out << YAML::Key << "SubTextureCoords" << YAML::Value << spriteRendererComponent.SubTextureCoords;
			out << YAML::Key << "SubTextureCellSize" << YAML::Value << spriteRendererComponent.SubTextureCellSize;
			out << YAML::Key << "SubTextureSpriteSize" << YAML::Value << spriteRendererComponent.SubTextureSpriteSize;
			
			out << YAML::Key << "Thickness" << YAML::Value << spriteRendererComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << spriteRendererComponent.Fade;


			out << YAML::EndMap; // SpriteRendererComponent
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->m_Name;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		
		for (auto entityID : m_Scene->m_Registry.view<entt::entity>())
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::filesystem::path& path)
	{
		// Not implemented
		ATLAS_CORE_ASSERT(false);
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e)
		{
			ATLAS_CORE_ERROR("Failed to load .atlas file '{0}'\n     {1}", path.string(), e.what());
			return false;
		}

		if (!data["Scene"])
		{
			return false;
		}

		std::string sceneName = data["Scene"].as<std::string>();
		ATLAS_CORE_TRACE("Deserializing scene '{0}'", sceneName);
		m_Scene->m_Name = sceneName;

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				ATLAS_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntity(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();

					if (transformComponent["Translation"])
					{
						tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					}

					if (transformComponent["Rotation"])
					{
						tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					}

					if (transformComponent["Scale"])
					{
						tc.Scale = transformComponent["Scale"].as<glm::vec3>();
					}
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];

					if (cameraComponent["ProjectionType"])
					{
						cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
					}

					if (cameraComponent["PerspectiveFOV"])
					{
						cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					}

					if (cameraComponent["PerspectiveNear"])
					{
						cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					}

					if (cameraComponent["PerspectiveFar"])
					{
						cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
					}

					if (cameraComponent["OrthographicSize"])
					{
						cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					}

					if (cameraComponent["OrthographicNear"])
					{
						cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					}

					if (cameraComponent["OrthographicFar"])
					{
						cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
					}

					if (cameraComponent["Primary"])
					{
						cc.Primary = cameraComponent["Primary"].as<bool>();
					}

					if (cameraComponent["FixedAspectRatio"])
					{
						cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
					}
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					//src.Type = (SpriteRendererComponent::RenderType)spriteRendererComponent["Type"].as<int>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					if (spriteRendererComponent["TexturePath"])
					{
						src.Texture = Texture2D::Create(spriteRendererComponent["TexturePath"].as<std::string>());
					}

					if (spriteRendererComponent["TilingFactor"])
					{
						src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
					}

					if (spriteRendererComponent["SpriteSheet"])
					{
						src.SpriteSheet = spriteRendererComponent["SpriteSheet"].as<bool>();
					}

					if (spriteRendererComponent["SubTextureCoords"])
					{
						src.SubTextureCoords = spriteRendererComponent["SubTextureCoords"].as<glm::vec2>();
					}

					if (spriteRendererComponent["SubTextureCellSize"])
					{
						src.SubTextureCellSize = spriteRendererComponent["SubTextureCellSize"].as<glm::vec2>();
					}

					if (spriteRendererComponent["SubTextureSpriteSize"])
					{
						src.SubTextureSpriteSize = spriteRendererComponent["SubTextureSpriteSize"].as<glm::vec2>();
					}

					if (spriteRendererComponent["Thickness"])
					{
						src.Thickness = spriteRendererComponent["Thickness"].as<float>();
					}

					if (spriteRendererComponent["Fade"])
					{
						src.Fade = spriteRendererComponent["Fade"].as<float>();
					}

					if (src.SpriteSheet)
					{
						src.SubTexture = SubTexture2D::CreateFromCoords(src.Texture, src.SubTextureCoords, src.SubTextureCellSize, src.SubTextureSpriteSize);
					}
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::filesystem::path& path)
	{
		// Not implemented
		ATLAS_CORE_ASSERT(false);
		return false;
	}

}