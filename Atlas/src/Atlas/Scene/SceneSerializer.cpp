#include "atlaspch.h"
#include "Atlas/Scene/SceneSerializer.h"

#include "Atlas/Scene/Entity.h"
#include "Atlas/Scene/Components.h"

#include "Atlas/Project/Project.h"

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

		if (entity.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap; // MeshComponent

			out << YAML::EndMap; // MeshComponent
		}

		if (entity.HasComponent<MaterialComponent>())
		{
			out << YAML::Key << "MaterialComponent";
			out << YAML::BeginMap; // MaterialComponent

			auto& materialComponent = entity.GetComponent<MaterialComponent>();
			auto& material = materialComponent.Material;

			out << YAML::Key << "Preset" << YAML::Value << (int)material->GetMaterialPreset();

			out << YAML::Key << "Color"     << YAML::Value << material->GetColor();
			out << YAML::Key << "Metallic"  << YAML::Value << material->GetMetallic();
			out << YAML::Key << "Roughness" << YAML::Value << material->GetRoughness();

			auto& albedoTexture = material->GetAlbedoTexture();
			if (albedoTexture)
			{
				out << YAML::Key << "AlbedoTexturePath" << YAML::Value << albedoTexture->GetPath().string();
			}

			auto& normalTexture = material->GetNormalTexture();
			if (normalTexture)
			{
				out << YAML::Key << "NormalTexturePath" << YAML::Value << normalTexture->GetPath().string();
			}

			auto& metallicTexture = material->GetMetallicTexture();
			if (metallicTexture)
			{
				out << YAML::Key << "MetallicTexturePath" << YAML::Value << metallicTexture->GetPath().string();
			}

			auto& roughnessTexture = material->GetRoughnessTexture();
			if (roughnessTexture)
			{
				out << YAML::Key << "RoughnessTexturePath" << YAML::Value << roughnessTexture->GetPath().string();
			}

			auto& aoTexture = material->GetAOTexture();
			if (aoTexture)
			{
				out << YAML::Key << "AOTexturePath" << YAML::Value << aoTexture->GetPath().string();
			}

			auto& displacementTexture = material->GetDisplacementTexture();
			if (displacementTexture)
			{
				out << YAML::Key << "DisplacementTexturePath" << YAML::Value << displacementTexture->GetPath().string();
			}

			out << YAML::EndMap; // MaterialComponent
		}

		if (entity.HasComponent<LightSourceComponent>())
		{
			out << YAML::Key << "LightSourceComponent";
			out << YAML::BeginMap; // LightSourceComponent

			auto& lightSourceComponent = entity.GetComponent<LightSourceComponent>();
			auto& light = lightSourceComponent.Light;

			out << YAML::Key << "CastType" << YAML::Value << (int)light->GetCastType();

			out << YAML::Key << "Color" << YAML::Value << light->GetColor();
			out << YAML::Key << "Intensity" << YAML::Value << light->GetIntensity();
			out << YAML::Key << "CutOff" << YAML::Value << light->GetCutOff();

			out << YAML::EndMap; // LightSourceComponent
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

				Entity* deserializedEntity = m_Scene->CreateEntity(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity->GetComponent<TransformComponent>();

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
					auto& cc = deserializedEntity->AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];

					if (cameraProps["ProjectionType"])
					{
						cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
					}

					if (cameraProps["PerspectiveFOV"])
					{
						cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					}

					if (cameraProps["PerspectiveNear"])
					{
						cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					}

					if (cameraProps["PerspectiveFar"])
					{
						cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
					}

					if (cameraProps["OrthographicSize"])
					{
						cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					}

					if (cameraProps["OrthographicNear"])
					{
						cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					}

					if (cameraProps["OrthographicFar"])
					{
						cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
					}

					if (cameraComponent["FixedAspectRatio"])
					{
						cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
					}
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity->AddComponent<SpriteRendererComponent>();
					src.Type = (SpriteRendererComponent::RenderType)spriteRendererComponent["Type"].as<int>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					if (spriteRendererComponent["TexturePath"])
					{
						std::string texturePath = spriteRendererComponent["TexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Texture = Texture2D::Create(path.string());
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

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					auto& src = deserializedEntity->AddComponent<MeshComponent>();
				}

				auto materialComponent = entity["MaterialComponent"];
				if (materialComponent)
				{
					auto& src = deserializedEntity->AddComponent<MaterialComponent>();

					if (materialComponent["Preset"])
					{
						src.Material->SetMaterialPreset((Material::MaterialPresets)materialComponent["Preset"].as<int>());
					}

					if (materialComponent["Color"])
					{
						src.Material->SetColor(materialComponent["Color"].as<glm::vec3>());
					}

					if (materialComponent["Metallic"])
					{
						src.Material->SetMetallic(materialComponent["Metallic"].as<float>());
					}

					if (materialComponent["Roughness"])
					{
						src.Material->SetRoughness(materialComponent["Roughness"].as<float>());
					}

					if (materialComponent["AlbedoTexturePath"])
					{
						std::string texturePath = materialComponent["AlbedoTexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Material->SetAlbedoTexture(Texture2D::Create(path.string()));
					}

					if (materialComponent["NormalTexturePath"])
					{
						std::string texturePath = materialComponent["NormalTexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Material->SetNormalTexture(Texture2D::Create(path.string()));
					}

					if (materialComponent["MetallicTexturePath"])
					{
						std::string texturePath = materialComponent["MetallicTexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Material->SetMetallicTexture(Texture2D::Create(path.string()));
					}

					if (materialComponent["RoughnessTexturePath"])
					{
						std::string texturePath = materialComponent["RoughnessTexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Material->SetRoughnessTexture(Texture2D::Create(path.string()));
					}

					if (materialComponent["AOTexturePath"])
					{
						std::string texturePath = materialComponent["AOTexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Material->SetAOTexture(Texture2D::Create(path.string()));
					}

					if (materialComponent["DisplacementTexturePath"])
					{
						std::string texturePath = materialComponent["DisplacementTexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Material->SetDisplacementTexture(Texture2D::Create(path.string()));
					}
				}

				auto lightSourceComponent = entity["LightSourceComponent"];
				if (lightSourceComponent)
				{
					auto& src = deserializedEntity->AddComponent<LightSourceComponent>();

					if (lightSourceComponent["CastType"])
					{
						src.Light->SetCastType((Light::CastType)lightSourceComponent["CastType"].as<int>());
					}

					if (lightSourceComponent["Color"])
					{
						src.Light->SetColor(lightSourceComponent["Color"].as<glm::vec3>());
					}

					if (lightSourceComponent["Intensity"])
					{
						src.Light->SetIntensity(lightSourceComponent["Intensity"].as<float>());
					}

					if (lightSourceComponent["CutOff"])
					{
						src.Light->SetCutOff(lightSourceComponent["CutOff"].as<glm::vec2>());
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