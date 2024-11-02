#include "atlaspch.h"

#include "Atlas/Renderer/Buffer.h"
#include "Atlas/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/Vulkan/VulkanBuffer.h"

namespace Atlas
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanVertexBuffer>(size);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size)
	{
		switch (RenderCommand::GetAPI())
		{
			case RendererAPI::API::None:
				ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(data, size);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanVertexBuffer>(data, size);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t size)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(size);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanIndexBuffer>(size);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (RenderCommand::GetAPI())
		{
			case RendererAPI::API::None:
				ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, size);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanIndexBuffer>(indices, size);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}