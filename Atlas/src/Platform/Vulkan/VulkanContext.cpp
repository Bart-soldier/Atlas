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
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContext::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		#ifdef ATLAS_DEBUG
			const bool enableValidationLayers = true;
		#else
			const bool enableValidationLayers = false;
		#endif

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

		ATLAS_CORE_INFO("Vulkan Info:");

		/* ------------------------------ */
		/* --------- EXTENSIONS --------- */
		/* ------------------------------ */

		ATLAS_CORE_INFO("  Extensions:");

		std::vector<const char*> extensions;
		GetRequiredExtensions(extensions, enableValidationLayers);

		if (VerifyExtensionSupport(extensions))
		{
			instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
		}
		else
		{
			instanceCreateInfo.enabledExtensionCount = 0;
		}

		ATLAS_CORE_INFO("    Enabled:   {0}", instanceCreateInfo.enabledExtensionCount);

		/* ------------------------------ */
		/* ----------- LAYERS ----------- */
		/* ------------------------------ */

		ATLAS_CORE_INFO("  Layers:");

		std::vector<const char*> layers;
		GetRequiredLayers(layers, enableValidationLayers);

		if (VerifyLayerSupport(layers))
		{
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
			instanceCreateInfo.ppEnabledLayerNames = layers.data();
		}
		else
		{
			instanceCreateInfo.enabledLayerCount = 0;
		}

		ATLAS_CORE_INFO("    Enabled:   {0}", instanceCreateInfo.enabledLayerCount);

		VkResult status = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
		if (status != VK_SUCCESS)
		{
			ATLAS_CORE_ERROR("Failed to created Vulkan instance!");
			throw std::runtime_error("Failed to created Vulkan instance!");
		}

		SelectPhysicalDevice();
		CreateLogicalDevice(layers, enableValidationLayers);
	}

	void VulkanContext::GetRequiredExtensions(std::vector<const char*>& extensions, bool enableValidationLayers)
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		extensions = std::vector(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
	}

	bool VulkanContext::VerifyExtensionSupport(const std::vector<const char*>& extensions)
	{
		if (extensions.size() == 0)
		{
			return false;
		}

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		ATLAS_CORE_INFO("    Available: {0}", extensionCount);

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

	void VulkanContext::GetRequiredLayers(std::vector<const char*>& layers, bool enableValidationLayers)
	{
		if (enableValidationLayers)
		{
			layers.push_back("VK_LAYER_KHRONOS_validation");
		}
	}

	bool VulkanContext::VerifyLayerSupport(const std::vector<const char*>& layers)
	{
		if (layers.size() == 0)
		{
			return false;
		}

		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		ATLAS_CORE_INFO("    Available: {0}", layerCount);

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

	void VulkanContext::SelectPhysicalDevice()
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
			if (IsDeviceSuitable(device))
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

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &deviceFeatures);

		ATLAS_CORE_INFO("  Physical Device:");
		ATLAS_CORE_INFO("    Name: {0}", deviceProperties.deviceName);
		ATLAS_CORE_INFO("    Driver version: {0}", deviceProperties.driverVersion);
	}

	bool VulkanContext::IsDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;
		FindQueueFamilies(device, indices);

		return indices.IsComplete();
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

			if (indices.IsComplete()) {
				return;
			}

			i++;
		}
	}

	void VulkanContext::CreateLogicalDevice(const std::vector<const char*>& layers, bool enableValidationLayers)
	{
		QueueFamilyIndices indices;
		FindQueueFamilies(m_PhysicalDevice, indices);

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.GraphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
			createInfo.ppEnabledLayerNames = layers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
		{
			ATLAS_CORE_ERROR("Failed to create logical device!");
			throw std::runtime_error("Failed to create logical device!");
		}

		vkGetDeviceQueue(m_LogicalDevice, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
	}

	void VulkanContext::SwapBuffers()
	{
		ATLAS_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}