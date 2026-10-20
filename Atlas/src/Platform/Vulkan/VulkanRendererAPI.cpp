#include "atlaspch.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Renderer/GraphicsContext.h"
#include "Platform/Vulkan/VulkanResources.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Atlas
{
	namespace Utils
	{
		//static GLenum PolygonModeToGLenum(const RendererAPI::PolygonMode& mode)
		//{
		//	switch (mode)
		//	{
		//	case RendererAPI::PolygonMode::Fill:  return GL_FILL;
		//	case RendererAPI::PolygonMode::Line:  return GL_LINE;
		//	case RendererAPI::PolygonMode::Point: return GL_POINT;
		//	}

		//	ATLAS_CORE_ASSERT(false, "Unknown polygon mode!");
		//	return 0;
		//}

		//static GLenum TestFunctionToGLenum(const RendererAPI::TestFunction& function)
		//{
		//	switch (function)
		//	{
		//	case RendererAPI::TestFunction::Always:   return GL_ALWAYS;
		//	case RendererAPI::TestFunction::Never:    return GL_NEVER;
		//	case RendererAPI::TestFunction::Less:     return GL_LESS;
		//	case RendererAPI::TestFunction::Equal:    return GL_EQUAL;
		//	case RendererAPI::TestFunction::LEqual:   return GL_LEQUAL;
		//	case RendererAPI::TestFunction::Greater:  return GL_GREATER;
		//	case RendererAPI::TestFunction::NotEqual: return GL_NOTEQUAL;
		//	case RendererAPI::TestFunction::GEqual:   return GL_GEQUAL;
		//	}

		//	ATLAS_CORE_ASSERT(false);
		//	return 0;
		//}

		//static GLenum StencilOperationToGLenum(const RendererAPI::StencilOperation& operation)
		//{
		//	switch (operation)
		//	{
		//	case RendererAPI::StencilOperation::Keep:          return GL_KEEP;
		//	case RendererAPI::StencilOperation::Zero:          return GL_ZERO;
		//	case RendererAPI::StencilOperation::Replace:       return GL_REPLACE;
		//	case RendererAPI::StencilOperation::Increase:      return GL_INCR;
		//	case RendererAPI::StencilOperation::Increase_Wrap: return GL_INCR_WRAP;
		//	case RendererAPI::StencilOperation::Decrease:      return GL_DECR;
		//	case RendererAPI::StencilOperation::Decrease_Wrap: return GL_DECR_WRAP;
		//	case RendererAPI::StencilOperation::Invert:        return GL_INVERT;
		//	}

		//	ATLAS_CORE_ASSERT(false);
		//	return 0;
		//}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   ATLAS_CORE_ERROR(pCallbackData->pMessage); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: ATLAS_CORE_WARN (pCallbackData->pMessage); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    ATLAS_CORE_INFO (pCallbackData->pMessage); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: ATLAS_CORE_TRACE(pCallbackData->pMessage); break;

		default: ATLAS_CORE_ASSERT(false, "Unknown severity level!"); break;
		}

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			ATLAS_CORE_ERROR("Failed to set up debug messenger!");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	void VulkanRendererAPI::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		CreateDebugMessenger();
		CreateCommandPool();
		CreateCommandBuffer();
		CreateSyncObjects();

//
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//		glEnable(GL_DEPTH_TEST);
//		glDepthFunc(GL_LEQUAL);
//
//		glEnable(GL_STENCIL_TEST);
//		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//		glStencilMask(0x00);
//
//		glEnable(GL_LINE_SMOOTH);
//
//		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void VulkanRendererAPI::CreateDebugMessenger()
	{
#ifdef ATLAS_DEBUG
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = VulkanMessageCallback;

		Application& app = Application::Get();
		VkInstance instance = (VkInstance)app.GetGraphicsContext()->GetInstance();

		CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &m_DebugMessenger);
#endif
	}

	void VulkanRendererAPI::CreateCommandPool()
	{
		Application& app = Application::Get();
		VkDevice device = (VkDevice)app.GetGraphicsContext()->GetLogicalDevice();

		const GraphicsContext::QueueFamilyIndices& queueFamilyIndices = app.GetGraphicsContext()->GetQueueFamilyIndices();

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

		VkResult status = vkCreateCommandPool(device, &poolInfo, nullptr, &m_CommandPool);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create command pool!");
	}

	void VulkanRendererAPI::CreateCommandBuffer()
	{
		Application& app = Application::Get();
		VkDevice device = (VkDevice)app.GetGraphicsContext()->GetLogicalDevice();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VkResult status = vkAllocateCommandBuffers(device, &allocInfo, &m_CommandBuffer);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to allocate command buffers!");
	}

	void VulkanRendererAPI::CreateSyncObjects()
	{
		Application& app = Application::Get();
		VkDevice device = (VkDevice)app.GetGraphicsContext()->GetLogicalDevice();

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkResult status = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create renderFinished semaphore!");

		status = vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFence);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create frames in flight fence!");
	}

	void VulkanRendererAPI::BeginCommandBufferRecording()
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		VkResult status = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to begin command buffer recording!");
	}

	void VulkanRendererAPI::EndCommandBufferRecording()
	{
		VkResult status = vkEndCommandBuffer(m_CommandBuffer);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to end command buffer recording!");
	}

	void VulkanRendererAPI::SubmitCommandBuffer()
	{
		Application& app = Application::Get();
		VkQueue graphicsQueue = (VkQueue)app.GetGraphicsContext()->GetGraphicsQueue();
		VkSemaphore imageAvailableSempahore = (VkSemaphore)app.GetGraphicsContext()->GetImageAvailableSemaphore();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSempahore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;

		VkResult status = vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_InFlightFence);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to submit draw command buffer!");
	}

	void VulkanRendererAPI::Shutdown()
	{
		Application& app = Application::Get();
		VkDevice device = (VkDevice)app.GetGraphicsContext()->GetLogicalDevice();

		vkDeviceWaitIdle(device);

		vkDestroySemaphore(device, m_RenderFinishedSemaphore, nullptr);
		vkDestroyFence(device, m_InFlightFence, nullptr);

		vkDestroyCommandPool(device, m_CommandPool, nullptr);

#ifdef ATLAS_DEBUG
		VkInstance instance = (VkInstance)app.GetGraphicsContext()->GetInstance();

		DestroyDebugUtilsMessengerEXT(instance, m_DebugMessenger, nullptr);
#endif
	}

	void VulkanRendererAPI::BeginRenderPass(const Ref<Shader>& shader)
	{
		s_Instance = this;
		m_ActiveShader = shader;
		Application& app = Application::Get();
		VkDevice device = (VkDevice)app.GetGraphicsContext()->GetLogicalDevice();
		VkExtent2D swapChainExtent = { app.GetGraphicsContext()->GetSwapChainExtentWidth(), app.GetGraphicsContext()->GetSwapChainExtentHeight() };

		vkWaitForFences(device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(device, 1, &m_InFlightFence);

		app.GetGraphicsContext()->SwapBuffers();

		vkResetCommandBuffer(m_CommandBuffer, 0);
		BeginCommandBufferRecording();
		s_ActiveCommandBuffer = m_CommandBuffer;

		VulkanFramebuffer* framebuffer = VulkanFramebuffer::GetActive();
		ATLAS_CORE_ASSERT(framebuffer, "No active Vulkan framebuffer!");
		m_RenderingFramebuffer = framebuffer;
		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.srcAccessMask = framebuffer->GetColorImageLayout() == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			? VK_ACCESS_SHADER_READ_BIT : 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.oldLayout = framebuffer->GetColorImageLayout();
		barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.image = framebuffer->GetColorImage();
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;
		VkPipelineStageFlags colorSourceStage = barrier.oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		vkCmdPipelineBarrier(m_CommandBuffer, colorSourceStage, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		framebuffer->SetColorImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		VkImageMemoryBarrier depthBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		depthBarrier.srcAccessMask = framebuffer->GetDepthImageLayout() == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : 0;
		depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		depthBarrier.oldLayout = framebuffer->GetDepthImageLayout();
		depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthBarrier.image = framebuffer->GetDepthImage();
		depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		depthBarrier.subresourceRange.levelCount = 1;
		depthBarrier.subresourceRange.layerCount = 1;
		if (depthBarrier.image != VK_NULL_HANDLE)
		{
			VkPipelineStageFlags depthSourceStage = depthBarrier.oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ? VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			vkCmdPipelineBarrier(m_CommandBuffer, depthSourceStage, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, 1, &depthBarrier);
			framebuffer->SetDepthImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		}

		VkRenderingAttachmentInfo colorAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		colorAttachment.imageView = framebuffer->GetColorImageView();
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.clearValue = m_ClearColor;
		VkRenderingInfo renderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
		renderingInfo.renderArea = { { 0, 0 }, { framebuffer->GetSpecification().Width, framebuffer->GetSpecification().Height } };
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachment;
		VkRenderingAttachmentInfo depthAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		depthAttachment.imageView = framebuffer->GetDepthImageView();
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.clearValue.depthStencil = { 1.0f, 0 };
		if (depthAttachment.imageView != VK_NULL_HANDLE)
			renderingInfo.pDepthAttachment = &depthAttachment;
		vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);

		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipeline)shader->GetGraphicsPipeline());
		if (auto vulkanShader = dynamic_cast<VulkanShader*>(shader.get()))
			vulkanShader->BindResources(m_CommandBuffer);

		SetViewport(0, 0, framebuffer->GetSpecification().Width, framebuffer->GetSpecification().Height);

		// TODO: Create method
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { framebuffer->GetSpecification().Width, framebuffer->GetSpecification().Height };
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
	}

	void VulkanRendererAPI::SwitchFramebuffer(VulkanFramebuffer* framebuffer)
	{
		if (!s_Instance || !s_Instance->m_RenderingFramebuffer || !framebuffer || s_Instance->m_RenderingFramebuffer == framebuffer)
			return;

		vkCmdEndRendering(s_ActiveCommandBuffer);
		s_Instance->EndDynamicRendering(s_Instance->m_RenderingFramebuffer);
		s_Instance->BeginDynamicRendering(framebuffer);
		s_Instance->m_RenderingFramebuffer = framebuffer;
	}

	void VulkanRendererAPI::EndDynamicRendering(VulkanFramebuffer* framebuffer)
	{
		if (!framebuffer)
			return;
		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.image = framebuffer->GetColorImage();
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(m_CommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		framebuffer->SetColorImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void VulkanRendererAPI::BeginDynamicRendering(VulkanFramebuffer* framebuffer)
	{
		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.srcAccessMask = framebuffer->GetColorImageLayout() == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ? VK_ACCESS_SHADER_READ_BIT : 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.oldLayout = framebuffer->GetColorImageLayout();
		barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.image = framebuffer->GetColorImage();
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;
		VkPipelineStageFlags sourceStage = barrier.oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		vkCmdPipelineBarrier(m_CommandBuffer, sourceStage, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		framebuffer->SetColorImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkImageMemoryBarrier depthBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		depthBarrier.srcAccessMask = framebuffer->GetDepthImageLayout() == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : 0;
		depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		depthBarrier.oldLayout = framebuffer->GetDepthImageLayout();
		depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthBarrier.image = framebuffer->GetDepthImage();
		depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		depthBarrier.subresourceRange.levelCount = 1;
		depthBarrier.subresourceRange.layerCount = 1;
		if (depthBarrier.image != VK_NULL_HANDLE)
		{
			VkPipelineStageFlags depthSourceStage = depthBarrier.oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ? VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			vkCmdPipelineBarrier(m_CommandBuffer, depthSourceStage, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, 1, &depthBarrier);
			framebuffer->SetDepthImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		}

		VkRenderingAttachmentInfo color{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		color.imageView = framebuffer->GetColorImageView();
		color.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color.clearValue = m_ClearColor;
		VkRenderingInfo info{ VK_STRUCTURE_TYPE_RENDERING_INFO };
		info.renderArea = { { 0, 0 }, { framebuffer->GetSpecification().Width, framebuffer->GetSpecification().Height } };
		info.layerCount = 1;
		info.colorAttachmentCount = 1;
		info.pColorAttachments = &color;
		VkRenderingAttachmentInfo depth{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		depth.imageView = framebuffer->GetDepthImageView();
		depth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depth.clearValue.depthStencil = { 1.0f, 0 };
		if (depth.imageView != VK_NULL_HANDLE)
			info.pDepthAttachment = &depth;
		vkCmdBeginRendering(m_CommandBuffer, &info);
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipeline)m_ActiveShader->GetGraphicsPipeline());
		if (auto vulkanShader = dynamic_cast<VulkanShader*>(m_ActiveShader.get()))
			vulkanShader->BindResources(m_CommandBuffer);
		SetViewport(0, 0, framebuffer->GetSpecification().Width, framebuffer->GetSpecification().Height);
	}

	void VulkanRendererAPI::EndRenderPass()
	{
		vkCmdEndRendering(m_CommandBuffer);
		VulkanFramebuffer* framebuffer = VulkanFramebuffer::GetActive();
		if (framebuffer)
		{
			VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.image = framebuffer->GetColorImage();
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.layerCount = 1;
			vkCmdPipelineBarrier(m_CommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
			framebuffer->SetColorImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		EndCommandBufferRecording();
		s_ActiveCommandBuffer = VK_NULL_HANDLE;
		
		SubmitCommandBuffer();
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		VkViewport viewport{};
		viewport.x = x;
		viewport.y = y;
		viewport.width = static_cast<float>(width);
		viewport.height = static_cast<float>(height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
	}

	void VulkanRendererAPI::EnableDepthTest()
	{
		//glEnable(GL_DEPTH_TEST);
	}

	void VulkanRendererAPI::DisableDepthTest()
	{
		//glDisable(GL_DEPTH_TEST);
	}

	void VulkanRendererAPI::SetStencilMask(const uint32_t& mask)
	{
		//glStencilMask(mask);
	}

	void VulkanRendererAPI::SetStencilFunction(const TestFunction& function, const int& reference, const uint32_t& mask)
	{
		//glStencilFunc(Utils::TestFunctionToGLenum(function), reference, mask);
	}

	void VulkanRendererAPI::EnableCulling()
	{
		//glEnable(GL_CULL_FACE);
	}

	void VulkanRendererAPI::DisableCulling()
	{
		//glDisable(GL_CULL_FACE);
	}

	void VulkanRendererAPI::SetBackCulling()
	{
		//glCullFace(GL_BACK);
	}

	void VulkanRendererAPI::SetFrontCulling()
	{
		//glCullFace(GL_FRONT);
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		m_ClearColor.color = { color.r, color.g, color.b, color.a };
	}

	void VulkanRendererAPI::Clear()
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void VulkanRendererAPI::ClearColor()
	{
		//glClear(GL_COLOR_BUFFER_BIT);
	}

	void VulkanRendererAPI::ClearDepth()
	{
		//glClear(GL_DEPTH_BUFFER_BIT);
	}

	void VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		vkCmdDrawIndexed(m_CommandBuffer, indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetMaxCount(), 1, 0, 0, 0);
		//glDrawElements(GL_TRIANGLES, indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetMaxCount(), GL_UNSIGNED_INT, nullptr);
	}

	void VulkanRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		vkCmdDraw(m_CommandBuffer, vertexCount, 1, 0, 0);
	}

	void VulkanRendererAPI::BindTextureSlot(uint32_t slot, uint32_t rendererID)
	{
		VulkanResources::SetTextureSlot(slot, rendererID);
	}

	void VulkanRendererAPI::SetPointSize(const float& size)
	{
		//glPointSize(size);
	}

	void VulkanRendererAPI::SetLineWidth(const float& width)
	{
		//glLineWidth(width);
	}

	void VulkanRendererAPI::SetPolygonMode(const PolygonMode& mode)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, Utils::PolygonModeToGLenum(mode));
	}
}