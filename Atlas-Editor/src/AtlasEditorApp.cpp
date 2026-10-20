#include <Atlas.h>
#include <Atlas/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Atlas
{
	static std::filesystem::path FindEditorWorkingDirectory()
	{
		std::filesystem::path directory = std::filesystem::current_path();
		for (uint32_t depth = 0; depth < 6; ++depth)
		{
			if (std::filesystem::exists(directory / "assets" / "shaders" / "3D" / "Test_Vert.glsl"))
				return directory;
			if (std::filesystem::exists(directory / "Atlas-Editor" / "assets" / "shaders" / "3D" / "Test_Vert.glsl"))
				return directory / "Atlas-Editor";
			if (directory == directory.root_path())
				break;
			directory = directory.parent_path();
		}
		return std::filesystem::current_path();
	}

	class AtlasEditor : public Application
	{
	public:
		AtlasEditor(const ApplicationSpecification& spec)
			: Application(spec)
		{
			PushLayer(new EditorLayer());
		}

		~AtlasEditor()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Atlas Editor";
		spec.CommandLineArgs = args;
		spec.WorkingDirectory = FindEditorWorkingDirectory().string();

		return new AtlasEditor(spec);
	}
}