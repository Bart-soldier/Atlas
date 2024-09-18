#pragma once

#include "Atlas/Core/Timestep.h"
#include "Atlas/Core/UUID.h"
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
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

		std::string const GetName() { return m_Name; }
		Entity GetPrimaryCameraEntity();

		void SetAmbientLightColor(const glm::vec3 ambientLightColor) { m_AmbientLight.SetColor(ambientLightColor); }
		const glm::vec3& GetAmbientLightColor() { return m_AmbientLight.GetColor(); }
		void SetAmbientLightStrength(const float ambientLightStrength) { m_AmbientLight.SetStrength(ambientLightStrength); }
		const float& GetAmbientLightStrength() { return m_AmbientLight.GetStrength(); }

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		std::string m_Name;

		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		Light m_AmbientLight;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}


