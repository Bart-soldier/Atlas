#pragma once

#include "Atlas/Core/Window.h"
#include "Atlas/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Atlas
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetSecondaryTitle(const std::string title) override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const override { return m_Window; }
		virtual const Scope<GraphicsContext>& GetGraphicsContext() const override { return m_Context; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			int posX, posY;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}