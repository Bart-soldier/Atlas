#pragma once

#include "Atlas/Renderer/RendererAPI.h"

namespace Atlas
{
	class RenderCommand
	{
	public:
		static void Init()
		{
			s_RendererAPI->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void EnableDepthTest()
		{
			s_RendererAPI->EnableDepthTest();
		}

		static void DisableDepthTest()
		{
			s_RendererAPI->DisableDepthTest();
		}

		static void SetStencilMask(const uint32_t& mask)
		{
			s_RendererAPI->SetStencilMask(mask);
		}

		static void SetStencilFunction(const RendererAPI::TestFunction& function, const int& reference, const uint32_t& mask)
		{
			s_RendererAPI->SetStencilFunction(function, reference, mask);
		}

		static void EnableBackCulling()
		{
			s_RendererAPI->EnableBackCulling();
		}

		static void DisableBackCulling()
		{
			s_RendererAPI->DisableBackCulling();
		}

		static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		static void Clear()
		{
			s_RendererAPI->Clear();
		}

		static void ClearColor()
		{
			s_RendererAPI->ClearColor();
		}

		static void ClearDepth()
		{
			s_RendererAPI->ClearDepth();
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}

		static void BindTextureSlot(uint32_t slot, uint32_t rendererID)
		{
			s_RendererAPI->BindTextureSlot(slot, rendererID);
		}

		static void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}

		static void SetPolygonMode(RendererAPI::PolygonMode mode)
		{
			s_RendererAPI->SetPolygonMode(mode);
		}

		static RendererAPI::API GetAPI()
		{ 
			return RendererAPI::GetAPI();
		}


	private:
		static Scope<RendererAPI> s_RendererAPI;
	};
}


