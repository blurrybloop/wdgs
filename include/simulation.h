#ifndef _WDGS_SIMULATIONBASE_H
#define _WDGS_SIMULATIONBASE_H

#include "memmng.h"

#include "object.h"
#include "body.h"

#include "physics/gravity.h"
#include "graphics/texture.h"
#include "ui.h"

#include "camera.h"
#include "environment.h"

namespace WDGS
{
	class Simulation : public Saveable, public BarOwner
	{
		DECLARE_MEMMNG(Simulation)

		static Ptr CreateFromResource(const char* name);

	protected:

		ComboBox::Ptr comboObjects;

		double timestep;
		double prevTime;

		Physics::GravityController::Ptr gc;

		std::vector<Body::Ptr> models;

		Camera::Ptr camera;
		GLint focusIndex;
		Star* lightSource;
		double prevX, prevY;
		
		int fboW, fboH;
		GLuint fboHdr, fboMs, rboDepthMs;
		Graphics::Quad::Ptr screen;

		GLuint ubo;
		Environment::Ptr env;

		Simulation();

	public:
		~Simulation();

		static void TW_CALL SetFocusedObject(const void * value, void * pthis)
		{
			Simulation* sim = (Simulation*)pthis;
			sim->ChangeFocus(*(int*)value);
		}

		static void TW_CALL GetFocusedObject(void * value, void * pthis)
		{
			Simulation* sim = (Simulation*)pthis;
			*(int*)value = sim->focusIndex;
		}

		static void TW_CALL SetEnvironment(const void * value, void * pthis)
		{
			Simulation* sim = (Simulation*)pthis;
			sim->env = Environment::Create(*(int*)value);
		}

		static void TW_CALL GetEnvironment(void * value, void * pthis)
		{
			Simulation* sim = (Simulation*)pthis;
			*(int*)value = sim->env->envId;
		}

		void Refresh(double time);
		void Render();

		void SetTimestep(double step);
		double GetTimestep();

		void AddModel(Body::Ptr& model);
		void RemoveModel(Body::Ptr& model);
		//std::vector<Object::Ptr>& GetObjects();

		void OnResize(GLFWwindow*, int w, int h);
		void OnKey(GLFWwindow*, int key, int scancode, int action, int mode);
		void OnMouseButton(GLFWwindow*, int button, int action, int mods);
		void OnMouseMove(GLFWwindow*, double x, double y);
		void OnMouseWheel(GLFWwindow*, double xoffset, double yoffset);

		void CreateSceenBuffers(int w, int h, int samples);

		virtual void Save(std::ostream& fs);
		virtual void Load(std::istream& fs);

	protected:
		void ChangeFocus(GLint newIndex);
	};

}

#endif