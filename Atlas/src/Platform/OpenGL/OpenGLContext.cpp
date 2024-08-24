#include "atlaspch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>

namespace Atlas
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ATLAS_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ATLAS_CORE_ASSERT(status, "Failed to initialized Glad!");

		ATLAS_CORE_INFO("OpenGL Info:");
		ATLAS_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		ATLAS_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		ATLAS_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));
	
		#ifdef ATLAS_ENABLE_ASSERTS
			int versionMajor;
			int versionMinor;
			glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
			glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

			ATLAS_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Atlas requires at least OpenGL version 4.5!");
		#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}