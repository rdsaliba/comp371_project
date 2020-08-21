#include "Light.h"

Light::Light() {

}

Light::Light(vec3 pos, vec3 lookAt, vec3 up, float near, float far) {
	_pos = pos;
	_lookAt = lookAt;
	_up = up;
	_near = near;
	_far = far;
	//_proj = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); //Light with an angle
	//lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane); //Light fromt the top
	_proj = ortho(-30.0f, 30.0f, -30.0f, 30.0f, _near, _far);
	_view = glm::lookAt(_pos, _lookAt, _up);
	_spaceMatrix = _proj * _view;
	_on = true;
	_phi = 0;
	_rad = glm::length(pos);
}

vec3 Light::getPos()
{
	return _pos;
}

vec3 Light::getLookAt()
{
	return _lookAt;
}

vec3 Light::getUp()
{
	return _up;
}

mat4 Light::getProj()
{
	return _proj;
}

mat4 Light::getSpaceMatrix()
{
	return _spaceMatrix;
}

void Light::rotateCW()
{
	_phi -= 0.1f;
	updatePos();
}

void Light::rotateCCW()
{
	_phi += 0.1f;
	updatePos();
}

void Light::approach()
{
	if (_rad > 1)
	{
		_rad -= 0.5f;
		updatePos();
	}

}

void Light::backup()
{
	if (_rad <= 100)
	{
		_rad += 0.5f;
		updatePos();
	}

}

void Light::updatePos()
{
	_pos.x = _rad * glm::sin(_phi);
	_pos.z = _rad * glm::cos(_phi);
	updateLVM();
}

void Light::updateLVM()
{
	_view = glm::lookAt(_pos, _lookAt, _up);
	_spaceMatrix = _proj * _view;
}
