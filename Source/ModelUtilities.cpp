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

	/// <summary>
	/// Build component for model building
	/// </summary>
	/// <param name="size">Size of desired component</param>
	/// <returns></returns>
	mat4 createComponent(ComponentSize size, ComponentOrientation orientation) {
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

		switch (orientation) {
		case ComponentOrientation::VERTICAL:
			component = rotateComponentOnZ(component);
			break;
		case ComponentOrientation::ANGLED_LEFT:
			component = rotateComponent(component, -135.0f, vec3(0.0f, 0.0f, 1.0f));
			break;
		case ComponentOrientation::ANGLED_RIGHT:
			component = rotateComponent(component, 135.0f, vec3(0.0f, 0.0f, 1.0f));
			break;
		}

		return component;
	}

	mat4 rotateComponent(mat4 component, float angle, vec3 rotationVector) {
		return rotate(mat4(1.0f), radians(angle), rotationVector) * component;
	}

	/// <summary>
	/// Rotates component parallel to ground grid
	/// </summary>
	/// <param name="component"></param>
	/// <returns></returns>
	mat4 rotateComponentOnZ(mat4 component) {
		vec3 rotation(0.0f, 0.0f, 1.0f);
		return rotateComponent(component, 90.0f, rotation);
	}

	/// <summary>
	/// Rotates model component to be angled
	/// </summary>
	/// <param name="component"></param>
	/// <param name="componentType">Side where bottom of the angled component points towards. (/: LEFT, \: RIGHT)</param>
	/// <returns></returns>
	mat4 rotateComponentOnZAngled(mat4 component, ComponentType componentType) {
		vec3 rotation(0.0f, 0.0f, 1.0f);
		float angle = componentType == ComponentType::RIGHT ? 135.0f : -135.0f; //sets angle according to desired position
		return rotateComponent(component, angle, rotation);
	}
	
	mat4 rotateComponentOnY(mat4 component) {
		vec3 rotation(0.0f, 1.0f, 0.0f);
		return rotateComponent(component, 90.0f, rotation);
	}

	/// <summary>
	/// Create position vector associated with component location within a model
	/// </summary>
	/// <param name="component"></param>
	/// <param name="componentType"></param>
	/// <param name="componentSize"></param>
	/// <param name="modelType"></param>
	/// <returns></returns>
	vec3 getComponentPosition(mat4 component, ComponentType type, ComponentSize size, ModelType modelType, ComponentOrientation orientation) {
		vec3 position(0.0f, 0.5f, 0.0f);

		if (orientation == ComponentOrientation::VERTICAL) {
			switch (size) {
			case ComponentSize::LONG:
				position.y = 2.5f;
				break;
			case ComponentSize::SHORT:
				position.y = 1.5f;
				break;
			case ComponentSize::MED:
				position.y = 2.0f;
				break;
			}
		}

		switch (type) {
		case ComponentType::TOP:
			position.y += 4.0f;
			break;
		case ComponentType::LEFT:
			position.x += -1.0f;
			break;
		case ComponentType::BOTTOM:
			position.x += 0.5f;
			break;
		case ComponentType::RIGHT:
			position.x += 2.0f;
			break;
		case ComponentType::CENTER:
			position.y += 2.0f;
			break;
		case ComponentType::TOP_LEFT:
			if (orientation == ComponentOrientation::VERTICAL) {
				position.y += 2.0f;
				position.x += -1.0f;
			}
			else {
				position.y += 3.0f;
				position.x += -0.5f;
			}
			break;
		case ComponentType::TOP_RIGHT:
			if (orientation == ComponentOrientation::VERTICAL) {
				position.y += 2.0f;
				position.x += 1.0f;
			}
			else {
				position.y += 3.0f;
				position.x += 1.5f;
			}
			break;
		case ComponentType::BOTTOM_RIGHT:
			position.x += 2.0f;
			break;
		}

		switch (modelType) {
		case ModelType::LETTER:
			position.x -= 3.5f;
			break;
		case ModelType::DIGIT:
			if (size == ComponentSize::MED) {
				position.x += 3.0f;
			}
			else {
				position.x += 2.5f;
			}
			break;
		}

		return position;
	}
}