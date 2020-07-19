#include "RoyModel.h"
#include "ModelUtilities.h"
using namespace ModelUtilities;

RoyModel::~RoyModel() { Model::~Model(); }

void RoyModel::draw(mat4 worldRotationUpdate) {
	this->drawLetter(worldRotationUpdate);
	this->drawDigit(worldRotationUpdate);
}

void RoyModel::drawLetter(mat4 worldRotationUpdate) {
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

void RoyModel::drawDigit(mat4 worldRotationUpdate) {
	//TO DO
}