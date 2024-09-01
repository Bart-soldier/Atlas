#pragma once

#include "Atlas/Core/Base.h"

#ifdef ATLAS_PLATFORM_WINDOWS

extern Atlas::Application* Atlas::CreateApplication();

int main(int argc, char** argv)
{
	Atlas::Log::Init();

	ATLAS_PROFILE_BEGIN_SESSION("Startup", "AtlasProfile-Startup.json");
	auto app = Atlas::CreateApplication();
	ATLAS_PROFILE_END_SESSION();

	ATLAS_PROFILE_BEGIN_SESSION("Runtime", "AtlasProfile-Runtime.json");
	app->Run();
	ATLAS_PROFILE_END_SESSION();

	ATLAS_PROFILE_BEGIN_SESSION("Shutdown", "AtlasProfile-Shutdown.json");
	delete app;
	ATLAS_PROFILE_END_SESSION();
}

#endif