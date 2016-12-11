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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace WDGS
{
	namespace Graphics
	{
		class Model : public Saveable
		{
		public:
			typedef std::shared_ptr<Model> Ptr;

			enum ModelType
			{
				Rocky = 1, GasGiant = 1<<1, Star = 1<<2
			};
			
			int type;
			std::string resName;
			Object::Ptr object;

			virtual void Save(std::ostream& os)
			{
				size_t s = resName.length();
				os.write((char*)&s, sizeof(s));
				os.write(resName.c_str(), resName.length());

				object->Save(os);
			}

			virtual void Load(std::istream& is)
			{
				size_t len;
				//is >> len;
				is.read((char*)&len, sizeof(len));
				resName.resize(len);
				is.read((char*)resName.c_str(), len);

				LoadModelResource(resName.c_str(), false);
				object->Load(is);
			}

		protected:
			std::vector<MeshBase::Ptr> meshes;

			Model()
			{
				this->type = 0;
			}

			virtual void LoadModelResource(const GLchar* name, bool withObject) = 0;

			glm::dmat4 GetModelMatrix()
			{
				glm::dvec4 axis = glm::rotate(glm::dmat4(1.0), object->axisInclination.x, glm::dvec3(1.0, 0.0, 0.0)) *
					glm::rotate(glm::dmat4(1.0), object->axisInclination.y, glm::dvec3(0.0, 1.0, 0.0)) *
					glm::rotate(glm::dmat4(1.0), object->axisInclination.z, glm::dvec3(0.0, 0.0, 1.0)) *
					glm::dvec4(0.0, 1.0, 0.0, 1.0);


				return 
					glm::scale(
						glm::rotate(
							glm::translate(
								glm::dmat4(1.0), object->worldPosition),
							object->rotAngle, glm::dvec3(axis)),
						glm::dvec3(object->type & Object::Spheric ? std::static_pointer_cast<SphericObject>(object)->radius : 1.0));
			}

		public:
			virtual void Render(Camera::Ptr& cam, Light& light) = 0;
		};


		class RockyModel : public Model
		{
			DECLARE_MEMMNG(RockyModel)

		public:
			static Ptr Create(const GLchar* name, bool withObject)
			{
				Ptr ptr = Create();

				ptr->LoadModelResource(name, withObject);

				return ptr;
			}

		protected:

			virtual void LoadModelResource(const GLchar* name, bool withObject)
			{
				resName = name;

				if (withObject)
				{
					WDGS::Planet *planet = (WDGS::Planet*)object.get();

					planet->name = Resources::GetModelString(name, this->type, "name_ru");
					planet->mass = Resources::GetModelDouble(name, this->type, "mass");
					planet->radius = Resources::GetModelDouble(name, this->type, "radius");
					planet->axisInclination = Resources::GetModelVec3(name, this->type, "inclination");
					planet->rotPeriod = Resources::GetModelDouble(name, this->type, "rot_period");
				}

				athmoColor = Resources::GetModelVec4(name, this->type, "athmo_color");

				Sphere::Ptr sphereMesh = Sphere::Create();

				std::string path = Resources::GetModelPath() + Resources::GetModelString(name, this->type, "tex_path");
				sphereMesh->AddTexturesFromFolder(path.c_str());

				Program::Ptr prog = Program::Create();

				Shader::Ptr vs = Shader::CreateFromResource(GL_VERTEX_SHADER, "rocky");
				Shader::Ptr fs = Shader::CreateFromResource(GL_FRAGMENT_SHADER, "rocky");

				prog->AddShader(vs);
				prog->AddShader(fs);
				prog->Link();

				sphereMesh->SetProgram(prog);

				this->meshes.push_back(sphereMesh);

				sphereMesh = Sphere::Create();

				prog = Program::Create();

				vs = Shader::CreateFromResource(GL_VERTEX_SHADER, "rocky_athmo");
				fs = Shader::CreateFromResource(GL_FRAGMENT_SHADER, "rocky_athmo");

				prog->AddShader(vs);
				prog->AddShader(fs);
				prog->Link();

				sphereMesh->SetProgram(prog);
				this->meshes.push_back(sphereMesh);
			}

			glm::vec4 athmoColor;

			RockyModel() : Model()
			{
				type |= ModelType::Rocky;
				object = Planet::Create();
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

			virtual void Save(std::ostream& os)
			{
				Model::Save(os);
			}

			virtual void Load(std::istream& is)
			{
				Model::Load(is);
			}


			virtual void Render(Camera::Ptr& cam, Light& light)
			{
				Program::Ptr& renderProg1 = meshes[0]->GetProgram();
				Program::Ptr& renderProg2 = meshes[1]->GetProgram();

				static GLuint modelLoc = glGetUniformLocation(*renderProg1, "model");
				static GLuint mvpLoc = glGetUniformLocation(*renderProg1, "mvp");
				static GLuint camLoc = glGetUniformBlockIndex(*renderProg1, "Camera");
				static GLuint lightPos = glGetUniformBlockIndex(*renderProg1, "LightSource");
				static GLuint modelLoc2 = glGetUniformLocation(*renderProg2, "model");
				static GLuint mvpLoc2 = glGetUniformLocation(*renderProg2, "mvp");

				static GLuint camLoc2 = glGetUniformBlockIndex(*renderProg1, "Camera");
				static GLuint lightPos2 = glGetUniformBlockIndex(*renderProg1, "LightSource");
				static GLuint athmoLoc = glGetUniformLocation(*renderProg2, "athmoColor");

				Planet* planet = (Planet*)object.get();

				glm::dmat4 model = GetModelMatrix();

				glm::mat4 mvp = glm::mat4(cam->GetTransform() * model);

				renderProg1->Use();
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
				glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

				glUniformBlockBinding(*renderProg1, lightPos, 0);
				glUniformBlockBinding(*renderProg1, camLoc, 1);

				meshes[0]->Render();

				model = glm::scale(model, glm::dvec3(1.05, 1.05, 1.05));

				mvp = glm::mat4(cam->GetTransform() * model);

				renderProg2->Use();
				glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
				glUniformMatrix4fv(mvpLoc2, 1, GL_FALSE, glm::value_ptr(mvp));

				glUniformBlockBinding(*renderProg2, lightPos2, 0);
				glUniformBlockBinding(*renderProg2, camLoc2, 1);

				glUniform4fv(athmoLoc, 1, glm::value_ptr(athmoColor));
				
				meshes[1]->Render();
			}
		};


		class StarModel : public Model
		{
			DECLARE_MEMMNG(StarModel)

		public:
			static Ptr Create(const GLchar* name, bool withObject)
			{
				Ptr ptr = Create();
				ptr->LoadModelResource(name, withObject);
				return ptr;
			}

			virtual void Save(std::ostream& os)
			{
				Model::Save(os);
			}

			virtual void Load(std::istream& is)
			{
				Model::Load(is);
			}

		protected:

			virtual void LoadModelResource(const GLchar* name, bool withObject)
			{
				this->resName = name;

				if (withObject)
				{
					WDGS::Star *star = (WDGS::Star*)this->object.get();

					star->name = Resources::GetModelString(name, this->type, "name_ru");
					star->mass = Resources::GetModelDouble(name, this->type, "mass");
					star->radius = Resources::GetModelDouble(name, this->type, "radius");
					star->axisInclination = Resources::GetModelVec3(name, this->type, "inclination");
					star->rotPeriod = Resources::GetModelDouble(name, this->type, "rot_period");
				}

				Sphere::Ptr sphereMesh = Sphere::Create();

				std::string path = Resources::GetModelPath() + Resources::GetModelString(name, this->type, "tex_path");
				sphereMesh->AddTexturesFromFolder(path.c_str());

				Program::Ptr prog = Program::Create();

				Shader::Ptr vs = Shader::CreateFromResource(GL_VERTEX_SHADER, "star");
				Shader::Ptr fs = Shader::CreateFromResource(GL_FRAGMENT_SHADER, "star");

				prog->AddShader(vs);
				prog->AddShader(fs);
				prog->Link();

				sphereMesh->SetProgram(prog);

				this->meshes.push_back(sphereMesh);

				sphereMesh->BindTextures();
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			StarModel() : Model()
			{
				type |= ModelType::Star;
				object = Star::Create();
			}

		public:

			virtual void Render(Camera::Ptr& cam, Light& light)
			{
				Program::Ptr& renderProg = meshes[0]->GetProgram();

				static GLuint modelLoc = glGetUniformLocation(*renderProg, "model");
				static GLuint mvpLoc = glGetUniformLocation(*renderProg, "mvp");

				WDGS::Star* star = (WDGS::Star*)object.get();

				glm::dmat4 model = GetModelMatrix();

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