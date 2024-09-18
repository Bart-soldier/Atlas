#pragma once

#include "Atlas/Renderer/RenderCommand.h"

#include "Atlas/Renderer/OrthographicCamera.h"
#include "Atlas/Renderer/Shader.h"

namespace Atlas
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};
}
