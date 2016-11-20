
#ifndef _WDGS_CONFIG_H
#define _WDGS_CONFIG_H

#include <map>
#include <string>


namespace WDGS
{
	union VariantVal
	{
		char Int8;
		short Int16;
		int Int32;
		long long int64;
		unsigned char Uint8;
		unsigned short Uint16;
		unsigned int Uint32;
		unsigned long long Uint64;
		float SinglePrecis;
		double DoublePrecis;
	};

	class Config
	{
		static std::map<std::string, VariantVal> valData;
		static std::map<std::string, std::string> stringData;

		static bool isDataLoaded;

		static void Load()
		{
			if (!isDataLoaded)
			{
				valData["glMajorVersion"].Int32 =  3 ;
				valData["glMinorVersion"].Int32 = 2;
				valData["fullscreen"].Int32 = 0;
				valData["windowWidth"].Int32 =  800;
				valData["windowHeight"].Int32 = 600;

				stringData["title"] = "Walking Dinosaur's Gravity Simulator";

				isDataLoaded = true;
			}
		}

	public:
		static VariantVal& GetVal(const char* name)
		{
			Load();
			static VariantVal empty{ 0 };

			try
			{
				return valData.at(name);
			}
			catch (std::out_of_range&) 
			{
				return empty;
			}
		}

		static std::string& GetString(const char* name)
		{
			Load();
			
			static std::string empty("");

			try
			{
				return stringData.at(name);
			}
			catch (std::out_of_range&)
			{
				return empty;
			}
		}
	};



};

#endif
