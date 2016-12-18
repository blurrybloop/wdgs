#ifndef _WDGS_OBJECT_H
#define _WDGS_OBJECT_H

#include "physics/gravity.h"
#include "memmng.h"

namespace WDGS
{
	struct Object : public Physics::MaterialPoint, public Saveable
	{
		DECLARE_MEMMNG(Object)

	public:

		enum ObjectType
		{
			General = 1, Spheric = 1 << 1, Planet = 1 << 2, Star = 1 << 3
		};

		int type;
		std::string name;

		glm::dvec3 axisInclination; //наклон оси вращения
		double rotPeriod; //период вращения
		double rotAngle;

		virtual void Save(std::ostream& os)
		{
			size_t s = name.length();
			os.write((char*)&s, sizeof(s));
			os.write(name.c_str(), s);

			os.write((char*)glm::value_ptr(worldPosition), sizeof(worldPosition));
			os.write((char*)glm::value_ptr(worldVelocity), sizeof(worldVelocity));

			os.write((char*)&mass, sizeof(mass));

			os.write((char*)glm::value_ptr(axisInclination), sizeof(axisInclination));

			os.write((char*)&rotPeriod, sizeof(rotPeriod));
			os.write((char*)&rotAngle, sizeof(rotAngle));

			/*os << name.length() << name;
			os << worldPosition.x << worldPosition.y << worldPosition.z;
			os << worldVelocity.x << worldVelocity.y << worldVelocity.z;
			os << mass;
			os << axisInclination.x << axisInclination.y << axisInclination.z;
			os << rotPeriod << rotAngle;*/
		}

		virtual void Load(std::istream& is)
		{
			size_t s = name.length();
			is.read((char*)&s, sizeof(s));
			name.resize(s, 0);
			is.read((char*)name.c_str(), s);

			is.read((char*)glm::value_ptr(worldPosition), sizeof(worldPosition));
			is.read((char*)glm::value_ptr(worldVelocity), sizeof(worldVelocity));

			is.read((char*)&mass, sizeof(mass));

			is.read((char*)glm::value_ptr(axisInclination), sizeof(axisInclination));

			is.read((char*)&rotPeriod, sizeof(rotPeriod));
			is.read((char*)&rotAngle, sizeof(rotAngle));

			//size_t len;
			//is >> len;
			//

			//is >> worldPosition.x >> worldPosition.y >> worldPosition.z;
			//is >> worldVelocity.x >> worldVelocity.y >> worldVelocity.z;
			//is >> mass;
			//is >> axisInclination.x >> axisInclination.y >> axisInclination.z;
			//is >> rotPeriod >> rotAngle;
		}

	protected:
		Object()
		{
			type = ObjectType::General;
			rotAngle = 0.0;
		}
	};


	class SphericObject : public Object
	{
		DECLARE_MEMMNG(SphericObject)

	public:
		double radius; //радиус

		SphericObject() : Object()
		{
			type |= ObjectType::Spheric;
		}

		virtual void Save(std::ostream& os)
		{
			Object::Save(os);
			int p = os.tellp();
			double huy = 2439711.1;
			char* a = (char*)&huy;

			os.write((char*)&radius, sizeof(radius));
			int p2 = os.tellp();
			//os << radius;
		}

		virtual void Load(std::istream& is)
		{
			Object::Load(is);
			int p = is.tellg();
			is.read((char*)&radius, sizeof(radius));
			int p2 = is.tellg();
			//is >> radius;
		}
	};

	class Planet : public SphericObject
	{
		DECLARE_MEMMNG(Planet)
	public:

		Planet() : SphericObject()
		{
			type |= ObjectType::Planet;
		}
	};

	class Star : public SphericObject
	{
		DECLARE_MEMMNG(Star)

	public:

		double luminosity;

		Star() : SphericObject()
		{
			type |= ObjectType::Star;
		}

		virtual void Save(std::ostream& os)
		{
			SphericObject::Save(os);
			os.write((char*)&luminosity, sizeof(luminosity));
		}

		virtual void Load(std::istream& is)
		{
			SphericObject::Load(is);
			is.read((char*)&luminosity, sizeof(luminosity));
		}
	};
}
#endif
