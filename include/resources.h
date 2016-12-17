#ifndef _WDGS_RESOURCES_H
#define _WDGS_RESOURCES_H
	
#include "ini.h"

namespace WDGS
{
	class Resources
	{
		static INI ini;

		static void GetModelSection(const char* modelName, int type, std::string& out);
		static void GetEnvSection(GLint envId, std::string& out);

	public:
		static std::string GetShaderPath();
		static std::string GetEnvPath();
		static std::string GetModelPath();
		static std::string GetSimPath();

		static std::string GetModelString(const char* modelName, int type, const char* propName);
		static double GetModelDouble(const char* modelName, int type, const char* propName);
		static int GetModelInt(const char* modelName, int type, const char* propName);
		static glm::dvec3 GetModelVec3(const char* modelName, int type, const char* propName);
		static glm::dvec4 GetModelVec4(const char* modelName, int type, const char* propName);

		static void GetEnvIds(std::vector<GLint>& v);
		static const char* GetEnvString(GLint envId, const char* propName);
		static double GetEnvDouble(GLint envId, const char* propName);
		static int GetEnvInt(GLint envId, const char* propName);
		static glm::dvec3 GetEnvVec3(GLint envId, const char* propName);

		static std::string GetSimString(const char* simName, const char* propName);

	};
}

#endif