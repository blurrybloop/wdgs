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
		Graphics::Texture::Ptr environment;
		std::map<Physics::Object*, Graphics::Model::Ptr> models;
		Camera::Ptr camera;
		Physics::Object* lightSource;
		double prevX, prevY;
		GLuint focusIndex;

		GLuint ubo;
		Graphics::Cube::Ptr envCube;

		SimulationRenderer();
	public:
		~SimulationRenderer();


		Physics::Simulation* sim;

		virtual void RenderEnvironment();

		virtual void Render();
		virtual void OnAddObject(Physics::Object::Ptr& obj);
		virtual void OnRemoveObject(Physics::Object::Ptr& obj);

		virtual void OnAttach(Physics::Simulation* sim);
		virtual void OnDetach(Physics::Simulation* sim);

		void OnResize(GLFWwindow*, int w, int h);
		void OnKey(GLFWwindow*, int key, int scancode, int action, int mode);
		void OnMouseButton(GLFWwindow*, int button, int action, int mods);
		void OnMouseMove(GLFWwindow*, double x, double y);
		void OnMouseWheel(GLFWwindow*, double xoffset, double yoffset);
	};

}

#endif
