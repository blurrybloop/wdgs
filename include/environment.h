#ifndef _WDGS_ENVIRONMENT_H
#define _WDGS_ENVIRONMENT_H

#include "memmng.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"
#include "camera.h"

namespace WDGS
{
	class Environment
	{
		DECLARE_MEMMNG(Environment)

		static Ptr Create(GLuint id)
		{
			Ptr ptr = Create();

			ptr->envId = id;

			ptr->name = Resources::GetEnvString(ptr->envId, "name_ru");

			std::string tp = Resources::GetEnvString(ptr->envId, "tex_path");

			Graphics::Texture::Ptr tex;

			if (!tp.empty())
			{
				std::string fp = Resources::GetEnvPath() + tp;
				tex = Graphics::Texture::Create(fp.c_str());
			}

			Graphics::Shader::Ptr vs = Graphics::Shader::CreateFromResource(GL_VERTEX_SHADER, "environment");
			Graphics::Shader::Ptr fs = Graphics::Shader::CreateFromResource(GL_FRAGMENT_SHADER, "environment");

			Graphics::Program::Ptr p = Graphics::Program::Create();
			p->AddShader(vs);
			p->AddShader(fs);
			p->Link();

			GLuint loc = glGetUniformLocation(*p, "useTex");
			p->Use();
			glUniform1i(loc, tex ? 1 : 0);

			ptr->envCube = Graphics::Cube::Create();
			ptr->envCube->SetProgram(p);

			if (tex)
				ptr->envCube->AddTexture(tex, "environment");
			
			return ptr;
		}

		GLuint envId;
		std::string name;

		void Render(Camera::Ptr& cam)
		{
			Graphics::Program::Ptr& prog = envCube->GetProgram();
			static GLuint vpLoc = glGetUniformLocation(*prog, "vp");

			glm::mat4 vp = glm::mat4(cam->GetProjection()) * glm::mat4(glm::mat3(cam->GetLookat()));
			prog->Use();
			glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp));

			envCube->Render();
		}

	protected:

		Graphics::Cube::Ptr envCube;

	};
}

#endif