
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace WDGS
{

	void Camera::FocusOn(Physics::Object::Ptr& obj)
	{
		this->focusedOn = obj;
		UpdateTransform();
	}

	void Camera::UpdateTransform()
	{
		position = 

			glm::rotate(
				glm::rotate(
					glm::rotate(
						glm::translate(
							glm::dmat4(1.0), focusedOn->worldPosition), 
						glm::radians(angles.x), 
						glm::dvec3(1.0, 0.0, 0.0)), 
					glm::radians(angles.y), 
					glm::dvec3(0.0, 1.0, 0.0)), 
				glm::radians(angles.z), 
				glm::dvec3(0.0, 0.0, 1.0)) *
			glm::dvec4(0.0, 0.0, distanceToFocus, 1.0);

		glm::dvec4 up = glm::rotate(
			glm::rotate(
				glm::rotate(
					glm::dmat4(1.0),
					glm::radians(angles.x),
					glm::dvec3(1.0, 0.0, 0.0)),
				glm::radians(angles.y),
				glm::dvec3(0.0, 1.0, 0.0)),
			glm::radians(angles.z),
			glm::dvec3(0.0, 0.0, 1.0)) *
			glm::dvec4(0.0, 1.0, 0.0, 0.0);

		transform = glm::perspective(fov, aspect, 1000000., 1E+20) * 
			glm::lookAt(glm::dvec3(position),
			focusedOn->worldPosition,
			glm::dvec3(up));


	}

	glm::dmat4& Camera::GetTransform()
	{
		UpdateTransform();
		return transform;
	}

}