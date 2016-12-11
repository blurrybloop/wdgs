#ifndef _WDGS_PHYSICS_SIMHELPERS_H
#define _WDGS_PHYSICS_SIMHELPERS_H

#include <vector>
#include "physics/simulation.h"

namespace WDGS
{
		class SimHelpers
		{
		public:
			inline static double ClampCyclic(double v, double min, double max)
			{
				while (v < min || v > max)
				{
					if (v < min) v = max - (min - v);
					else if (v > max) v = min + (v - max);
				}

				return v;
			}

			void AutoOrbit(Object* host, Object* obj)
			{
				//double d = glm::distance(obj->worldPosition, host->worldPosition);
				//obj->worldVelocity.y = sqrt(Simulation::gravityConst * host->mass / d);
			}
		};
}

#endif
