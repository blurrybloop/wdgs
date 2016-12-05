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

		fboHdr = 0;
		rboDepth = 0;

		glGenBuffers(1, &ubo);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 280, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 64);
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo, 256, 16);

		Graphics::Shader::Ptr vs = Graphics::Shader::CreateFromFile(GL_VERTEX_SHADER, "res/shaders/hdr.vs.glsl");
		Graphics::Shader::Ptr fs = Graphics::Shader::CreateFromFile(GL_FRAGMENT_SHADER, "res/shaders/blur.fs.glsl");

		blurProg = Graphics::Program::Create();

		blurProg->AddShader(vs);
		blurProg->AddShader(fs);
		blurProg->Link();
		
		bloomProg = Graphics::Program::Create();
		fs = Graphics::Shader::CreateFromFile(GL_FRAGMENT_SHADER, "res/shaders/bloom.fs.glsl");

		bloomProg->AddShader(vs);
		bloomProg->AddShader(fs);
		bloomProg->Link();
	}

	SimulationRenderer::~SimulationRenderer()
	{
		glDeleteBuffers(1, &ubo);

		glDeleteFramebuffers(1, &fboHdr);
		glDeleteRenderbuffers(1, &rboDepth);

	}

	void SimulationRenderer::Render()
	{
		GLfloat color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat ones[3] = { 1.0f, 1.0f, 1.0f };

		glBindFramebuffer(GL_FRAMEBUFFER, fboHdr);

		glClearBufferfv(GL_COLOR, 0, color);
		glClearBufferfv(GL_COLOR, 1, color);
		glClear(GL_DEPTH_BUFFER_BIT);

	
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		Graphics::Light l;
		l.position = lightSource->worldPosition;
		l.ambient = glm::vec3(0.0, 0.0, 0.0);
		l.diffuse = glm::vec3(1.0f, 0.9f, 0.9f);
		l.specular = glm::vec3(0.4f, 0.3f, 0.3f);

		camera->UpdateTransform();

		glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(l.position));
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, glm::value_ptr(l.ambient));
		glBufferSubData(GL_UNIFORM_BUFFER, 32, 16, glm::value_ptr(l.diffuse));
		glBufferSubData(GL_UNIFORM_BUFFER, 48, 16, glm::value_ptr(l.specular));

		glm::vec3 cp = glm::vec3(camera->position);
		glBufferSubData(GL_UNIFORM_BUFFER, 256, 16, glm::value_ptr(cp));

		for (auto it = models.begin(); it != models.end(); ++it)
		{
			it->second->Render(camera, l);
		}

		Graphics::Quad::Ptr quad = Graphics::Quad::Create();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. Blur bright fragments w/ two-pass Gaussian Blur 
		GLboolean horizontal = true, first_iteration = true;
		GLuint amount = 10;

		quad->SetProgram(blurProg);
		quad->GetProgram()->Use();
		static GLuint p = glGetUniformLocation(*blurProg, "horizontal");

		for (GLuint i = 0; i < amount; i++)
		{

			glBindFramebuffer(GL_FRAMEBUFFER, fboPingpong[horizontal]);
			glUniform1i(p, horizontal);
			quad->ClearTextures();
			quad->AddTexture(first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal], "image");

			//glBindTexture(GL_TEXTURE_2D, first_iteration ? *colorBuffers[1] : *pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
			quad->Render();

			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		quad->SetProgram(bloomProg);
		quad->ClearTextures();
		quad->AddTexture(colorBuffers[0], "scene");
		quad->AddTexture(pingpongColorbuffers[!horizontal], "bloomBlur");

		// 2. Now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		quad->Render();

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
		em->SetAthmoColor(glm::vec4(0.0f, 0.1f, 1.0f, 0.2f));
		
		models[earth.get()] = em;
		models[sun.get()] = Graphics::StarModel::Create("sun", sun);

		Physics::Object::Ptr o = earth;

		sim->AddObject(o);

		camera->distanceToFocus = earth->radius + 30000000.0;

		camera->FocusOn(o);

		o = sun;
		sim->AddObject(o);

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


	/*	glDeleteFramebuffers(1, &fboHdr);
		glDeleteRenderbuffers(1, &rboDepth);

		glGenFramebuffers(1, &fboHdr);
		colorBuffers[0] = Graphics::Texture::Create(GL_TEXTURE_2D);
		quad->ClearTextures();
		quad->AddTexture(colorBuffers[0], "scene");

		glBindTexture(GL_TEXTURE_2D, *colorBuffers[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);

		glBindFramebuffer(GL_FRAMEBUFFER, fboHdr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *colorBuffers[0], 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cdbg << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/


		glDeleteFramebuffers(1, &fboHdr);
		glDeleteRenderbuffers(1, &rboDepth);

		glGenFramebuffers(1, &fboHdr);
		glBindFramebuffer(GL_FRAMEBUFFER, fboHdr);

		//colorBuffers[0] = Graphics::Texture::Create(GL_TEXTURE_2D);
		//colorBuffers[0]->Bind();
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *colorBuffers[0], 0);

		for (GLuint i = 0; i < 2; i++)
		{
			colorBuffers[i] = Graphics::Texture::Create(GL_TEXTURE_2D);
			colorBuffers[i]->Bind();

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, *colorBuffers[i], 0);
		}

		//quads[0]->ClearTextures();
		//quads[0]->AddTexture(colorBuffers[0], "hdrBuffer");


		//glBindTexture(GL_TEXTURE_2D, *colorBuffer);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);

		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *colorBuffer, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

		GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cdbg << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDeleteFramebuffers(2, fboPingpong);
		glGenFramebuffers(2, fboPingpong);

		for (GLuint i = 0; i < 2; i++)
		{
			pingpongColorbuffers[i] = Graphics::Texture::Create(GL_TEXTURE_2D);

			glBindFramebuffer(GL_FRAMEBUFFER, fboPingpong[i]);
			pingpongColorbuffers[i]->Bind();

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *pingpongColorbuffers[i], 0);
			// Also check if framebuffers are complete (no need for depth buffer)
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				cdbg << "Framebuffer not complete!" << std::endl;
		}
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