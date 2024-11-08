#include "atlaspch.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Atlas
{
	/*static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::UInt:     return GL_UNSIGNED_INT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}

		ATLAS_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}*/

	VulkanVertexArray::VulkanVertexArray()
	{
		ATLAS_PROFILE_FUNCTION();

		//glCreateVertexArrays(1, &m_RendererID);
	}

	VulkanVertexArray::~VulkanVertexArray()
	{
		ATLAS_PROFILE_FUNCTION();

		//glDeleteVertexArrays(1, &m_RendererID);
	}

	void VulkanVertexArray::Bind() const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindVertexArray(m_RendererID);
	}

	void VulkanVertexArray::Unbind() const
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindVertexArray(0);
	}

	void VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		ATLAS_PROFILE_FUNCTION();

		ATLAS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

		//glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				//glEnableVertexAttribArray(m_VertexBufferIndex);
				//glVertexAttribPointer(m_VertexBufferIndex,
				//	element.GetComponentCount(),
				//	ShaderDataTypeToOpenGLBaseType(element.Type),
				//	element.Normalized ? GL_TRUE : GL_FALSE,
				//	layout.GetStride(),
				//	(const void*)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::UInt:
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				//glEnableVertexAttribArray(m_VertexBufferIndex);
				//glVertexAttribIPointer(m_VertexBufferIndex,
				//	element.GetComponentCount(),
				//	ShaderDataTypeToOpenGLBaseType(element.Type),
				//	layout.GetStride(),
				//	(const void*)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					//glEnableVertexAttribArray(m_VertexBufferIndex);
					//glVertexAttribPointer(m_VertexBufferIndex,
					//	count,
					//	ShaderDataTypeToOpenGLBaseType(element.Type),
					//	element.Normalized ? GL_TRUE : GL_FALSE,
					//	layout.GetStride(),
					//	(const void*)(element.Offset + sizeof(float) * count * i));
					//glVertexAttribDivisor(m_VertexBufferIndex, 1);
					m_VertexBufferIndex++;
				}
				break;
			}
			default:
				ATLAS_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		ATLAS_PROFILE_FUNCTION();

		//glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}