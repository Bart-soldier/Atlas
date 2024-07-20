#include <Atlas.h>

class Sandbox : public Atlas::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}