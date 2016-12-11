#include "application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "physics/simhelpers.h"
#include <iostream>
#include <fstream>

namespace WDGS
{
	bool Application::running(false);
	GLFWwindow* Application::window(0);

	Simulation::Ptr Application::sim(0);

	void APIENTRY Application::DebugCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		GLvoid* userParam)
	{
		cdbg << message << "\n";
	}

	int Application::OnStartup()
	{
		sim = Simulation::CreateFromResource("0");

		//sim->SetTimestep(10 * 24 * 60.0 * 60.0);

		return 1;
	}

	int Application::OnShutdown()
	{
		return 1;
	}


	void Application::OnRender(double time)
	{
		sim->Refresh(time);
		sim->Render();
	}

	void Application::OnResize(GLFWwindow* wnd, int w, int h)
	{
		glViewport(0, 0, w, h);
		sim->OnResize(wnd, w, h);
	}

	void Application::OnKey(GLFWwindow* w, int key, int scancode, int action, int mode)
	{
		sim->OnKey(w, key, scancode, action, mode);
	}

	void Application::OnMouseButton(GLFWwindow* w, int button, int action, int mods)
	{
		sim->OnMouseButton(w, button, action, mods);
	}

	void Application::OnMouseMove(GLFWwindow* w, double x, double y)
	{
		sim->OnMouseMove(w, x, y);
	}

	void Application::OnMouseWheel(GLFWwindow* w , double xoffset, double yoffset)
	{
		sim->OnMouseWheel(w, xoffset, yoffset);

	}

	void Application::OnError(const char* message)
	{
		#ifdef WIN32
			MessageBoxA(0, message, "Error", MB_ICONEXCLAMATION);
		#endif
	}


	int Application::Run(int argc, const char** argv)
	{
		//инициализация GLFW
		if (!glfwInit())
		{
			OnError("Failed to initialize GLFW\n");
			return 0;
		}

		#ifdef _DEBUG
			//дополнительная информация для отладки
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		#endif

		//установка минимальной версии
		int major = Config::GetVal("glMajorVersion").Int32;
		int minor = Config::GetVal("glMinorVersion").Int32;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);

		//без устаревшей функциональности (ниже 3.2)
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		int fullscreen = Config::GetVal("fullscreen").Int32;
		int w = Config::GetVal("windowWidth").Int32;
		int h = Config::GetVal("windowHeight").Int32;

		//полноэкранный режим - установка размеров окна под экран (если не заданы явно)
		if (fullscreen && (w == 0 || h == 0))
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			w = mode->width;
			h = mode->height;
		}

		glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

		//создание окна
		window = glfwCreateWindow(w, h,
			Config::GetString("title").c_str(),
			fullscreen ? glfwGetPrimaryMonitor() : NULL,
			NULL);

		if (!window)
		{
			OnError("Failed to open window\n");
			return 0;
		}

		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwMakeContextCurrent(window);

		glewExperimental = GL_TRUE;

		//инициализация GLEW
		GLenum r = glewInit();
		if (r != GLEW_OK)
		{
			std::string err = "Failed to initialize GLEW.\n";
			err += (const char*)glewGetErrorString(r);
			err += "\n";

			OnError(err.c_str());
			return 0;
		}

		char vs[100];
		sprintf(vs, "GL_VERSION_%d_%d", major, minor);

		if (!glewIsSupported(vs))
		{
			sprintf(vs, "OpenGL %d.%d or greater is required.", major, minor);
			OnError(vs);
			return 0;
		}

		//установка обработчиков событий
		glfwSetWindowSizeCallback(window, OnResize);	
		glfwSetKeyCallback(window, OnKey);
		glfwSetMouseButtonCallback(window, OnMouseButton);
		glfwSetCursorPosCallback(window, OnMouseMove);
		glfwSetScrollCallback(window, OnMouseWheel);
			
		if (GL_VERSION_4_3)
		{
			glDebugMessageCallback((GLDEBUGPROC)DebugCallback, 0);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}
		else if (GLEW_ARB_debug_output)
		{
			glDebugMessageCallbackARB((GLDEBUGPROC)DebugCallback, 0);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		}

		running = true;
	
		if (OnStartup())
		{
			glfwShowWindow(window);
			OnResize(window, w, h);

			//std::ofstream fs;
			//fs.open("res/simulations/solar.sim", std::ios::binary);

			//sim->Save(fs);
			//fs.close();

			//главный цикл приложения
			do
			{
				OnRender(glfwGetTime());

				glfwSwapBuffers(window);
				glfwPollEvents();

				running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);
				running &= (glfwWindowShouldClose(window) != GL_TRUE);
			} while (running);
		}
		else
			OnError("Failed to initialize application.\nSee log for details.");

		OnShutdown();

		glfwDestroyWindow(window);
		glfwTerminate();

		return 0;
	}
}