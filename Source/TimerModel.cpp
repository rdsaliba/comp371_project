#include "TimerModel.h"
#include "ModelUtilities.h"

using namespace ModelUtilities;

TimerModel::~TimerModel() { Model::~Model(); }

void TimerModel::draw(mat4 worldRotationUpdate, GLuint textureArray[]) {
	this->drawTimerFarLeft(worldRotationUpdate, textureArray[0], textureArray[34]);
	this->drawTimerLeft(worldRotationUpdate, textureArray[0], textureArray[33]);
	this->drawTimerCenter(worldRotationUpdate, textureArray[0], textureArray[32]);
	this->drawTimerRight(worldRotationUpdate, textureArray[0], textureArray[31]);
}

void TimerModel::drawTimerFarLeft(mat4 worldRotationUpdate, GLuint toggle, GLuint texture)
{
	setTexture(toggle, texture);

	mat4 component = createComponent(ComponentSize::MED, ComponentOrientation::HORIZONTAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::TIMER_FARLEFT, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);

	//Bottom
	this->drawPart(worldRotationUpdate, component, componentPosition);


}

void TimerModel::drawTimerLeft(mat4 worldRotationUpdate, GLuint toggle, GLuint texture)
{
	setTexture(toggle, texture);

	mat4 component = createComponent(ComponentSize::MED, ComponentOrientation::HORIZONTAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::TIMER_LEFT, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);

	//Bottom
	this->drawPart(worldRotationUpdate, component, componentPosition);


}

void TimerModel::drawTimerCenter(mat4 worldRotationUpdate, GLuint toggle, GLuint texture)
{
	setTexture(toggle, texture);

	mat4 component = createComponent(ComponentSize::MED, ComponentOrientation::HORIZONTAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::TIMER_CENTER, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);

	//Bottom
	this->drawPart(worldRotationUpdate, component, componentPosition);


}

void TimerModel::drawTimerRight(mat4 worldRotationUpdate, GLuint toggle, GLuint texture)
{
	setTexture(toggle, texture);

	mat4 component = createComponent(ComponentSize::MED, ComponentOrientation::HORIZONTAL);
	vec3 componentPosition = getComponentPosition(component, ComponentType::TIMER_RIGHT, ComponentSize::SHORT, ModelType::DIGIT, ComponentOrientation::HORIZONTAL);

	//Bottom
	this->drawPart(worldRotationUpdate, component, componentPosition);


}


