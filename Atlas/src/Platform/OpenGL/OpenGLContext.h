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
		virtual void* GetPresentationQueue() const override { return nullptr; }
		virtual void* GetSwapChain() const override { return nullptr; }
		virtual int GetSwapChainImageFormat() const override { return -1; }
		virtual uint32_t GetSwapChainExtentWidth() const { return 0; }
		virtual uint32_t GetSwapChainExtentHeight() const { return 0; }
		virtual const QueueFamilyIndices& GetQueueFamilyIndices() const override { return {}; }
		virtual void* GetImageAvailableSemaphore() const override { return nullptr; }

		virtual uint32_t GetCurrentSwapChainImageIndex() const override { return 0; }
		virtual void* GetCurrentFramebuffer() const override { return nullptr; }

		virtual void ConfigureRenderPass(const Ref<Shader>& shader) override {};

	private:
		GLFWwindow* m_WindowHandle;
	};
}