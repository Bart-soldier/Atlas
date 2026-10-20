#pragma once

#include "Atlas/Renderer/StorageBuffer.h"
#include <vma/vk_mem_alloc.h>

namespace Atlas
{
	class VulkanStorageBuffer : public StorageBuffer
	{
	public:
		VulkanStorageBuffer(uint32_t size, uint32_t binding);
		VulkanStorageBuffer(uint32_t binding);
		virtual ~VulkanStorageBuffer();

		virtual void SetSize(uint32_t size) override;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		VkBuffer GetBuffer() const { return m_RendererID; }
		uint32_t GetSize() const { return m_Size; }

	private:
		VkBuffer m_RendererID = VK_NULL_HANDLE;
		VmaAllocation m_Allocation = VK_NULL_HANDLE;
		uint32_t m_Binding = 0;
		uint32_t m_Size = 0;
	};
}