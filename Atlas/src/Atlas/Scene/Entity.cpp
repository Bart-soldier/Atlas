#include "atlaspch.h"
#include "Atlas/Scene/Entity.h"

namespace Atlas
{
	Entity::Entity(entt::entity handle, Scene* scene, Entity* parent)
		: m_EntityHandle(handle), m_Scene(scene)
	{
		SetParent(parent);
	}

	Entity* Entity::GetParent()
	{
		return m_Parent;
	}

	void Entity::SetParent(Entity* parent)
	{
		if (m_Parent == parent)
		{
			return;
		}

		if (m_Parent != nullptr)
		{
			m_Parent->RemoveChild(this);
		}

		if (parent != nullptr)
		{
			parent->AddChild(this);
		}

		m_Parent = parent;
	}

	const std::vector<Entity*>& Entity::GetChildren()
	{
		return m_Children;
	}

	void Entity::AddChild(Entity* child)
	{
		m_Children.push_back(child);
	}

	void Entity::RemoveChild(Entity* child)
	{
		auto it = std::find(m_Children.begin(), m_Children.end(), child);

		if (it != m_Children.end()) {
			m_Children.erase(it);
		}
	}
}