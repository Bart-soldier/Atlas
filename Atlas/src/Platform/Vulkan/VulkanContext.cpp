#include "atlaspch.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>

namespace Atlas
{
	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ATLAS_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	VulkanContext::~VulkanContext()
	{
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContext::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);

		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "Atlas Editor";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "Atlas Engine";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
		instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
		instanceCreateInfo.enabledLayerCount = 0;

		VkResult status = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to created Vulkan instance!");

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		//std::vector<VkExtensionProperties> extensions(extensionCount);
		//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		ATLAS_CORE_INFO("Vulkan Info:");
		ATLAS_CORE_INFO("  Extensions: {0}", extensionCount);
		//for (const auto& extension : extensions) {
		//	ATLAS_CORE_INFO("    {0}", extension.extensionName);
		//}



		//ATLAS_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		//ATLAS_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		//ATLAS_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		//ATLAS_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Atlas requires at least OpenGL version 4.5!");
	}

	void VulkanContext::SwapBuffers()
	{
		ATLAS_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}