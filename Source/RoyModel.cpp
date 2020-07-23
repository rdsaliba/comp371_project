#include "RoyModel.h"
#include "ModelUtilities.h"
using namespace ModelUtilities;

RoyModel::~RoyModel() { Model::~Model(); }

void RoyModel::draw(mat4 worldRotationUpdate, GLuint textureArray[]) {
	this->drawLetter(worldRotationUpdate, textureArray[0], textureArray[1]);
	this->drawDigit(worldRotationUpdate, textureArray[0], textureArray[2]);
}

void RoyModel::drawLetter(mat4 worldRotationUpdate, GLuint toggle, GLuint texture) {

	setTexture(toggle, texture);

	//base
	mat4 component = createComponent(ComponentSize::SHORT, ComponentOrientation::VERTICAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::SHORT, ModelType::LETTER, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	component = createComponent(ComponentSize::SHORT, ComponentOrientation::ANGLED_RIGHT);
	componentPosition = getComponentPosition(component, ComponentType::TOP_LEFT, ComponentSize::SHORT, ModelType::LETTER, ComponentOrientation::ANGLED_RIGHT);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//right angled branch
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::ANGLED_LEFT);
	componentPosition = getComponentPosition(component, ComponentType::TOP_RIGHT, ComponentSize::SHORT, ModelType::LETTER, ComponentOrientation::ANGLED_LEFT);
	this->drawPart(worldRotationUpdate, component, componentPosition);
}

void RoyModel::drawDigit(mat4 worldRotationUpdate, GLuint toggle, GLuint texture) {

	setTexture(toggle, texture);

	//Bottom
	mat4 component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Left side
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::LEFT, ComponentSize::LONG, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Top
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	componentPosition = getComponentPosition(component, ComponentType::TOP, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Right side
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::LONG, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Middle
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	componentPosition = getComponentPosition(component, ComponentType::CENTER, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);
}