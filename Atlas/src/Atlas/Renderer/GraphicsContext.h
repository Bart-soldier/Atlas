#pragma once

#include <optional>

#include "Atlas/Renderer/Shader.h"

namespace Atlas
{
	class GraphicsContext
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

		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		virtual void* GetInstance() const = 0;
		virtual void* GetPhysicalDevice() const = 0;
		virtual void* GetLogicalDevice() const = 0;
		virtual void* GetGraphicsQueue() const = 0;
		virtual void* GetPresentationQueue() const = 0;
		virtual void* GetSwapChain() const = 0;
		virtual int GetSwapChainImageFormat() const = 0;
		virtual uint32_t GetSwapChainExtentWidth() const = 0;
		virtual uint32_t GetSwapChainExtentHeight() const = 0;
		virtual const QueueFamilyIndices& GetQueueFamilyIndices() const = 0;
		virtual void* GetImageAvailableSemaphore() const = 0;

		virtual uint32_t GetCurrentSwapChainImageIndex() const = 0;
		virtual void* GetCurrentFramebuffer() const = 0;

		virtual void ConfigureRenderPass(const Ref<Shader>& shader) = 0;

		static Scope<GraphicsContext> Create(void* window);
	};
}