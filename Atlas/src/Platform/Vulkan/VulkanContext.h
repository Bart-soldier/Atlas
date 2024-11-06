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
			std::optional<uint32_t> PresentationFamily;

			bool IsComplete() const
			{
				return GraphicsFamily    .has_value() &&
					   PresentationFamily.has_value();
			}
		};

		VulkanContext(GLFWwindow* windowHandle);
		~VulkanContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void* GetInstance() const override { return m_Instance; }
		virtual void* GetPhysicalDevice() const override { return m_PhysicalDevice; }
		virtual void* GetLogicalDevice() const override { return m_LogicalDevice; }
		virtual void* GetGraphicsQueue() const override { return m_GraphicsQueue; }

	private:
		void GetRequiredExtensions(std::vector<const char*>& extensions, bool enableValidationLayers);
		bool VerifyExtensionSupport(const std::vector<const char*>& extensions);
		void GetRequiredLayers(std::vector<const char*>& layers, bool enableValidationLayers);
		bool VerifyLayerSupport(const std::vector<const char*>& layers);

		void CreateSurface();

		void SelectPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);

		void FindQueueFamilies(VkPhysicalDevice device, QueueFamilyIndices& indices);

		void CreateLogicalDevice(const std::vector<const char*>& layers, bool enableValidationLayers);

		GLFWwindow* m_WindowHandle;

		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_LogicalDevice;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentationQueue;
	};
}