#include "TaqiModel.h"
#include "ModelUtilities.h"
#include <iostream>

using namespace ModelUtilities;

TaqiModel::~TaqiModel() { Model::~Model(); }

void TaqiModel::draw(mat4 worldRotationUpdate, GLuint textureArray[]) {
	this->drawLetter(worldRotationUpdate, textureArray[0], textureArray[1]);
	this->drawDigit(worldRotationUpdate, textureArray[0], textureArray[2]);
}

void TaqiModel::drawLetter(mat4 worldRotationUpdate, GLuint toggle, GLuint texture) {

	//std::cerr << getToggle();
	//toggleTexture(texture);
	//std::cerr << texture[0];
	setTexture(toggle, texture);

	mat4 component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::SHORT, ModelType::LETTER, ComponentOrientation::HORIZONTAL);

	//Bottom
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Left-Branch
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::LEFT, ComponentSize::LONG, ModelType::LETTER, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Right-Branch
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::LONG, ModelType::LETTER, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Top
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	componentPosition = getComponentPosition(component, ComponentType::TOP, ComponentSize::LONG, ModelType::LETTER, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Tail
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::ANGLED_RIGHT);
	componentPosition = getComponentPosition(component, ComponentType::BOTTOM_RIGHT, ComponentSize::SHORT, ModelType::LETTER, ComponentOrientation::ANGLED_RIGHT);
	this->drawPart(worldRotationUpdate, component, componentPosition);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void TaqiModel::drawDigit(mat4 worldRotationUpdate, GLuint toggle, GLuint texture) {

	//toggleTexture(texture);
	setTexture(toggle, texture);

	//Right side of 4
	mat4 component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::LONG, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Center
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	componentPosition = getComponentPosition(component, ComponentType::CENTER, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Left side of 4
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::TOP_LEFT, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//glBindTexture(GL_TEXTURE_2D, 0);
}