#include "atlaspch.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Atlas
{
	namespace Utils
	{
		static GLenum PolygonModeToGLenum(const RendererAPI::PolygonMode& mode)
		{
			switch (mode)
			{
			case RendererAPI::PolygonMode::Fill:  return GL_FILL;
			case RendererAPI::PolygonMode::Line:  return GL_LINE;
			case RendererAPI::PolygonMode::Point: return GL_POINT;
			}

			ATLAS_CORE_ASSERT(false, "Unknown polygon mode!");
			return 0;
		}

		static GLenum TestFunctionToGLenum(const RendererAPI::TestFunction& function)
		{
			switch (function)
			{
			case RendererAPI::TestFunction::Always:   return GL_ALWAYS;
			case RendererAPI::TestFunction::Never:    return GL_NEVER;
			case RendererAPI::TestFunction::Less:     return GL_LESS;
			case RendererAPI::TestFunction::Equal:    return GL_EQUAL;
			case RendererAPI::TestFunction::LEqual:   return GL_LEQUAL;
			case RendererAPI::TestFunction::Greater:  return GL_GREATER;
			case RendererAPI::TestFunction::NotEqual: return GL_NOTEQUAL;
			case RendererAPI::TestFunction::GEqual:   return GL_GEQUAL;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}

		static GLenum StencilOperationToGLenum(const RendererAPI::StencilOperation& operation)
		{
			switch (operation)
			{
			case RendererAPI::StencilOperation::Keep:          return GL_KEEP;
			case RendererAPI::StencilOperation::Zero:          return GL_ZERO;
			case RendererAPI::StencilOperation::Replace:       return GL_REPLACE;
			case RendererAPI::StencilOperation::Increase:      return GL_INCR;
			case RendererAPI::StencilOperation::Increase_Wrap: return GL_INCR_WRAP;
			case RendererAPI::StencilOperation::Decrease:      return GL_DECR;
			case RendererAPI::StencilOperation::Decrease_Wrap: return GL_DECR_WRAP;
			case RendererAPI::StencilOperation::Invert:        return GL_INVERT;
			}

			ATLAS_CORE_ASSERT(false);
			return 0;
		}
	}

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         ATLAS_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       ATLAS_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          ATLAS_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: ATLAS_CORE_TRACE(message); return;
		}
		ATLAS_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		#ifdef ATLAS_DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);

			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
	}

	void OpenGLRendererAPI::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetStencilMask(const uint32_t& mask)
	{
		glStencilMask(mask);
	}

	void OpenGLRendererAPI::SetStencilFunction(const TestFunction& function, const int& reference, const uint32_t& mask)
	{
		glStencilFunc(Utils::TestFunctionToGLenum(function), reference, mask);
	}

	void OpenGLRendererAPI::EnableBackCulling()
	{
		glEnable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::DisableBackCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetMaxCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::SetLineWidth(const float& width)
	{
		glLineWidth(width);
	}

	void OpenGLRendererAPI::SetPolygonMode(const PolygonMode& mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, Utils::PolygonModeToGLenum(mode));
	}
}