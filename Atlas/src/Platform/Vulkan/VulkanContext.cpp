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
		vkDestroyDevice(m_LogicalDevice, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContext::Init()
	{
		ATLAS_PROFILE_FUNCTION();

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

		/* ------------------------------ */
		/* --------- EXTENSIONS --------- */
		/* ------------------------------ */

		std::vector<const char*> extensions;
		GetRequiredInstanceExtensions(extensions);

		if (VerifyInstanceExtensionSupport(extensions))
		{
			instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
		}
		else
		{
			throw std::runtime_error("Failed to retrieve all requested extensions!");
		}

		/* ------------------------------ */
		/* ----------- LAYERS ----------- */
		/* ------------------------------ */

		std::vector<const char*> layers;
		GetRequiredLayers(layers);

		if (VerifyLayerSupport(layers))
		{
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
			instanceCreateInfo.ppEnabledLayerNames = layers.data();
		}
		else
		{
			throw std::runtime_error("Failed to retrieve all requested layers!");
		}

		/* ------------------------------ */
		/* ------ DEVICE EXTENSIONS ----- */
		/* ------------------------------ */

		std::vector<const char*> deviceExtensions;
		GetRequiredDeviceExtensions(deviceExtensions);

		CreateInstance(instanceCreateInfo);
		CreateSurface();
		SelectPhysicalDevice(deviceExtensions);
		CreateLogicalDevice(deviceExtensions, layers);

		Reflect();
	}

	void VulkanContext::GetRequiredInstanceExtensions(std::vector<const char*>& extensions)
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		extensions = std::vector(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef ATLAS_DEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
	}

	bool VulkanContext::VerifyInstanceExtensionSupport(const std::vector<const char*>& extensions)
	{
		if (extensions.size() == 0)
		{
			return true;
		}

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		for (const char* extensionName : extensions) {
			bool extensionFound = false;

			for (const auto& extensionProperties : availableExtensions) {
				if (strcmp(extensionName, extensionProperties.extensionName) == 0) {
					extensionFound = true;
					break;
				}
			}

			if (!extensionFound) {
				ATLAS_CORE_ERROR("Unavailable Vulkan extension requested: {0}", extensionName);
				return false;
			}
		}

		return true;
	}

	void VulkanContext::GetRequiredLayers(std::vector<const char*>& layers)
	{
#ifdef ATLAS_DEBUG
		layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
	}

	bool VulkanContext::VerifyLayerSupport(const std::vector<const char*>& layers)
	{
		if (layers.size() == 0)
		{
			return true;
		}

		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : layers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				ATLAS_CORE_ERROR("Unavailable Vulkan layer requested: {0}", layerName);
				return false;
			}
		}

		return true;
	}

	void VulkanContext::CreateInstance(const VkInstanceCreateInfo& instanceCreateInfo)
	{
		if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			ATLAS_CORE_ERROR("Failed to created Vulkan instance!");
			throw std::runtime_error("Failed to created Vulkan instance!");
		}
	}

	void VulkanContext::CreateSurface()
	{
		if (glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface) != VK_SUCCESS)
		{
			ATLAS_CORE_ERROR("Failed to create window surface!");
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	void VulkanContext::SelectPhysicalDevice(const std::vector<const char*>& requiredExtensions)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			ATLAS_CORE_ERROR("Failed to find GPUs with Vulkan support!");
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device, requiredExtensions))
			{
				m_PhysicalDevice = device;
				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			ATLAS_CORE_ERROR("Failed to find a suitable GPU!");
			throw std::runtime_error("Failed to find a suitable GPU!");
		}
	}

	bool VulkanContext::IsDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions)
	{
		QueueFamilyIndices indices;
		FindQueueFamilies(device, indices);

		bool extensionsSupported = VerifyDeviceExtensionSupport(device, requiredExtensions);

		return indices.IsComplete() && extensionsSupported;
	}

	void VulkanContext::GetRequiredDeviceExtensions(std::vector<const char*>& extensions)
	{
		extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	bool VulkanContext::VerifyDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& extensions)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	void VulkanContext::FindQueueFamilies(VkPhysicalDevice device, QueueFamilyIndices& indices)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.GraphicsFamily = i;
			}

			VkBool32 presentationSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentationSupport);

			if (presentationSupport)
			{
				indices.PresentationFamily = i;
			}

			if (indices.IsComplete()) {
				return;
			}

			i++;
		}
	}

	void VulkanContext::CreateLogicalDevice(const std::vector<const char*>& extensions, const std::vector<const char*>& layers)
	{
		QueueFamilyIndices indices;
		FindQueueFamilies(m_PhysicalDevice, indices);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentationFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames = layers.data();

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
		{
			ATLAS_CORE_ERROR("Failed to create logical device!");
			throw std::runtime_error("Failed to create logical device!");
		}

		vkGetDeviceQueue(m_LogicalDevice, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, indices.PresentationFamily.value(), 0, &m_PresentationQueue);
	}

	void VulkanContext::Reflect()
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);

		ATLAS_CORE_INFO("Vulkan Info:");
		ATLAS_CORE_INFO("  Physical Device:");
		ATLAS_CORE_INFO("    Name: {0}", deviceProperties.deviceName);
		ATLAS_CORE_INFO("    Driver version: {0}", deviceProperties.driverVersion);
	}

	void VulkanContext::SwapBuffers()
	{
		ATLAS_PROFILE_FUNCTION();

		//glfwSwapBuffers(m_WindowHandle);
	}
}