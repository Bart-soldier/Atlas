#pragma once

#include "Atlas/Core/UUID.h"
#include "Atlas/Scene/Scene.h"
#include "Atlas/Scene/Components.h"

#include "entt.hpp"

namespace Atlas
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene, Entity* parent = nullptr);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ATLAS_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			ATLAS_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		T* TryGetComponent()
		{
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			ATLAS_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			T& component = m_Scene->m_Registry.get<T>(m_EntityHandle);
			m_Scene->OnComponentRemoved<T>(this, component);
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		
		operator entt::entity() const { return m_EntityHandle; }

		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		operator int() const { return (int)m_EntityHandle; }

		entt::entity GetHandle() const { return m_EntityHandle; }
		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		Entity* GetParent();
		void SetParent(Entity* parent);
		const std::vector<Entity*>& GetChildren();

	private:
		void AddChild(Entity* child);
		void RemoveChild(Entity* child);

		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		Entity* m_Parent = nullptr;
		std::vector<Entity*> m_Children;
	};
}