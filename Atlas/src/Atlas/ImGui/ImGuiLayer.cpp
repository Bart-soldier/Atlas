#include "atlaspch.h"
#include "Atlas/ImGui/ImGuiLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>

#include <ImGuizmo.h>

#include "Atlas/Core/Application.h"
#include "Atlas/Renderer/RenderCommand.h"

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

		Init();
	}

	void ImGuiLayer::OnDetach()
	{
		ATLAS_PROFILE_FUNCTION();

		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			ImGui_ImplOpenGL3_Shutdown();
			break;
		case RendererAPI::API::Vulkan:
			ImGui_ImplVulkan_Shutdown();
			break;
		}

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

		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			ImGui_ImplOpenGL3_NewFrame();
			break;
		case RendererAPI::API::Vulkan:
			ImGui_ImplVulkan_NewFrame();
			break;
		}

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		ATLAS_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow()->GetWidth(), (float)app.GetWindow()->GetHeight());

		// Rendering
		ImGui::Render();
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			break;
		case RendererAPI::API::Vulkan:
			//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData()); TODO: Complete
			break;
		}

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

		// Scrollbar
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
	
		auto& gizmoColors = ImGuizmo::GetStyle().Colors;

		gizmoColors[ImGuizmo::TEXT] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f }; // White

		gizmoColors[ImGuizmo::DIRECTION_X] = ImVec4{ 0.933f, 0.357f, 0.416f, 1.0f }; // Red
		gizmoColors[ImGuizmo::DIRECTION_Y] = ImVec4{ 0.212f, 0.655f, 0.330f, 1.0f }; // Green
		gizmoColors[ImGuizmo::DIRECTION_Z] = ImVec4{ 0.271f, 0.590f, 0.900f, 1.0f }; // Blue

		gizmoColors[ImGuizmo::PLANE_X] = ImVec4{ 0.933f, 0.357f, 0.416f, 1.0f }; // Red
		gizmoColors[ImGuizmo::PLANE_Y] = ImVec4{ 0.212f, 0.655f, 0.330f, 1.0f }; // Green
		gizmoColors[ImGuizmo::PLANE_Z] = ImVec4{ 0.271f, 0.590f, 0.900f, 1.0f }; // Blue

		gizmoColors[ImGuizmo::SELECTION] = ImVec4{ 0.698f, 0.122f, 0.848f, 1.0f }; // Purple

		ImGuizmo::GetStyle().HatchedAxisLineThickness = 0.0f;

		// TODO : Add Color Palette and link RGB In Panel and Viewport Background color
	}

	uint32_t ImGuiLayer::GetActiveWidgetID() const
	{
		return GImGui->ActiveId;
	}

	void ImGuiLayer::Init()
	{
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow()->GetNativeWindow());
		const Scope<GraphicsContext>& context = app.GetGraphicsContext();

		// Setup Platform/Renderer bindings
		if (RenderCommand::GetAPI() == RendererAPI::API::OpenGL)
		{
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 410");
		}
		else if (RenderCommand::GetAPI() == RendererAPI::API::Vulkan)
		{
			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};

			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1000;
			pool_info.poolSizeCount = std::size(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;

			VkDescriptorPool imguiPool;
			VkResult status = vkCreateDescriptorPool((VkDevice)context->GetLogicalDevice(), &pool_info, nullptr, &imguiPool);
			ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to created ImGui descriptor pool!");

			ImGui_ImplVulkan_InitInfo initInfo = {};
			initInfo.Instance = (VkInstance)context->GetInstance();
			initInfo.PhysicalDevice = (VkPhysicalDevice)context->GetPhysicalDevice();
			initInfo.Device = (VkDevice)context->GetLogicalDevice();
			initInfo.Queue = (VkQueue)context->GetGraphicsQueue();
			initInfo.DescriptorPool = imguiPool;
			initInfo.MinImageCount = 3;
			initInfo.ImageCount = 3;
			initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

			ImGui_ImplGlfw_InitForVulkan(window, true);
			ImGui_ImplVulkan_Init(&initInfo); // TODO: Complete
		}
	}
}