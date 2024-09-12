#include <Atlas.h>
#include <Atlas/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Atlas
{
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

		return new AtlasEditor(spec);
	}
}