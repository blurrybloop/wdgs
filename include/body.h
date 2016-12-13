#ifndef _WDGS_BODY_H
#define _WDGS_BODY_H

#include "memmng.h"
#include "graphics/program.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"
#include "graphics/light.h"

#include "object.h"
#include "camera.h"
#include "ui.h"

namespace WDGS
{
	using namespace Graphics;

	class Body : public Saveable, public BarOwner
	{
		DECLARE_MEMMNG_NOCREATE(Body)
		
	protected:
		std::vector<MeshBase::Ptr> meshes;

	public:
		enum BodyType
		{
			Rocky = 1, GasGiant = 1 << 1, Star = 1 << 2
		};

		int type;
		std::string resName;
		Object::Ptr object;

	protected:

		Body() : BarOwner()
		{
			this->type = 0;
		}

		static void TW_CALL GetVelocityCB(void * value, void * pthis)
		{
			*(double*)value = glm::length(((Body*)pthis)->object->worldVelocity);
		}

		virtual void LoadUIBar()
		{
			bar = Bar::Create(resName.c_str());

			bar->AddRWVariable("_name", TW_TYPE_STDSTRING, "���", &object->name);
			bar->AddRWVariable("mass", TW_TYPE_DOUBLE, "�����", &object->mass);

			bar->AddRWVariable("position_x", TW_TYPE_DOUBLE, "X", &object->worldPosition.x, "����������");
			bar->AddRWVariable("position_y", TW_TYPE_DOUBLE, "Y", &object->worldPosition.y, "����������");
			bar->AddRWVariable("position_z", TW_TYPE_DOUBLE, "Z", &object->worldPosition.z, "����������");

			bar->AddCBVariable("velocity", TW_TYPE_DOUBLE, "��������", NULL, GetVelocityCB, this);

			bar->AddRWVariable("velocity_x", TW_TYPE_DOUBLE, "X", &object->worldVelocity.x, "�������� XYZ");
			bar->AddRWVariable("velocity_y", TW_TYPE_DOUBLE, "Y", &object->worldVelocity.y, "�������� XYZ");
			bar->AddRWVariable("velocity_z", TW_TYPE_DOUBLE, "Z", &object->worldVelocity.z, "�������� XYZ");

			bar->AddRWVariable("inclination_x", TW_TYPE_DOUBLE, "X", &object->axisInclination.x, "������ ���");
			bar->AddRWVariable("inclination_y", TW_TYPE_DOUBLE, "Y", &object->axisInclination.y, "������ ���");
			bar->AddRWVariable("inclination_z", TW_TYPE_DOUBLE, "Z", &object->axisInclination.z, "������ ���");

			bar->AddRWVariable("rot_period", TW_TYPE_DOUBLE, "������ ��������", &object->rotPeriod);

			
		}

		virtual void LoadObject(const GLchar* name)
		{
			object->name = Resources::GetModelString(name, this->type, "name_ru");
			object->mass = Resources::GetModelDouble(name, this->type, "mass");
			object->axisInclination = Resources::GetModelVec3(name, this->type, "inclination");
			object->rotPeriod = Resources::GetModelDouble(name, this->type, "rot_period");
		}

		virtual void LoadBody(const GLchar* name, bool withObject)
		{
			resName = name;
			LoadUIBar();

			if (withObject)
			{
				LoadObject(name);
				bar->SetLabel(object->name.c_str());
			}
		}

		virtual void GetModelMatrix(glm::dmat4& mat)
		{
			glm::dvec4 axis = glm::rotate(glm::dmat4(1.0), object->axisInclination.x, glm::dvec3(1.0, 0.0, 0.0)) *
				glm::rotate(glm::dmat4(1.0), object->axisInclination.y, glm::dvec3(0.0, 1.0, 0.0)) *
				glm::rotate(glm::dmat4(1.0), object->axisInclination.z, glm::dvec3(0.0, 0.0, 1.0)) *
				glm::dvec4(0.0, 1.0, 0.0, 1.0);


			mat = glm::rotate(
					glm::translate(
						glm::dmat4(1.0), object->worldPosition),
					object->rotAngle, glm::dvec3(axis));
		}

	public:

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
			is.read((char*)&len, sizeof(len));
			resName.resize(len);
			is.read((char*)resName.c_str(), len);

			LoadBody(resName.c_str(), false);
			object->Load(is);
			bar->SetLabel(object->name.c_str());

		}

		virtual void Render(Camera::Ptr& cam, Light& light) = 0;
	};

	class SphericBody : public Body
	{
	protected:

		SphericBody() : Body()
		{
			this->type = 0;
		}

		virtual void GetModelMatrix(glm::dmat4& mat)
		{
			Body::GetModelMatrix(mat);
			mat = glm::scale(mat, glm::dvec3(((SphericObject*)object.get())->radius));
		}

		virtual void LoadUIBar()
		{
			Body::LoadUIBar();
			SphericObject* so = (SphericObject*)object.get();

			bar->AddRWVariable("radius", TW_TYPE_DOUBLE, "������", &so->radius);
		}

		virtual void LoadObject(const GLchar* name)
		{
			SphericObject* so = (SphericObject*)object.get();
			so->radius = Resources::GetModelDouble(name, this->type, "radius");
		}

	};


	class RockyBody : public SphericBody
	{
		DECLARE_MEMMNG(RockyBody)

	public:
		static Ptr Create(const GLchar* name, bool withObject)
		{
			Ptr ptr = Create();

			ptr->LoadBody(name, withObject);

			return ptr;
		}

	protected:

		virtual void LoadBody(const GLchar* name, bool withObject)
		{
			SphericBody::LoadBody(name, withObject);
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

		RockyBody() : SphericBody()
		{
			type |= BodyType::Rocky;
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
			Body::Save(os);
		}

		virtual void Load(std::istream& is)
		{
			Body::Load(is);
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

			glm::dmat4 model;
			GetModelMatrix(model);

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


	class StarModel : public SphericBody
	{
		DECLARE_MEMMNG(StarModel)

	public:
		static Ptr Create(const GLchar* name, bool withObject)
		{
			Ptr ptr = Create();
			ptr->LoadBody(name, withObject);
			return ptr;
		}

		virtual void Save(std::ostream& os)
		{
			Body::Save(os);
		}

		virtual void Load(std::istream& is)
		{
			Body::Load(is);
		}

	protected:

		virtual void LoadBody(const GLchar* name, bool withObject)
		{
			SphericBody::LoadBody(name, withObject);

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

		StarModel() : SphericBody()
		{
			type |= BodyType::Star;
			object = Star::Create();
		}

	public:

		virtual void Render(Camera::Ptr& cam, Light& light)
		{
			Program::Ptr& renderProg = meshes[0]->GetProgram();

			static GLuint modelLoc = glGetUniformLocation(*renderProg, "model");
			static GLuint mvpLoc = glGetUniformLocation(*renderProg, "mvp");

			WDGS::Star* star = (WDGS::Star*)object.get();

			glm::dmat4 model;
			GetModelMatrix(model);

			glm::mat4 mvp = glm::mat4(cam->GetTransform() * model);

			renderProg->Use();
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

			meshes[0]->Render();

		}
	};
}

#endif