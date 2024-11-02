#include "atlaspch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Atlas
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ATLAS_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		ATLAS_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ATLAS_CORE_ASSERT(status, "Failed to initialized Glad!");

		ATLAS_CORE_INFO("OpenGL Info:");
		ATLAS_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		ATLAS_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		ATLAS_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		ATLAS_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Atlas requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		ATLAS_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}