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
			static Ptr Create(const GLchar* name, Physics::Planet::Ptr& obj)
			{
				char path[PATH_MAX];
				sprintf(path, "res/models/%s", name);

				struct stat info;
				if (stat(path, &info) != 0 || !(info.st_mode & S_IFDIR))
					return Ptr();

				Ptr ptr = Create();

				Sphere::Ptr sphereMesh = Sphere::Create();

				sprintf(path, "res/models/%s/textures/%s", name, "surf_diffuse.dds");
				Graphics::Texture::Ptr tex = Graphics::Texture::Create(path);
				sphereMesh->AddTexture(tex, "surf_diffuse");
				sprintf(path, "res/models/%s/textures/%s", name, "surf_specular.dds");
				tex = Graphics::Texture::Create(path);
				sphereMesh->AddTexture(tex, "surf_specular");
				sprintf(path, "res/models/%s/textures/%s", name, "surf_emission.dds");
				tex = Graphics::Texture::Create(path);
				sphereMesh->AddTexture(tex, "surf_emission");
				sprintf(path, "res/models/%s/textures/%s", name, "surf_clouds.dds");
				tex = Graphics::Texture::Create(path);
				sphereMesh->AddTexture(tex, "surf_clouds");


				Program::Ptr prog = Program::Create();

				Shader::Ptr vs = Shader::CreateFromFile(GL_VERTEX_SHADER, "res/shaders/rocky.vs.glsl");
				Shader::Ptr fs = Shader::CreateFromFile(GL_FRAGMENT_SHADER, "res/shaders/rocky.fs.glsl");

				prog->AddShader(vs);
				prog->AddShader(fs);
				prog->Link();

				sphereMesh->SetProgram(prog);

				ptr->meshes.push_back(sphereMesh);
				ptr->object = obj;

				sphereMesh = Sphere::Create();

				prog = Program::Create();

				vs = Shader::CreateFromFile(GL_VERTEX_SHADER, "res/shaders/rocky_athmo.vs.glsl");
				fs = Shader::CreateFromFile(GL_FRAGMENT_SHADER, "res/shaders/rocky_athmo.fs.glsl");

				prog->AddShader(vs);
				prog->AddShader(fs);
				prog->Link();

				sphereMesh->SetProgram(prog);
				ptr->meshes.push_back(sphereMesh);

				return ptr;
			}

		protected:

			glm::vec4 athmoColor;

			RockyModel()
			{
				type |= ModelType::Rocky;
			}

		public:

			void SetAthmoColor(glm::vec4 color)
			{
				athmoColor = color;
			}

			glm::vec4& GetAthmoColor()
			{
				return athmoColor;
			}

			virtual void Render(Camera::Ptr& cam, Light& light)
			{
				Program::Ptr& renderProg1 = meshes[0]->GetProgram();
				Program::Ptr& renderProg2 = meshes[1]->GetProgram();

				static GLuint modelLoc = glGetUniformLocation(*renderProg1, "model");
				static GLuint mvpLoc = glGetUniformLocation(*renderProg1, "mvp");
				static GLuint camLoc = glGetUniformBlockIndex(*renderProg1, "Camera");
				static GLuint lightPos = glGetUniformBlockIndex(*renderProg1, "LightSource");
				//static GLuint lightAmbientPos = glGetUniformLocation(*renderProg1, "light.ambient");
				//static GLuint lightDiffPos = glGetUniformLocation(*renderProg1, "light.diffuse");
				//static GLuint lightSpecPos = glGetUniformLocation(*renderProg1, "light.specular");

				static GLuint modelLoc2 = glGetUniformLocation(*renderProg2, "model");
				static GLuint mvpLoc2 = glGetUniformLocation(*renderProg2, "mvp");

				static GLuint camLoc2 = glGetUniformBlockIndex(*renderProg1, "Camera");
				static GLuint lightPos2 = glGetUniformBlockIndex(*renderProg1, "LightSource");

			/*	static GLuint camLoc2 = glGetUniformLocation(*renderProg2, "cameraPos");
				static GLuint lightPosLoc2 = glGetUniformLocation(*renderProg2, "light.position");
				static GLuint lightAmbientPos2 = glGetUniformLocation(*renderProg2, "light.ambient");
				static GLuint lightDiffPos2 = glGetUniformLocation(*renderProg2, "light.diffuse");
				static GLuint lightSpecPos2 = glGetUniformLocation(*renderProg2, "light.specular");*/
				static GLuint athmoLoc = glGetUniformLocation(*renderProg2, "athmoColor");

				Physics::Planet* planet = (Physics::Planet*)object.get();

				glm::dmat4 model =
					glm::scale(
						glm::rotate(
							glm::translate(
								glm::dmat4(1.0), planet->worldPosition),
							planet->rotAngle, glm::dvec3(0.0, 1.0, 0.0)),
						glm::dvec3(planet->radius));

				glm::mat4 mvp = glm::mat4(cam->GetTransform() * model);

				renderProg1->Use();
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
				glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

				glUniformBlockBinding(*renderProg1, lightPos, 0);
				glUniformBlockBinding(*renderProg1, camLoc, 1);

				/*glUniform3fv(lightPosLoc, 1, glm::value_ptr(light.position));
				glUniform3fv(lightAmbientPos, 1, glm::value_ptr(light.ambient));
				glUniform3fv(lightDiffPos, 1, glm::value_ptr(light.diffuse));
				glUniform3fv(lightSpecPos, 1, glm::value_ptr(light.specular));
				glUniform3f(camLoc, (GLfloat)cam->position.x, (GLfloat)cam->position.y, (GLfloat)cam->position.z);*/

				meshes[0]->Render();

				model = glm::scale(model, glm::dvec3(1.05, 1.05, 1.05));

				mvp = glm::mat4(cam->GetTransform() * model);

				renderProg2->Use();
				glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
				glUniformMatrix4fv(mvpLoc2, 1, GL_FALSE, glm::value_ptr(mvp));

				glUniformBlockBinding(*renderProg2, lightPos2, 0);
				glUniformBlockBinding(*renderProg2, camLoc2, 1);

				/*glUniform3fv(lightPosLoc2, 1, glm::value_ptr(light.position));
				glUniform3fv(lightAmbientPos2, 1, glm::value_ptr(light.ambient));
				glUniform3fv(lightDiffPos2, 1, glm::value_ptr(light.diffuse));
				glUniform3fv(lightSpecPos2, 1, glm::value_ptr(light.specular));*/
				//glUniform3f(camLoc2, (GLfloat)cam->position.x, (GLfloat)cam->position.y, (GLfloat)cam->position.z);
				glUniform4fv(athmoLoc, 1, glm::value_ptr(athmoColor));
				

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				meshes[1]->Render();

				glDisable(GL_BLEND);
			}
		};


		class StarModel : public Model
		{
			DECLARE_MEMMNG(StarModel)

		public:
			static Ptr Create(const GLchar* name, Physics::Star::Ptr& obj)
			{
				char path[PATH_MAX];
				sprintf(path, "res/models/%s", name);

				struct stat info;
				if (stat(path, &info) != 0 || !(info.st_mode & S_IFDIR))
					return Ptr();

				Ptr ptr = Create();

				Sphere::Ptr sphereMesh = Sphere::Create();

				sprintf(path, "res/models/%s/textures", name);
				sphereMesh->AddTexturesFromFolder(path);

				Program::Ptr prog = Program::Create();

				Shader::Ptr vs = Shader::CreateFromFile(GL_VERTEX_SHADER, "res/shaders/star.vs.glsl");
				Shader::Ptr fs = Shader::CreateFromFile(GL_FRAGMENT_SHADER, "res/shaders/star.fs.glsl");

				prog->AddShader(vs);
				prog->AddShader(fs);
				prog->Link();

				sphereMesh->SetProgram(prog);

				ptr->meshes.push_back(sphereMesh);
				ptr->object = obj;

				sphereMesh->BindTextures();
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

				meshes[0]->Render();

			}
		};

	}
}

#endif