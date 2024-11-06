#pragma once

#include <vulkan/vulkan.h>
#include <optional>

#include "Atlas/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Atlas
{
	class VulkanContext : public GraphicsContext
	{
	public:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> GraphicsFamily;

			bool IsComplete() const
			{
				return GraphicsFamily.has_value();
			}
		};

		VulkanContext(GLFWwindow* windowHandle);
		~VulkanContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void* GetInstance() const override { return m_Instance; }

	private:
		void GetRequiredExtensions(std::vector<const char*>& extensions, bool enableValidationLayers);
		bool VerifyExtensionSupport(const std::vector<const char*>& extensions);
		void GetRequiredLayers(std::vector<const char*>& layers, bool enableValidationLayers);
		bool VerifyLayerSupport(const std::vector<const char*>& layers);

		void SelectPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);

		void FindQueueFamilies(VkPhysicalDevice device, QueueFamilyIndices& indices);

		GLFWwindow* m_WindowHandle;

		VkInstance m_Instance;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	};
}