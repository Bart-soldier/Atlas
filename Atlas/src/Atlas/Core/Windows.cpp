#include "atlaspch.h"
#include "Atlas/Core/Window.h"

#ifdef ATLAS_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Atlas
{

	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef ATLAS_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		ATLAS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

}