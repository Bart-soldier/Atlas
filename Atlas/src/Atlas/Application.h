#pragma once

#include "Core.h"

#include "Window.h"
#include "Atlas/LayerStack.h"
#include "Atlas/Events/Event.h"
#include "Atlas/Events/ApplicationEvent.h"

#include "Atlas/Core/Timestep.h"

#include "Atlas/ImGui/ImGuiLayer.h"

namespace Atlas
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

