#ifndef _WDGS_APPLICATION_H
#define _WDGS_APPLICATION_H

#include "config.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"
#include "simulation.h"
#include "ui.h"

#define WDGS_GL_MAJOR_VERSION 3
#define WDGS_GL_MINOR_VERSION 3

#define WDGS_TITLE "Walking Dinosaur's Gravity Simulator"

namespace WDGS
{
	class Application
	{
	protected:

		static GLFWwindow* window;
		static bool running;

		static Simulation::Ptr sim;
		static UI::Ptr ui;
		static Bar::Ptr bar;

		static void OnError(const char* message);
		static void OnRender(double time);
		static int OnStartup();
		static int OnShutdown();

		static void OnResize(GLFWwindow*, int w, int h);
		static void OnKey(GLFWwindow*, int key, int scancode, int action, int mode);
		static void OnMouseButton(GLFWwindow*, int button, int action, int mods);
		static void OnMouseMove(GLFWwindow*, double x, double y);
		static void OnMouseWheel(GLFWwindow*, double xoffset, double yoffset);
		static void OnChar(GLFWwindow*, unsigned int);

		static void APIENTRY DebugCallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			GLvoid* userParam);

		static void TW_CALL SetFullscreen(const void * value, void * pthis)
		{
			int fs = *(int*)value;

			if (fs)
			{
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			}
			else
				glfwSetWindowMonitor(
					window, 
					NULL,
					Config::GetInt("WindowX"), 
					Config::GetInt("WindowY"),
					Config::GetInt("WindowWidth"),
					Config::GetInt("WindowHeight"),
					0);

			Config::SetInt("Fullscreen", fs);
		}

		static void TW_CALL GetFullscreen(void * value, void * pthis)
		{
			*(int*)value = Config::GetInt("Fullscreen");
		}

		static void TW_CALL SetMSAA(const void * value, void * pthis)
		{
			Config::SetInt("MSAA", *(int*)value);

			if (sim)
			{
				int w, h;
				glfwGetWindowSize(window, &w, &h);
				sim->CreateSceenBuffers(w, h, Config::GetInt("MSAA"));
			}
		}

		static void TW_CALL GetMSAA(void * value, void * pthis)
		{
			*(int*)value = Config::GetInt("MSAA");
		}

		static void TW_CALL SetVSync(const void * value, void * pthis)
		{
			glfwSwapInterval(*(int*)value);
			Config::SetInt("VSync", *(int*)value);
		}

		static void TW_CALL GetVSync(void * value, void * pthis)
		{
			*(int*)value = Config::GetInt("VSync");
		}

	public:
		
		static int Run(int argc, const char** argv);
	};
};

#endif