#include "atlaspch.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

#include "Atlas/Core/Application.h"
#include "Atlas/Renderer/GraphicsContext.h"

namespace Atlas
{
	namespace Utils
	{
		//static GLenum PolygonModeToGLenum(const RendererAPI::PolygonMode& mode)
		//{
		//	switch (mode)
		//	{
		//	case RendererAPI::PolygonMode::Fill:  return GL_FILL;
		//	case RendererAPI::PolygonMode::Line:  return GL_LINE;
		//	case RendererAPI::PolygonMode::Point: return GL_POINT;
		//	}

		//	ATLAS_CORE_ASSERT(false, "Unknown polygon mode!");
		//	return 0;
		//}

		//static GLenum TestFunctionToGLenum(const RendererAPI::TestFunction& function)
		//{
		//	switch (function)
		//	{
		//	case RendererAPI::TestFunction::Always:   return GL_ALWAYS;
		//	case RendererAPI::TestFunction::Never:    return GL_NEVER;
		//	case RendererAPI::TestFunction::Less:     return GL_LESS;
		//	case RendererAPI::TestFunction::Equal:    return GL_EQUAL;
		//	case RendererAPI::TestFunction::LEqual:   return GL_LEQUAL;
		//	case RendererAPI::TestFunction::Greater:  return GL_GREATER;
		//	case RendererAPI::TestFunction::NotEqual: return GL_NOTEQUAL;
		//	case RendererAPI::TestFunction::GEqual:   return GL_GEQUAL;
		//	}

		//	ATLAS_CORE_ASSERT(false);
		//	return 0;
		//}

		//static GLenum StencilOperationToGLenum(const RendererAPI::StencilOperation& operation)
		//{
		//	switch (operation)
		//	{
		//	case RendererAPI::StencilOperation::Keep:          return GL_KEEP;
		//	case RendererAPI::StencilOperation::Zero:          return GL_ZERO;
		//	case RendererAPI::StencilOperation::Replace:       return GL_REPLACE;
		//	case RendererAPI::StencilOperation::Increase:      return GL_INCR;
		//	case RendererAPI::StencilOperation::Increase_Wrap: return GL_INCR_WRAP;
		//	case RendererAPI::StencilOperation::Decrease:      return GL_DECR;
		//	case RendererAPI::StencilOperation::Decrease_Wrap: return GL_DECR_WRAP;
		//	case RendererAPI::StencilOperation::Invert:        return GL_INVERT;
		//	}

		//	ATLAS_CORE_ASSERT(false);
		//	return 0;
		//}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   ATLAS_CORE_ERROR(pCallbackData->pMessage); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: ATLAS_CORE_WARN (pCallbackData->pMessage); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    ATLAS_CORE_INFO (pCallbackData->pMessage); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: ATLAS_CORE_TRACE(pCallbackData->pMessage); break;

		default: ATLAS_CORE_ASSERT(false, "Unknown severity level!"); break;
		}

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			ATLAS_CORE_ERROR("Failed to set up debug messenger!");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	void VulkanRendererAPI::Init()
	{
		ATLAS_PROFILE_FUNCTION();

#ifdef ATLAS_DEBUG
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = VulkanMessageCallback;

		Application& app = Application::Get();
		VkInstance instance = (VkInstance)app.GetGraphicsContext()->GetInstance();

		CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &m_DebugMessenger);
#endif


//#ifdef ATLAS_DEBUG
//		glEnable(GL_DEBUG_OUTPUT);
//		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
//		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
//
//		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
//#endif
//
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//		glEnable(GL_DEPTH_TEST);
//		glDepthFunc(GL_LEQUAL);
//
//		glEnable(GL_STENCIL_TEST);
//		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//		glStencilMask(0x00);
//
//		glEnable(GL_LINE_SMOOTH);
//
//		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void VulkanRendererAPI::Shutdown()
	{
#ifdef ATLAS_DEBUG
		Application& app = Application::Get();
		VkInstance instance = (VkInstance)app.GetGraphicsContext()->GetInstance();

		DestroyDebugUtilsMessengerEXT(instance, m_DebugMessenger, nullptr);
#endif
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		//glViewport(x, y, width, height);
	}

	void VulkanRendererAPI::EnableDepthTest()
	{
		//glEnable(GL_DEPTH_TEST);
	}

	void VulkanRendererAPI::DisableDepthTest()
	{
		//glDisable(GL_DEPTH_TEST);
	}

	void VulkanRendererAPI::SetStencilMask(const uint32_t& mask)
	{
		//glStencilMask(mask);
	}

	void VulkanRendererAPI::SetStencilFunction(const TestFunction& function, const int& reference, const uint32_t& mask)
	{
		//glStencilFunc(Utils::TestFunctionToGLenum(function), reference, mask);
	}

	void VulkanRendererAPI::EnableCulling()
	{
		//glEnable(GL_CULL_FACE);
	}

	void VulkanRendererAPI::DisableCulling()
	{
		//glDisable(GL_CULL_FACE);
	}

	void VulkanRendererAPI::SetBackCulling()
	{
		//glCullFace(GL_BACK);
	}

	void VulkanRendererAPI::SetFrontCulling()
	{
		//glCullFace(GL_FRONT);
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		//glClearColor(color.r, color.g, color.b, color.a);
	}

	void VulkanRendererAPI::Clear()
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void VulkanRendererAPI::ClearColor()
	{
		//glClear(GL_COLOR_BUFFER_BIT);
	}

	void VulkanRendererAPI::ClearDepth()
	{
		//glClear(GL_DEPTH_BUFFER_BIT);
	}

	void VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		//glDrawElements(GL_TRIANGLES, indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetMaxCount(), GL_UNSIGNED_INT, nullptr);
	}

	void VulkanRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		//glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void VulkanRendererAPI::BindTextureSlot(uint32_t slot, uint32_t rendererID)
	{
		//glBindTextureUnit(slot, rendererID);
	}

	void VulkanRendererAPI::SetPointSize(const float& size)
	{
		//glPointSize(size);
	}

	void VulkanRendererAPI::SetLineWidth(const float& width)
	{
		//glLineWidth(width);
	}

	void VulkanRendererAPI::SetPolygonMode(const PolygonMode& mode)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, Utils::PolygonModeToGLenum(mode));
	}
}