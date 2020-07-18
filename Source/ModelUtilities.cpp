//#include "ModelUtilities.h"
#include <list>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include "ModelUtilities.h"
using namespace glm;
namespace ModelUtilities {
	//Constants for size standardization across the different models
	vec3 longComponentScalingVector() { return vec3(5.0f, 1.0f, 1.0f); }
	vec3 shortComponentScalingVector() { return vec3(3.0f, 1.0f, 1.0f); }
	vec3 medComponentScalingVector() { return vec3(4.0f, 1.0f, 1.0f); }
	
	mat4 scaleComponent(mat4 component, vec3 scalingVector) {
		mat4 scaling = scale(mat4(1.0f), scalingVector);
		return component * scaling;
	}

	mat4 createComponent(ComponentSize size) {
		mat4 component = mat4(1.0f);
		switch (size) {
		case ComponentSize::SHORT:
			component = scaleComponent(mat4(1.0f), shortComponentScalingVector());
			break;
		case ComponentSize::LONG:
			component = scaleComponent(mat4(1.0f), longComponentScalingVector());
			break;
		case ComponentSize::MED:
			component = scaleComponent(mat4(1.0f), medComponentScalingVector());
			break;
		}
		return component;
	}

	mat4 rotateComponent(mat4 component, float angle, vec3 rotationVector) {
		return rotate(mat4(1.0f), radians(angle), rotationVector) * component;
	}

	mat4 rotateComponentOnZ(mat4 component) {
		vec3 rotation(0.0f, 0.0f, 1.0f);
		return rotateComponent(component, 90.0f, rotation);
	}
	
	mat4 rotateComponentOnY(mat4 component) {
		vec3 rotation(0.0f, 1.0f, 0.0f);
		return rotateComponent(component, 90.0f, rotation);
	}

	vec3 getComponentPosition(mat4 component, ComponentType componentType, ComponentSize componentSize, ModelType modelType) {
		vec3 position(0.0f, 0.5f, 0.0f);
		switch (componentType) {
		case ComponentType::TOP:
			position.y = 4.5f;
			break;
		case ComponentType::LEFT:
			position.x = -1.0f;
			break;
		case ComponentType::BOTTOM:
			position.x = 0.5f;
			break;
		case ComponentType::RIGHT:
			position.x = 2.0f;
			break;
		case ComponentType::CENTER:
			position.y = 2.5f;
			break;
		}

		switch (modelType) {
		case ModelType::LETTER:
			position.x -= 3.5f;
			break;
		case ModelType::DIGIT:
			if (componentSize == ComponentSize::MED) {
				position.x += 3.0f;
			}
			else {
				position.x += 2.5f;
			}
			break;
		}

		if (componentType == ComponentType::LEFT || componentType == ComponentType::RIGHT) {
			switch (componentSize) {
			case ComponentSize::LONG:
				position.y = 2.5f;
				break;
			case ComponentSize::SHORT:
				position.y = 1.5f;
				break;
			}
		}
		return position;
	}
}