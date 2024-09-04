#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Core/Log.h"
#include "Atlas/Scene/Scene.h"
#include "Atlas/Scene/Entity.h"

namespace Atlas
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

	private:
		void DrawEntityNode(Entity entity);

		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}