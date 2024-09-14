#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Scene/Scene.h"
#include "Atlas/Scene/Entity.h"

#include <glm/gtc/type_ptr.hpp>

namespace Atlas
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);

		void OnImGuiRender();

	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		/* --------------- CUSTOM UI HELPERS --------------- */

		static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		static void DrawTextureViewerPreDragDropTarget(const Ref<Texture2D> texture, float desiredWidth, float desiredHeight, bool flipped);
		static bool DrawTextureViewerPostDragDropTarget(const Ref<Texture2D> texture);

		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction);

		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}