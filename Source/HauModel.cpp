#include "HauModel.h"
#include "ModelUtilities.h"
using namespace ModelUtilities;

HauModel::~HauModel() { Model::~Model(); }

void HauModel::draw(mat4 worldRotationUpdate, GLuint textureArray[]) {
	this->drawLetter(worldRotationUpdate, textureArray[0], textureArray[1]);
	this->drawDigit(worldRotationUpdate, textureArray[0], textureArray[2]);
}

void HauModel::drawLetter(mat4 worldRotationUpdate, GLuint toggle, GLuint texture) {

	setTexture(toggle, texture);
	//Bottom
	mat4 component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::SHORT, ModelType::LETTER, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Left-Branch
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::LEFT, ComponentSize::LONG, ModelType::LETTER, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Right-Branch
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::LONG, ModelType::LETTER, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);
}

void HauModel::drawDigit(mat4 worldRotationUpdate, GLuint toggle, GLuint texture){

	setTexture(toggle, texture);
	//Bottom
	mat4 component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Left-Branch
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::LEFT, ComponentSize::LONG, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Right-Branch
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Center
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	componentPosition = getComponentPosition(component, ComponentType::CENTER, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Top-Branch
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	componentPosition = getComponentPosition(component, ComponentType::TOP, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);
}