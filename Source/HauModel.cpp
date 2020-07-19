#include "HauModel.h"
#include "ModelUtilities.h"
using namespace ModelUtilities;

HauModel::~HauModel() {}

void HauModel::draw(mat4 worldRotationUpdate) {
	glBindVertexArray(this->vao);
	this->drawLetter(worldRotationUpdate);
	this->drawDigit(worldRotationUpdate);
}

void HauModel::drawLetter(mat4 worldRotationUpdate) {
	mat4 component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	vec3 position = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::SHORT, ModelType::LETTER, ComponentOrientation::HORIZONTAL);
	
	//Bottom
	this->drawPart(worldRotationUpdate, component, position);

	//Left-Branch
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	position = getComponentPosition(component, ComponentType::LEFT, ComponentSize::LONG, ModelType::LETTER, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, position);

	//Right-Branch
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	position = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::LONG, ModelType::LETTER, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, position);
}

void HauModel::drawDigit(mat4 worldRotationUpdate){
	mat4 component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	vec3 position = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	//Bottom
	this->drawPart(worldRotationUpdate, component, position);

	//Left-Branch
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	position = getComponentPosition(component, ComponentType::LEFT, ComponentSize::LONG, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, position);

	//Right-Branch
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::VERTICAL);
	position = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, position);

	//Center
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	position = getComponentPosition(component, ComponentType::CENTER, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, position);

	//Top-Branch
	component = createComponent(ComponentSize::MED, ComponentOrientation::HORIZONTAL);
	position = getComponentPosition(component, ComponentType::TOP, ComponentSize::MED, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, position);
}