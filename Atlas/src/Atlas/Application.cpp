#include "atlaspch.h"
#include "Application.h"

#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Log.h"
#include "Events/Event.h"

namespace Atlas
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			ATLAS_TRACE(e.ToString());
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			ATLAS_TRACE(e.ToString());
		}

		while (true);
	}
}
