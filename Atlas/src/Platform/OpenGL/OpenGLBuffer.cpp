#include "atlaspch.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

#include <glad/glad.h>

namespace Atlas
{
	////////////////////////////////////////////////////////////////////////////////////////
	// VertexBuffer ////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		Initialize(nullptr, size);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		Initialize(vertices, size);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		ATLAS_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::Initialize(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		glNamedBufferData(m_RendererID, size, data, data == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		glBindBufferBase(GL_ARRAY_BUFFER, 0, m_RendererID);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		glNamedBufferSubData(m_RendererID, 0, size, data);
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// IndexBuffer /////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size)
		: m_MaxCount(size / sizeof(uint32_t))
	{
		ATLAS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		Initialize(nullptr, size);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t size)
		: m_MaxCount(size / sizeof(uint32_t))
	{
		ATLAS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		Initialize(indices, size);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		ATLAS_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::Initialize(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		glNamedBufferData(m_RendererID, size, data, data == nullptr ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		glBindBufferBase(GL_ELEMENT_ARRAY_BUFFER, 0, m_RendererID);
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		glNamedBufferSubData(m_RendererID, 0, size, data);
	}
}