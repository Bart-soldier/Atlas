#pragma once

#include <vulkan/vulkan.h>

#include "Atlas/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Atlas
{
	class VulkanContext : public GraphicsContext
	{
	public:
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR Capabilities;
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentationModes;
		};

		VulkanContext(GLFWwindow* windowHandle);
		~VulkanContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void* GetInstance() const override { return m_Instance; }
		virtual void* GetPhysicalDevice() const override { return m_PhysicalDevice; }
		virtual void* GetLogicalDevice() const override { return m_LogicalDevice; }
		virtual void* GetGraphicsQueue() const override { return m_GraphicsQueue; }
		virtual int GetSwapChainImageFormat() const override { return m_SwapChainImageFormat; }
		virtual uint32_t GetSwapChainExtentWidth() const { return m_SwapChainExtent.width; }
		virtual uint32_t GetSwapChainExtentHeight() const { return m_SwapChainExtent.height; }
		virtual const QueueFamilyIndices& GetQueueFamilyIndices() const override { return m_QueueFamilyIndices; }

		virtual void* GetCurrentFramebuffer() const override { return m_SwapChainFramebuffers[m_SwapChainIndex]; }

		virtual void ConfigureRenderPass(const Ref<Shader>& shader) override;

	private:
		void GetRequiredInstanceExtensions(std::vector<const char*>& extensions);
		bool VerifyInstanceExtensionSupport(const std::vector<const char*>& extensions);

		void GetRequiredLayers(std::vector<const char*>& layers);
		bool VerifyLayerSupport(const std::vector<const char*>& layers);

		void CreateInstance(const VkInstanceCreateInfo& instanceCreateInfo);
		void CreateSurface();

		void SelectPhysicalDevice(const std::vector<const char*>& requiredExtensions);
		bool IsDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& requiredExtensions);
		void GetRequiredDeviceExtensions(std::vector<const char*>& extensions);
		bool VerifyDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& extensions);

		void FindQueueFamilies(VkPhysicalDevice device, QueueFamilyIndices& indices);

		void CreateLogicalDevice(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

		void QuerySwapChainSupport(VkPhysicalDevice device, SwapChainSupportDetails& details);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void CreateSwapChain();
		void CreateSwapChainImageViews();
		void CreateSwapChainFramebuffers(const VkRenderPass& renderPass);

		void Reflect();

		GLFWwindow* m_WindowHandle;

		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_LogicalDevice;
		QueueFamilyIndices m_QueueFamilyIndices;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentationQueue;

		uint32_t m_SwapChainIndex;
		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
	};
}