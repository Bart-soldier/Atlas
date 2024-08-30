#include "atlaspch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Atlas
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Atlas::ShaderDataType::Float:    return GL_FLOAT;
			case Atlas::ShaderDataType::Float2:   return GL_FLOAT;
			case Atlas::ShaderDataType::Float3:   return GL_FLOAT;
			case Atlas::ShaderDataType::Float4:   return GL_FLOAT;
			case Atlas::ShaderDataType::Mat3:     return GL_FLOAT;
			case Atlas::ShaderDataType::Mat4:     return GL_FLOAT;
			case Atlas::ShaderDataType::Int:      return GL_INT;
			case Atlas::ShaderDataType::Int2:     return GL_INT;
			case Atlas::ShaderDataType::Int3:     return GL_INT;
			case Atlas::ShaderDataType::Int4:     return GL_INT;
			case Atlas::ShaderDataType::Bool:     return GL_BOOL;
		}

		ATLAS_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		ATLAS_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		ATLAS_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		ATLAS_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		ATLAS_PROFILE_FUNCTION();

		ATLAS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");
		
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			m_VertexBufferIndex++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		ATLAS_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}