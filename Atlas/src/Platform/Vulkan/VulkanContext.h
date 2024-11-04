#pragma once

#include <vulkan/vulkan.h>

#include "Atlas/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Atlas
{
	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);
		~VulkanContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
		VkInstance m_Instance;
	};
}