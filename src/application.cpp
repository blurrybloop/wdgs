#include "pch.h"
#include "application.h"
#include "simhelpers.h"

namespace WDGS
{
	bool Application::running(false);
	GLFWwindow* Application::window(nullptr);

	Simulation::Ptr Application::sim(nullptr);
	 UI::Ptr Application::ui(nullptr);
	 Bar::Ptr Application::bar(nullptr);

	void APIENTRY Application::DebugCallback(GLenum /* source */,
		GLenum /* type */,
		GLuint /* id */,
		GLenum /* severity */,
		GLsizei /* length */,
		const GLchar* message,
		GLvoid* /* userParam */)
	{
		cdbg << message << "\n";
	}

	int Application::OnStartup()
	{
	    ui = UI::Create();

		bar = Bar::Create("Application");
		bar->SetLabel("Свойства приложения");
		bar->AddCBVariable("fullscreen", TW_TYPE_BOOL32, "Полноэкранный режим", SetFullscreen, GetFullscreen, nullptr);
		
		ComboBox::Ptr combo = ComboBox::Create("MSAA");
		combo->SetLabel("Сглаживание (MSAA)");
		combo->AddItem(1, "Нет");
		combo->AddItem(2, "2x");
		combo->AddItem(4, "4x");
		combo->AddItem(8, "8x");

		bar->AddComboBox(combo, SetMSAA, GetMSAA, nullptr);
		bar->AddCBVariable("VSync", TW_TYPE_BOOL32, "Вертикальная синхронизация", SetVSync, GetVSync, nullptr);

		sim = Simulation::CreateFromResource(0);
		//sim = Simulation::Create();

		//sim->SetTimestep(24 * 60.0 * 60.0);
		//sim->SetTimestep(60.0 * 60.0);
		return 1;
	}

	int Application::OnShutdown()
	{
		sim = nullptr;
		bar = nullptr;

		return 1;
	}


	void Application::OnRender(double time)
	{
		sim->Refresh(time);
		sim->Render();
		ui->Render();
	}

	void Application::OnResize(GLFWwindow* wnd, int w, int h)
	{
		if (h != 0)
		{
			glViewport(0, 0, w, h);
			ui->OnResize(w, h);
			sim->OnResize(wnd, w, h);		
		}
	}

	void Application::OnKey(GLFWwindow* w, int key, int scancode, int action, int mode)
	{
		if (!ui->OnKey(key, action))
			sim->OnKey(w, key, scancode, action, mode);
	}

	void Application::OnMouseButton(GLFWwindow* w, int button, int action, int mods)
	{
		if (!ui->OnMouseButton(button, action))
			sim->OnMouseButton(w, button, action, mods);
	}

	void Application::OnMouseMove(GLFWwindow* w, double x, double y)
	{
		if (!ui->OnMouseMove(int(x), int(y)))
			sim->OnMouseMove(w, x, y);
	}

	void Application::OnMouseWheel(GLFWwindow* w , double xoffset, double yoffset)
	{
		if (!ui->OnMouseWheel(int(yoffset)))
			sim->OnMouseWheel(w, xoffset, yoffset);
	}

	void Application::OnChar(GLFWwindow*, unsigned int ch)
	{
		ui->OnChar(ch);
	}

	void Application::OnError(int /* error */, const char* message)
	{
		#ifdef WIN32
			MessageBoxA(nullptr, message, "Error", MB_ICONEXCLAMATION);
		#endif
	}


	int Application::Run(int /* argc */, const char** /* argv */)
	{
		//инициализация GLFW
		if (!glfwInit())
		{
			OnError(0, "Failed to initialize GLFW\n");
			glfwTerminate();
			return 0;
		}

		glfwSetErrorCallback(OnError);

		#ifdef _DEBUG
			//дополнительная информация для отладки
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		#endif

		//установка минимальной версии
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, WDGS_GL_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, WDGS_GL_MINOR_VERSION);

		//без устаревшей функциональности (ниже 3.2)
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		int fullscreen = Config::GetInt("Fullscreen");
		int w = Config::GetInt("WindowWidth", 1200);
		int h = Config::GetInt("WindowHeight", 700);

		//полноэкранный режим - установка размеров окна под экран (если не заданы явно)
		if (fullscreen)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			w = mode->width;
			h = mode->height;
		}

		//glfwWindowHint(GLFW_SAMPLES, glm::max(Config::GetInt("MSAA"), 1));
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

		//создание окна
		window = glfwCreateWindow(w, h,
			WDGS_TITLE,
			fullscreen ? glfwGetPrimaryMonitor() : nullptr,
			nullptr);

		if (!window)
		{
			glfwTerminate();
			return 0;
		}


		glfwSetWindowPos(window, Config::GetInt("WindowX", 100), Config::GetInt("WindowY", 100));


		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwMakeContextCurrent(window);

		//glewExperimental = GL_TRUE;

		//инициализация GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::string err = "Failed to initialize GLAD.\n";

			OnError(0, err.c_str());
			glfwTerminate();
			return 0;
		}

		char vs[100];
		sprintf(vs, "GL_VERSION_%d_%d", WDGS_GL_MAJOR_VERSION, WDGS_GL_MINOR_VERSION);

		if (GLVersion.major < WDGS_GL_MAJOR_VERSION || (GLVersion.major == WDGS_GL_MAJOR_VERSION && GLVersion.minor < WDGS_GL_MINOR_VERSION))
		{
			sprintf(vs, "OpenGL %d.%d or greater is required.", WDGS_GL_MAJOR_VERSION, WDGS_GL_MINOR_VERSION);
			OnError(0, vs);
			glfwTerminate();
			return 0;
		}

		//установка обработчиков событий
		glfwSetWindowSizeCallback(window, OnResize);	
		glfwSetKeyCallback(window, OnKey);
		glfwSetMouseButtonCallback(window, OnMouseButton);
		glfwSetCursorPosCallback(window, OnMouseMove);
		glfwSetScrollCallback(window, OnMouseWheel);
		glfwSetCharCallback(window, OnChar);

		if (GLAD_GL_ARB_debug_output)
		{
			glDebugMessageCallbackARB((GLDEBUGPROC)DebugCallback, nullptr);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		}

		glfwSwapInterval(Config::GetInt("VSync", 1));

		running = true;

		if (OnStartup())
		{
			glfwShowWindow(window);
			OnResize(window, w, h);

			//std::ofstream os;
			//os.open("res/simulations/sun_earth_moon.sim", std::ios::binary);
			//sim->Save(os);
			//os.close();

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
			OnError(0, "Failed to initialize application.\nSee log for details.");

		OnShutdown();

		int x, y;
		glfwGetWindowPos(window, &x, &y);
		glfwGetWindowSize(window, &w, &h);

		if (!Config::GetInt("Fullscreen", 0))
		{

			Config::SetInt("WindowX", x);
			Config::SetInt("WindowY", y);

			Config::SetInt("WindowWidth", w);
			Config::SetInt("WindowHeight", h);
		}

		Config::Save();

		glfwDestroyWindow(window);
		glfwTerminate();

		return 0;
	}
}