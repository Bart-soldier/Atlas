#pragma once

#include <memory>

#include "Atlas/Core/PlatformDetection.h"

#ifdef ATLAS_DEBUG
	#if defined(ATLAS_PLATFORM_WINDOWS)
		#define ATLAS_DEBUGBREAK() __debugbreak()
	#elif defined(ATLAS_PLATFORM_LINUX)
		#include <signal.h>
		#define ATLAS_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define ATLAS_ENABLE_ASSERTS
#else
	#define ATLAS_DEBUGBREAK()
#endif

#ifdef ATLAS_ENABLE_ASSERTS
	#define ATLAS_ASSERT(x, ...) { if(!(x)) {ATLAS_ERROR("Assertion Failed: {0}", __VA_ARGS__); ATLAS_DEBUGBREAK(); } }
	#define ATLAS_CORE_ASSERT(x, ...) { if(!(x)) {ATLAS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); ATLAS_DEBUGBREAK(); } }
#else
	#define ATLAS_ASSERT(x, ...)
	#define ATLAS_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define ATLAS_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Atlas
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}