#include "atlaspch.h"
#include "Atlas/Scene/Scene.h"

#include "Atlas/Scene/Components.h"
#include "Atlas/Renderer/Renderer.h"

#include "Atlas/Scene/Entity.h"

namespace Atlas
{
	////////////////////////////////////////////////////////////////////////////////////////
	// DrawComponent ///////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	void Scene::DrawComponent(Entity entity, const glm::mat4& transform, const T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::DrawComponent<SpriteRendererComponent>(Entity entity, const glm::mat4& transform, const SpriteRendererComponent& component)
	{
		Renderer::DrawSprite(transform, component, (int)entity);
	}

	template<>
	void Scene::DrawComponent<MeshComponent>(Entity entity, const glm::mat4& transform, const MeshComponent& component)
	{
		MaterialComponent* material = m_Registry.try_get<MaterialComponent>(entity);

		Renderer::DrawMesh(transform, component, material, (int)entity);
	}

	template<>
	void Scene::DrawComponent<LightSourceComponent>(Entity entity, const glm::mat4& transform, const LightSourceComponent& component)
	{
		Renderer::DrawCircle(transform, glm::vec4(component.Light->GetColor(), 1.0f), 0.1f, 0.0f, (int)entity);
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// Scene class /////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	Scene::Scene()
	{
		m_Name = "Untitled Scene";
		m_Lights.reserve(Renderer::GetLightStorageBufferCapacity());
	}

	Scene::Scene(std::string name)
		: m_Name(name)
	{
		m_Lights.reserve(Renderer::GetLightStorageBufferCapacity());
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
			UpdateLights();

			Renderer::BeginScene(*mainCamera, *cameraTransform, m_Lights);
			DrawScene(cameraTransform->Translation, {});
			Renderer::EndScene();
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera, Entity selectedEntity)
	{
		UpdateLights();
		Renderer::BeginScene(camera, m_Lights);
		DrawScene(camera.GetPosition(), selectedEntity);
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

		m_EntityMap[uuid] = entity;

		return entity;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		// Copy name because we're going to modify component data structure
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
		return newEntity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		OnComponentRemoved(entity, AllComponents{});
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	Entity Scene::GetEntity(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
		{
			return { m_EntityMap.at(uuid), this };
		}

		return {};
	}

	Entity Scene::GetEntity(entt::entity entityHandle)
	{
		return { entityHandle, this };
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

	void Scene::UpdateLights()
	{
		m_Lights.clear();

		auto view = m_Registry.view<TransformComponent, LightSourceComponent>();
		for (auto entity : view)
		{
			auto [transform, light] = view.get<TransformComponent, LightSourceComponent>(entity);	

			glm::vec4 lightDirection = glm::vec4(0.0f); // w is a flag to indicate if light direction is spot direction
			switch (light.Light->GetCastType())
			{
				case Light::CastType::DirectionalLight:
					lightDirection = glm::vec4(transform.GetDirection());
					break;
				case Light::CastType::Spotlight:
					lightDirection = glm::vec4(transform.GetDirection());
					lightDirection.w = 0.0f;
					break;
			}

			Renderer::LightData lightData;
			lightData.Position         = glm::vec4(transform.Translation, 1.0f);
			lightData.Color            = glm::vec4(light.Light->GetColor(), 1.0f);
			lightData.Direction        = lightDirection;
			lightData.Radius           = light.Light->GetRadius();
			lightData.Intensity        = light.Light->GetIntensity();
			lightData.CutOffs          = light.Light->GetCutOff().x >= 0 ? glm::cos(glm::radians(light.Light->GetCutOff())) : light.Light->GetCutOff();
			lightData.AmbientStrength  = light.Light->GetAmbientStrength();
			lightData.DiffuseStrength  = light.Light->GetDiffuseStrength();
			lightData.SpecularStrength = light.Light->GetSpecularStrength();
			m_Lights.push_back(lightData);
		}
	}

	void Scene::DrawScene(const glm::vec3& cameraPosition, Entity selectedEntity)
	{
		Renderer::DisableStencilWriting();

		std::map<float, entt::entity> transparentEntities;
		bool isSelectedEntityTransparent = false;

		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				if (sprite.Color.a < 1.0f)
				{
					transparentEntities[glm::length(cameraPosition - transform.Translation)] = entity;

					if (entity == selectedEntity)
					{
						isSelectedEntityTransparent = true;
					}
				}
				else
				{
					if (entity != selectedEntity)
					{
						DrawComponent<SpriteRendererComponent>(GetEntity(entity), transform.GetTransform(), sprite);
					}
				}
			}
		}
;
		{
			auto view = m_Registry.view<TransformComponent, MeshComponent>();
			for (auto entity : view)
			{
				if (entity == selectedEntity)
				{
					continue;
				}

				auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

				DrawComponent<MeshComponent>(GetEntity(entity), transform.GetTransform(), mesh);
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, LightSourceComponent>();
			for (auto entity : view)
			{
				if (entity == selectedEntity)
				{
					continue;
				}

				auto [transform, light] = view.get<TransformComponent, LightSourceComponent>(entity);
				DrawComponent<LightSourceComponent>(GetEntity(entity), transform.GetTransform(), light);
			}
		}

		if (selectedEntity && !isSelectedEntityTransparent)
		{
			DrawSelectedEntityAndOutline(selectedEntity);
		}

		if (transparentEntities.size())
		{
			Renderer::NextBatch();

			for (std::map<float, entt::entity>::reverse_iterator it = transparentEntities.rbegin(); it != transparentEntities.rend(); ++it)
			{
				if (isSelectedEntityTransparent && it->second == selectedEntity)
				{
					DrawSelectedEntityAndOutline(selectedEntity);
				}
				else
				{
					DrawEntity(GetEntity(it->second));
				}
			}
		}

		Renderer::EnableStencilWriting();
	}

	void Scene::DrawEntity(Entity entity)
	{
		glm::mat4 transform = m_Registry.get<TransformComponent>(entity).GetTransform();

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			DrawComponent<SpriteRendererComponent>(GetEntity(entity), transform, m_Registry.get<SpriteRendererComponent>(entity));
		}
		else if (entity.HasComponent<MeshComponent>())
		{
			DrawComponent<MeshComponent>(GetEntity(entity), transform, m_Registry.get<MeshComponent>(entity));
		}
		else if (entity.HasComponent<LightSourceComponent>())
		{
			DrawComponent<LightSourceComponent>(GetEntity(entity), transform, m_Registry.get<LightSourceComponent>(entity));
		}
	}

	void Scene::DrawSelectedEntityAndOutline(Entity entity)
	{
		Renderer::NextBatch();

		Renderer::EnableStencilWriting();
		DrawEntity(entity);
		Renderer::NextBatch();

		Renderer::DisableStencilWriting();
		RenderCommand::SetStencilFunction(RendererAPI::TestFunction::NotEqual, 1, 0xFF);

		glm::mat4 transform = m_Registry.get<TransformComponent>(entity).GetTransform();
		glm::vec3 scale = m_Registry.get<TransformComponent>(entity).Scale;
		float outlineSize = 0.1f;
		transform = glm::scale(transform, glm::vec3(1.0f + outlineSize / scale.x, 1.0f + outlineSize / scale.y, 1.0f + outlineSize / scale.z));
		glm::vec4 selectionColor = { 0.400f, 0.733f, 0.417f, 1.0f }; // TODO: Link to palette (selection green)

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			Renderer::DrawQuad(transform, selectionColor, entity);
		}
		else if (entity.HasComponent<MeshComponent>())
		{
			Renderer::DrawMeshOutline(transform, m_Registry.get<MeshComponent>(entity), selectionColor, entity); 
		}
		else if (entity.HasComponent<LightSourceComponent>())
		{
			Renderer::DrawCircle(transform, selectionColor, 0.1f, 0.0f, entity);
		}

		Renderer::NextBatch();
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// OnComponentAdded ////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
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
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
		if (component.Mesh == nullptr)
		{
			component.Mesh = CreateRef<Mesh>();
		}
	}

	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{
		if (component.Material == nullptr)
		{
			component.Material = CreateRef<Material>();
		}
	}

	template<>
	void Scene::OnComponentAdded<LightSourceComponent>(Entity entity, LightSourceComponent& component)
	{
		if (component.Light == nullptr)
		{
			component.Light = CreateRef<Light>();
		}
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
	}
}