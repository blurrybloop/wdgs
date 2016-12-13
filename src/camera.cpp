#include "pch.h"
#include "camera.h"

namespace WDGS
{

	void Camera::FocusOn(Object::Ptr& obj, double distance, double minDistance)
	{
		this->focusedOn = obj;
		this->distanceToFocus = distance;
		this->minDistance = minDistance;
		UpdateTransform();
	}

	Object::Ptr Camera::GetFocus()
	{
		return focusedOn;
	}

	void Camera::UpdateTransform()
	{
		glm::dmat4 rotX, rotY, rotZ;

		double cx = glm::cos(glm::radians(angles.x)), sx = glm::sin(glm::radians(angles.x));
		double cy = glm::cos(glm::radians(angles.y)), sy = glm::sin(glm::radians(angles.y));
		double cz = glm::cos(glm::radians(angles.z)), sz = glm::sin(glm::radians(angles.z));

		rotX[0][0] = 1.0;
		rotX[1][1] = (cx);
		rotX[1][2] = (sx);
		rotX[2][1] = (-sx);
		rotX[2][2] = (cx);
		rotX[3][3] = (1.0);

		rotY[0][0] = (cy);
		rotY[0][2] = (-sy);
		rotY[1][1] = (1);
		rotY[2][0] = (sy);
		rotY[2][2] = (cy);
		rotY[3][3] = (1);

		rotZ[0][0] = (cz);
		rotZ[0][1] = (sz);
		rotZ[1][0] = (-sz);
		rotZ[1][1] = (cz);
		rotZ[2][2] = (1.0);
		rotZ[3][3] = (1.0);

		glm::dvec3 up = glm::dvec3((rotZ * rotY * rotX) * glm::dvec4(0.0, 1.0, 0.0, 0.0));
		position = glm::dvec3(glm::translate(glm::dmat4(1.0), focusedOn->worldPosition) * rotZ * rotY * rotX * glm::dvec4(0.0, 0.0, distanceToFocus, 1.0));

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