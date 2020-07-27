#include "WilliamModel.h"
#include "ModelUtilities.h"
using namespace ModelUtilities;

WilliamModel::~WilliamModel() { Model::~Model(); }

void WilliamModel::draw(mat4 worldRotationUpdate) {
	this->drawLetter(worldRotationUpdate);
	this->drawDigit(worldRotationUpdate);
}

void WilliamModel::drawLetter(mat4 worldRotationUpdate) {
	//Bottom
	mat4 component = createComponent(ComponentSize::MED, ComponentOrientation::HORIZONTAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::BOTTOM, ComponentSize::MED, ModelType::LETTER, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//verticale line
	component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::LEFT, ComponentSize::LONG, ModelType::LETTER, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);
}

void WilliamModel::drawDigit(mat4 worldRotationUpdate) {
	//Right
	mat4 component = createComponent(ComponentSize::LONG, ComponentOrientation::VERTICAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::RIGHT, ComponentSize::LONG, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Bottom
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	componentPosition = getComponentPosition(component, ComponentType::CENTER, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Left
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::VERTICAL);
	componentPosition = getComponentPosition(component, ComponentType::TOP_LEFT, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::VERTICAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

	//Top
	component = createComponent(ComponentSize::SHORT, ComponentOrientation::HORIZONTAL);
	componentPosition = getComponentPosition(component, ComponentType::TOP, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);
	this->drawPart(worldRotationUpdate, component, componentPosition);

}