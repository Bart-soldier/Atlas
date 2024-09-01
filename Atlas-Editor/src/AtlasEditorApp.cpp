#include <Atlas.h>
#include <Atlas/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Atlas
{
	class AtlasEditor : public Application
	{
	public:
		AtlasEditor()
			: Application("Atlas Editor")
		{
			PushLayer(new EditorLayer());
		}

		~AtlasEditor()
		{

		}
	};

	Application* CreateApplication()
	{
		return new AtlasEditor();
	}
}