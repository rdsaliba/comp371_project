#include "Model.h"
class HauModel : public Model {
public:
	HauModel() :Model() {};
	HauModel(vec3 position, float size) :Model(position, size) {};
	HauModel(const Model& model) :Model(model) {};
	virtual ~HauModel();
	virtual void draw(mat4 worldRotationUpdate, GLuint textureArray[]);
	virtual void drawLetter(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawDigit(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
};