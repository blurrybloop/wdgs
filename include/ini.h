#ifndef _WDGS_INI_H
#define _WDGS_INI_H

#include <glm/glm.hpp>
#include <map>
#include <string>
#include <iostream>
#include <fstream>


namespace WDGS
{
	class INI
	{
		std::map <std::string, std::map<std::string, std::string>> mp;

		static std::string trim(std::string& str)
		{
			std::string res;
			size_t endpos = str.find_last_not_of(" \t");
			size_t startpos = res.find_first_not_of(" \t");

			if (startpos == std::string::npos) startpos = 0;
			if (endpos == std::string::npos) endpos = str.length() - 1;

			return str.substr(startpos, endpos - startpos + 1);
		}

	public:

		INI(const char* path)
		{
			if (mp.empty())
			{
				std::fstream fs;
				fs.open(path, std::ios::in);
				std::string from = "";

				while (1)
				{
					std::string s;
					std::getline(fs, s);

					if (fs.eof()) break;

					for (size_t j = 0; j < s.size();)
					{
						if (s[j] == ' ')
							s.erase(j, 1);
						else
							j++;
					}

					if (s.size() == 0 || s[0] == ';')
						continue;

					if (s[0] == '[')
					{
						s.erase(0, 1);
						s.erase(s.size() - 1, 1);
						from = trim(s);
						mp[from];
					}
					else
					{
						std::string key = s.substr(0, s.find("="));
						std::string val = s.substr(s.find("=") + 1, s.size() - s.find("="));
						mp[from][trim(key)] = trim(val);
					}
				}
			}
		}

		std::string GetString(const char* section, const char* key)
		{
			auto it = mp.find(section);
			if (it != mp.end())
			{
				auto it2 = it->second.find(key);
				if (it2 != it->second.end())
					return it2->second;
			}

			return "";
		}


		double GetDouble(const char* section, const char* key)
		{
			std::string str = GetString(section, key);
			return atof(str.c_str());
		}

		int GetInt(const char* section, const char* key)
		{
			std::string str = GetString(section, key);
			return atoi(str.c_str());
		}

		glm::dvec3 GetVec3(const char* section, const char* key)
		{
			std::string str = GetString(section, key);

			glm::dvec3 v;
			size_t offset1 = 0, offset2;
			for (int i = 0; i < 3; ++i)
			{
				offset2 = str.find(',', offset1);

				std::string s = str.substr(offset1, offset2 - offset1);

				s = trim(s);
				v[i] = atof(s.c_str());

				offset1 = offset2 + 1;
			}

			return v;
		}

		glm::dvec4 GetVec4(const char* section, const char* key)
		{
			std::string str = GetString(section, key);

			glm::dvec4 v;
			size_t offset1 = 0, offset2;
			for (int i = 0; i < 4; ++i)
			{
				offset2 = str.find(',', offset1);

				std::string s = str.substr(offset1, offset2 - offset1);

				s = trim(s);
				v[i] = atof(s.c_str());

				offset1 = offset2 + 1;
			}

			return v;
		}


	};
}

#endif

