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
	void Scene::DrawComponent(Entity* entity, const glm::mat4& transform, const T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::DrawComponent<SpriteRendererComponent>(Entity* entity, const glm::mat4& transform, const SpriteRendererComponent& component)
	{
		Renderer::DrawSprite(transform, component, (int)entity->GetHandle());
	}

	template<>
	void Scene::DrawComponent<MeshComponent>(Entity* entity, const glm::mat4& transform, const MeshComponent& component)
	{
		MaterialComponent* material = m_Registry.try_get<MaterialComponent>(entity->GetHandle());

		Renderer::DrawMesh(transform, component, material, (int)entity->GetHandle());
	}

	template<>
	void Scene::DrawComponent<LightSourceComponent>(Entity* entity, const glm::mat4& transform, const LightSourceComponent& component)
	{
		Renderer::DrawCircle(transform, glm::vec4(component.Light->GetColor(), 1.0f), 0.1f, 0.0f, (int)entity->GetHandle());
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// Scene class /////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	Scene::Scene()
	{
		m_Name = "Untitled Scene";
		m_Lights.reserve(Renderer::GetLightStorageBufferCapacity());
		m_Skybox = Cubemap::Create();
	}

	Scene::Scene(std::string name)
		: m_Name(name)
	{
		m_Lights.reserve(Renderer::GetLightStorageBufferCapacity());
		m_Skybox = Cubemap::Create();
	}

	Scene::~Scene()
	{
		DestroyAllEntities();
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<entt::entity, Entity*>& correspondanceEnttMap)
	{
		([&]()
		{
			auto view = src.view<Component>();
			for (auto srcEntity : view)
			{
				Entity* dstEntity = correspondanceEnttMap.at(srcEntity);

				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity->GetHandle(), srcComponent);
			}
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<entt::entity, Entity*>& correspondanceEnttMap)
	{
		CopyComponent<Component...>(dst, src, correspondanceEnttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity* dst, Entity* src)
	{
		([&]()
		{
			if (src->HasComponent<Component>())
				dst->AddOrReplaceComponent<Component>(src->GetComponent<Component>());
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity* dst, Entity* src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_Name = other->m_Name;
		newScene->m_Skybox = other->m_Skybox;

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;
		newScene->m_EntityHandleMap.reserve(other->m_EntityHandleMap.size());
		newScene->m_EntityUUIDMap.reserve(other->m_EntityUUIDMap.size());

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<entt::entity, Entity*> correspondanceEnttMap;
		correspondanceEnttMap.reserve(other->m_EntityHandleMap.size());

		// Create entities in new scene
		for (auto& [handle, entity] : other->m_EntityHandleMap)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(handle).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(handle).Tag;
			Entity* newEntity = newScene->CreateEntity(uuid, name);
			newScene->m_EntityUUIDMap[uuid] = newEntity;
			newScene->m_EntityHandleMap[newEntity->GetHandle()] = newEntity;
			correspondanceEnttMap[handle] = newEntity;
		}

		for (auto& [handle, entity] : other->m_EntityHandleMap)
		{
			if (entity->GetParent() != nullptr)
			{
				correspondanceEnttMap[handle]->SetParent(correspondanceEnttMap[entity->GetParent()->GetHandle()]);
			}
		}

		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, correspondanceEnttMap);

		if (other->m_PrimaryCamera)
		{
			newScene->m_PrimaryCamera = correspondanceEnttMap[other->m_PrimaryCamera->GetHandle()];
		}

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
		if (m_PrimaryCamera)
		{
			const SceneCamera& camera = m_PrimaryCamera->GetComponent<CameraComponent>().Camera;
			const TransformComponent& cameraTransform = m_PrimaryCamera->GetComponent<TransformComponent>();

			UpdateLights();

			Renderer::BeginScene(camera, cameraTransform, m_Lights);
			DrawSceneDeferred(cameraTransform.Translation, false, nullptr);
			Renderer::EndScene();

			Renderer::DrawSkybox(m_Skybox);

			Renderer::BeginPostProcessing();
			Renderer::DrawPostProcessing(m_PrimaryCamera->TryGetComponent<PostProcessorComponent>());
			Renderer::EndPostProcessing();
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera, Entity* selectedEntity)
	{
		{
			ATLAS_PROFILE_SCOPE("Lights Prep");
			UpdateLights();
		}

		{
			ATLAS_PROFILE_SCOPE("GBuffer Pass");
			Renderer::BeginScene(camera, m_Lights);
			DrawSceneDeferred(camera.GetPosition(), true, selectedEntity);
			Renderer::NextBatch();
		}

		{
			ATLAS_PROFILE_SCOPE("SSAO Pass");
			Renderer::SSAOPass();
		}

		{
			ATLAS_PROFILE_SCOPE("Deferred Rendering");
			Renderer::DeferredRenderingPass(m_Skybox);
		}

		{
			ATLAS_PROFILE_SCOPE("Forward Rendering");
			DrawSceneForward(camera.GetPosition(), true, selectedEntity);
			Renderer::EndScene();

			Renderer::DrawSkybox(m_Skybox);
		}

		{
			ATLAS_PROFILE_SCOPE("Post Processing");
			Renderer::BeginPostProcessing();
			if (camera.IsPostProcessEnabled())
			{
				if (m_PrimaryCamera)
				{
					Renderer::DrawPostProcessing(m_PrimaryCamera->TryGetComponent<PostProcessorComponent>());
				}
			}
			Renderer::EndPostProcessing();
		}
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

	Entity* Scene::CreateEntity(const std::string& name, Entity* parent)
	{
		return CreateEntity(UUID(), name, parent);
	}

	Entity* Scene::CreateEntity(UUID uuid, const std::string& name, Entity* parent)
	{
		Entity* entity = new Entity(m_Registry.create(), this, parent);
		entity->AddComponent<IDComponent>(uuid);
		entity->AddComponent<TransformComponent>();
		auto& tag = entity->AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityUUIDMap[uuid] = entity;
		m_EntityHandleMap[entity->GetHandle()] = entity;

		return entity;
	}

	Entity* Scene::DuplicateEntity(Entity* entity, Entity* parent)
	{
		std::string name = entity->GetName();
		Entity* newEntity = CreateEntity(name, parent == nullptr ? entity->GetParent() : parent);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);

		for (Entity* child : entity->GetChildren())
		{
			DuplicateEntity(child, newEntity);
		}

		return newEntity;
	}

	void Scene::DestroyEntity(Entity* entity, bool isRoot)
	{
		if (isRoot)
		{
			entity->SetParent(nullptr);
		}

		for (Entity* child : entity->GetChildren())
		{
			DestroyEntity(child, false);
		}

		OnComponentRemoved(entity, AllComponents{});
		m_EntityUUIDMap.erase(entity->GetUUID());
		m_EntityHandleMap.erase(entity->GetHandle());
		m_Registry.destroy(entity->GetHandle());
		delete entity;
	}

	void Scene::DestroyAllEntities()
	{
		for (auto const& [handle, entity] : m_EntityHandleMap)
		{
			OnComponentRemoved(entity, AllComponents{});
			m_EntityUUIDMap.erase(entity->GetUUID());
			m_Registry.destroy(handle);
			delete entity;
		}

		m_EntityHandleMap.clear();
	}

	Entity* Scene::GetEntity(UUID uuid)
	{
		if (m_EntityUUIDMap.find(uuid) != m_EntityUUIDMap.end())
		{
			return m_EntityUUIDMap.at(uuid);
		}

		return {};
	}

	Entity* Scene::GetEntity(entt::entity entityHandle)
	{
		if (m_EntityHandleMap.find(entityHandle) != m_EntityHandleMap.end())
		{
			return m_EntityHandleMap.at(entityHandle);
		}

		return {};
	}

	Entity* Scene::GetPrimaryCamera()
	{
		return m_PrimaryCamera;
	}

	void Scene::SetPrimaryCamera(Entity* entity)
	{
		m_PrimaryCamera = entity;
	}

	std::vector<Entity*> Scene::GetCameras()
	{
		auto view = m_Registry.view<CameraComponent>();
		std::vector<Entity*> cameras;
		cameras.reserve(view.size());

		for (auto entity : view)
		{
			cameras.push_back(GetEntity(entity));
		}

		return cameras;
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
			lightData.Intensity        = light.Light->GetIntensity();
			lightData.CutOffs          = light.Light->GetCutOff().x >= 0 ? glm::cos(glm::radians(light.Light->GetCutOff())) : light.Light->GetCutOff();
			m_Lights.push_back(lightData);
		}
	}

	void Scene::DrawSceneDeferred(const glm::vec3& cameraPosition, bool isEditor, Entity* selectedEntity)
	{
		//std::map<float, entt::entity> transparentEntities;
		//bool isSelectedEntityTransparent = false;
;
		{
			auto view = m_Registry.view<TransformComponent, MeshComponent>();
			for (auto entity : view)
			{
				if (selectedEntity != nullptr && entity == *selectedEntity)
				{
					continue;
				}

				auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

				DrawComponent<MeshComponent>(GetEntity(entity), GetEntityTransform(GetEntity(entity)), mesh);
			}
		}

		//if (selectedEntity && !isSelectedEntityTransparent)
		if (selectedEntity)
		{
			DrawSelectedEntity(selectedEntity);
		}

		//if (transparentEntities.size())
		//{
		//	Renderer::NextBatch();

		//	for (std::map<float, entt::entity>::reverse_iterator it = transparentEntities.rbegin(); it != transparentEntities.rend(); ++it)
		//	{
		//		if (isSelectedEntityTransparent && it->second == *selectedEntity)
		//		{
		//			DrawSelectedEntityAndOutline(selectedEntity);
		//		}
		//		else
		//		{
		//			DrawEntity(GetEntity(it->second));
		//		}
		//	}
		//}
	}

	void Scene::DrawSceneForward(const glm::vec3& cameraPosition, bool isEditor, Entity* selectedEntity)
	{
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

					if (selectedEntity != nullptr && entity == *selectedEntity)
					{
						isSelectedEntityTransparent = true;
					}
				}
				else
				{
					if (selectedEntity != nullptr && entity == *selectedEntity)
					{
						continue;
					}

					DrawComponent<SpriteRendererComponent>(GetEntity(entity), GetEntityTransform(GetEntity(entity)), sprite);
				}
			}
		}

		if (isEditor)
		{
			auto view = m_Registry.view<TransformComponent, LightSourceComponent>();
			for (auto entity : view)
			{
				if (selectedEntity != nullptr && entity == *selectedEntity)
				{
					continue;
				}

				auto [transform, light] = view.get<TransformComponent, LightSourceComponent>(entity);
				DrawComponent<LightSourceComponent>(GetEntity(entity), GetEntityTransform(GetEntity(entity)), light);
			}
		}

		if (selectedEntity && !isSelectedEntityTransparent)
		{
			if (selectedEntity->TryGetComponent<MeshComponent>() == nullptr)
			{
				DrawSelectedEntity(selectedEntity);
			}

			DrawSelectedEntityOutline(selectedEntity);
		}

		if (transparentEntities.size())
		{
			Renderer::NextBatch();

			for (std::map<float, entt::entity>::reverse_iterator it = transparentEntities.rbegin(); it != transparentEntities.rend(); ++it)
			{
				if (isSelectedEntityTransparent && it->second == *selectedEntity)
				{
					if (selectedEntity->TryGetComponent<MeshComponent>() == nullptr)
					{
						DrawSelectedEntity(selectedEntity);
					}

					DrawSelectedEntityOutline(selectedEntity);
				}
				else
				{
					DrawEntity(GetEntity(it->second));
				}
			}
		}
	}

	void Scene::DrawEntity(Entity* entity)
	{
		glm::mat4 transform = GetEntityTransform(entity);

		if (entity->HasComponent<SpriteRendererComponent>())
		{
			DrawComponent<SpriteRendererComponent>(entity, transform, m_Registry.get<SpriteRendererComponent>(entity->GetHandle()));
		}
		else if (entity->HasComponent<MeshComponent>())
		{
			DrawComponent<MeshComponent>(entity, transform, m_Registry.get<MeshComponent>(entity->GetHandle()));
		}
		else if (entity->HasComponent<LightSourceComponent>())
		{
			DrawComponent<LightSourceComponent>(entity, transform, m_Registry.get<LightSourceComponent>(entity->GetHandle()));
		}
	}

	void Scene::DrawSelectedEntity(Entity* entity)
	{
		Renderer::NextBatch();

		DrawEntity(entity);

		RenderCommand::SetStencilMask(0xFF);
		Renderer::NextBatch();
		RenderCommand::SetStencilMask(0x00);
	}

	void Scene::DrawSelectedEntityOutline(Entity* entity)
	{
		Renderer::NextBatch();

		glm::mat4 transform = GetEntityTransform(entity);
		glm::vec3 scale = m_Registry.get<TransformComponent>(entity->GetHandle()).Scale;
		float outlineSize = 0.1f;
		transform = glm::scale(transform, glm::vec3(1.0f + outlineSize / scale.x, 1.0f + outlineSize / scale.y, 1.0f + outlineSize / scale.z)); // TODO: Fix; ex: bunny
		glm::vec4 selectionColor = { 0.400f, 0.733f, 0.417f, 1.0f }; // TODO: Link to palette (selection green)

		if (entity->HasComponent<SpriteRendererComponent>())
		{
			Renderer::DrawQuad(transform, selectionColor, (int)entity->GetHandle());
		}
		else if (entity->HasComponent<MeshComponent>())
		{
			Renderer::DrawMeshOutline(transform, m_Registry.get<MeshComponent>(entity->GetHandle()), selectionColor, (int)entity->GetHandle());
		}
		else if (entity->HasComponent<LightSourceComponent>())
		{
			Renderer::DrawCircle(transform, selectionColor, 0.1f, 0.0f, (int)entity->GetHandle());
		}

		RenderCommand::SetStencilFunction(RendererAPI::TestFunction::NotEqual, 1, 0xFF);
		Renderer::NextBatch();
		RenderCommand::SetStencilFunction(RendererAPI::TestFunction::Always, 1, 0xFF);
	}

	glm::mat4 Scene::GetEntityTransform(Entity* entity)
	{
		// TODO: Fix? Weird behavior when rotating child then moving parent
		glm::mat4 transform = m_Registry.get<TransformComponent>(entity->GetHandle()).GetTransform();

		if (entity->GetParent() != nullptr)
		{
			transform *= GetEntityTransform(entity->GetParent());
		}

		return transform;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// OnComponentAdded ////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	void Scene::OnComponentAdded(Entity* entity, T& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity* entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity* entity, MeshComponent& component)
	{
		if (component.Mesh == nullptr)
		{
			component.Mesh = CreateRef<Mesh>();
		}
	}

	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity* entity, MaterialComponent& component)
	{
		if (component.Material == nullptr)
		{
			component.Material = CreateRef<Material>();
		}
	}

	template<>
	void Scene::OnComponentAdded<LightSourceComponent>(Entity* entity, LightSourceComponent& component)
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
	void Scene::OnComponentRemoved(Entity* entity, ComponentGroup<Component...>)
	{
		([&]()
			{
				if (entity->HasComponent<Component>())
				{
					OnComponentRemoved<Component>(entity, entity->GetComponent<Component>());
				}
			}
		(), ...);
	}

	template<typename T>
	void Scene::OnComponentRemoved(Entity* entity, T& component)
	{
	}
}