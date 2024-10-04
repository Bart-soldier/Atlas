#include "atlaspch.h"
#include "Atlas/Scene/Entity.h"

namespace Atlas
{
	Entity::Entity(entt::entity handle, Scene* scene, Ref<Entity> parent)
		: m_EntityHandle(handle), m_Scene(scene), m_Parent(parent)
	{
	}

	Ref<Entity> Entity::GetParent()
	{
		return m_Parent;
	}

	// WARNING: This does not add child to parent
	void Entity::SetParent(Ref<Entity> parent)
	{
		m_Parent = parent;
	}

	const std::vector<Ref<Entity>>& Entity::GetChildren()
	{
		return m_Children;
	}

	void Entity::AddChild(Ref<Entity> child)
	{
		m_Children.push_back(child);
	}

	void Entity::RemoveChild(Ref<Entity> child)
	{
		auto it = std::find(m_Children.begin(), m_Children.end(), child);

		if (it != m_Children.end()) {
			m_Children.erase(it);
		}
	}
}