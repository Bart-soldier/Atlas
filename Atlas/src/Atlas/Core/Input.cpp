#include "atlaspch.h"
#include "Atlas/Core/Input.h"

#ifdef ATLAS_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowInput.h"
#endif

namespace Atlas {

	Scope<Input> Input::s_Instance = Input::Create();

	Scope<Input> Input::Create()
	{
#ifdef ATLAS_PLATFORM_WINDOWS
		return CreateScope<WindowsInput>();
#else
		ATLAS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}