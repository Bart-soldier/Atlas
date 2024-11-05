#pragma once

#include <sstream>

#include "Atlas/Core/Base.h"
#include "Atlas/Events/Event.h"
#include "Atlas/Renderer/GraphicsContext.h"

namespace Atlas
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Atlas Engine",
			uint32_t width = 1920,
			uint32_t height = 1080)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetSecondaryTitle(const std::string title) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual const Scope<GraphicsContext>& GetGraphicsContext() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}