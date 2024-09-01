#include "atlaspch.h"

#include "Atlas/Renderer/RenderCommand.h"

namespace Atlas
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}