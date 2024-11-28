#include "atlaspch.h"
#include "Platform/Windows/WindowsWindow.h"

#include "Atlas/Core/Input.h"

#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Events/MouseEvent.h"
#include "Atlas/Events/KeyEvent.h"

#include "Atlas/Renderer/RenderCommand.h"

namespace Atlas
{
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		ATLAS_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		ATLAS_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		ATLAS_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		ATLAS_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		ATLAS_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		switch (RenderCommand::GetAPI())
		{
			case RendererAPI::API::OpenGL: m_Data.Title += " - OpenGL"; break;
			case RendererAPI::API::Vulkan: m_Data.Title += " - Vulkan"; break;
		}

		if (s_GLFWWindowCount == 0)
		{
			ATLAS_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			ATLAS_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			ATLAS_PROFILE_SCOPE("glfwCreateWindow");

			#if defined(ATLAS_DEBUG)
				if (RenderCommand::GetAPI() == RendererAPI::API::OpenGL)
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			#endif

			if (RenderCommand::GetAPI() == RendererAPI::API::Vulkan)
			{
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			}

			glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			glfwGetWindowPos(m_Window, &m_Data.posX, &m_Data.posY);
			++s_GLFWWindowCount;
		}

		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();
		
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.posX = xpos;
			data.posY = ypos;

			WindowMovedEvent event(xpos, ypos);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, false);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{
		ATLAS_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		ATLAS_PROFILE_FUNCTION();

		glfwPollEvents();
		//m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		ATLAS_PROFILE_FUNCTION();

		//if(enabled)
		//	glfwSwapInterval(1);
		//else
		//	glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	void WindowsWindow::SetSecondaryTitle(const std::string name)
	{
		std::string windowTitle = m_Data.Title + " - " + name;
		glfwSetWindowTitle(m_Window,  windowTitle.c_str());
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}