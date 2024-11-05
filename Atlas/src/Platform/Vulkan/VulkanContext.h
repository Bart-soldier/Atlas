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
		void GetRequiredExtensions(std::vector<const char*>& extensions, bool enableValidationLayers);
		void GetRequiredLayers(std::vector<const char*>& layers, bool enableValidationLayers);
		bool VerifyExtensionSupport(const std::vector<const char*>& extensions);
		bool VerifyLayerSupport(const std::vector<const char*>& layers);

		GLFWwindow* m_WindowHandle;
		VkInstance m_Instance;
	};
}