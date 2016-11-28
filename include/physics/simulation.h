#ifndef _WDGS_PHYSICS_SIMULATION_H
#define _WDGS_PHYSICS_SIMULATION_H

#include <vector>

#include "memmng.h"

#include "physics/object.h"
#include "simrenderer.h"

namespace WDGS
{
	namespace Physics
	{
		const int RV_LENGTH = sizeof(MaterialPoint) / sizeof(double);

		class Simulation
		{
			DECLARE_MEMMNG(Simulation)

		protected:
			double timestep;
			double currentStep;
			double prevTime;
			std::vector<Object::Ptr> objects;
			std::vector<SimulationRenderer::Ptr> renderers;

		public:
			static const double gravityConst;

			Simulation()
			{
				prevTime = 0;
			}

			~Simulation()
			{
				for (size_t i = 0; i < objects.size(); ++i)
				{
					delete buf1[i];
					delete buf2[i];
					delete buf3[i];
				}
			}

			void Recalc(double time);
			void SetTimestep(double step);
			double GetTimestep();

			void AddObject(Object::Ptr& obj);
			void RemoveObject(Object::Ptr& obj);

			void AttachRenderer(SimulationRenderer::Ptr& renderer);
			void DetachRenderer(SimulationRenderer::Ptr& renderer);

		protected:
			//Вектор производных для закона всемирного притяжения
			static void Equations(std::vector<MaterialPoint*>& objs, size_t index, std::vector<double>& flow);

			std::vector<std::vector<double>> k[4];
			std::vector<MaterialPoint*> buf0, buf1, buf2, buf3;
			std::vector<std::vector<double>> scale;
			std::vector<std::vector<double>> Delta;


			void RK4Step(std::vector<MaterialPoint*>& in,
				std::vector<MaterialPoint*>& out,
				double dt          // fixed time step
			);

			double RK4AdaptiveStep(std::vector<MaterialPoint*>& in,// returns adapted time step
				std::vector<MaterialPoint*>& out,
				double dt,              // initial time step
				double& adt,
				double accuracy = 1e-6);
		};


	}
}

#endif