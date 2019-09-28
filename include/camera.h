#ifndef _WDGS_CAMERA_H
#define _WDGS_CAMERA_H

#include "memmng.h"
#include "object.h"

namespace WDGS
{
	class Camera
	{
		DECLARE_MEMMNG(Camera)

	public:
		Object::Ptr focusedOn;

		glm::dvec3 angles;
		double distanceToFocus;
		double minDistance;
		double sensitivity;
		double fov;
		double aspect;
		glm::dvec3 position;

		glm::dmat4 lookat;
		glm::dmat4 projection;
		glm::dmat4 transform;

		Camera()
		{
			distanceToFocus = 0.0;
			sensitivity = 1.5;
		}


	public:
		void FocusOn(Object::Ptr& model, double distance, double mindistance = 0.0);
		Object::Ptr GetFocus();

		void MoveIn();
		void MoveOut();

		void SetSensitivity(double s);

		void UpdateTransform();
		glm::dmat4& GetTransform();
		glm::dmat4& GetLookat();
		glm::dmat4& GetProjection();
	};
}

#endif