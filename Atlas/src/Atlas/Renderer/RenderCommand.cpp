#include "atlaspch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Atlas
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}