#pragma once

namespace Atlas
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		virtual void* GetInstance() const = 0;
		virtual void* GetPhysicalDevice() const = 0;
		virtual void* GetLogicalDevice() const = 0;
		virtual void* GetGraphicsQueue() const = 0;

		static Scope<GraphicsContext> Create(void* window);
	};
}