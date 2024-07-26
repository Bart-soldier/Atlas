#include <Atlas.h>

class ExampleLayer : public Atlas::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		// ATLAS_INFO("ExampleLayer::OnUpdate");
	}

	void OnEvent(Atlas::Event& event) override
	{
		// ATLAS_TRACE("{0}", event.ToString());
	}
};

class Sandbox : public Atlas::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Atlas::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}