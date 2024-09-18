#pragma once

#include "Atlas.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneSettingsPanel.h"
#include "Panels/ContentBrowserPanel.h"

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
		void OnOverlayRender();

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		bool IsViewportClickable();

		void NewProject(bool saveCurrentScene = false);
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		Ref<Scene> NewScene();
		Ref<Scene> NewScene(std::string name);
		Ref<Scene> New2DStarterScene();
		Ref<Scene> New3DStarterScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SetEditorScene(Ref<Scene> scene);
		void SetEditorScene(Ref<Scene> scene, const std::filesystem::path& path);

		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		// Events
		bool OnWindowMoved(WindowMovedEvent& e) { m_ViewportInvalidated = true; return false; }

		// Toolbar Buttons
		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();

		// Viewport related
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;
		Entity m_HoveredEntity;
		bool m_ViewportInvalidated = false;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;

		// Temp
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		SceneSettingsPanel m_SceneSettingsPanel;
		Scope<ContentBrowserPanel> m_ContentBrowserPanel;

		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconStop;
	};
}

