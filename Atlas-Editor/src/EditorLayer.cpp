#include "EditorLayer.h"
#include "Atlas/Scene/SceneSerializer.h"

#include "Atlas/Utils/PlatformUtils.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGuizmo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Atlas/Math/Math.h"

namespace Atlas
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		ATLAS_PROFILE_FUNCTION();

		// Editor resources
		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png", false);
		m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png", false);

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			OpenProject(commandLineArgs[1]);
		}
		else
		{
			m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
			New3DStarterScene();
			m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		}
	}

	void EditorLayer::OnDetach()
	{
		ATLAS_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		ATLAS_PROFILE_FUNCTION();

		if (m_ActiveScene == nullptr)
		{
			return;
		}

		// Resize
		FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ViewportInvalidated = true;
		}

		// Get mouse position
		auto [mx, my] = ImGui::GetMousePos();
		int mouseX = (int)(mx - m_ViewportBounds[0].x);
		int mouseY = (int)(m_ViewportSize.y - (my - m_ViewportBounds[0].y));
		m_ViewportHovered = mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y;

		// Render
		Renderer::ResetStats();
		{
			ATLAS_PROFILE_SCOPE("Renderer Prep");
			m_Framebuffer->Bind();
			// TODO: Link to palette
			RenderCommand::SetClearColor({ 0.090f, 0.114f, 0.133f, 1.0f });
			RenderCommand::Clear();

			// Clear our entity ID attachment to -1
			m_Framebuffer->ClearAttachment(1, -1);
		}

		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				if (m_ViewportHovered)
				{
					m_EditorCamera.OnUpdate(ts);
				}

				if (m_ActiveScene)
				{
					m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				}
				break;
			}
			case SceneState::Play:
			{
				if (m_ActiveScene)
				{
					m_ActiveScene->OnUpdateRuntime(ts);
				}
				break;
			}
		}

		// Check viewport boundaries
		if (m_ViewportHovered)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		//OnOverlayRender();

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		/*ImGui::ShowDemoWindow();*/

		ATLAS_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project"))
				{
					NewProject();
				}

				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
				{
					OpenProject();
				}

				if (ImGui::MenuItem("Save Project"))
				{
					SaveProject();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open Scene"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					Application::Get().Close();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();
		m_SceneSettingsPanel.OnImGuiRender();
		m_ContentBrowserPanel->OnImGuiRender();

		ImGui::Begin("Renderer Stats");

		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Renderer::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		if (m_ViewportInvalidated)
		{
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
			m_ViewportInvalidated = false;
		}

		m_ViewportFocused = ImGui::IsWindowFocused();
		// TODO : Check blocked shortcut events
		//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1,0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(path);
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic((int)m_EditorCamera.GetProjectionType());
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			{
				snapValue = 45.0f;
			}

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}
		
		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (!camera)
			{
				return;
			}

			// Overlay should not be affected by lights
			//Renderer::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>(), nullptr);
		}
		else
		{
			// Overlay should not be affected by lights
			//Renderer::BeginScene(m_EditorCamera);
		}

		// Draw selected entity outline 
		if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity()) {
			const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
			Renderer::DrawRect(transform.GetTransform(), glm::vec4(0.400f, 0.733f, 0.417f, 1.0f)); // TODO: Link to palette (selection green)
		}

		Renderer::EndScene();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (m_SceneState == SceneState::Edit)
		{
			m_EditorCamera.OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowMovedEvent>(ATLAS_BIND_EVENT_FN(EditorLayer::OnWindowMoved));
		dispatcher.Dispatch<KeyPressedEvent>(ATLAS_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ATLAS_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
			case Key::N:
				if (control)
				{
					NewScene();
				}
				break;
			case Key::O:
				if (control)
				{
					OpenProject();
				}
				break;
			case Key::S:
				if (control)
				{
					if (shift)
					{
						SaveSceneAs();
					}
					else
					{
						SaveScene();
					}
				}
				break;
			// Scene Commands
			case Key::D:
				if (control)
				{
					OnDuplicateEntity();
				}
				break;
			// Gizmos
			case Key::Q:
				if (!ImGuizmo::IsUsing() && Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
				{
					m_GizmoType = -1;
				}
				break;
			case Key::T:
				if (!ImGuizmo::IsUsing() && Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
				{
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				}
				break;
			case Key::R:
				if (!ImGuizmo::IsUsing() && Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
				{
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				}
				break;
			case Key::E:
				if (!ImGuizmo::IsUsing() && Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
				{
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
				}
				break;
			case Key::Delete:
			{
				if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
				{
					Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
					if (selectedEntity)
					{
						m_SceneHierarchyPanel.SetSelectedEntity({});
						m_ActiveScene->DestroyEntity(selectedEntity);
					}
				}
				break;
			}
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		switch (e.GetMouseButton())
		{
			case Mouse::ButtonLeft:
				if (IsViewportClickable())
				{
					m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
				}
				break;
		}

		return false;
	}

	bool EditorLayer::IsViewportClickable()
	{
		return m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt);
	}

	void EditorLayer::NewProject(bool saveCurrentScene)
	{
		std::filesystem::path path = FileDialogs::SaveFile("Atlas Project\0");
		if (!path.empty())
		{
			std::filesystem::path projectDirectory = path.replace_extension();
			FileHelpers::CreateDirectoryIfNeeded(projectDirectory);
			std::filesystem::path projectSolution = (path / path.stem()).replace_extension(".atlasproj");

			Ref<Project> newProject = Project::New(projectSolution);
			if (!saveCurrentScene || !m_EditorScene)
			{
				New3DStarterScene();
			}

			std::filesystem::path scenePath = newProject->GetAssetDirectory() / "Scenes";
			FileHelpers::CreateDirectoryIfNeeded(scenePath);
			std::string sceneFileName = m_EditorScene->GetName() + ".atlas";
			m_EditorScenePath = scenePath / sceneFileName;
			SaveScene();

			newProject->GetConfig().Name = path.stem().string();
			newProject->GetConfig().StartScene = m_EditorScenePath;
			SaveProject();


			Application::Get().GetWindow().SetSecondaryTitle(newProject->GetConfig().Name);
			m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
		}
	}

	bool EditorLayer::OpenProject()
	{
		std::string filepath = FileDialogs::OpenFile("Atlas Project (*.atlasproj)\0*.atlasproj\0");
		if (filepath.empty())
		{
			return false;
		}

		OpenProject(filepath);
		return true;
	}

	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		if (Project::Load(path))
		{
			auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
			OpenScene(startScenePath);

			Application::Get().GetWindow().SetSecondaryTitle(Project::GetActive()->GetConfig().Name);
			m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
		}
	}

	void EditorLayer::SaveProject()
	{
		if (!m_EditorScenePath.empty())
		{
			Project::SaveActive();
		}
		else
		{
			NewProject(true);
		}
	}

	Ref<Scene> EditorLayer::NewScene()
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		SetEditorScene(newScene);
		return newScene;
	}

	Ref<Scene> EditorLayer::NewScene(std::string name)
	{
		Ref<Scene> newScene = CreateRef<Scene>(name);
		SetEditorScene(newScene);
		return newScene;
	}

	Ref<Scene> EditorLayer::New2DStarterScene()
	{
		Ref<Scene> newScene = CreateRef<Scene>("Starter Scene");
		SetEditorScene(newScene);

		Entity squareEntity = newScene->CreateEntity("White Square");
		squareEntity.AddComponent<SpriteRendererComponent>();

		Entity cameraEntity = newScene->CreateEntity("Camera");
		cameraEntity.AddComponent<CameraComponent>();
		CameraComponent cameraComponent = cameraEntity.GetComponent<CameraComponent>();
		cameraComponent.Camera.SetProjectionType(Camera::ProjectionType::Orthographic);
		cameraComponent.Camera.SetOrthographicFarClip(2.0f);

		return newScene;
	}

	Ref<Scene> EditorLayer::New3DStarterScene()
	{
		Ref<Scene> newScene = CreateRef<Scene>("Starter Scene");
		SetEditorScene(newScene);

		Entity squareEntity = newScene->CreateEntity("White Cube");
		squareEntity.AddComponent<MeshComponent>();
		squareEntity.AddComponent<MaterialComponent>();
		squareEntity.GetComponent<MaterialComponent>().Material.SetMaterialPreset(Material::MaterialPresets::Gold);

		Entity cameraEntity = newScene->CreateEntity("Camera");
		cameraEntity.AddComponent<CameraComponent>();
		cameraEntity.GetComponent<CameraComponent>().Camera.SetProjectionType(Camera::ProjectionType::Perspective);
		TransformComponent* cameraTransform = &cameraEntity.GetComponent<TransformComponent>();
		cameraTransform->Translation = glm::vec3(3.5f, 2.1f, 3.5f);
		cameraTransform->Rotation = glm::radians(glm::vec3(-25.0f, 45.0f, 0.0f));

		Entity lightEntity = newScene->CreateEntity("Point Light");
		lightEntity.GetComponent<TransformComponent>().Translation = glm::vec3(3.0f, 2.0f, 1.5f);
		lightEntity.AddComponent<LightSourceComponent>();

		return newScene;
	}

	void EditorLayer::OpenScene()
	{
		std::filesystem::path path = FileDialogs::OpenFile("Atlas Scene (*.atlas)\0*.atlas\0");
		if (!path.empty())
		{
			OpenScene(path);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
		{
			OnSceneStop();
		}

		if (path.extension().string() != ".atlas")
		{
			ATLAS_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path))
		{
			SetEditorScene(newScene, path);
		}
	}

	void EditorLayer::SetEditorScene(Ref<Scene> scene)
	{
		SetEditorScene(scene, std::filesystem::path());
	}

	void EditorLayer::SetEditorScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		m_EditorScene = scene;
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);
		m_SceneSettingsPanel.SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;
		m_EditorScenePath = path;
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
		{
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		}
		else
		{
			SaveSceneAs();
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::filesystem::path path = FileDialogs::SaveFile("Atlas Scene (*.atlas)\0*.atlas\0");
		if (!path.empty())
		{
			SerializeScene(m_ActiveScene, path);
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path);
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SceneSettingsPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;

		m_ActiveScene->OnRuntimeStop();
		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_SceneSettingsPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
		{
			return;
		}

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			Entity newEntity = m_EditorScene->DuplicateEntity(selectedEntity);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}

	void EditorLayer::UI_Toolbar()
	{
		float padding = GImGui->Style.FramePadding.x;

		/* ---------- STYLE ---------- */
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, padding));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_ActiveScene;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
		{
			tintColor.w = 0.5f;
		}

		/* ---------- POLYGON TYPE ---------- */
		ImGui::PushItemWidth(120);
		ImGui::SetCursorPosX(padding);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.5 - ImGui::GetTextLineHeightWithSpacing() * 0.5);
		const char* polygonTypeStrings[] = { "Fill", "Line", "Point"};
		const char* currentPolygonTypeString = polygonTypeStrings[(int)Renderer::GetPolygonMode()];
		if (ImGui::BeginCombo("##PolygonType", currentPolygonTypeString))
		{
			for (int i = 0; i < 3; i++)
			{
				bool isSelected = currentPolygonTypeString == polygonTypeStrings[i];
				if (ImGui::Selectable(polygonTypeStrings[i], isSelected))
				{
					currentPolygonTypeString = polygonTypeStrings[i];
					Renderer::SetPolygonMode((RendererAPI::PolygonMode)i);
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		/* ---------- PLAY / STOP ---------- */
		float size = ImGui::GetWindowHeight() - 2 * padding;
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		ImGui::SetCursorPosY(padding);
		if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}
			else if (m_SceneState == SceneState::Play)
			{
				OnSceneStop();
			}
		}

		if (!toolbarEnabled)
		{
			ImGui::BeginDisabled();
		}

		/* ---------- EDITOR CAMERA ---------- */
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.5 - ImGui::GetTextLineHeightWithSpacing() * 0.5);
		bool isLocked = m_EditorCamera.IsRotationLocked();
		if (ImGui::Checkbox("Lock Rotation", &isLocked))
		{
			m_EditorCamera.SetRotationLock(isLocked);
		}

		ImGui::PushItemWidth(-1);
		ImGui::SameLine(0);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.5 - ImGui::GetTextLineHeightWithSpacing() * 0.5);
		const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
		const char* currentProjectionTypeString = projectionTypeStrings[(int)m_EditorCamera.GetProjectionType()];
		if (ImGui::BeginCombo("##Projection", currentProjectionTypeString))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
				{
					currentProjectionTypeString = projectionTypeStrings[i];
					m_EditorCamera.SetProjectionType((EditorCamera::ProjectionType)i);
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		if (!toolbarEnabled)
		{
			ImGui::EndDisabled();
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}
}