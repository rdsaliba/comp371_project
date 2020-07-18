#pragma once
#include <list>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
using namespace glm;
namespace ModelUtilities{
	//Constants for size standardization across the different models
	vec3 longComponentScalingVector();
	vec3 shortComponentScalingVector();
	vec3 medComponentScalingVector();

	/// Enum associated to segments building a Letter model
	enum class ComponentType {
		TOP,
		LEFT,
		BOTTOM,
		RIGHT,
		CENTER
	};

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
	mat4 rotateComponentOnZ(mat4 component);
	mat4 rotateComponentOnY(mat4 component);
	vec3 getComponentPosition(mat4 component, ComponentType componentType, ComponentSize componentSize, ModelType modelType);
};

