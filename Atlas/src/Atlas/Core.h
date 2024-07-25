#pragma once

#ifdef ATLAS_PLATFORM_WINDOWS
	#ifdef ATLAS_BUILD_DLL
		#define ATLAS_API __declspec(dllexport)
	#else
		#define ATLAS_API __declspec(dllimport)
	#endif
#else
	#error Atlas only supports Windows
#endif

#ifdef ATLAS_ENABLE_ASSERTS
	#define ATLAS_ASSERT(x, ...) { if(!(x)) {ATLAS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define ATLAS_CORE_ASSERT(x, ...) { if(!(x)) {ATLAS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ATLAS_ASSERT(x, ...)
	#define ATLAS_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define ATLAS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)