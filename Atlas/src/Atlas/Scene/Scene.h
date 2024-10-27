#pragma once

#include "Atlas/Core/Timestep.h"
#include "Atlas/Core/UUID.h"

#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Renderer/EditorCamera.h"
#include "Atlas/Renderer/Light.h"
#include "Atlas/Renderer/Cubemap.h"

#include "Atlas/Scene/Components.h"

#include "entt.hpp"

namespace Atlas
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		Scene(std::string name);
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera, Entity* selectedEntity);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity* CreateEntity(const std::string& name = std::string(), Entity* parent = nullptr);
		Entity* CreateEntity(UUID uuid, const std::string& name = std::string(), Entity* parent = nullptr);
		Entity* DuplicateEntity(Entity* entity, Entity* parent = nullptr);
		void DestroyEntity(Entity* entity, bool isRoot = true);
		void DestroyAllEntities();
		Entity* GetEntity(UUID uuid);
		Entity* GetEntity(entt::entity entityHandle);
		glm::mat4 GetEntityTransform(Entity* entity);

		std::string const GetName() { return m_Name; }
		Entity* GetPrimaryCamera();
		void SetPrimaryCamera(Entity* entity);
		std::vector<Entity*> GetCameras();

	private:
		void UpdateLights();
		void DrawSceneDeferred(const glm::vec3& cameraPosition, bool isEditor, Entity* selectedEntity);
		void DrawSceneForward(const glm::vec3& cameraPosition, bool isEditor, Entity* selectedEntity);
		void DrawEntity(Entity* entity);
		void DrawSelectedEntity(Entity* entity);
		void DrawSelectedEntityOutline(Entity* entity);

		template<typename T>
		void DrawComponent(Entity* entity, const glm::mat4& transform, const T& component);

		template<typename T>
		void OnComponentAdded(Entity* entity, T& component);

		template<typename... Component>
		void OnComponentRemoved(Entity* entity, ComponentGroup<Component...>);

		template<typename T>
		void OnComponentRemoved(Entity* entity, T& component);

		std::string m_Name;

		entt::registry m_Registry;
		std::unordered_map<UUID, Entity*> m_EntityUUIDMap;
		std::unordered_map<entt::entity, Entity*> m_EntityHandleMap;
		Entity* m_PrimaryCamera = nullptr;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;
		std::vector<Renderer::LightData> m_Lights;
		Ref<Cubemap> m_Skybox;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		friend class SceneSettingsPanel;
	};
}


