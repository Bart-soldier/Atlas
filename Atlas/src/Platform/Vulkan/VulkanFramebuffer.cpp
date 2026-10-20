#include "atlaspch.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanResources.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include <backends/imgui_impl_vulkan.h>
#include "Atlas/Core/Application.h"

namespace Atlas
{
	static const uint32_t s_MaxFrameBufferSize = 8192;

	namespace Utils
	{
		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24_STENCIL8:  return true;
			case FramebufferTextureFormat::DEPTH32F:          return true;
			}

			return false;
		}

		//static GLenum AtlasFBTextureFormatToGLAttachementType(FramebufferTextureFormat format)
		//{
		//	switch (format)
		//	{
		//	case FramebufferTextureFormat::DEPTH24_STENCIL8: return GL_DEPTH_STENCIL_ATTACHMENT;
		//	case FramebufferTextureFormat::DEPTH32F:         return GL_DEPTH_ATTACHMENT;
		//	}

		//	ATLAS_CORE_ASSERT(false, "Format not supported for depth/stencil framebuffers!");
		//	return 0;
		//}

		//static GLenum GLInternalFormatToGLType(GLenum format)
		//{
		//	switch (format)
		//	{
		//	case GL_RGBA8:
		//	case GL_RGBA16:
		//	case GL_R32I:    return GL_UNSIGNED_BYTE;
		//	case GL_R32F:
		//	case GL_RGBA16F: return GL_FLOAT;
		//	}

		//	ATLAS_CORE_ASSERT(false, "Format not supported for color framebuffers!");
		//	return 0;
		//}

		//static GLenum AtlasFBTextureFormatToGLDataFormat(FramebufferTextureFormat format)
		//{
		//	switch (format)
		//	{
		//	case FramebufferTextureFormat::RGBA8:
		//	case FramebufferTextureFormat::RGBA16:
		//	case FramebufferTextureFormat::RGBA16F:     return GL_RGBA;
		//	case FramebufferTextureFormat::RED32I:      return GL_RED_INTEGER;
		//	case FramebufferTextureFormat::RED32F:      return GL_RED;
		//	}

		//	ATLAS_CORE_ASSERT(false, "Format not supported for color framebuffers!");
		//	return 0;
		//}

		//static GLenum AtlasFBTextureFormatToGLInternalFormat(FramebufferTextureFormat format)
		//{
		//	switch (format)
		//	{
		//	case FramebufferTextureFormat::RGBA8:            return GL_RGBA8;
		//	case FramebufferTextureFormat::RGBA16:           return GL_RGBA16;
		//	case FramebufferTextureFormat::RGBA16F:          return GL_RGBA16F;
		//	case FramebufferTextureFormat::RED32I:           return GL_R32I;
		//	case FramebufferTextureFormat::RED32F:           return GL_R32F;
		//	case FramebufferTextureFormat::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
		//	case FramebufferTextureFormat::DEPTH32F:         return GL_DEPTH_COMPONENT32F;
		//	}

		//	ATLAS_CORE_ASSERT(false, "Format not supported for framebuffers!");
		//	return 0;
		//}

		//static GLenum TextureTarget(bool multisampled)
		//{
		//	return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		//}

		//static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		//{
		//	glCreateTextures(TextureTarget(multisampled), count, outID);
		//}

		//static void BindTexture(bool multisampled, uint32_t id)
		//{
		//	glBindTexture(TextureTarget(multisampled), id);
		//}

		//static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		//{
		//	bool multisampled = samples > 1;
		//	if (multisampled)
		//	{
		//		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		//	}
		//	else
		//	{
		//		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GLInternalFormatToGLType(internalFormat), nullptr);

		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//	}

		//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		//}

		//static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		//{
		//	bool multisampled = samples > 1;
		//	if (multisampled)
		//	{
		//		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		//	}
		//	else
		//	{
		//		glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//	}

		//	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		//}
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		Reset();
	}

	void VulkanFramebuffer::Invalidate()
	{
		if (!m_ColorImages.empty() || m_DepthImage != VK_NULL_HANDLE || m_ColorSampler != VK_NULL_HANDLE)
		{
			Reset();
		}

		//glCreateFramebuffers(1, &m_RendererID);
		//glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;
		if (m_RendererID == 0)
		{
			static uint32_t nextFramebufferID = 1;
			m_RendererID = nextFramebufferID++;
		}

		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
			VkSamplerCreateInfo samplerInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			VkResult samplerStatus = vkCreateSampler(context->GetDevice(), &samplerInfo, nullptr, &m_ColorSampler);
			ATLAS_CORE_ASSERT(samplerStatus == VK_SUCCESS, "Failed to create Vulkan framebuffer sampler!");
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			m_ColorImages.resize(m_ColorAttachments.size());
			m_ColorImageViews.resize(m_ColorAttachments.size());
			m_ColorAllocations.resize(m_ColorAttachments.size());
			m_ColorFormats.resize(m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8: format = VK_FORMAT_R8G8B8A8_UNORM; break;
				case FramebufferTextureFormat::RED32I: format = VK_FORMAT_R32_SINT; break;
				case FramebufferTextureFormat::RED32F: format = VK_FORMAT_R32_SFLOAT; break;
				default: break;
				}
				CreateAttachment(format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					m_ColorImages[i], m_ColorImageViews[i], m_ColorAllocations[i]);
				m_ColorFormats[i] = format;
				static uint32_t nextAttachmentID = 100000;
				m_ColorAttachments[i] = nextAttachmentID++;
				if (i == 0)
					VulkanResources::RegisterTexture(m_ColorAttachments[i], m_ColorImageViews[i], m_ColorSampler);
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			VkFormat format = m_DepthAttachmentSpecification.TextureFormat == FramebufferTextureFormat::DEPTH32F
				? VK_FORMAT_D32_SFLOAT : VK_FORMAT_D24_UNORM_S8_UINT;
			CreateAttachment(format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
				m_DepthImage, m_DepthImageView, m_DepthAllocation);
			m_DepthAttachment = 1;
		}

		EnableAllColorAttachments();

		//ATLAS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void VulkanFramebuffer::Bind()
	{
		if (s_Active && s_Active != this)
			VulkanRendererAPI::SwitchFramebuffer(this);
		s_Active = this;
	}

	void VulkanFramebuffer::Unbind()
	{
		if (s_Active == this)
			s_Active = nullptr;
	}

	void VulkanFramebuffer::SetColorImageLayout(VkImageLayout layout)
	{
		m_ColorImageLayout = layout;
		if (!m_ColorAttachments.empty())
			VulkanResources::SetTextureLayout(m_ColorAttachments[0], layout);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			ATLAS_CORE_WARN("Attempted to resize frame buffer to {0}, {1}", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int VulkanFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		ATLAS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());
		if (x < 0 || y < 0 || static_cast<uint32_t>(x) >= m_Specification.Width || static_cast<uint32_t>(y) >= m_Specification.Height)
			return -1;
		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		VkBuffer staging = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = sizeof(int32_t);
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		VmaAllocationInfo mapped{};
		ATLAS_CORE_ASSERT(vmaCreateBuffer(context->GetAllocator(), &bufferInfo, &allocationInfo, &staging, &allocation, &mapped) == VK_SUCCESS);
		VkCommandBuffer commandBuffer = context->BeginImmediateCommands();
		VkImageMemoryBarrier toTransfer{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		toTransfer.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		toTransfer.oldLayout = m_ColorImageLayout;
		toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		toTransfer.image = m_ColorImages[attachmentIndex];
		toTransfer.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		toTransfer.subresourceRange.levelCount = 1;
		toTransfer.subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &toTransfer);
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { x, y, 0 };
		region.imageExtent = { 1, 1, 1 };
		vkCmdCopyImageToBuffer(commandBuffer, m_ColorImages[attachmentIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, staging, 1, &region);
		VkImageMemoryBarrier restore = toTransfer;
		restore.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		restore.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		restore.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		restore.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &restore);
		context->EndImmediateCommands(commandBuffer);
		m_ColorImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		int32_t pixelData = *static_cast<int32_t*>(mapped.pMappedData);
		vmaDestroyBuffer(context->GetAllocator(), staging, allocation);
		return pixelData;
	}

	void VulkanFramebuffer::CopyColor(Ref<Framebuffer> framebuffer)
	{
		VulkanFramebuffer* source = dynamic_cast<VulkanFramebuffer*>(framebuffer.get());
		if (!source || source == this || source->m_ColorImages.empty() || m_ColorImages.empty())
			return;
		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		VkCommandBuffer commandBuffer = context->BeginImmediateCommands();
		VkImageMemoryBarrier barriers[2]{};
		barriers[0] = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barriers[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barriers[0].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barriers[0].oldLayout = source->m_ColorImageLayout;
		barriers[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barriers[0].image = source->m_ColorImages[0];
		barriers[1] = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barriers[1].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barriers[1].oldLayout = m_ColorImageLayout;
		barriers[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barriers[1].image = m_ColorImages[0];
		for (auto& barrier : barriers) { barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; barrier.subresourceRange.levelCount = 1; barrier.subresourceRange.layerCount = 1; }
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 2, barriers);
		VkImageCopy copy{};
		copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; copy.srcSubresource.layerCount = 1;
		copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; copy.dstSubresource.layerCount = 1;
		copy.extent = { std::min(source->m_Specification.Width, m_Specification.Width), std::min(source->m_Specification.Height, m_Specification.Height), 1 };
		vkCmdCopyImage(commandBuffer, source->m_ColorImages[0], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_ColorImages[0], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
		barriers[0].srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT; barriers[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT; barriers[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; barriers[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barriers[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; barriers[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT; barriers[1].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; barriers[1].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 2, barriers);
		context->EndImmediateCommands(commandBuffer);
		source->m_ColorImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_ColorImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	void VulkanFramebuffer::CopyDepth(Ref<Framebuffer> framebuffer)
	{
		VulkanFramebuffer* source = dynamic_cast<VulkanFramebuffer*>(framebuffer.get());
		if (!source || source == this || !source->m_DepthImage || !m_DepthImage)
			return;
		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		VkCommandBuffer commandBuffer = context->BeginImmediateCommands();
		VkImageMemoryBarrier barriers[2]{};
		barriers[0] = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barriers[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		barriers[0].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barriers[0].oldLayout = source->m_DepthImageLayout;
		barriers[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barriers[0].image = source->m_DepthImage;
		barriers[1] = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barriers[1].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barriers[1].oldLayout = m_DepthImageLayout;
		barriers[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barriers[1].image = m_DepthImage;
		for (auto& barrier : barriers) { barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT; barrier.subresourceRange.levelCount = 1; barrier.subresourceRange.layerCount = 1; }
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 2, barriers);
		VkImageCopy copy{};
		copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT; copy.srcSubresource.layerCount = 1;
		copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT; copy.dstSubresource.layerCount = 1;
		copy.extent = { std::min(source->m_Specification.Width, m_Specification.Width), std::min(source->m_Specification.Height, m_Specification.Height), 1 };
		vkCmdCopyImage(commandBuffer, source->m_DepthImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_DepthImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
		context->EndImmediateCommands(commandBuffer);
		source->m_DepthImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		m_DepthImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		ATLAS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		VkCommandBuffer commandBuffer = VulkanRendererAPI::GetActiveCommandBuffer();
		if (commandBuffer == VK_NULL_HANDLE)
			return;
		if (attachmentIndex != 0)
		{
			ATLAS_CORE_WARN("Vulkan ClearAttachment only supports the active color attachment until multi-attachment rendering is enabled");
			return;
		}
		VkClearAttachment attachment{};
		attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		attachment.colorAttachment = 0;
		attachment.clearValue.color.int32[0] = value;
		attachment.clearValue.color.int32[1] = value;
		attachment.clearValue.color.int32[2] = value;
		attachment.clearValue.color.int32[3] = value;
		VkClearRect clearRect{};
		clearRect.rect.extent = { m_Specification.Width, m_Specification.Height };
		clearRect.layerCount = 1;
		vkCmdClearAttachments(commandBuffer, 1, &attachment, 1, &clearRect);
	}

	void VulkanFramebuffer::EnableColorAttachment(uint32_t attachmentIndex)
	{
		ATLAS_CORE_ASSERT(m_ColorAttachments.size() >= 1);
		ATLAS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());
		m_ActiveColorAttachments = { attachmentIndex };
	}

	void VulkanFramebuffer::EnableColorAttachments(std::initializer_list<uint32_t> attachmentIndices)
	{
		ATLAS_CORE_ASSERT(attachmentIndices.size() <= m_ColorAttachments.size());

		if (attachmentIndices.size() >= 1)
		{
			//ATLAS_CORE_ASSERT(m_ColorAttachments.size() <= 8);

			//std::vector<GLenum> buffers;
			//buffers.reserve(8);

			//for (uint32_t index : attachmentIndices)
			//{
			//	ATLAS_CORE_ASSERT(index < 8);
			//	buffers.push_back(GL_COLOR_ATTACHMENT0 + index);
			//}

			//glDrawBuffers(buffers.size(), buffers.data());
		}
		else
		{
			// Only depth-pass
			//glDrawBuffer(GL_NONE);
		}
	}

	void VulkanFramebuffer::EnableAllColorAttachments()
	{
		if (m_ColorAttachments.size() > 1)
		{
			//ATLAS_CORE_ASSERT(m_ColorAttachments.size() <= 8);
			//GLenum buffers[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
			//					  GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
			//glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			//glDrawBuffer(GL_NONE);
		}
	}

	void VulkanFramebuffer::Reset()
	{
		if (s_Active == this)
			s_Active = nullptr;
		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		if (!m_ColorAttachments.empty())
		{
			VkDescriptorSet descriptorSet = VulkanResources::GetImGuiTexture(m_ColorAttachments[0]);
			if (descriptorSet != VK_NULL_HANDLE)
				ImGui_ImplVulkan_RemoveTexture(descriptorSet);
			VulkanResources::RemoveTexture(m_ColorAttachments[0]);
		}
		for (auto view : m_ColorImageViews)
			if (view) vkDestroyImageView(context->GetDevice(), view, nullptr);
		for (size_t i = 0; i < m_ColorImages.size(); ++i)
			if (m_ColorImages[i]) vmaDestroyImage(context->GetAllocator(), m_ColorImages[i], m_ColorAllocations[i]);
		if (m_DepthImageView) vkDestroyImageView(context->GetDevice(), m_DepthImageView, nullptr);
		if (m_DepthImage) vmaDestroyImage(context->GetAllocator(), m_DepthImage, m_DepthAllocation);
		if (m_ColorSampler) vkDestroySampler(context->GetDevice(), m_ColorSampler, nullptr);

		m_ColorAttachments.clear();
		m_DepthAttachment = 0;
		m_ColorImages.clear();
		m_ColorImageViews.clear();
		m_ColorAllocations.clear();
		m_ColorFormats.clear();
		m_ActiveColorAttachments.clear();
		m_DepthImage = VK_NULL_HANDLE;
		m_DepthImageView = VK_NULL_HANDLE;
		m_DepthAllocation = VK_NULL_HANDLE;
		m_ColorSampler = VK_NULL_HANDLE;
		m_ColorImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_DepthImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	}

	void VulkanFramebuffer::CreateAttachment(VkFormat format, VkImageUsageFlags usage, VkImage& image, VkImageView& view, VmaAllocation& allocation)
	{
		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = format;
		imageInfo.extent = { m_Specification.Width, m_Specification.Height, 1 };
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = static_cast<VkSampleCountFlagBits>(m_Specification.Samples > 1 ? VK_SAMPLE_COUNT_1_BIT : VK_SAMPLE_COUNT_1_BIT);
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = usage;
		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		VkResult status = vmaCreateImage(context->GetAllocator(), &imageInfo, &allocationInfo, &image, &allocation, nullptr);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create Vulkan framebuffer attachment!");
		VkImageViewCreateInfo viewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
			? (format == VK_FORMAT_D24_UNORM_S8_UINT
				? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
				: VK_IMAGE_ASPECT_DEPTH_BIT)
			: VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		status = vkCreateImageView(context->GetDevice(), &viewInfo, nullptr, &view);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create Vulkan framebuffer attachment view!");
	}
}