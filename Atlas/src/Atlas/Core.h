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

#define BIT(x) (1 << x)
