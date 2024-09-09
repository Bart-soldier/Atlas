#include <Atlas.h>
#include <Atlas/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Atlas
{
	class AtlasEditor : public Application
	{
	public:
		AtlasEditor(ApplicationCommandLineArgs args)
			: Application("Atlas Editor", args)
		{
			PushLayer(new EditorLayer());
		}

		~AtlasEditor()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new AtlasEditor(args);
	}
}