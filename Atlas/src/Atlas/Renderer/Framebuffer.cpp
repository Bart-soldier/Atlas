#include "atlaspch.h"
#include "Atlas/Renderer/Framebuffer.h"

#include "Atlas/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace Atlas
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
		case RendererAPI::API::Vulkan: return CreateRef<VulkanFramebuffer>(spec);
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}