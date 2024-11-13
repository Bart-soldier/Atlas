#pragma once

#include "Atlas/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Atlas
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void* GetInstance() const override { return nullptr; }
		virtual void* GetPhysicalDevice() const override { return nullptr; }
		virtual void* GetLogicalDevice() const override { return nullptr; }
		virtual void* GetGraphicsQueue() const override { return nullptr; }
		virtual int GetSwapChainImageFormat() const override { return -1; }

	private:
		GLFWwindow* m_WindowHandle;
	};
}