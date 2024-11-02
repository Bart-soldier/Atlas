#include "atlaspch.h"
#include "Atlas/Renderer/Cubemap.h"

#include "Atlas/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLCubemap.h"
#include "Platform/Vulkan/VulkanCubemap.h"

namespace Atlas
{
	Ref<Cubemap> Cubemap::Create()
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLCubemap>();
		case RendererAPI::API::Vulkan: return CreateRef<VulkanCubemap>();
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}