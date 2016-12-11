#include "physics/simulation.h"
#include "physics/simhelpers.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <fstream>

namespace WDGS
{
	Simulation::Ptr Simulation::CreateFromResource(const char* name)
	{
		Ptr p = Create();

		std::ifstream fs;
		std::string path = Resources::GetSimPath() + Resources::GetSimString(name, "path");

		fs.open(path, std::ios::binary);
		p->Load(fs);
		fs.close();

		return p;
	}

	Simulation::Simulation()
	{
		prevTime = 0;
		gc = Physics::GravityController::Create();

		camera = Camera::Create();
		prevX = prevY = -1.0;

		glGenBuffers(1, &ubo);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 80, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 64);
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo, 64, 16);

		environment = Graphics::Texture::Create("res/environments/milky.dds");

		Graphics::Shader::Ptr vs = Graphics::Shader::CreateFromFile(GL_VERTEX_SHADER, "res/shaders/environment.vs.glsl");
		Graphics::Shader::Ptr fs = Graphics::Shader::CreateFromFile(GL_FRAGMENT_SHADER, "res/shaders/environment.fs.glsl");

		Graphics::Program::Ptr p = Graphics::Program::Create();
		p->AddShader(vs);
		p->AddShader(fs);
		p->Link();

		envCube = Graphics::Cube::Create();
		envCube->SetProgram(p);
		envCube->AddTexture(environment, "environment");



	/*	Graphics::StarModel::Ptr sun = Graphics::StarModel::Create("Sun", true);

		sun->object->worldPosition = glm::dvec3(0.0, 0.0, 0.0);
		sun->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);

		Graphics::RockyModel::Ptr earth = Graphics::RockyModel::Create("Earth", true);

		earth->object->worldPosition = glm::dvec3(0.0, 0.0, 149598261000.0);
		earth->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);


		earth->object->worldVelocity.x = glm::sqrt(glm::abs(Physics::GravityController::gravityConst * sun->object->mass / earth->object->worldPosition.z));
		if (earth->object->worldPosition.z < 0.0)
			earth->object->worldVelocity.x = -earth->object->worldVelocity.x;

		earth->SetAthmoColor(glm::vec4(0.0f, 0.4f, 1.0f, 0.2f));

		Graphics::Model::Ptr model = sun;

		AddModel(model);

		model = earth;
		AddModel(model);
		double r = std::static_pointer_cast<SphericObject>(earth->object)->radius;

		camera->FocusOn(model->object, r + 30000000.0, 1.2 * r);
		focusIndex = 1;

		lightSource = sun->object.get();*/
	}

	Simulation::~Simulation()
	{
		glDeleteBuffers(1, &ubo);
	}

		void Simulation::AddModel(Graphics::Model::Ptr& model)
		{
			models.push_back(model);
			gc->AddMP(model->object.get());
		}

		void Simulation::RemoveModel(Graphics::Model::Ptr& model)
		{
			//auto it = models.find(model->object.get());
			//if (it != models.end())
			//{
			//	models.erase(it);
			//	gc->RemoveMP(model->object.get());
			//}
		}

		//std::vector<Object::Ptr>& Simulation::GetObjects()
		//{
		//	return objects;
		//}

		void Simulation::SetTimestep(double step)
		{
			timestep = step;
		}

		double Simulation::GetTimestep()
		{
			return timestep;
		}

		void Simulation::Refresh(double time)
		{
			double step = (time - prevTime) * timestep;

			gc->Refresh(step);

			for (auto it = models.begin(); it != models.end(); ++it)
			{
				(*it)->object->rotAngle += 2 * glm::pi<double>() * step / (*it)->object->rotPeriod;
				(*it)->object->rotAngle = SimHelpers::ClampCyclic((*it)->object->rotAngle, 0, 2 * glm::pi<double>());
			}

			prevTime = time;

		}

		void Simulation::Render()
		{
			camera->UpdateTransform();

			GLfloat color[3] = { 0.0f, 0.0f, 0.0f };

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

			glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(l.position));
			glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, glm::value_ptr(l.ambient));
			glBufferSubData(GL_UNIFORM_BUFFER, 32, 16, glm::value_ptr(l.diffuse));
			glBufferSubData(GL_UNIFORM_BUFFER, 48, 16, glm::value_ptr(l.specular));

			glm::vec3 cp = glm::vec3(camera->position);
			glBufferSubData(GL_UNIFORM_BUFFER, 64, 16, glm::value_ptr(cp));

			for (auto it = models.begin(); it != models.end(); ++it)
			{
				(*it)->Render(camera, l);
			}



			glDisable(GL_MULTISAMPLE);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);

			glDepthFunc(GL_LEQUAL);

			RenderEnvironment();

			glDepthFunc(GL_LESS);
		}

		void Simulation::RenderEnvironment()
		{
			Graphics::Program::Ptr& prog = envCube->GetProgram();
			static GLuint vpLoc = glGetUniformLocation(*prog, "vp");

			glm::mat4 vp = glm::mat4(camera->GetProjection()) * glm::mat4(glm::mat3(camera->GetLookat()));
			prog->Use();
			glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp));

			envCube->Render();
		}

		void Simulation::OnResize(GLFWwindow*, int w, int h)
		{
			camera->aspect = (double)w / h;
			camera->fov = 45.0;
		}

		void Simulation::OnKey(GLFWwindow*, int key, int scancode, int action, int mode)
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
					/*auto it = models.find(camera->GetFocus().get());
					if (it != models.end())
					{
						Object::Ptr p((Object*)(++it)->first);
						camera->FocusOn(p);
					}*/
				}

				camera->angles.y = SimHelpers::ClampCyclic(camera->angles.y, 0.0, 360.0);
				camera->angles.x = SimHelpers::ClampCyclic(camera->angles.x, 0.0, 360.0);
			}

		}

		void Simulation::OnMouseButton(GLFWwindow*, int button, int action, int mods)
		{

		}

		void Simulation::OnMouseMove(GLFWwindow* wnd, double x, double y)
		{
			if (prevX != -1)
			{

				if (glfwGetMouseButton(wnd, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
				{
					camera->angles.y -= (x - prevX) / 3.0;
					camera->angles.x -= (y - prevY) / 3.0;

					camera->angles.y = SimHelpers::ClampCyclic(camera->angles.y, 0.0, 360.0);
					camera->angles.x = SimHelpers::ClampCyclic(camera->angles.x, 0.0, 360.0);
				}
			}

			prevX = x;
			prevY = y;
		}

		void Simulation::OnMouseWheel(GLFWwindow*, double xoffset, double yoffset)
		{
			if (yoffset < 0)
				camera->MoveOut();
			else
				camera->MoveIn();
		}

		void Simulation::Save(std::ostream& os)
		{
			//выгрузка моделей
			size_t s = models.size();
			os.write((char*)&s, sizeof(s));
			int t;

			for (auto it = models.begin(); it != models.end(); ++it)
			{
				t = (*it)->type;
				os.write((char*)&t, sizeof(t));

				(*it)->Save(os);
			}

			//выгрузка состо€ни€ камеры

			os.write((char*)&focusIndex, sizeof(focusIndex));
			os.write((char*)&camera->minDistance, sizeof(camera->minDistance));
			os.write((char*)&camera->distanceToFocus, sizeof(camera->distanceToFocus));

			os.write((char*)glm::value_ptr(camera->angles), sizeof(camera->angles));

			os.write((char*)&timestep, sizeof(timestep));

			//os << focusIndex << camera->minDistance << camera->distanceToFocus;
			//os << camera->angles.x << camera->angles.y << camera->angles.z;
		}

		void Simulation::Load(std::istream& is)
		{
			int type;
			size_t len;
			Graphics::Model::Ptr model;

			//загрузка моделей
			is.read((char*)&len, sizeof(len));

			//is >> len;
			models.clear();
			models.reserve(len);

			for (int i = 0; i < len; ++i)
			{
				//is >> type;
				is.read((char*)&type, sizeof(type));

				if (type & Graphics::Model::Rocky)
					model = Graphics::RockyModel::Create();
				else if (type & Graphics::Model::Star)
				{
					model = Graphics::StarModel::Create();
					lightSource = model->object.get();
				}

				model->Load(is);
				AddModel(model);
			}

			//загрузка состо€ни€ камеры

			double md, d;

			is.read((char*)&focusIndex, sizeof(focusIndex));
			is.read((char*)&md, sizeof(md));
			is.read((char*)&d, sizeof(d));

			is.read((char*)glm::value_ptr(camera->angles), sizeof(camera->angles));

			is.read((char*)&timestep, sizeof(timestep));
			//is >> focusIndex >> md >> d;
			//is >> camera->angles.x >> camera->angles.y >> camera->angles.z;

			camera->FocusOn(models[focusIndex]->object, d, md);

		}
}