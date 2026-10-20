#include "atlaspch.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_vulkan.h>

#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanResources.h"

namespace Atlas
{
	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ATLAS_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void* VulkanContext::GetCommandBuffer() const
	{
		return nullptr;
	}

	VulkanContext::~VulkanContext()
	{
		if (m_LogicalDevice == VK_NULL_HANDLE)
			return;

		vkDeviceWaitIdle(m_LogicalDevice);

		vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphore, nullptr);
		if (m_ImGuiRenderFinishedSemaphore)
			vkDestroySemaphore(m_LogicalDevice, m_ImGuiRenderFinishedSemaphore, nullptr);

		for (auto framebuffer : m_SwapChainFramebuffers)
		{
			vkDestroyFramebuffer(m_LogicalDevice, framebuffer, nullptr);
		}

		for (auto imageView : m_SwapChainImageViews)
		{
			vkDestroyImageView(m_LogicalDevice, imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
		if (m_ImmediateCommandPool)
			vkDestroyCommandPool(m_LogicalDevice, m_ImmediateCommandPool, nullptr);
		if (m_Allocator != VK_NULL_HANDLE)
			vmaDestroyAllocator(m_Allocator);
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
		applicationInfo.apiVersion = VK_API_VERSION_1_3;

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
			ATLAS_CORE_ASSERT(false, "Failed to retrieve all requested extensions!");
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
			ATLAS_CORE_ASSERT(false, "Failed to retrieve all requested layers!");
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
		CreateVulkanMemoryAllocator();
		VkCommandPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		poolInfo.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamily.value();
		VkResult poolStatus = vkCreateCommandPool(m_LogicalDevice, &poolInfo, nullptr, &m_ImmediateCommandPool);
		ATLAS_CORE_ASSERT(poolStatus == VK_SUCCESS, "Failed to create immediate command pool!");
		CreateSwapChain();
		CreateSyncObjects();
		VkSemaphoreCreateInfo imguiSemaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VkResult imguiSemaphoreStatus = vkCreateSemaphore(m_LogicalDevice, &imguiSemaphoreInfo, nullptr, &m_ImGuiRenderFinishedSemaphore);
		ATLAS_CORE_ASSERT(imguiSemaphoreStatus == VK_SUCCESS, "Failed to create ImGui render semaphore!");

		Reflect();
	}

	void VulkanContext::RenderImGui(ImDrawData* drawData)
	{
		if (!drawData || !m_LogicalDevice || !m_SwapChain)
			return;

		// The renderer submits and presents its frame before ImGui is built by the
		// current layer architecture. Wait for that submission before reusing the
		// acquire semaphore for the ImGui pass.
		vkDeviceWaitIdle(m_LogicalDevice);

		// SwapBuffers acquired the image before the engine render pass. The engine
		// submits that pass without presenting; ImGui is the final pass and owns
		// the single presentation for the frame.
		if (m_SwapChainImageIndex >= m_SwapChainImages.size())
			return;
		vkQueueWaitIdle(m_GraphicsQueue);

		VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = m_ImmediateCommandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		VkResult status = vkAllocateCommandBuffers(m_LogicalDevice, &allocateInfo, &commandBuffer);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to allocate ImGui command buffer!");

		VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		status = vkBeginCommandBuffer(commandBuffer, &beginInfo);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to begin ImGui command buffer!");

		VkImageMemoryBarrier toColor{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		toColor.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		toColor.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		toColor.oldLayout = m_SwapChainImageLayouts[m_SwapChainImageIndex];
		toColor.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		toColor.image = m_SwapChainImages[m_SwapChainImageIndex];
		toColor.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		toColor.subresourceRange.levelCount = 1;
		toColor.subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &toColor);
		m_SwapChainImageLayouts[m_SwapChainImageIndex] = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkRenderingAttachmentInfo colorAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		colorAttachment.imageView = m_SwapChainImageViews[m_SwapChainImageIndex];
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		VkRenderingInfo renderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
		renderingInfo.renderArea = { { 0, 0 }, m_SwapChainExtent };
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachment;
		vkCmdBeginRendering(commandBuffer, &renderingInfo);
		for (const auto& [textureID, imageInfo] : VulkanResources::s_Textures)
		{
			if (VulkanResources::GetImGuiTexture(textureID) == VK_NULL_HANDLE)
				VulkanResources::SetImGuiTexture(textureID, ImGui_ImplVulkan_AddTexture(imageInfo.sampler, imageInfo.imageView, imageInfo.imageLayout));
		}
		std::vector<std::pair<ImDrawCmd*, ImTextureID>> originalTextureIDs;
		for (int listIndex = 0; listIndex < drawData->CmdListsCount; ++listIndex)
		{
			ImDrawList* list = drawData->CmdLists[listIndex];
			for (ImDrawCmd& command : list->CmdBuffer)
			{
				uint32_t textureID = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(command.TextureId));
				VkDescriptorSet descriptorSet = VulkanResources::GetImGuiTexture(textureID);
				if (descriptorSet == VK_NULL_HANDLE && command.TextureId == nullptr)
					descriptorSet = reinterpret_cast<VkDescriptorSet>(ImGui::GetIO().Fonts->TexID);
				if (descriptorSet != VK_NULL_HANDLE)
				{
					originalTextureIDs.emplace_back(&command, command.TextureId);
					command.TextureId = reinterpret_cast<ImTextureID>(descriptorSet);
				}
			}
		}
		ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
		for (const auto& [command, textureID] : originalTextureIDs)
			command->TextureId = textureID;
		vkCmdEndRendering(commandBuffer);

		VkImageMemoryBarrier toPresent = toColor;
		toPresent.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		toPresent.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		toPresent.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		toPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &toPresent);
		m_SwapChainImageLayouts[m_SwapChainImageIndex] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		status = vkEndCommandBuffer(commandBuffer);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to end ImGui command buffer!");

		VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_ImGuiRenderFinishedSemaphore;
		status = vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to submit ImGui command buffer!");

		VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_ImGuiRenderFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.pImageIndices = &m_SwapChainImageIndex;
		status = vkQueuePresentKHR(m_PresentationQueue, &presentInfo);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS || status == VK_SUBOPTIMAL_KHR, "Failed to present ImGui frame!");
		vkQueueWaitIdle(m_GraphicsQueue);
		vkFreeCommandBuffers(m_LogicalDevice, m_ImmediateCommandPool, 1, &commandBuffer);
	}

	void VulkanContext::ConfigureRenderPass(const Ref<Shader>& shader)
	{
		for (auto framebuffer : m_SwapChainFramebuffers)
			vkDestroyFramebuffer(m_LogicalDevice, framebuffer, nullptr);
		m_SwapChainFramebuffers.clear();
		m_ConfiguredRenderPass = (VkRenderPass)shader->GetRenderPass();
		CreateSwapChainFramebuffers(m_ConfiguredRenderPass);
	}

	void VulkanContext::Resize(uint32_t width, uint32_t height)
	{
		if (!width || !height || !m_LogicalDevice)
			return;
		vkDeviceWaitIdle(m_LogicalDevice);
		for (auto framebuffer : m_SwapChainFramebuffers)
			vkDestroyFramebuffer(m_LogicalDevice, framebuffer, nullptr);
		for (auto imageView : m_SwapChainImageViews)
			vkDestroyImageView(m_LogicalDevice, imageView, nullptr);
		vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
		m_SwapChainFramebuffers.clear();
		m_SwapChainImageViews.clear();
		m_SwapChainImages.clear();
		CreateSwapChain();
		if (m_ConfiguredRenderPass)
			CreateSwapChainFramebuffers(m_ConfiguredRenderPass);
		ImGui_ImplVulkan_SetMinImageCount(std::max(2u, static_cast<uint32_t>(m_SwapChainImages.size())));
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
		VkResult status = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to created Vulkan instance!");
	}

	void VulkanContext::CreateSurface()
	{
		VkResult status = glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create window surface!");
	}

	void VulkanContext::SelectPhysicalDevice(const std::vector<const char*>& requiredExtensions)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			ATLAS_CORE_ASSERT(false, "Failed to find GPUs with Vulkan support!");
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
			ATLAS_CORE_ASSERT(false, "Failed to find a suitable GPU!");
		}

		FindQueueFamilies(m_PhysicalDevice, m_QueueFamilyIndices);
	}

	bool VulkanContext::IsDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions)
	{
		QueueFamilyIndices indices;
		FindQueueFamilies(device, indices);

		bool extensionsSupported = VerifyDeviceExtensionSupport(device, requiredExtensions);

		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport;
			QuerySwapChainSupport(device, swapChainSupport);
			swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentationModes.empty();
		}

		return indices.IsComplete() && extensionsSupported && swapChainAdequate;
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
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { m_QueueFamilyIndices.GraphicsFamily.value(), m_QueueFamilyIndices.PresentationFamily.value() };

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

		VkPhysicalDeviceVulkan12Features enabledVk12Features{};
		enabledVk12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		enabledVk12Features.descriptorIndexing = true;
		enabledVk12Features.shaderSampledImageArrayNonUniformIndexing = true;
		enabledVk12Features.descriptorBindingVariableDescriptorCount = true;
		enabledVk12Features.runtimeDescriptorArray = true;
		enabledVk12Features.bufferDeviceAddress = true;

		VkPhysicalDeviceVulkan13Features enabledVk13Features{};
		enabledVk13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		enabledVk13Features.pNext = &enabledVk12Features;
		enabledVk13Features.synchronization2 = true;
		enabledVk13Features.dynamicRendering = true;

		VkPhysicalDeviceFeatures enabledVk10Features{};
		enabledVk10Features.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &enabledVk13Features;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &enabledVk10Features;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		// Validation layers are instance-scoped and must not be passed to
		// VkDeviceCreateInfo.
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;

		VkResult status = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create logical device!");

		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.PresentationFamily.value(), 0, &m_PresentationQueue);
	}

	void VulkanContext::CreateVulkanMemoryAllocator()
	{
		VmaAllocatorCreateInfo allocatorCI{};
		allocatorCI.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		allocatorCI.physicalDevice = m_PhysicalDevice;
		allocatorCI.device = m_LogicalDevice;
		allocatorCI.instance = m_Instance;

		VkResult status = vmaCreateAllocator(&allocatorCI, &m_Allocator);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to VMA allocator!");
	}

	void VulkanContext::QuerySwapChainSupport(VkPhysicalDevice device, SwapChainSupportDetails& details)
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.Capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.Formats.data());
		}

		uint32_t presentationModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentationModeCount, nullptr);

		if (presentationModeCount != 0)
		{
			details.PresentationModes.resize(presentationModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentationModeCount, details.PresentationModes.data());
		}
	}

	VkSurfaceFormatKHR VulkanContext::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR VulkanContext::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanContext::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(m_WindowHandle, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	void VulkanContext::CreateSwapChain()
	{
		SwapChainSupportDetails swapChainSupport;
		QuerySwapChainSupport(m_PhysicalDevice, swapChainSupport);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentationModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;

		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.Capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { m_QueueFamilyIndices.GraphicsFamily.value(), m_QueueFamilyIndices.PresentationFamily.value() };

		if (m_QueueFamilyIndices.GraphicsFamily != m_QueueFamilyIndices.PresentationFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult status = vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &m_SwapChain);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create swap chain!");

		vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, m_SwapChainImages.data());

		m_SwapChainImageIndex = 0;
		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;

		CreateSwapChainImageViews();
	}

	void VulkanContext::CreateSwapChainImageViews()
	{
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkResult status = vkCreateImageView(m_LogicalDevice, &createInfo, nullptr, &m_SwapChainImageViews[i]);
			ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create image view!");
		}
	}

	void VulkanContext::CreateSwapChainFramebuffers(const VkRenderPass& renderPass)
	{
		m_SwapChainFramebuffers.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
			VkImageView attachments[] =
			{
				m_SwapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_SwapChainExtent.width;
			framebufferInfo.height = m_SwapChainExtent.height;
			framebufferInfo.layers = 1;

			VkResult status = vkCreateFramebuffer(m_LogicalDevice, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]);
			ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create framebuffer!");
		}
	}

	void VulkanContext::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkResult status = vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create imageAvailable semaphore!");
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

		VkResult result = vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &m_SwapChainImageIndex);
		ATLAS_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire a swap chain image!");

		//glfwSwapBuffers(m_WindowHandle);
	}

	VkCommandBuffer VulkanContext::BeginImmediateCommands() const
	{
		VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = m_ImmediateCommandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		VkResult status = vkAllocateCommandBuffers(m_LogicalDevice, &allocateInfo, &commandBuffer);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to allocate immediate command buffer!");
		VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		status = vkBeginCommandBuffer(commandBuffer, &beginInfo);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to begin immediate command buffer!");
		return commandBuffer;
	}

	void VulkanContext::EndImmediateCommands(VkCommandBuffer commandBuffer) const
	{
		VkResult status = vkEndCommandBuffer(commandBuffer);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to end immediate command buffer!");
		VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		status = vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to submit immediate command buffer!");
		vkQueueWaitIdle(m_GraphicsQueue);
		vkFreeCommandBuffers(m_LogicalDevice, m_ImmediateCommandPool, 1, &commandBuffer);
	}
}