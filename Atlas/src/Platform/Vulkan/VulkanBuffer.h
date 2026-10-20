#pragma once

#include "Atlas/Renderer/Buffer.h"
#include <vma/vk_mem_alloc.h>

namespace Atlas
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(uint32_t size);
		VulkanVertexBuffer(const void* data, uint32_t size);
		virtual ~VulkanVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }

		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		VkBuffer GetBuffer() const { return m_RendererID; }
		uint32_t GetSize() const { return m_Size; }

	private:
		void Initialize(const void* data, uint32_t size);

		VkBuffer m_RendererID = VK_NULL_HANDLE;
		VmaAllocation m_Allocation = VK_NULL_HANDLE;
		uint32_t m_Size = 0;
		BufferLayout m_Layout;
	};

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t size);
		VulkanIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~VulkanIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual uint32_t GetMaxCount() const override { return m_MaxCount; }
		VkBuffer GetBuffer() const { return m_RendererID; }

	private:
		void Initialize(const void* data, uint32_t size);

		VkBuffer m_RendererID = VK_NULL_HANDLE;
		VmaAllocation m_Allocation = VK_NULL_HANDLE;
		uint32_t m_MaxCount;
	};
}
