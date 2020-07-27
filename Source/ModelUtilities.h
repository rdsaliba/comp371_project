#pragma once
#include <list>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <vector>
using namespace glm;
namespace ModelUtilities{
	//Constants for size standardization across the different models
	vec3 longComponentScalingVector(); //5 units wide
	vec3 shortComponentScalingVector(); //3 units wide
	vec3 medComponentScalingVector(); //4 units wide

	///Segment positions for model building
	enum class ComponentType {
		TOP,
		LEFT,
		BOTTOM,
		RIGHT,
		CENTER,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_RIGHT
	};

	enum class ComponentOrientation {
		HORIZONTAL,
		VERTICAL,
		ANGLED_LEFT,
		ANGLED_RIGHT
	};

	//Size of desired component to generate for model building
	enum class ComponentSize {
		SHORT,
		LONG,
		MED
	};

	enum class ModelType {
		LETTER,
		DIGIT
	};

	mat4 createComponent(ComponentSize size, ComponentOrientation orientation);
	mat4 scaleComponent(mat4 component, vec3 scalingVector);
	mat4 rotateComponent(mat4 component, float angle, vec3 rotationVector);
	mat4 rotateComponentOnZ(mat4 component); //rotate component 90 degrees along the Z axis
	mat4 rotateComponentOnZAngled(mat4 component, ComponentType componentType);
	mat4 rotateComponentOnY(mat4 component); //rotate component 90 degrees along the Y axis
	vec3 getComponentPosition(mat4 component, ComponentType type, ComponentSize size, ModelType modelType, ComponentOrientation orientation);
};

