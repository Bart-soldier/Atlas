#pragma once

#include "Atlas/Renderer/Buffer.h"

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

	private:
		void Initialize(const void* data, uint32_t size);

		uint32_t m_RendererID;
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

	private:
		void Initialize(const void* data, uint32_t size);

		uint32_t m_RendererID;
		uint32_t m_MaxCount;
	};
}
