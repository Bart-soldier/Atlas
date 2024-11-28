#pragma once

#include <vulkan/vulkan.h>

#include "Atlas/Renderer/RendererAPI.h"

namespace Atlas
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void BeginRenderPass(const Ref<Shader>& shader) override;
		virtual void EndRenderPass() override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void EnableDepthTest() override;
		virtual void DisableDepthTest() override;
		virtual void SetStencilMask(const uint32_t& mask) override;
		virtual void SetStencilFunction(const TestFunction& function, const int& reference, const uint32_t& mask) override;

		virtual void EnableCulling() override;
		virtual void DisableCulling() override;
		virtual void SetBackCulling() override;
		virtual void SetFrontCulling() override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void ClearColor() override;
		virtual void ClearDepth() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;

		virtual void BindTextureSlot(uint32_t slot, uint32_t rendererID) override;

		virtual void SetPointSize(const float& size) override;
		virtual void SetLineWidth(const float& width) override;

		virtual void SetPolygonMode(const PolygonMode& mode) override;

	private:
		void CreateDebugMessenger();
		void CreateCommandPool();
		void CreateCommandBuffer();

		void BeginCommandBufferRecording();
		void EndCommandBufferRecording();

		VkDebugUtilsMessengerEXT m_DebugMessenger;

		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CommandBuffer;

		VkClearValue m_ClearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	};
}