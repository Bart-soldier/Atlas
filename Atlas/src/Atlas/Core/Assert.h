#pragma once

#include "Atlas/Core/Base.h"
#include "Atlas/Core/Log.h"
#include <filesystem>

#ifdef ATLAS_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define ATLAS_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { ATLAS##type##ERROR(msg, __VA_ARGS__); ATLAS_DEBUGBREAK(); } }
	#define ATLAS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) ATLAS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define ATLAS_INTERNAL_ASSERT_NO_MSG(type, check) ATLAS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", ATLAS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define ATLAS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define ATLAS_INTERNAL_ASSERT_GET_MACRO(...) ATLAS_EXPAND_MACRO( ATLAS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, ATLAS_INTERNAL_ASSERT_WITH_MSG, ATLAS_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define ATLAS_ASSERT(...) ATLAS_EXPAND_MACRO( ATLAS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define ATLAS_CORE_ASSERT(...) ATLAS_EXPAND_MACRO( ATLAS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define ATLAS_ASSERT(...)
	#define ATLAS_CORE_ASSERT(...)
#endif