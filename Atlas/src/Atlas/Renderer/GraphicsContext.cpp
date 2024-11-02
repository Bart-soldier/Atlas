#include "atlaspch.h"
#include "Atlas/Renderer/GraphicsContext.h"

#include "Atlas/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace Atlas {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (RenderCommand::GetAPI())
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL: return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		case RendererAPI::API::Vulkan: return CreateScope<VulkanContext>(static_cast<GLFWwindow*>(window));
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}