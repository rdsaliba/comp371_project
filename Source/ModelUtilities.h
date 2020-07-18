#pragma once
#include <list>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
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
		CENTER
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

	mat4 createComponent(ComponentSize size);
	mat4 scaleComponent(mat4 component, vec3 scalingVector);
	mat4 rotateComponent(mat4 component, float angle, vec3 rotationVector);
	mat4 rotateComponentOnZ(mat4 component); //rotate component 90 degrees along the Z axis
	mat4 rotateComponentOnY(mat4 component); //rotate component 90 degrees along the Y axis
	vec3 getComponentPosition(mat4 component, ComponentType componentType, ComponentSize componentSize, ModelType modelType);

};

