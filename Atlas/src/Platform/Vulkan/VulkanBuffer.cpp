#include "atlaspch.h"
#include "Platform/Vulkan/VulkanBuffer.h"

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

		//glDeleteBuffers(1, &m_RendererID);
	}

	void VulkanVertexBuffer::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void VulkanVertexBuffer::Unbind() const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VulkanVertexBuffer::Initialize(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		//glNamedBufferData(m_RendererID, size, data, data == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		//glNamedBufferSubData(m_RendererID, 0, size, data);
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

		//glDeleteBuffers(1, &m_RendererID);
	}

	void VulkanIndexBuffer::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void VulkanIndexBuffer::Unbind() const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void VulkanIndexBuffer::Initialize(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		//glNamedBufferData(m_RendererID, size, data, data == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	void VulkanIndexBuffer::SetData(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		//glNamedBufferSubData(m_RendererID, 0, size, data);
	}
}