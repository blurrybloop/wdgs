#include "pch.h"
#include "application.h"
#include "simulation.h"
#include "simhelpers.h"
#include "config.h"

#define SUN_LUMINOSITY 3.828E+26

namespace WDGS
{
	Simulation::Ptr Simulation::CreateFromResource(GLint res)
	{
		Ptr p = Create();

		p->resId = res;
		std::ifstream fs;
		std::string path = Resources::GetSimPath() + Resources::GetSimString(res, "path");

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
		focusIndex = -1;

		fboHdr = 0;
		fboMs = 0;
		rboDepthMs = 0;
		fboW = fboH = 0;

		screen = Graphics::Quad::Create();

		Graphics::Shader::Ptr vs = Graphics::Shader::CreateFromResource(GL_VERTEX_SHADER, "hdr");
		Graphics::Shader::Ptr fs = Graphics::Shader::CreateFromResource(GL_FRAGMENT_SHADER, "hdr");

		Graphics::Program::Ptr hdrProg = Graphics::Program::Create();

		hdrProg->AddShader(vs);
		hdrProg->AddShader(fs);
		hdrProg->Link();

		screen->SetProgram(hdrProg);


		bar = Bar::Create("Simulation");

		bar->SetLabel("Свойства симуляции");
		bar->AddRWVariable("timestep", TW_TYPE_DOUBLE, "Шаг времени", &timestep);
		
		comboObjects = ComboBox::Create("objects");
		comboObjects->SetLabel("Фокус");
		bar->AddComboBox(comboObjects, SetFocusedObject, GetFocusedObject, this, "Камера");
		bar->AddRWVariable("distanceToFocus", TW_TYPE_DOUBLE, "Расстояние до фокуса", &camera->distanceToFocus, "Камера");

		bar->AddRWVariable("angleX", TW_TYPE_DOUBLE, "Поворот X", &camera->angles.x, "Камера");
		bar->AddRWVariable("angleY", TW_TYPE_DOUBLE, "Поворот Y", &camera->angles.y, "Камера");


		//env = Environment::Create(1);


		ComboBox::Ptr combo = ComboBox::Create("Environment");
		combo->SetLabel("Фон");

		std::vector<GLint> envIds;
		Resources::GetEnvIds(envIds);

		for (size_t i = 0; i < envIds.size(); ++i)
			combo->AddItem(envIds[i], Resources::GetEnvString(envIds[i], "name_ru"));

		bar->AddComboBox(combo, SetEnvironment, GetEnvironment, this);


		combo = ComboBox::Create("Simulation");
		combo->SetLabel("Симуляция");

		std::vector<GLint> simIds;
		Resources::GetSimIds(simIds);

		for (size_t i = 0; i < simIds.size(); ++i)
			combo->AddItem(simIds[i], Resources::GetSimString(simIds[i], "name_ru"));

		bar->AddComboBox(combo, Application::SetSim, Application::GetSim, this);

		glGenBuffers(1, &ubo);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 300, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 64);
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo, 256, 16);


		/*StarModel::Ptr sun = StarModel::Create("Sun", true);

		sun->object->worldPosition = glm::dvec3(0.0, 0.0, 0.0);
		sun->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);

		RockyBody::Ptr earth = RockyBody::Create("Earth", true);

		earth->object->worldPosition = glm::dvec3(0.0, 0.0, 149598261000.0);
		earth->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);


		earth->object->worldVelocity.x = glm::sqrt(glm::abs(Physics::GravityController::gravityConst * sun->object->mass / earth->object->worldPosition.z));
		if (earth->object->worldPosition.z < 0.0)
			earth->object->worldVelocity.x = -earth->object->worldVelocity.x;


		RockyBody::Ptr mars = RockyBody::Create("Mars", true);

		mars->object->worldPosition = glm::dvec3(0.0, 0.0, 2.2794382E+11);
		mars->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);


		mars->object->worldVelocity.x = glm::sqrt(glm::abs(Physics::GravityController::gravityConst * sun->object->mass / mars->object->worldPosition.z));
		if (mars->object->worldPosition.z < 0.0)
			mars->object->worldVelocity.x = -mars->object->worldVelocity.x;


		RockyBody::Ptr mercury = RockyBody::Create("Mercury", true);

		mercury->object->worldPosition = glm::dvec3(0.0, 0.0, 5E+10);
		mercury->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);


		mercury->object->worldVelocity.x = glm::sqrt(glm::abs(Physics::GravityController::gravityConst * sun->object->mass / mercury->object->worldPosition.z));
		if (mercury->object->worldPosition.z < 0.0)
			mercury->object->worldVelocity.x = -mercury->object->worldVelocity.x;

		RockyBody::Ptr venus = RockyBody::Create("Venus", true);

		venus->object->worldPosition = glm::dvec3(0.0, 0.0, 1.0820893E+11);
		venus->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);


		venus->object->worldVelocity.x = glm::sqrt(glm::abs(Physics::GravityController::gravityConst * sun->object->mass / venus->object->worldPosition.z));
		if (venus->object->worldPosition.z < 0.0)
			venus->object->worldVelocity.x = -venus->object->worldVelocity.x;


		Body::Ptr model = sun;

		AddModel(model);

		model = mercury;
		AddModel(model);
		model = venus;
		AddModel(model);
		model = earth;
		AddModel(model);
		model = mars;
		AddModel(model);

		double r = std::static_pointer_cast<SphericObject>(earth->object)->radius;

		camera->FocusOn(model->object, r + 30000000.0, 1.2 * r);
		focusIndex = 1;

		lightSource = (Star*)sun->object.get();*/



		//StarModel::Ptr sun = StarModel::Create("Sun", true);

		//sun->object->worldPosition = glm::dvec3(0.0, 0.0, 0.0);
		//sun->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);

		//RockyBody::Ptr earth = RockyBody::Create("Earth", true);

		//earth->object->worldPosition = glm::dvec3(0.0, 0.0, 149598261000.0);
		//earth->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);


		//earth->object->worldVelocity.x = glm::sqrt(glm::abs(Physics::GravityController::gravityConst * sun->object->mass / earth->object->worldPosition.z));
		//if (earth->object->worldPosition.z < 0.0)
		//earth->object->worldVelocity.x = -earth->object->worldVelocity.x;


		//RockyBody::Ptr moon = RockyBody::Create("Moon", true);

		//moon->object->worldPosition = glm::dvec3(0.0, 0.0, 149598261000.0 + 384399000);
		//moon->object->worldVelocity = glm::dvec3(0.0, 0.0, 0.0);


		//moon->object->worldVelocity.x = earth->object->worldVelocity.x +glm::sqrt(glm::abs(Physics::GravityController::gravityConst * earth->object->mass / 384399000));
		//if (moon->object->worldPosition.z < 0.0)
		//	moon->object->worldVelocity.x = -moon->object->worldVelocity.x;

		//
		//Body::Ptr model = sun;

		//AddModel(model);
		//model = earth;
		//AddModel(model);
		//model = moon;
		//AddModel(model);

		//double r = std::static_pointer_cast<SphericObject>(model->object)->radius;

		//camera->FocusOn(model->object, r + 30000000.0, 1.2 * r);
		//focusIndex = 2;

		//lightSource = (Star*)sun->object.get();

	}

	Simulation::~Simulation()
	{
		glDeleteBuffers(1, &ubo);
		glDeleteFramebuffers(1, &fboHdr);
		glDeleteFramebuffers(1, &fboMs);
		glDeleteRenderbuffers(1, &rboDepthMs);
	}

		void Simulation::AddModel(Body::Ptr& model)
		{
			models.push_back(model);
			gc->AddMP(model->object.get());
			comboObjects->AddItem(models.size() - 1, model->object->name.c_str());
		}

		void Simulation::RemoveModel(Body::Ptr& model)
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

			glBindFramebuffer(GL_FRAMEBUFFER, fboMs);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_MULTISAMPLE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


			Graphics::Light l;
			l.position = lightSource->worldPosition;
			l.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
			l.diffuse = glm::vec3(50.0f * (lightSource->luminosity / SUN_LUMINOSITY),40.0f* (lightSource->luminosity / SUN_LUMINOSITY),40.0f* (lightSource->luminosity / SUN_LUMINOSITY));
			l.specular = glm::vec3(0.4f, 0.3f, 0.3f);

			glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(l.position));
			glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, glm::value_ptr(l.ambient));
			glBufferSubData(GL_UNIFORM_BUFFER, 32, 16, glm::value_ptr(l.diffuse));
			glBufferSubData(GL_UNIFORM_BUFFER, 48, 16, glm::value_ptr(l.specular));

			glm::vec3 cp = glm::vec3(camera->position);

			glBufferSubData(GL_UNIFORM_BUFFER, 256, 16, glm::value_ptr(cp));

			for (auto it = models.begin(); it != models.end(); ++it)
			{
				(*it)->Render(camera, l);
			}

			glDisable(GL_MULTISAMPLE);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);

			glDepthFunc(GL_LEQUAL);

			env->Render(camera);

			glDepthFunc(GL_LESS);
			glEnable(GL_MULTISAMPLE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			for (auto it = models.begin(); it != models.end(); ++it)
			{
				if ((*it)->type & Body::Rocky)
				{
					RockyBody* rb = (RockyBody*)(*it).get();
					rb->RenderAthmo(camera, l);
				}
			}

			glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMs);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHdr);

			glBlitFramebuffer(0, 0, fboW, fboH, 0, 0, fboW, fboH, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glDisable(GL_MULTISAMPLE);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);


			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			screen->Render();
		}

		void Simulation::CreateSceenBuffers(int w, int h, int samples)
		{
			fboW = w;
			fboH = h;

			glDeleteFramebuffers(1, &fboHdr);
			glDeleteFramebuffers(1, &fboMs);
			glDeleteRenderbuffers(1, &rboDepthMs);

			fboMs = 0;
			rboDepthMs = 0;

			glGenFramebuffers(1, &fboHdr);

			if (samples > 1)
			{
				glGenFramebuffers(1, &fboMs);

				Graphics::Texture::Ptr tex = Graphics::Texture::Create(GL_TEXTURE_2D_MULTISAMPLE);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *tex);

				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, fboW, fboH, GL_TRUE);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glGenRenderbuffers(1, &rboDepthMs);
				glBindRenderbuffer(GL_RENDERBUFFER, rboDepthMs);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, fboW, fboH);

				glBindFramebuffer(GL_FRAMEBUFFER, fboMs);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *tex, 0);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthMs);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					cdbg << "Framebuffer not complete!" << std::endl;
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			Graphics::Texture::Ptr tex = Graphics::Texture::Create(GL_TEXTURE_2D);
			screen->ClearTextures();
			screen->AddTexture(tex, "screen");

			glBindTexture(GL_TEXTURE_2D, *tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, fboW, fboH, 0, GL_RGBA, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindFramebuffer(GL_FRAMEBUFFER, fboHdr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				cdbg << "Framebuffer not complete!" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Simulation::OnResize(GLFWwindow*, int w, int h)
		{
			camera->aspect = (double)w / h;
			camera->fov = 45.0;

			CreateSceenBuffers(w, h, Config::GetInt("MSAA"));
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
					ChangeFocus(focusIndex + 1);
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

			//выгрузка состояния камеры

			os.write((char*)&focusIndex, sizeof(focusIndex));
			os.write((char*)&camera->minDistance, sizeof(camera->minDistance));
			os.write((char*)&camera->distanceToFocus, sizeof(camera->distanceToFocus));

			os.write((char*)glm::value_ptr(camera->angles), sizeof(camera->angles));

			os.write((char*)&timestep, sizeof(timestep));
			os.write((char*)&env->envId, sizeof(env->envId));
		}

		void Simulation::Load(std::istream& is)
		{
			int type;
			size_t len;
			Body::Ptr model;

			//загрузка моделей
			is.read((char*)&len, sizeof(len));

			models.clear();
			models.reserve(len);

			for (size_t i = 0; i < len; ++i)
			{
				//is >> type;
				is.read((char*)&type, sizeof(type));

				if (type & Body::Rocky)
					model = RockyBody::Create();
				else if (type & Body::Star)
				{
					model = StarModel::Create();
					lightSource = (Star*)model->object.get();
				}

				model->Load(is);
				AddModel(model);
			}

			//загрузка состояния камеры

			double md, d;

			is.read((char*)&focusIndex, sizeof(focusIndex));
			is.read((char*)&md, sizeof(md));
			is.read((char*)&d, sizeof(d));

			is.read((char*)glm::value_ptr(camera->angles), sizeof(camera->angles));

			is.read((char*)&timestep, sizeof(timestep));

			int envId;
			is.read((char*)&envId, sizeof(envId));
			env = Environment::Create(envId);

			camera->FocusOn(models[focusIndex]->object, d, md);
			models[focusIndex]->Maximize();
		}

		void Simulation::ChangeFocus(GLint newIndex)
		{
			if (this->models.empty()) return;

			if (this->focusIndex != -1)
				this->models[this->focusIndex]->Minimize();

			if (newIndex >= models.size() || newIndex < 0)
				this->focusIndex = 0;
			else
				this->focusIndex = newIndex;

			Object::Ptr o = this->models[this->focusIndex]->object;
			this->camera->FocusOn(o, this->camera->distanceToFocus, 1.2 * ((SphericObject*)o.get())->radius);

			this->models[this->focusIndex]->Maximize();
		}
}