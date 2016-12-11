#include "physics/gravity.h"

namespace WDGS
{
	namespace Physics
	{
		const double GravityController::gravityConst(6.6740831E-11);

		void GravityController::AddMP(MaterialPoint* mp)
		{
			buf0.push_back(mp);

			k[0].push_back(std::vector<double>(MP_LENGTH));
			k[1].push_back(std::vector<double>(MP_LENGTH));
			k[2].push_back(std::vector<double>(MP_LENGTH));
			k[3].push_back(std::vector<double>(MP_LENGTH));

			buf1.push_back(new MaterialPoint);
			buf2.push_back(new MaterialPoint);
			buf3.push_back(new MaterialPoint);

			scale.push_back(std::vector<double>(MP_LENGTH));
			Delta.push_back(std::vector<double>(MP_LENGTH));
		}

		void GravityController::RemoveMP(MaterialPoint* mp)
		{
			auto it = std::find(buf0.begin(), buf0.end(), mp);
			if (it != buf0.end())
			{
				ptrdiff_t diff = it - buf0.end();
				buf0.erase(it);
				buf0.erase(buf0.begin() + diff);

				buf1.pop_back();
				buf2.pop_back();
				buf3.pop_back();

				k[0].pop_back();
				k[1].pop_back();
				k[2].pop_back();
				k[3].pop_back();

				scale.pop_back();
				Delta.pop_back();
			}

		}

		void GravityController::Refresh(double step)
		{
			double adt, h = 0;

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
		}

		//Вектор производных для закона всемирного притяжения
		void GravityController::Equations(std::vector<MaterialPoint*>& objs, size_t index, std::vector<double>& flow)
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

		void GravityController::RK4Step(std::vector<MaterialPoint*>& in,
			std::vector<MaterialPoint*>& out,
			double dt          // fixed time step
		)
		{
			static std::vector<double> f(MP_LENGTH);
			size_t n = in.size();

			double *buf1_w, *buf3_w, *in_w, *out_w;

			for (int m = 0; m < 4; ++m)
			{
				for (size_t j = 0; j < n; ++j)
				{
					Equations(m == 0 ? in : (m % 2 ? buf1 : buf3), j, f);
					buf1_w = (double*)buf1[j], buf3_w = (double*)buf3[j], in_w = (double*)in[j];
					for (int i = 0; i < MP_LENGTH; i++) {
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
				for (int i = 0; i < MP_LENGTH; i++) {
					out_w[i] = in_w[i] + (k[0][j][i] + 2 * k[1][j][i] + 2 * k[2][j][i] + k[3][j][i]) / 6;
				}
			}
		}

		double GravityController::RK4AdaptiveStep(std::vector<MaterialPoint*>& in,// returns adapted time step
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

				for (int i = 0; i < MP_LENGTH; ++i)
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

					for (int i = 0; i < MP_LENGTH; ++i)
						Delta[j][i] = buf2_w[i] - buf3_w[i];

					error = 0.0;
					for (int i = 0; i < MP_LENGTH; i++)
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
				for (int i = 0; i < MP_LENGTH; ++i)
					out_w[i] = buf2_w[i] + Delta[j][i] / 15;
			}
			return dt;
		}

	}
}