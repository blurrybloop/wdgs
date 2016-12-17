#include "pch.h"
#include "camera.h"

namespace WDGS
{

	void Camera::FocusOn(Object::Ptr& obj, double distance, double minDistance)
	{
		this->focusedOn = obj;
		this->minDistance = minDistance;
		this->distanceToFocus = glm::max(distance, minDistance);
		UpdateTransform();
	}

	Object::Ptr Camera::GetFocus()
	{
		return focusedOn;
	}

	void Camera::UpdateTransform()
	{
		glm::dquat q = glm::dquat(glm::dvec3(glm::radians(angles.x), glm::radians(angles.y), glm::radians(angles.z)));
		glm::dvec3 up = q * glm::dvec3(0.0, 1.0, 0.0);

		position = 
			glm::translate(glm::dmat4(1.0), focusedOn->worldPosition) * 
			(glm::dmat4)q * 
			glm::dvec4(0.0, 0.0, distanceToFocus, 1.0);

		lookat = glm::lookAt(position, focusedOn->worldPosition, up);
		projection = glm::perspective(fov, aspect, 100000.0, 5E+11);
		transform = projection * lookat;
	}

	void Camera::MoveIn()
	{
		distanceToFocus = glm::max(minDistance, distanceToFocus / sensitivity);
	}

	void Camera::MoveOut()
	{
		distanceToFocus *= sensitivity;
	}

	void Camera::SetSensitivity(double s)
	{
		sensitivity = s;
	}

	glm::dmat4& Camera::GetTransform()
	{
		//UpdateTransform();
		return transform;
	}

	glm::dmat4& Camera::GetLookat()
	{
		//UpdateTransform();
		return lookat;
	}

	glm::dmat4& Camera::GetProjection()
	{
		//UpdateTransform();
		return projection;
	}

}