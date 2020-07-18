#include "HauModel.h"
#include "ModelUtilities.h"
using namespace ModelUtilities;
HauModel::HauModel():Model() {}
HauModel::HauModel(vec3 position, float size) : Model(position, size) {}

HauModel::~HauModel() {}

void HauModel::draw(mat4 worldRotationUpdate) {
	glBindVertexArray(this->vao);
	this->drawLetter(worldRotationUpdate);
	this->drawDigit(worldRotationUpdate);
}

void HauModel::drawLetter(mat4 worldRotationUpdate) {
	mat4 component = createComponent(ComponentSize::SHORT);
	vec3 position = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::SHORT, ModelType::LETTER);

	//Bottom
	this->drawPart(worldRotationUpdate, component, position);

	//Left-Branch
	component = rotateComponentOnZ(createComponent(ComponentSize::LONG));
	position = getComponentPosition(component, ComponentType::LEFT, ComponentSize::LONG, ModelType::LETTER);
	this->drawPart(worldRotationUpdate, component, position);

	//Right-Branch
	component = rotateComponentOnZ(createComponent(ComponentSize::LONG));
	position = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::LONG, ModelType::LETTER);
	this->drawPart(worldRotationUpdate, component, position);
}

void HauModel::drawDigit(mat4 worldRotationUpdate){
	mat4 component = createComponent(ComponentSize::SHORT);
	vec3 position = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::SHORT, ModelType::DIGIT);
	//Bottom
	this->drawPart(worldRotationUpdate, component, position);

	//Left-Branch
	component = rotateComponentOnZ(createComponent(ComponentSize::LONG));
	position = getComponentPosition(component, ComponentType::LEFT, ComponentSize::LONG, ModelType::DIGIT);
	this->drawPart(worldRotationUpdate, component, position);

	//Right-Branch
	component = rotateComponentOnZ(createComponent(ComponentSize::SHORT));
	position = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::SHORT, ModelType::DIGIT);
	this->drawPart(worldRotationUpdate, component, position);

	//Center
	component = createComponent(ComponentSize::SHORT);
	position = getComponentPosition(component, ComponentType::CENTER, ComponentSize::SHORT, ModelType::DIGIT);
	this->drawPart(worldRotationUpdate, component, position);

	//Top-Branch
	component = createComponent(ComponentSize::MED);
	position = getComponentPosition(component, ComponentType::TOP, ComponentSize::MED, ModelType::DIGIT);
	this->drawPart(worldRotationUpdate, component, position);
}