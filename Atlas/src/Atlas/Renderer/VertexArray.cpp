#include "atlaspch.h"
#include "Atlas/Renderer/VertexArray.h"

#include "Atlas/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Atlas
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (RenderCommand::GetAPI())
		{
			case RendererAPI::API::None:
				ATLAS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return CreateRef<OpenGLVertexArray>();
		}

		ATLAS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}