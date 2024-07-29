#include <Atlas.h>

#include "imgui/imgui.h"

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

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
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
	}

	~Sandbox()
	{

	}
};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}