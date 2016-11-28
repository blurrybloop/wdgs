#ifndef _WDGS_CAMERA_H
#define _WDGS_CAMERA_H

#include "memmng.h"
#include "physics/object.h"
#include "glm/glm.hpp"

namespace WDGS
{
	class Camera
	{
		DECLARE_MEMMNG(Camera)

	public:

		Physics::Object::Ptr focusedOn;
		glm::dvec3 angles;
		double distanceToFocus;
		double fov;
		double aspect;
		glm::dvec4 position;
		glm::dmat4 transform;

		Camera()
		{
			distanceToFocus = 0.0;
		}


	public:
		void FocusOn(Physics::Object::Ptr& obj);

		void UpdateTransform();
		glm::dmat4& GetTransform();
	};
}

#endif