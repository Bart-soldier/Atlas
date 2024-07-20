#pragma once

#ifdef ATLAS_PLATFORM_WINDOWS

extern Atlas::Application* Atlas::CreateApplication();

int main(int argc, char** argv)
{
	Atlas::Log::Init();
	ATLAS_CORE_WARN("Initialized Log!");
	int a = 5;
	ATLAS_INFO("Initialized Log {0}!", a);

	auto app = Atlas::CreateApplication();
	app->Run();
	delete app;
}

#endif
