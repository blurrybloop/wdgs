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

		static void TW_CALL SetInclinationXCB(const void * value, void * pthis)
		{
			((Body*)pthis)->object->axisInclination.x = glm::radians(*(const double*)value);
		}

		static void TW_CALL GetInclinationXCB(void * value, void * pthis)
		{
			*(double*)value = glm::degrees(((Body*)pthis)->object->axisInclination.x);
		}

		static void TW_CALL SetInclinationYCB(const void * value, void * pthis)
		{
			((Body*)pthis)->object->axisInclination.y = glm::radians(*(const double*)value);
		}

		static void TW_CALL GetInclinationYCB(void * value, void * pthis)
		{
			*(double*)value = glm::degrees(((Body*)pthis)->object->axisInclination.y);
		}

		static void TW_CALL SetInclinationZCB(const void * value, void * pthis)
		{
			((Body*)pthis)->object->axisInclination.z = glm::radians(*(const double*)value);
		}

		static void TW_CALL GetInclinationZCB(void * value, void * pthis)
		{
			*(double*)value = glm::degrees(((Body*)pthis)->object->axisInclination.z);
		}

		static void TW_CALL SetInclinationCB(const void * value, void * pthis)
		{
			glm::dquat q;
			const double* p = (const double*)value;

			for (int i = 0; i < 4; ++i)
				q[i] = p[i];

			((Body*)pthis)->object->axisInclination = glm::eulerAngles(q);
		}

		static void TW_CALL GetInclinationCB(void * value, void * pthis)
		{
			glm::dquat q = glm::dquat(((Body*)pthis)->object->axisInclination);

			double* p = (double*)value;

			for (int i = 0; i < 4; ++i)
				p[i] = q[i];
		}

		virtual void LoadUIBar()
		{
			bar = Bar::Create(resName.c_str());

			bar->AddRWVariable("_name", TW_TYPE_STDSTRING, "Имя", &object->name);
			bar->AddRWVariable("mass", TW_TYPE_DOUBLE, "Масса", &object->mass);

			bar->AddRWVariable("position_x", TW_TYPE_DOUBLE, "X", &object->worldPosition.x, "Координаты");
			bar->AddRWVariable("position_y", TW_TYPE_DOUBLE, "Y", &object->worldPosition.y, "Координаты");
			bar->AddRWVariable("position_z", TW_TYPE_DOUBLE, "Z", &object->worldPosition.z, "Координаты");

			bar->AddCBVariable("velocity", TW_TYPE_DOUBLE, "Скорость", NULL, GetVelocityCB, this);

			bar->AddRWVariable("velocity_x", TW_TYPE_DOUBLE, "X", &object->worldVelocity.x, "Скорость XYZ");
			bar->AddRWVariable("velocity_y", TW_TYPE_DOUBLE, "Y", &object->worldVelocity.y, "Скорость XYZ");
			bar->AddRWVariable("velocity_z", TW_TYPE_DOUBLE, "Z", &object->worldVelocity.z, "Скорость XYZ");

			bar->AddCBVariable("inclination", TW_TYPE_QUAT4D, " ", SetInclinationCB, GetInclinationCB, this, "Наклон оси");

			bar->AddCBVariable("inclination_x", TW_TYPE_DOUBLE, "X", SetInclinationXCB, GetInclinationXCB, this, "Наклон оси");
			bar->AddCBVariable("inclination_y", TW_TYPE_DOUBLE, "Y", SetInclinationYCB, GetInclinationYCB, this, "Наклон оси");
			bar->AddCBVariable("inclination_z", TW_TYPE_DOUBLE, "Z", SetInclinationZCB, GetInclinationZCB, this, "Наклон оси");

			bar->AddRWVariable("rot_period", TW_TYPE_DOUBLE, "Период вращения", &object->rotPeriod);			
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
			glm::dquat q = glm::dquat(object->axisInclination);
			glm::dvec3 axis = q * glm::dvec3(0.0, 1.0, 0.0);

			glm::dmat4 rot = glm::mat4_cast(glm::angleAxis(object->rotAngle, axis));
			mat = glm::translate(glm::dmat4(1.0), object->worldPosition) * rot;

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
		virtual ~Body() {}
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

		bool IsVisible(Camera::Ptr& cam, glm::dmat4& model, glm::mat4& mv)
		{
			SphericObject* s = (SphericObject*)object.get();

			glm::mat4 mvp = glm::mat4(cam->GetTransform() * model);

			glm::vec4 p = mvp * glm::vec4(0.0, 0.0, 0.0, 1.0);
			glm::vec4 p2 = cam->GetLookat() * model * glm::vec4(0.0, 0.0, 0.0, 1.0);

			p2.x += s->radius;
			p2 = cam->GetProjection() * p2;

			p = p / p.w;
			p2 = p2 / p2.w;

			if ((p.x - p2.x) * (p.x - p2.x) + (p.y - p2.y) * (p.y - p2.y) < 1E-6)
				return false;

			return true;
		}

		virtual void LoadUIBar()
		{
			Body::LoadUIBar();
			SphericObject* so = (SphericObject*)object.get();

			bar->AddRWVariable("radius", TW_TYPE_DOUBLE, "Радиус", &so->radius);
		}

		virtual void LoadObject(const GLchar* name)
		{
			Body::LoadObject(name);
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

			Program::Ptr prog = Program::Create();
			Shader::Ptr vs = Shader::CreateFromResource(GL_VERTEX_SHADER, "rocky");
			Shader::Ptr fs = Shader::CreateFromResource(GL_FRAGMENT_SHADER, "rocky");

			prog->AddShader(vs);
			prog->AddShader(fs);
			prog->Link();
			prog->Use();

			sphereMesh->SetProgram(prog);

			std::string path = Resources::GetModelPath() + Resources::GetModelString(name, this->type, "tex_path");
			std::string tex_path = path + "surf_diffuse.dds";
			Graphics::Texture::Ptr tex = Graphics::Texture::Create(tex_path.c_str());
			if (*tex) sphereMesh->AddTexture(tex, "surf_diffuse");

			tex_path = path + "surf_specular.dds";
			tex = Graphics::Texture::Create(tex_path.c_str());
			if (*tex) sphereMesh->AddTexture(tex, "surf_specular");
			glUniform1i(glGetUniformLocation(*prog, "has_specular"), *tex);

			tex_path = path + "surf_emission.dds";
			tex = Graphics::Texture::Create(tex_path.c_str());
			if (*tex) sphereMesh->AddTexture(tex, "surf_emission");
			glUniform1i(glGetUniformLocation(*prog, "has_emission"), *tex);

			tex_path = path + "surf_clouds.dds";
			tex = Graphics::Texture::Create(tex_path.c_str());
			if (*tex) sphereMesh->AddTexture(tex, "surf_clouds");
			glUniform1i(glGetUniformLocation(*prog, "has_clouds"), *tex);


			this->meshes.push_back(sphereMesh);

			sphereMesh = Sphere::Create();

			prog = Program::Create();

			vs = Shader::CreateFromResource(GL_VERTEX_SHADER, "rocky_athmo");
			fs = Shader::CreateFromResource(GL_FRAGMENT_SHADER, "rocky_athmo");

			prog->AddShader(vs);
			prog->AddShader(fs);
			prog->Link();

			prog->Use();

			sphereMesh->SetProgram(prog);
			this->meshes.push_back(sphereMesh);
		}

		glm::vec4 athmoColor;

		RockyBody() : SphericBody()
		{
			type |= Body::Rocky;
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
			Planet* planet = (Planet*)object.get();

			glm::dmat4 model;
			GetModelMatrix(model);

			glm::mat4 mvp = glm::mat4(cam->GetTransform() * model);

			if (!IsVisible(cam, model, mvp)) return;

			Program::Ptr& renderProg1 = meshes[0]->GetProgram();
		
			static GLuint modelLoc = glGetUniformLocation(*renderProg1, "model");
			static GLuint mvpLoc = glGetUniformLocation(*renderProg1, "mvp");
			static GLuint camLoc = glGetUniformBlockIndex(*renderProg1, "Camera");
			static GLuint lightPos = glGetUniformBlockIndex(*renderProg1, "LightSource");


			renderProg1->Use();
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

			glUniformBlockBinding(*renderProg1, lightPos, 0);
			glUniformBlockBinding(*renderProg1, camLoc, 1);

			meshes[0]->Render();
		}

		virtual void RenderAthmo(Camera::Ptr& cam, Light& light)
		{
			if (athmoColor.a == 0.0) return;


			glm::dmat4 model;
			GetModelMatrix(model);
			model = glm::scale(model, glm::dvec3(1.05, 1.05, 1.05));

			glm::mat4 mvp = glm::mat4(cam->GetTransform() * model);

			if (!IsVisible(cam, model, mvp)) return;

			Program::Ptr& renderProg = meshes[1]->GetProgram();

			static GLuint modelLoc2 = glGetUniformLocation(*renderProg, "model");
			static GLuint mvpLoc2 = glGetUniformLocation(*renderProg, "mvp");

			static GLuint camLoc2 = glGetUniformBlockIndex(*renderProg, "Camera");
			static GLuint lightPos2 = glGetUniformBlockIndex(*renderProg, "LightSource");
			static GLuint athmoLoc = glGetUniformLocation(*renderProg, "athmoColor");


			renderProg->Use();
			glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
			glUniformMatrix4fv(mvpLoc2, 1, GL_FALSE, glm::value_ptr(mvp));

			glUniformBlockBinding(*renderProg, lightPos2, 0);
			glUniformBlockBinding(*renderProg, camLoc2, 1);

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

		virtual void LoadUIBar()
		{
			SphericBody::LoadUIBar();
			WDGS::Star* s = (WDGS::Star*)object.get();

			bar->AddRWVariable("luminosity", TW_TYPE_DOUBLE, "Светимость (Вт)", &s->luminosity);
		}

		virtual void LoadObject(const GLchar* name)
		{
			SphericBody::LoadObject(name);
			WDGS::Star* s = (WDGS::Star*)object.get();
			s->luminosity = Resources::GetModelDouble(name, this->type, "luminosity");
		}

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
			type |= Body::Star;
			object = Star::Create();
		}

	public:

		virtual void Render(Camera::Ptr& cam, Light& light)
		{

			glm::dmat4 model;
			GetModelMatrix(model);

			glm::mat4 mvp = glm::mat4(cam->GetTransform() * model);


			if (!IsVisible(cam, model, mvp)) return;

			Program::Ptr& renderProg = meshes[0]->GetProgram();

			static GLuint modelLoc = glGetUniformLocation(*renderProg, "model");
			static GLuint mvpLoc = glGetUniformLocation(*renderProg, "mvp");

			static GLuint camLoc2 = glGetUniformBlockIndex(*renderProg, "Camera");
			static GLuint lightPos2 = glGetUniformBlockIndex(*renderProg, "LightSource");

			WDGS::Star* star = (WDGS::Star*)object.get();

			renderProg->Use();
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(model)));
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

			glUniformBlockBinding(*renderProg, lightPos2, 0);
			glUniformBlockBinding(*renderProg, camLoc2, 1);

			meshes[0]->Render();

		}
	};
}

#endif