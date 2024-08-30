#include <Atlas.h>
#include <Atlas/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "Sandbox3D.h"

class Sandbox : public Atlas::Application
{
public:
	Sandbox()
	{
		PushLayer(new Sandbox2D());
		//PushLayer(new Sandbox3D());
	}

	~Sandbox()
	{

	}
};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}