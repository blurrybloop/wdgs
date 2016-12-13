#ifndef _WDGS_INI_H
#define _WDGS_INI_H

namespace WDGS
{
	class INI
	{
		std::map <std::string, std::map<std::string, std::string>> mp;
		std::string path;

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
			if (this->path.empty())
			{
				std::ifstream fs;
				this->path = path;
				fs.open(path);
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

				fs.close();
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


		void SetString(const char* section, const char* key, const char* val)
		{
			mp[section][key] = val;
		}

		void SetInt(const char* section, const char* key, int val)
		{
			char ch[20];
			sprintf(ch, "%d", val);
			SetString(section, key, ch);
		}

		void SetDouble(const char* section, const char* key, double val)
		{
			char ch[50];
			sprintf(ch, "%G", val);
			SetString(section, key, ch);
		}

		void SetVec3(const char* section, const char* key, glm::dvec3& val)
		{
			char ch[100];
			sprintf(ch, "%G,%G,%G", val[0], val[1], val[2]);
			SetString(section, key, ch);
		}

		void SetVec4(const char* section, const char* key, glm::dvec4& val)
		{
			char ch[100];
			sprintf(ch, "%G,%G,%G,%G", val[0], val[1], val[2], val[3]);
			SetString(section, key, ch);
		}

		void Save()
		{
			if (!this->path.empty())
			{
				std::ofstream fs;
				fs.open(this->path.c_str());

				for (auto it = mp.begin(); it != mp.end(); it++)
				{
					if (it->first != "")
					{
						fs << "[" << it->first << "]" << std::endl;
					}
					
					for (auto itt = mp[it->first].begin(); itt != mp[it->first].end(); itt++)
					{
						fs << itt->first << "=" << itt->second << std::endl;
					}
				}
			}
		}

	};
}

#endif

