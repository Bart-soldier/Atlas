#pragma once

#include "Atlas/Renderer/RendererAPI.h"

namespace Atlas
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		
		virtual void EnableDepthTest() override;
		virtual void DisableDepthTest() override;
		virtual void SetStencilMask(const uint32_t& mask) override;
		virtual void SetStencilFunction(const TestFunction& function, const int& reference, const uint32_t& mask) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;

		virtual void SetLineWidth(const float& width) override;

		virtual void SetPolygonMode(const PolygonMode& mode) override;
	};
}



