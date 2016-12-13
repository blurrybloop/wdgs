
#ifndef _WDGS_CONFIG_H
#define _WDGS_CONFIG_H

#include <map>
#include <string>

#include "ini.h"

namespace WDGS
{
	class Config
	{
		static INI ini;

	public:
		static std::string GetString(const char* name)
		{
			return ini.GetString("Application", name);
		}

		static int GetInt(const char* name)
		{
			return ini.GetInt("Application", name);
		}

		static void SetInt(const char* name, int val)
		{
			ini.SetInt("Application", name, val);
		}

		static void SetString(const char* name, const char* val)
		{
			ini.SetString("Application", name, val);
		}

		static void Save()
		{
			ini.Save();
		}
	};



};

#endif
