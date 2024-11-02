#include "atlaspch.h"
#include "Atlas/Renderer/StorageBuffer.h"

#include "Atlas/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLStorageBuffer.h"
#include "Platform/Vulkan/VulkanStorageBuffer.h"

namespace Atlas
{
	Ref<StorageBuffer> StorageBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLStorageBuffer>(size, binding);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanStorageBuffer>(size, binding);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<StorageBuffer> StorageBuffer::Create(uint32_t binding)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLStorageBuffer>(binding);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanStorageBuffer>(binding);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}