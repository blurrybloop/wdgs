#ifndef _WDGS_GRAPHICS_MODEL_H
#define _WDGS_GRAPHICS_MODEL_H

#include <vector>
#include <memory>
#include <sys/stat.h>

#include "memmng.h"
#include "graphics/program.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"
#include "graphics/light.h"

#include "physics/object.h"
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace WDGS
{
	namespace Graphics
	{
		class Model
		{
		public:
			typedef std::shared_ptr<Model> Ptr;

			enum ModelType
			{
				Rocky = 1, GasGiant = 1<<1, Star = 1<<2
			};
			
			int type;


		protected:
			std::vector<Mesh::Ptr> meshes;
			Physics::Object::Ptr object;

		public:
			virtual void Render(Camera::Ptr& cam, Light& light) = 0;
		};


		class RockyModel : public Model
		{
			DECLARE_MEMMNG(RockyModel)

		public:
			static Ptr Create(const GLchar* name, Physics::Planet::Ptr obj)
			{
				char path[PATH_MAX];
				sprintf(path, "res/models/%s", name);

				struct stat info;
				if (stat(path, &info) != 0 || !(info.st_mode & S_IFDIR))
					return Ptr();

				Ptr ptr = Create();

				Sphere::Ptr sphereMesh = Sphere::Create(1, 6);

				sprintf(path, "res/models/%s/textures", name);
				sphereMesh->AddTexturesFromFolder(path);

				Program::Ptr prog = Program::Create();

				prog->AddShader(Shader::CreateFromFile(GL_VERTEX_SHADER, "res/shaders/rocky.vs.glsl"));
				prog->AddShader(Shader::CreateFromFile(GL_FRAGMENT_SHADER, "res/shaders/rocky.fs.glsl"));
				prog->Link();

				sphereMesh->SetProgram(prog);

				ptr->meshes.push_back(sphereMesh);
				ptr->object = obj;

				sphereMesh->BindTextures();
				//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

				return ptr;
			}

		protected:

			RockyModel()
			{
				type |= ModelType::Rocky;
			}

		public:

			virtual void Render(Camera::Ptr& cam, Light& light)
			{
				Program::Ptr& renderProg = meshes[0]->GetProgram();

				static GLuint modelLoc = glGetUniformLocation(*renderProg, "model");
				static GLuint mvpLoc = glGetUniformLocation(*renderProg, "mvp");
				static GLuint camLoc = glGetUniformLocation(*renderProg, "cameraPos");
				static GLuint lightPosLoc = glGetUniformLocation(*renderProg, "light.position");
				static GLuint lightAmbientPos = glGetUniformLocation(*renderProg, "light.ambient");
				static GLuint lightDiffPos = glGetUniformLocation(*renderProg, "light.diffuse");
				static GLuint lightSpecPos = glGetUniformLocation(*renderProg, "light.specular");

				Physics::Planet* planet = (Physics::Planet*)object.get();

				glm::dmat4 model =
					glm::scale(
						glm::rotate(
							glm::translate(
								glm::dmat4(1.0), planet->worldPosition),
							planet->rotAngle, glm::dvec3(0.0, 1.0, 0.0)),
						glm::dvec3(planet->radius));

				glm::mat4 mvp = glm::mat4(cam->GetTransform() * model);

				renderProg->Use();
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
				glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
				glUniform3fv(lightPosLoc, 1, glm::value_ptr(light.position));
				glUniform3fv(lightAmbientPos, 1, glm::value_ptr(light.ambient));
				glUniform3fv(lightDiffPos, 1, glm::value_ptr(light.diffuse));
				glUniform3fv(lightSpecPos, 1, glm::value_ptr(light.specular));
				glUniform3f(camLoc, (GLfloat)cam->position.x, (GLfloat)cam->position.y, (GLfloat)cam->position.z);

				meshes[0]->BindTextures();

				glBindVertexArray(meshes[0]->GetVAO());
				glDrawArrays(GL_TRIANGLES, 0, meshes[0]->GetCount());
				
			}
		};


		class StarModel : public Model
		{
			DECLARE_MEMMNG(StarModel)

		public:
			static Ptr Create(const GLchar* name, Physics::Star::Ptr obj)
			{
				char path[PATH_MAX];
				sprintf(path, "res/models/%s", name);

				struct stat info;
				if (stat(path, &info) != 0 || !(info.st_mode & S_IFDIR))
					return Ptr();

				Ptr ptr = Create();

				Sphere::Ptr sphereMesh = Sphere::Create(1, 6);

				sprintf(path, "res/models/%s/textures", name);
				sphereMesh->AddTexturesFromFolder(path);

				Program::Ptr prog = Program::Create();

				prog->AddShader(Shader::CreateFromFile(GL_VERTEX_SHADER, "res/shaders/star.vs.glsl"));
				prog->AddShader(Shader::CreateFromFile(GL_FRAGMENT_SHADER, "res/shaders/star.fs.glsl"));
				prog->Link();

				sphereMesh->SetProgram(prog);

				ptr->meshes.push_back(sphereMesh);
				ptr->object = obj;

				sphereMesh->BindTextures();
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

				return ptr;
			}

		protected:

			StarModel()
			{
				type |= ModelType::Star;
			}

		public:

			virtual void Render(Camera::Ptr& cam, Light& light)
			{
				Program::Ptr& renderProg = meshes[0]->GetProgram();

				static GLuint modelLoc = glGetUniformLocation(*renderProg, "model");
				static GLuint mvpLoc = glGetUniformLocation(*renderProg, "mvp");

				Physics::Star* star = (Physics::Star*)object.get();

				glm::dmat4 model =
					glm::scale(
						glm::rotate(
							glm::translate(
								glm::dmat4(1.0), star->worldPosition),
							star->rotAngle, glm::dvec3(0.0, 1.0, 0.0)),
						glm::dvec3(star->radius));

				glm::mat4 mvp = glm::mat4(cam->GetTransform() * model);

				renderProg->Use();
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
				glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

				meshes[0]->BindTextures();

				glBindVertexArray(meshes[0]->GetVAO());
				glDrawArrays(GL_TRIANGLES, 0, meshes[0]->GetCount());

			}
		};

	}
}

#endif