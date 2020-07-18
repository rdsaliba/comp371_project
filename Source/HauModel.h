#include "Model.h"
class HauModel : public Model {
public:
	HauModel();
	HauModel(vec3 position, float size);
	virtual ~HauModel();
	virtual void draw(mat4 worldRotationUpdate);
	virtual void drawLetter(mat4 worldRotationUpdate);
	virtual void drawDigit(mat4 worldRotationUpdate);
};