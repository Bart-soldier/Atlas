#pragma once

#include "Atlas/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Atlas
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};

		enum class TestFunction
		{
			Always = 0,
			Never,
			Less,
			Equal,
			LEqual,
			Greater,
			NotEqual,
			GEqual
		};

		enum class StencilOperation
		{
			Keep = 0,
			Zero,
			Replace,
			Increase,
			Increase_Wrap,
			Decrease,
			Decrease_Wrap,
			Invert
		};

		enum class PolygonMode
		{
			Fill,
			Line,
			Point
		};

		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void EnableDepthTest() = 0;
		virtual void DisableDepthTest() = 0;
		virtual void SetStencilMask(const uint32_t& mask) = 0;
		virtual void SetStencilFunction(const TestFunction& function, const int& reference, const uint32_t& mask) = 0;

		virtual void EnableBackCulling() = 0;
		virtual void DisableBackCulling() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void ClearDepth() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

		virtual void BindTextureSlot(uint32_t slot, uint32_t rendererID) = 0;

		virtual void SetLineWidth(const float& width) = 0;
		virtual void SetPolygonMode(const PolygonMode& mode) = 0;

		static API GetAPI() { return s_API; }
		static Scope<RendererAPI> Create();

	private:
		static API s_API;
	};
}

