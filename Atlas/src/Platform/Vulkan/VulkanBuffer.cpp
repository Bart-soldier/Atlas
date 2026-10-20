#include "atlaspch.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Atlas/Core/Application.h"

namespace Atlas
{
	////////////////////////////////////////////////////////////////////////////////////////
	// VertexBuffer ////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		//glCreateBuffers(1, &m_RendererID);
		Initialize(nullptr, size);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		//glCreateBuffers(1, &m_RendererID);
		Initialize(data, size);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		ATLAS_PROFILE_FUNCTION();

		if (m_RendererID)
			vmaDestroyBuffer(static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get())->GetAllocator(), m_RendererID, m_Allocation);
	}

	void VulkanVertexBuffer::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

	}

	void VulkanVertexBuffer::Unbind() const
	{
		ATLAS_PROFILE_FUNCTION();

	}

	void VulkanVertexBuffer::Initialize(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		m_Size = size;
		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		VmaAllocationInfo allocationResult{};
		VkResult status = vmaCreateBuffer(context->GetAllocator(), &bufferInfo, &allocationInfo, &m_RendererID, &m_Allocation, &allocationResult);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create Vulkan vertex buffer!");
		if (data)
			memcpy(allocationResult.pMappedData, data, size);
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		VmaAllocationInfo info{};
		vmaGetAllocationInfo(static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get())->GetAllocator(), m_Allocation, &info);
		memcpy(static_cast<uint8_t*>(info.pMappedData), data, size);
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// IndexBuffer /////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t size)
		: m_MaxCount(size / sizeof(uint32_t))
	{
		ATLAS_PROFILE_FUNCTION();

		//glCreateBuffers(1, &m_RendererID);
		Initialize(nullptr, size);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t size)
		: m_MaxCount(size / sizeof(uint32_t))
	{
		ATLAS_PROFILE_FUNCTION();

		//glCreateBuffers(1, &m_RendererID);
		Initialize(indices, size);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		ATLAS_PROFILE_FUNCTION();

		if (m_RendererID)
			vmaDestroyBuffer(static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get())->GetAllocator(), m_RendererID, m_Allocation);
	}

	void VulkanIndexBuffer::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

	}

	void VulkanIndexBuffer::Unbind() const
	{
		ATLAS_PROFILE_FUNCTION();

	}

	void VulkanIndexBuffer::Initialize(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		VulkanContext* context = static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get());
		VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		VmaAllocationInfo allocationResult{};
		VkResult status = vmaCreateBuffer(context->GetAllocator(), &bufferInfo, &allocationInfo, &m_RendererID, &m_Allocation, &allocationResult);
		ATLAS_CORE_ASSERT(status == VK_SUCCESS, "Failed to create Vulkan index buffer!");
		if (data)
			memcpy(allocationResult.pMappedData, data, size);
	}

	void VulkanIndexBuffer::SetData(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		VmaAllocationInfo info{};
		vmaGetAllocationInfo(static_cast<VulkanContext*>(Application::Get().GetGraphicsContext().get())->GetAllocator(), m_Allocation, &info);
		memcpy(static_cast<uint8_t*>(info.pMappedData), data, size);
	}
}