#include "atlaspch.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Atlas/Core/Application.h"
#include "Platform/Vulkan/VulkanResources.h"

namespace Atlas
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding)
	{
		m_Size = size;
		VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		VkResult status = vmaCreateBuffer(static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get())->GetAllocator(), &bufferInfo, &allocationInfo, &m_RendererID, &m_Allocation, nullptr);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create Vulkan uniform buffer!");
		VulkanResources::RegisterBuffer(binding, m_RendererID, size);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		if (m_RendererID)
			vmaDestroyBuffer(static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get())->GetAllocator(), m_RendererID, m_Allocation);
	}

	void VulkanUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		VmaAllocationInfo info{};
		VmaAllocator allocator = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get())->GetAllocator();
		vmaGetAllocationInfo(allocator, m_Allocation, &info);
		memcpy(static_cast<uint8_t*>(info.pMappedData) + offset, data, size);
	}
}