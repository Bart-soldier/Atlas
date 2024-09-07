#include "atlaspch.h"
#include "Atlas/ImGui/ImGuiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "Atlas/Core/Application.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Atlas
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ATLAS_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		ATLAS_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		ATLAS_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ATLAS_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		// Background
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.165f, 0.188f, 0.208f, 1.0f }; //BG60
		colors[ImGuiCol_PopupBg] = ImVec4{ 0.130f, 0.153f, 0.173f, 1.0f }; // BG70
		colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f }; // White
		colors[ImGuiCol_TextDisabled] = ImVec4{ 0.522f, 0.600f, 0.679f, 1.0f }; // White
		colors[ImGuiCol_Border] = ImVec4{ 0.416f, 0.440f, 0.459f, 1.0f }; //BG30

		// Scroolbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.165f, 0.188f, 0.208f, 1.0f }; //BG60
		colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.416f, 0.440f, 0.459f, 1.0f }; //BG30
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.682f, 0.706f, 0.725f, 1.0f }; //BG20
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.416f, 0.440f, 0.459f, 1.0f }; //BG30

		// Menu Bar
		colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.130f, 0.153f, 0.173f, 1.0f }; // BG70

		// Checkmark
		colors[ImGuiCol_CheckMark] = ImVec4{ 0.271f, 0.590f, 0.900f, 1.0f }; // Helper Blue

		// Window Headers Background
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.274f, 0.298f, 0.318f, 1.0f }; // BG40
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.274f, 0.298f, 0.318f, 1.0f }; // BG40
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.274f, 0.298f, 0.318f, 1.0f }; //BG40

		// Window Headers Tab
		colors[ImGuiCol_Tab] = ImVec4{ 0.274f, 0.298f, 0.318f, 1.0f }; // BG40
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.165f, 0.188f, 0.208f, 1.0f }; // BG60
		colors[ImGuiCol_TabActive] = ImVec4{ 0.204f, 0.227f, 0.247f, 1.0f }; // BG50
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.274f, 0.298f, 0.318f, 1.0f }; // BG40
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.274f, 0.298f, 0.318f, 1.0f }; // BG40

		// Primary Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.130f, 0.153f, 0.173f, 1.0f }; // BG70
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.090f, 0.114f, 0.133f, 1.0f }; // BG80
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.130f, 0.153f, 0.173f, 1.0f }; // BG70

		// Component Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.130f, 0.153f, 0.173f, 1.0f }; // BG70
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.090f, 0.114f, 0.133f, 1.0f }; // BG80
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.130f, 0.153f, 0.173f, 1.0f }; // BG70

		// Separators
		colors[ImGuiCol_Separator] = ImVec4{ 0.274f, 0.298f, 0.318f, 1.0f }; // BG40
		colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.165f, 0.188f, 0.208f, 1.0f }; // BG60
		colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.204f, 0.227f, 0.247f, 1.0f }; // BG50

		// Component Content
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.204f, 0.227f, 0.247f, 1.0f }; // BG50
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.130f, 0.153f, 0.173f, 1.0f }; // BG70
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.204f, 0.227f, 0.247f, 1.0f }; // BG50
	}
}