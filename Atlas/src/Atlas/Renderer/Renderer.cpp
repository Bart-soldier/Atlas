#include "atlaspch.h"

#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Renderer/Renderer2D.h"

namespace Atlas
{
	void Renderer::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		ATLAS_PROFILE_FUNCTION();

		Renderer2D::Shutdown();
	}
}