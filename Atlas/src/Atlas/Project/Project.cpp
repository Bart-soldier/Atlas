#include "atlaspch.h"
#include "Atlas/Project/Project.h"

#include "Atlas/Project/ProjectSerializer.h"

namespace Atlas
{
	Ref<Project> Project::New(const std::filesystem::path& path)
	{
		Ref<Project> newProject = CreateRef<Project>();
		newProject->m_ProjectDirectory = path.parent_path();
		newProject->m_ProjectSolutionPath = path;
		s_ActiveProject = newProject;
		
		FileHelpers::CreateDirectoryIfNeeded(newProject->GetAssetDirectory());

		return s_ActiveProject;
	}

	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			project->m_ProjectDirectory = path.parent_path();
			project->m_ProjectSolutionPath = path;
			s_ActiveProject = project;

			FileHelpers::CreateDirectoryIfNeeded(project->GetAssetDirectory());

			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive()
	{
		if (s_ActiveProject == nullptr)
		{
			return false;
		}

		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(s_ActiveProject->m_ProjectSolutionPath))
		{
			return true;
		}

		return false;
	}

}