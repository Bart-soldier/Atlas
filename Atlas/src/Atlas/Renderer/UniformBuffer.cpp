#include "atlaspch.h"
#include "Atlas/Renderer/UniformBuffer.h"

#include "Atlas/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Atlas
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:    
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(size, binding);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanUniformBuffer>(size, binding);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}