#pragma once

#include <string>
#include <filesystem>

#include "Atlas/Core/Base.h"
#include "Atlas/Utils/PlatformUtils.h"

namespace Atlas
{
	struct ProjectConfig
	{
		std::string Name = "Untitled Project";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory = "Assets";
	};

	class Project
	{
	public:
		static const std::filesystem::path& GetProjectDirectory()
		{
			if (s_ActiveProject == nullptr)
			{
				return std::filesystem::path();
			}
				
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			if (s_ActiveProject == nullptr)
			{
				return std::filesystem::path();
			}

			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		// TODO: move to asset manager when we have one
		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			return GetAssetDirectory() / path;
		}

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }

		static Ref<Project> New(const std::filesystem::path& path);
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive();
	
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectSolutionPath;
		std::filesystem::path m_ProjectDirectory;

		inline static Ref<Project> s_ActiveProject;
	};

}