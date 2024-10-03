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

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntity(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera, Entity selectedEntity);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

		std::string const GetName() { return m_Name; }
		Entity GetPrimaryCameraEntity();

	private:
		void UpdateLights();
		void DrawScene(Entity excludedEntity);
		void DrawSelectedEntity(Entity entity);
		void DrawSelectedEntityOutline(Entity entity);

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		template<typename... Component>
		void OnComponentRemoved(Entity entity, ComponentGroup<Component...>);

		template<typename T>
		void OnComponentRemoved(Entity entity, T& component);

		std::string m_Name;

		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;
		std::vector<Renderer::LightData> m_Lights;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}


