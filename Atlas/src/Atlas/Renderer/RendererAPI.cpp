#include "atlaspch.h"

#include "Atlas/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Atlas
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:
			ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateScope<OpenGLRendererAPI>();
		case RendererAPI::API::Vulkan:
			return CreateScope<VulkanRendererAPI>();
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}