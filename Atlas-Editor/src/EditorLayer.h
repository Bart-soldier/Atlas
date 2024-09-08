#pragma once

#include "Atlas.h"
#include "Panels/SceneHierarchyPanel.h"

#include "Atlas/Renderer/EditorCamera.h"

namespace Atlas
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();

		// Viewport related
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;

		// Temp
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}

