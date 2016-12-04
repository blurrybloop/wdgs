#include "simrenderer.h"
#include "physics/simulation.h"
#include "graphics/light.h"
#include "physics/simhelpers.h"


namespace WDGS
{
	SimulationRenderer::SimulationRenderer()
	{
		camera = Camera::Create();
		prevX = prevY = -1.0;

		glGenBuffers(1, &ubo);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 80, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 64);
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo, 64, 16);
	}

	SimulationRenderer::~SimulationRenderer()
	{
		glDeleteBuffers(1, &ubo);
	}

	void SimulationRenderer::Render()
	{
		GLfloat color[3] = { 0.0f, 0.0f, 0.0f };
		GLfloat ones[3] = { 1.0f, 1.0f, 1.0f };

		glClearBufferfv(GL_COLOR, 0, color);
		glClearBufferfv(GL_DEPTH, 0, ones);

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Graphics::Light l;
		l.position = lightSource->worldPosition;
		l.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		l.diffuse = glm::vec3(1.0f, 0.9f, 0.9f);
		l.specular = glm::vec3(0.4f, 0.3f, 0.3f);

		camera->UpdateTransform();

		glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(l.position));
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, glm::value_ptr(l.ambient));
		glBufferSubData(GL_UNIFORM_BUFFER, 32, 16, glm::value_ptr(l.diffuse));
		glBufferSubData(GL_UNIFORM_BUFFER, 48, 16, glm::value_ptr(l.specular));

		glm::vec3 cp = glm::vec3(camera->position);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 16, glm::value_ptr(cp));

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

		Graphics::RockyModel::Ptr em = Graphics::RockyModel::Create("earth", earth);
		em->SetAthmoColor(glm::vec4(0.0f, 0.4f, 1.0f, 0.2f));
		
		models[earth.get()] = em;
		models[sun.get()] = Graphics::StarModel::Create("sun", sun);

		Physics::Object::Ptr o = sun;
		sim->AddObject(o);

		o = earth;
		sim->AddObject(o);

		camera->distanceToFocus = earth->radius + 30000000.0;

		camera->FocusOn(o);
		focusIndex = 1;

	}

	void SimulationRenderer::OnDetach(Physics::Simulation* sim)
	{
		this->sim = 0;
		models.clear();
	}



	void SimulationRenderer::OnResize(GLFWwindow*, int w, int h)
	{
		camera->aspect = (double)w / h;
		camera->fov = 45.0;
	}

	void SimulationRenderer::OnKey(GLFWwindow*, int key, int scancode, int action, int mode)
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
			else if (key == GLFW_KEY_TAB)
			{
				std::vector<Physics::Object::Ptr>& objects = sim->GetObjects();

				++focusIndex;
				if (focusIndex >= objects.size())
					focusIndex = 0;

				camera->FocusOn(objects[focusIndex]);

			}

			camera->angles.y = Physics::SimHelpers::ClampCyclic(camera->angles.y, 0.0, 360.0);
			camera->angles.x = Physics::SimHelpers::ClampCyclic(camera->angles.x, 0.0, 360.0);
		}

	}

	void SimulationRenderer::OnMouseButton(GLFWwindow*, int button, int action, int mods)
	{

	}

	void SimulationRenderer::OnMouseMove(GLFWwindow* wnd, double x, double y)
	{
		if (prevX != -1)
		{
			
			if (glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				camera->angles.y -= (x - prevX) / 3.0;
				camera->angles.x -= (y - prevY) / 3.0;

				camera->angles.y = Physics::SimHelpers::ClampCyclic(camera->angles.y, 0.0, 360.0);
				camera->angles.x = Physics::SimHelpers::ClampCyclic(camera->angles.x, 0.0, 360.0);
			}
		}

		prevX = x;
		prevY = y;
	}

	void SimulationRenderer::OnMouseWheel(GLFWwindow*, double xoffset, double yoffset)
	{
		if (yoffset < 0)
			camera->distanceToFocus *= 1.5;
		else
			camera->distanceToFocus /= 1.5;
	}

}