#ifndef _WDGS_SIMRENDERER_H
#define _WDGS_SIMRENDERER_H

#include <map>

#include "memmng.h"
#include "physics/object.h"
#include "graphics/model.h"
#include "graphics/texture.h"

#include "camera.h"
#include <GLFW/glfw3.h>

namespace WDGS
{
	namespace Physics
	{
		class Simulation;
	}

	class SimulationRenderer
	{
		DECLARE_MEMMNG(SimulationRenderer)

	protected:
		Graphics::Texture::Ptr background;
		std::map<Physics::Object*, Graphics::Model::Ptr> models;
		Camera::Ptr camera;
		Physics::Object* lightSource;

		SimulationRenderer();

	public:
		Physics::Simulation* sim;

		virtual void Render();
		virtual void OnAddObject(Physics::Object::Ptr& obj);
		virtual void OnRemoveObject(Physics::Object::Ptr& obj);

		virtual void OnAttach(Physics::Simulation* sim);
		virtual void OnDetach(Physics::Simulation* sim);

		void OnResize(int w, int h);
		void OnKey(int key, int scancode, int action, int mode);
		void OnMouseButton(int button, int action, int mods);
		void OnMouseMove(double x, double y);
		void OnMouseWheel(double xoffset, double yoffset);
	};

}

#endif
