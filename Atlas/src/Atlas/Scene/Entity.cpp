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

	const std::vector<Entity*>& Entity::GetDirectChildren()
	{
		return m_DirectChildren;
	}

	std::vector<Entity*> Entity::GetAllChildren()
	{
		std::vector<Entity*> allChildren;

		size_t childrenNb = m_DirectChildren.size();

		if (childrenNb > 0)
		{
			allChildren.reserve(childrenNb);

			for (Entity* child : m_DirectChildren)
			{
				std::vector<Entity*> allChildDescendants = child->GetAllChildren();
				if (allChildDescendants.size() > 0)
				{
					allChildren.insert(allChildren.end(), allChildDescendants.begin(), allChildDescendants.end());
				}
				allChildren.push_back(child);
			}
		}

		return allChildren;
	}

	void Entity::AddChild(Entity* child)
	{
		m_DirectChildren.push_back(child);
	}

	void Entity::RemoveChild(Entity* child)
	{
		auto it = std::find(m_DirectChildren.begin(), m_DirectChildren.end(), child);

		if (it != m_DirectChildren.end()) {
			m_DirectChildren.erase(it);
		}
	}
}