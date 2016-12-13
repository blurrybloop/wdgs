#ifndef _WDGS_PHYSICS_GRAVITY_H
#define _WDGS_PHYSICS_GRAVITY_H

#include "memmng.h"

namespace WDGS
{
	namespace Physics
	{
		struct MaterialPoint
		{
			glm::dvec3 worldPosition; //������� � ������� �����������
			glm::dvec3 worldVelocity; //�������� � ������� �����������
			double mass; //�����
		};

		const int MP_LENGTH = sizeof(MaterialPoint) / sizeof(double);

		class GravityController
		{
			DECLARE_MEMMNG(GravityController)

		public:
			static const double gravityConst;

			void AddMP(MaterialPoint* mp);
			void RemoveMP(MaterialPoint* mp);


			void Refresh(double step);

		protected:
			double currentStep;

			GravityController()
			{
				currentStep = DBL_MAX;
			}

			//������ ����������� ��� ������ ���������� ����������
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

		public:

			~GravityController()
			{
				for (size_t i = 0; i < buf0.size(); ++i)
				{
					delete buf1[i];
					delete buf2[i];
					delete buf3[i];
				}
			}

		};
	}
}

#endif