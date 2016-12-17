#include "pch.h"
#include "resources.h"
#include "body.h"

namespace WDGS
{
	INI Resources::ini("res/resources.ini");

	std::string Resources::GetShaderPath()
	{
		return ini.GetString("Path", "shaders");
	}

	std::string Resources::GetEnvPath()
	{
		return ini.GetString("Path", "environments");
	}

	std::string Resources::GetModelPath()
	{
		return ini.GetString("Path", "models");
	}

	std::string Resources::GetSimPath()
	{
		return ini.GetString("Path", "simulations");
	}

	void Resources::GetModelSection(const char* modelName, int type, std::string& out)
	{
		out = "Body";
		switch (type)
		{
		case Body::Rocky:
			out += ".Planet";
			break;

		case Body::Star:
			out += ".Star";
			break;
		}

		out += '.';
		out += modelName;
	}

	std::string Resources::GetModelString(const char* modelName, int type, const char* propName)
	{
		std::string s;
		GetModelSection(modelName, type, s);
		return ini.GetString(s.c_str(), propName);
	}

	double Resources::GetModelDouble(const char* modelName, int type, const char* propName)
	{
		std::string s;
		GetModelSection(modelName, type, s);
		return ini.GetDouble(s.c_str(), propName);
	}

	int Resources::GetModelInt(const char* modelName, int type, const char* propName)
	{
		std::string s;
		GetModelSection(modelName, type, s);
		return ini.GetInt(s.c_str(), propName);
	}

	glm::dvec3 Resources::GetModelVec3(const char* modelName, int type, const char* propName)
	{
		std::string s;
		GetModelSection(modelName, type, s);
		return ini.GetVec3(s.c_str(), propName);
	}

	glm::dvec4 Resources::GetModelVec4(const char* modelName, int type, const char* propName)
	{
		std::string s;
		GetModelSection(modelName, type, s);
		return ini.GetVec4(s.c_str(), propName);
	}

	void Resources::GetEnvSection(GLint envId, std::string& out)
	{
		char ch[15];
		sprintf(ch, "%d", envId);
		out = "Environment.";
		out += ch;
	}

	void Resources::GetEnvIds(std::vector<GLint>& v)
	{
		auto m = ini.GetMap();
		const char* str = "Environment.";
		int len = strlen(str);
		std::string n;
		
		for (auto it = m.begin(); it != m.end(); ++it)
		{
			if (!it->first.compare(0, len, str))
			{
				n = it->first.substr(len);
				v.push_back(atoi(n.c_str()));
			}
		}

	}

	const char* Resources::GetEnvString(GLint envId, const char* propName)
	{
		std::string s;
		GetEnvSection(envId, s);
		return ini.GetString(s.c_str(), propName);
	}

	double Resources::GetEnvDouble(GLint envId, const char* propName)
	{
		std::string s;
		GetEnvSection(envId, s);
		return ini.GetDouble(s.c_str(), propName);
	}

	int Resources::GetEnvInt(GLint envId, const char* propName)
	{
		std::string s;
		GetEnvSection(envId, s);
		return ini.GetInt(s.c_str(), propName);
	}

	glm::dvec3 Resources::GetEnvVec3(GLint envId, const char* propName)
	{
		std::string s;
		GetEnvSection(envId, s);
		return ini.GetVec3(s.c_str(), propName);
	}

	std::string Resources::GetSimString(const char* simName, const char* propName)
	{
		std::string s = "Simulation.";
		s += simName;
		return ini.GetString(s.c_str(), propName);
	}

}