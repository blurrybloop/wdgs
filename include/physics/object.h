#ifndef _WDGS_PHYSICS_OBJECT_H
#define _WDGS_PHYSICS_OBJECT_H

#include "memmng.h"
#include <glm/glm.hpp>
#include <string>

namespace WDGS
{
	namespace Physics
	{
		struct MaterialPoint
		{
			glm::dvec3 worldPosition; //позиция в мировых координатах
			glm::dvec3 worldVelocity; //скорость в мировых координатах
			double mass; //масса

		};

		struct Object : public MaterialPoint
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
			Star() : SphericObject()
			{
				type |= ObjectType::Star;
			}
		};

	}
}
#endif
