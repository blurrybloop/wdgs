#ifndef _WDGS_CONFIG_H
#define _WDGS_CONFIG_H

#include "ini.h"

namespace WDGS
{
	class Config
	{
		static INI ini;

	public:
		static std::string GetString(const char* name, const char* def = "")
		{
			return ini.GetString("Application", name, def);
		}

		static int GetInt(const char* name, int def = 0)
		{
			return ini.GetInt("Application", name, def);
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
