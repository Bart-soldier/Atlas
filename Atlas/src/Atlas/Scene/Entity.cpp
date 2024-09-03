#include "atlaspch.h"
#include "Atlas/Scene/Entity.h"

namespace Atlas
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}