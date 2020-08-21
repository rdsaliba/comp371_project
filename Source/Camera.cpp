#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(vec3 pos, vec3 lookat, vec3 up)
{
	_pos = pos;
	_lookat = lookat;
	_up = up;
	_phi = 0;
	_rad = glm::length(pos);
}

vec3 Camera::getPos()
{
	return _pos;
}

vec3 Camera::getLookAt()
{
	return _lookat;
}

vec3 Camera::getUp()
{
	return _up;
}

void Camera::rotateCW()
{
	_phi -= 0.1;
	updatePos();
}

void Camera::rotateCCW()
{
	_phi += 0.1;
	updatePos();
}

void Camera::approach()
{
	if (_rad > 1)
	{
		_rad -= 0.5;
		updatePos();
	}

}

void Camera::backup()
{
	if (_rad <= 100)
	{
		_rad += 0.5;
		updatePos();
	}
	
}

void Camera::updatePos()
{
	_pos.x = _rad * glm::sin(_phi);
	_pos.z = _rad * glm::cos(_phi);
}
