#include "physics/simulation.h"
#include "physics/simhelpers.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace WDGS
{
	namespace Physics
	{
		const double Simulation::gravityConst(6.6740831E-11);


		void Simulation::AddObject(Object::Ptr& obj)
		{
			objects.push_back(obj);

			k[0].resize(objects.size(), std::vector<double>(RV_LENGTH));
			k[1].resize(objects.size(), std::vector<double>(RV_LENGTH)),
			k[2].resize(objects.size(), std::vector<double>(RV_LENGTH)),
			k[3].resize(objects.size(), std::vector<double>(RV_LENGTH));

			size_t old = buf1.size();

			buf0.push_back((MaterialPoint*)obj.get());

			buf1.resize(objects.size());
			buf2.resize(objects.size());
			buf3.resize(objects.size());

			for (size_t i = old; i < objects.size(); ++i)
			{
				buf1[i] = new MaterialPoint;
				buf2[i] = new MaterialPoint;
				buf3[i] = new MaterialPoint;
			}

			scale.resize(objects.size(), std::vector<double>(RV_LENGTH));
			Delta.resize(objects.size(), std::vector<double>(RV_LENGTH));

			for (size_t i = 0; i < renderers.size(); ++i)
				renderers[i]->OnAddObject(obj);

		}

		void Simulation::RemoveObject(Object::Ptr& obj)
		{
			auto it = std::find(objects.begin(), objects.end(), obj);
			if (it != objects.end())
			{
				ptrdiff_t diff = it - objects.end();
				objects.erase(it);
				buf0.erase(buf0.begin() + diff);
			}

			for (size_t i = 0; i < renderers.size(); ++i)
				renderers[i]->OnRemoveObject(obj);
		}

		std::vector<Object::Ptr>& Simulation::GetObjects()
		{
			return objects;
		}

		void Simulation::SetTimestep(double step)
		{
			timestep = step;
			currentStep = timestep;
		}

		double Simulation::GetTimestep()
		{
			return timestep;
		}

		void Simulation::AttachRenderer(SimulationRenderer::Ptr& renderer)
		{
			renderers.push_back(renderer);
			renderer->OnAttach(this);
		}

		void Simulation::DetachRenderer(SimulationRenderer::Ptr& renderer)
		{
			auto it = std::find(renderers.begin(), renderers.end(), renderer);

			if (it != renderers.end())
			{
				renderers.erase(it);
				renderer->OnDetach(this);
			}
		}

		void Simulation::Recalc(double time)
		{
			double adt, h = 0;
			double step = (time - prevTime) * timestep;
			currentStep = glm::min(currentStep, step);

			do
			{
				double fd = RK4AdaptiveStep(buf0, buf0, currentStep, adt, 1E-1);
				h += adt;
				currentStep = glm::min(step, fd);
			} while (h + currentStep < step);

			if (step - h > DBL_EPSILON)
			{
				RK4Step(buf0, buf0, step - h);
				h += step - h;
			}

			for (size_t i = 0; i < objects.size(); ++i)
			{
				objects[i]->rotAngle += 2 * glm::pi<double>() * step / objects[i]->rotPeriod;
				objects[i]->rotAngle = SimHelpers::ClampCyclic(objects[i]->rotAngle, 0, 2 * glm::pi<double>());
			}

			prevTime = time;
			
			for (size_t i = 0; i < renderers.size(); ++i)
				renderers[i]->Render();

		}


		//Вектор производных для закона всемирного притяжения
		void Simulation::Equations(std::vector<MaterialPoint*>& objs, size_t index, std::vector<double>& flow)
		{
			double d;
			MaterialPoint* obj = objs[index];

			//производные позиций - скорости
			flow[0] = obj->worldVelocity.x;
			flow[1] = obj->worldVelocity.y;
			flow[2] = obj->worldVelocity.z;

			//производные скоростей - ускорения
			flow[3] = 0.0;
			flow[4] = 0.0;
			flow[5] = 0.0;

			//масса
			flow[6] = 0.0;

			for (size_t i = 0; i < objs.size(); ++i)
			{
				if (i != index)
				{
					d = glm::distance(obj->worldPosition, objs[i]->worldPosition);
					flow[3] += -gravityConst *  objs[i]->mass * (obj->worldPosition.x - objs[i]->worldPosition.x) / (d * d * d);
					flow[4] += -gravityConst *  objs[i]->mass * (obj->worldPosition.y - objs[i]->worldPosition.y) / (d * d * d);
					flow[5] += -gravityConst *  objs[i]->mass * (obj->worldPosition.z - objs[i]->worldPosition.z) / (d * d * d);
				}
			}
		}

		void Simulation::RK4Step(std::vector<MaterialPoint*>& in,
			std::vector<MaterialPoint*>& out,
			double dt          // fixed time step
		)
		{
			static std::vector<double> f(RV_LENGTH);
			size_t n = in.size();

			double *buf1_w, *buf3_w, *in_w, *out_w;

			for (int m = 0; m < 4; ++m)
			{
				for (size_t j = 0; j < n; ++j)
				{
					Equations(m == 0 ? in : (m % 2 ? buf1 : buf3), j, f);
					buf1_w = (double*)buf1[j], buf3_w = (double*)buf3[j], in_w = (double*)in[j];
					for (int i = 0; i < RV_LENGTH; i++) {
						k[m][j][i] = dt * f[i];
						if (m < 2)
							(m % 2 ? buf3_w : buf1_w)[i] = in_w[i] + k[m][j][i] / 2;
						else if (m == 2)
							(m % 2 ? buf3_w : buf1_w)[i] = in_w[i] + k[m][j][i];
					}
				}


			}

			for (size_t j = 0; j < n; ++j)
			{
				out_w = (double*)out[j], in_w = (double*)in[j];
				for (int i = 0; i < RV_LENGTH; i++) {
					out_w[i] = in_w[i] + (k[0][j][i] + 2 * k[1][j][i] + 2 * k[2][j][i] + k[3][j][i]) / 6;
				}
			}
		}

		double Simulation::RK4AdaptiveStep(std::vector<MaterialPoint*>& in,// returns adapted time step
			std::vector<MaterialPoint*>& out,
			double dt,              // initial time step
			double& adt,
			double accuracy)
		{
			const double SAFETY = 0.9, PGROW = -0.2, PSHRINK = -0.25, ERRCON = 1.89E-4, TINY = 1.0E-30;
			double *buf2_w, *buf3_w, *in_w, *out_w;

			int n = in.size();

			for (int j = 0; j < n; ++j)
			{
				Equations(in, j, scale[j]);
				in_w = (double*)in[j];

				for (int i = 0; i < RV_LENGTH; ++i)
					scale[j][i] = glm::abs(in_w[i]) + glm::abs(scale[j][i] * dt) + TINY;
			}

			bool exit = false;
			double error;

			while (true) {
				adt = dt;

				RK4Step(in, buf2, dt / 2);
				RK4Step(buf2, buf2, dt / 2);
				RK4Step(in, buf3, dt);

				double dt_temp, dt_temp2 = DBL_MAX;
				int cnt = n;

				for (int j = 0; j < n; ++j)
				{
					buf2_w = (double*)buf2[j], buf3_w = (double*)buf3[j];

					for (int i = 0; i < RV_LENGTH; ++i)
						Delta[j][i] = buf2_w[i] - buf3_w[i];

					error = 0.0;
					for (int i = 0; i < RV_LENGTH; i++)
						error = glm::max(glm::abs(Delta[j][i] / scale[j][i]), error);
					error /= accuracy;
					if (error <= 1)
					{
						if ((--cnt) == 0)
						{
							exit = true;
						}

						continue;
					}

					dt_temp = SAFETY * dt * glm::pow(error, PSHRINK);
					if (dt >= 0)
						dt_temp = glm::max(dt_temp, 0.1 * dt);
					else
						dt_temp = glm::min(dt_temp, 0.1 * dt);
					if (glm::abs(dt_temp) == 0.0) {
						return dt;
					}

					dt_temp2 = glm::min(dt_temp2, dt_temp);
				}

				if (exit) break;
				dt = dt_temp2;
			}

			if (error > ERRCON)
				dt *= SAFETY * glm::pow(error, PGROW);
			else
				dt *= 5;
			for (int j = 0; j < n; ++j)
			{
				out_w = (double*)out[j], buf2_w = (double*)buf2[j], buf3_w = (double*)buf3[j];
				for (int i = 0; i < RV_LENGTH; ++i)
					out_w[i] = buf2_w[i] + Delta[j][i] / 15;
			}
			return dt;
		}
	}
}