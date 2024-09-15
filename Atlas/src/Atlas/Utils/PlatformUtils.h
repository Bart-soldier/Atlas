#pragma once

#include <string>

namespace Atlas
{
	class FileHelpers
	{
	public:
		static void CreateDirectoryIfNeeded(const std::filesystem::path directoryName)
		{
			if (!std::filesystem::exists(directoryName))
			{
				std::filesystem::create_directories(directoryName);
			}
		}
	};

	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

	class Time
	{
	public:
		static float GetTime();
	};
}