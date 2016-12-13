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

	public:
		
		static int Run(int argc, const char** argv);
	};
};

#endif