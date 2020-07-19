#include "TaqiModel.h"
#include "ModelUtilities.h"
using namespace ModelUtilities;

TaqiModel::~TaqiModel() { Model::~Model(); }

void TaqiModel::draw(mat4 worldRotationUpdate) {
	this->drawLetter(worldRotationUpdate);
	this->drawDigit(worldRotationUpdate);
}

void TaqiModel::drawLetter(mat4 worldRotationUpdate) {
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
}

void TaqiModel::drawDigit(mat4 worldRotationUpdate) {
	//TO DO
}