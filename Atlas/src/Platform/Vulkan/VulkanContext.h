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
		bool VerifyValidationLayerSupport(const std::vector<const char*>& validationLayers);

		GLFWwindow* m_WindowHandle;
		VkInstance m_Instance;
	};
}