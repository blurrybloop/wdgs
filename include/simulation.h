#ifndef _WDGS_SIMULATIONBASE_H
#define _WDGS_SIMULATIONBASE_H

#include "memmng.h"

#include "object.h"
#include "body.h"

#include "physics/gravity.h"
#include "graphics/texture.h"

#include "camera.h"

namespace WDGS
{
	class Simulation : public Saveable
	{
		DECLARE_MEMMNG(Simulation)

		static Ptr CreateFromResource(const char* name);

	protected:
		double timestep;
		double prevTime;

		Physics::GravityController::Ptr gc;

		std::vector<Body::Ptr> models;

		Graphics::Texture::Ptr environment;

		Camera::Ptr camera;
		GLuint focusIndex;
		Object* lightSource;
		double prevX, prevY;

		GLuint ubo;
		Graphics::Cube::Ptr envCube;

		Simulation();

	public:
		~Simulation();

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

		virtual void Save(std::ostream& fs);
		virtual void Load(std::istream& fs);

	protected:
		void RenderEnvironment();
	};

}

#endif