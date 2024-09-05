#pragma once

#include "Atlas/Core/Base.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Atlas
{
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define ATLAS_CORE_TRACE(...)     ::Atlas::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ATLAS_CORE_INFO(...)      ::Atlas::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ATLAS_CORE_WARN(...)      ::Atlas::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ATLAS_CORE_ERROR(...)     ::Atlas::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ATLAS_CORE_CRITICAL(...)  ::Atlas::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define ATLAS_TRACE(...)          ::Atlas::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ATLAS_INFO(...)           ::Atlas::Log::GetClientLogger()->info(__VA_ARGS__)
#define ATLAS_WARN(...)           ::Atlas::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ATLAS_ERROR(...)          ::Atlas::Log::GetClientLogger()->error(__VA_ARGS__)
#define ATLAS_CRITICAL(...)       ::Atlas::Log::GetClientLogger()->critical(__VA_ARGS__)
