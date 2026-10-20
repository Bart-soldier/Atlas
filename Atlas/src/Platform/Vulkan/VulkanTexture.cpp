#include "atlaspch.h"
#include "Platform/Vulkan/VulkanTexture.h"

#include <stb_image.h>
#include "Platform/Vulkan/VulkanContext.h"
#include "Atlas/Core/Application.h"
#include "Platform/Vulkan/VulkanResources.h"

namespace Atlas
{
	namespace Utils
	{
		/*static GLenum AtlasImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::R8:      return GL_RED;

			case ImageFormat::RG16F:   return GL_RG;

			case ImageFormat::RGB16F:
			case ImageFormat::RGB32F:
			case ImageFormat::RGB8:    return GL_RGB;

			case ImageFormat::RGBA8:
			case ImageFormat::RGBA16F:
			case ImageFormat::RGBA32F: return GL_RGBA;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}

		static GLenum AtlasImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::R8:      return GL_R8;

			case ImageFormat::RG16F:   return GL_RG16F;

			case ImageFormat::RGB8:    return GL_RGB8;
			case ImageFormat::RGB16F:  return GL_RGB16F;
			case ImageFormat::RGB32F:  return GL_RGB32F;

			case ImageFormat::RGBA8:   return GL_RGBA8;
			case ImageFormat::RGBA16F: return GL_RGBA16F;
			case ImageFormat::RGBA32F: return GL_RGBA32F;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}

		static GLint AtlasResizeFilterToGLInt(ResizeFilter filter)
		{
			switch (filter)
			{
			case Atlas::ResizeFilter::Linear:  return GL_LINEAR;
			case Atlas::ResizeFilter::Nearest: return GL_NEAREST;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}

		static GLint AtlasWrapToGLInt(Wrap wrap)
		{
			switch (wrap)
			{
			case Atlas::Wrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
			case Atlas::Wrap::Repeat:      return GL_REPEAT;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}*/
	}

	VulkanTexture2D::VulkanTexture2D(const TextureSpecification& specification)
		: m_Specification(specification)
	{
		CreateTextureStorage(m_Specification);
	}

	VulkanTexture2D::VulkanTexture2D(const std::filesystem::path& path, const bool generateMips, const bool flipOnLoad)
		: m_Path(path)
	{
		ATLAS_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(flipOnLoad);
		stbi_uc* data = nullptr;
		{
			ATLAS_PROFILE_SCOPE("stbi_load - VulkanTexture2D::VulkanTexture2D(const std::filesystem::path&) ");

			data = stbi_load(path.string().c_str(), &width, &height, &channels, 4);
		}

		if (data)
		{
			m_IsLoaded = true;

			m_Specification = TextureSpecification();
			m_Specification.Width = width;
			m_Specification.Height = height;
			m_Specification.GenerateMips = generateMips;

			if (channels == 4)
			{
				m_Specification.Format = ImageFormat::RGBA8;
			}
			else if (channels == 3)
			{
				if (path.extension() == ".hdr")
				{
					m_Specification.Format = ImageFormat::RGB16F;
				}
				else
				{
					m_Specification.Format = ImageFormat::RGB8;
				}
			}
			else if (channels == 1)
			{
				m_Specification.Format = ImageFormat::R8;
			}
			else
			{
				m_Specification.Format = ImageFormat::None;
				ATLAS_CORE_ASSERT(false, "Format not supported!");
			}

			CreateTextureStorage(m_Specification);
			SetData(data, static_cast<uint32_t>(m_Specification.Width * m_Specification.Height * 4));

			stbi_image_free(data);
		}
		else
		{
			ATLAS_CORE_WARN("Could not load texture {0}", path.string());
		}
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		ATLAS_PROFILE_FUNCTION();

		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		if (m_Sampler)
			vkDestroySampler(context->GetDevice(), m_Sampler, nullptr);
		if (m_ImageView)
			vkDestroyImageView(context->GetDevice(), m_ImageView, nullptr);
		if (m_Image)
			vmaDestroyImage(context->GetAllocator(), m_Image, m_Allocation);
	}

	void VulkanTexture2D::SetData(void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		Upload(data, size);
		m_IsLoaded = true;
	}

	void VulkanTexture2D::Bind(uint32_t slot) const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindTextureUnit(slot, m_RendererID);
	}

	void VulkanTexture2D::CreateTextureStorage(const TextureSpecification& specification)
	{
		ATLAS_PROFILE_FUNCTION();

		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
		if (specification.Format == ImageFormat::R8 || specification.Format == ImageFormat::RGB8) format = VK_FORMAT_R8G8B8A8_UNORM;
		else if (specification.Format == ImageFormat::RGBA16F || specification.Format == ImageFormat::RGB16F) format = VK_FORMAT_R16G16B16A16_SFLOAT;
		VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = format;
		imageInfo.extent = { specification.Width, specification.Height, 1 };
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		VkResult status = vmaCreateImage(context->GetAllocator(), &imageInfo, &allocationInfo, &m_Image, &m_Allocation, nullptr);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create Vulkan texture image!");
		VkImageViewCreateInfo viewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		status = vkCreateImageView(context->GetDevice(), &viewInfo, nullptr, &m_ImageView);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create Vulkan texture view!");
		VkSamplerCreateInfo samplerInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.maxLod = 1.0f;
		status = vkCreateSampler(context->GetDevice(), &samplerInfo, nullptr, &m_Sampler);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create Vulkan texture sampler!");
		static uint32_t nextID = 1;
		m_RendererID = nextID++;
		VulkanResources::RegisterTexture(m_RendererID, m_ImageView, m_Sampler);
	}

	void VulkanTexture2D::Upload(void* data, uint32_t size)
	{
		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		VkBuffer staging = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		VmaAllocationInfo mapped{};
		VkResult status = vmaCreateBuffer(context->GetAllocator(), &bufferInfo, &allocationInfo, &staging, &allocation, &mapped);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create texture staging buffer!");
		memcpy(mapped.pMappedData, data, size);
		VkCommandBuffer commandBuffer = context->BeginImmediateCommands();
		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.image = m_Image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		VkBufferImageCopy region{};
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount = 1;
		region.imageExtent = { m_Specification.Width, m_Specification.Height, 1 };
		vkCmdCopyBufferToImage(commandBuffer, staging, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		context->EndImmediateCommands(commandBuffer);
		vmaDestroyBuffer(context->GetAllocator(), staging, allocation);
	}
}