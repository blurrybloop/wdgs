#ifndef _WDGS_RESOURCES_H
#define _WDGS_RESOURCES_H
	
#include "ini.h"

namespace WDGS
{
	class Resources
	{
		static INI ini;

		static void GetModelSection(const char* modelName, int type, std::string& out);
		static void GetEnvSection(const char* envName, std::string& out);

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

		static std::string GetEnvString(const char* envName, const char* propName);
		static double GetEnvDouble(const char* envName, const char* propName);
		static int GetEnvInt(const char* envName, const char* propName);
		static glm::dvec3 GetEnvVec3(const char* envName, const char* propName);

		static std::string GetSimString(const char* simName, const char* propName);

	};
}

#endif