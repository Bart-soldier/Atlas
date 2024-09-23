#include "atlaspch.h"
#include "Atlas/Scene/Scene.h"

#include "Atlas/Scene/Components.h"
#include "Atlas/Renderer/Renderer.h"

#include "Atlas/Scene/Entity.h"

namespace Atlas
{
	Scene::Scene()
	{
		m_Name = "Untitled Scene";
	}

	Scene::Scene(std::string name)
		: m_Name(name)
	{
	}

	Scene::~Scene()
	{
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
		{
			auto view = src.view<Component>();
			for (auto srcEntity : view)
			{
				entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			}
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_Name = other->m_Name;

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		newScene->m_SceneLighting = other->m_SceneLighting;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntity(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntity(UUID(), name);
	}

	Entity Scene::CreateEntity(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		OnComponentRemoved(entity, AllComponents{});
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
	}

	void Scene::OnRuntimeStop()
	{
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Render 2D
		SceneCamera* mainCamera = nullptr;
		TransformComponent* cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = &transform;
					break;
				}
			}
		}

		if (mainCamera)
		{
			UpdateSceneLighting();

			Renderer::BeginScene(*mainCamera, *cameraTransform, m_SceneLighting);
			DrawScene();
			Renderer::EndScene();
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		UpdateSceneLighting();

		Renderer::BeginScene(camera, m_SceneLighting);
		DrawScene();
		Renderer::EndScene();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
		
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		// Copy name because we're going to modify component data structure
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
		return newEntity;
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
			{
				return Entity{ entity, this };
			}
		}
		return {};
	}

	void Scene::UpdateSceneLighting()
	{
		m_SceneLighting.LightPositions.clear();
		m_SceneLighting.LightColors.clear();
		m_SceneLighting.LightDirections.clear();
		m_SceneLighting.LightAttenuations.clear();
		m_SceneLighting.LightIntensities.clear();
		m_SceneLighting.LightAmbientStrengths.clear();
		m_SceneLighting.LightDiffuseStrengths.clear();
		m_SceneLighting.LightSpecularStrengths.clear();

		auto view = m_Registry.view<TransformComponent, LightSourceComponent>();
		for (auto entity : view)
		{
			auto [transform, light] = view.get<TransformComponent, LightSourceComponent>(entity);

			m_SceneLighting.LightPositions.push_back(transform.Translation);
			m_SceneLighting.LightColors.push_back(light.Light.GetColor());
			m_SceneLighting.LightDirections.push_back(light.Light.GetDirection());
			m_SceneLighting.LightAttenuations.push_back(light.Light.GetAttenuation());
			m_SceneLighting.LightIntensities.push_back(light.Light.GetIntensity());
			m_SceneLighting.LightAmbientStrengths.push_back(light.Light.GetAmbientStrength());
			m_SceneLighting.LightDiffuseStrengths.push_back(light.Light.GetDiffuseStrength());
			m_SceneLighting.LightSpecularStrengths.push_back(light.Light.GetSpecularStrength());
		}
	}

	void Scene::DrawScene()
	{
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}
;
		{
			auto view = m_Registry.view<TransformComponent, MeshComponent>();
			for (auto entity : view)
			{
				auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

				Renderer::DrawMesh(transform.GetTransform(), mesh, (int)entity);
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, LightSourceComponent>();
			for (auto entity : view)
			{
				auto [transform, light] = view.get<TransformComponent, LightSourceComponent>(entity);

				Renderer::DrawCircle(transform.GetTransform(), glm::vec4(light.Light.GetColor(), 1.0f), 0.1f, 0.0f, (int)entity);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// OnComponentAdded ////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<LightSourceComponent>(Entity entity, LightSourceComponent& component)
	{
		m_SceneLighting.LightCount++;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// OnComponentAdded ////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	template<typename... Component>
	void Scene::OnComponentRemoved(Entity entity, ComponentGroup<Component...>)
	{
		([&]()
			{
				if (entity.HasComponent<Component>())
				{
					OnComponentRemoved<Component>(entity, entity.GetComponent<Component>());
				}
			}
		(), ...);
	}

	template<typename T>
	void Scene::OnComponentRemoved(Entity entity, T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentRemoved<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved<CameraComponent>(Entity entity, CameraComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}

	template<>
	void Scene::OnComponentRemoved<LightSourceComponent>(Entity entity, LightSourceComponent& component)
	{
		m_SceneLighting.LightCount--;
	}
}