#include "simrenderer.h"
#include "physics/simulation.h"
#include "graphics/light.h"
#include "physics/simhelpers.h"


namespace WDGS
{
	SimulationRenderer::SimulationRenderer()
	{
		camera = Camera::Create();

	}

	void SimulationRenderer::Render()
	{
		GLfloat color[3] = { 0.0f, 0.0f, 0.0f };
		GLfloat ones[3] = { 1.0f, 1.0f, 1.0f };

		glClearBufferfv(GL_COLOR, 0, color);
		glClearBufferfv(GL_DEPTH, 0, ones);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		Graphics::Light l;
		l.position = lightSource->worldPosition;
		l.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		l.diffuse = glm::vec3(1.0f, 0.9f, 0.9f);
		l.specular = glm::vec3(0.2f, 0.15f, 0.15f);

		for (auto it = models.begin(); it != models.end(); ++it)
		{
			it->second->Render(camera, l);
		}

	}

	void SimulationRenderer::OnAddObject(Physics::Object::Ptr& obj)
	{
		if (obj->type & Physics::Object::Star)
			lightSource = obj.get();
	}

	void SimulationRenderer::OnRemoveObject(Physics::Object::Ptr& obj)
	{

	}

	void SimulationRenderer::OnAttach(Physics::Simulation* sim)
	{
		this->sim = sim;

		Physics::Star::Ptr sun = Physics::Star::Create();

		sun->mass = 1.9885E+30;
		sun->radius = 6.9551E+8;
		sun->worldPosition = glm::dvec3(0.0, 0.0, 0.0);
		sun->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);
		sun->axisInclination = glm::dvec3(0.0, 0.0, 0.0);
		sun->rotPeriod = DBL_MAX;

		Physics::Planet::Ptr earth = Physics::Planet::Create();

		earth->mass = 5.9726E+24;
		earth->radius = 6371000.0;
		earth->worldPosition = glm::dvec3(0.0, 0.0, 149598261000.0);
		earth->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);
		earth->axisInclination = glm::dvec3(0.0, 0.0, 0.0);
		earth->rotPeriod = DBL_MAX;

		earth->worldVelocity.x = sqrt(abs(Physics::Simulation::gravityConst * sun->mass / earth->worldPosition.z));
		if (earth->worldPosition.z < 0.0)
			earth->worldVelocity.x = -earth->worldVelocity.x;

		
		models[earth.get()] = Graphics::RockyModel::Create("earth", earth);
		models[sun.get()] = Graphics::StarModel::Create("sun", sun);

		sim->AddObject(sun);
		sim->AddObject(earth);

		camera->distanceToFocus = earth->radius + 30000000.0;

		camera->FocusOn(earth);

	}

	void SimulationRenderer::OnDetach(Physics::Simulation* sim)
	{
		this->sim = 0;
		models.clear();
	}



	void SimulationRenderer::OnResize(int w, int h)
	{
		camera->aspect = (double)w / h;
		camera->fov = 45.0;

		camera->UpdateTransform();
	}

	void SimulationRenderer::OnKey(int key, int scancode, int action, int mode)
	{
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_A)
				camera->angles.y += 10.0;
			else if (key == GLFW_KEY_D)
				camera->angles.y -= 10.0;

			else if (key == GLFW_KEY_S)
				camera->angles.x += 10.0;
			else if (key == GLFW_KEY_W)
				camera->angles.x -= 10.0;

			camera->angles.y = Physics::SimHelpers::ClampCyclic(camera->angles.y, 0.0, 360.0);
			camera->angles.x = Physics::SimHelpers::ClampCyclic(camera->angles.x, 0.0, 360.0);
			camera->UpdateTransform();
		}

	}

	void SimulationRenderer::OnMouseButton(int button, int action, int mods)
	{

	}

	void SimulationRenderer::OnMouseMove(double x, double y)
	{

	}

	void SimulationRenderer::OnMouseWheel(double xoffset, double yoffset)
	{
		if (yoffset < 0)
			camera->distanceToFocus *= 1.5;
		else
			camera->distanceToFocus /= 1.5;

		camera->UpdateTransform();
	}

}