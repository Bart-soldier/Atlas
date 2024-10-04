#pragma once

#include "Atlas/Core/Timestep.h"
#include "Atlas/Core/UUID.h"
#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Renderer/EditorCamera.h"
#include "Atlas/Renderer/Light.h"

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
		void OnUpdateEditor(Timestep ts, EditorCamera& camera, Ref<Entity> selectedEntity);
		void OnViewportResize(uint32_t width, uint32_t height);

		Ref<Entity> CreateEntity(const std::string& name = std::string(), Ref<Entity> parent = nullptr);
		Ref<Entity> CreateEntity(UUID uuid, const std::string& name = std::string(), Ref<Entity> parent = nullptr);
		Ref<Entity> DuplicateEntity(Ref<Entity> entity, Ref<Entity> parent = nullptr);
		void DestroyEntity(Ref<Entity> entity);
		Ref<Entity> GetEntity(UUID uuid);
		Ref<Entity> GetEntity(entt::entity entityHandle);

		std::string const GetName() { return m_Name; }
		Entity GetPrimaryCameraEntity();

	private:
		void UpdateLights();
		void DrawScene(const glm::vec3& cameraPosition, Ref<Entity> selectedEntity);
		void DrawEntity(Ref<Entity> entity);
		void DrawSelectedEntityAndOutline(Ref<Entity> entity);
		glm::mat4 GetEntityTransform(Ref<Entity> entity);

		template<typename T>
		void DrawComponent(Ref<Entity> entity, const glm::mat4& transform, const T& component);

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		template<typename... Component>
		void OnComponentRemoved(Entity entity, ComponentGroup<Component...>);

		template<typename T>
		void OnComponentRemoved(Entity entity, T& component);

		std::string m_Name;

		entt::registry m_Registry;
		std::unordered_map<UUID, Ref<Entity>> m_EntityUUIDMap;
		std::unordered_map<entt::entity, Ref<Entity>> m_EntityHandleMap;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;
		std::vector<Renderer::LightData> m_Lights;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}


