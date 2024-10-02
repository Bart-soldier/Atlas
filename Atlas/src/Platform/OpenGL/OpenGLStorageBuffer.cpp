#include "atlaspch.h"
#include "Platform/OpenGL/OpenGLStorageBuffer.h"

#include <glad/glad.h>

namespace Atlas
{
	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t size, uint32_t binding)
		: m_Binding(binding)
	{
		ATLAS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		SetSize(size);
	}

	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t binding)
		: m_Binding(binding)
	{
		ATLAS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
	}

	OpenGLStorageBuffer::~OpenGLStorageBuffer()
	{
		ATLAS_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLStorageBuffer::SetSize(uint32_t size)
	{
		ATLAS_PROFILE_FUNCTION();

		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_Binding, m_RendererID);
	}

	void OpenGLStorageBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		ATLAS_PROFILE_FUNCTION();

		glNamedBufferSubData(m_RendererID, offset, size, data);
	}
}