#include "Model.h"
class WilliamModel :public Model
{
public:
	WilliamModel() :Model() {};
	WilliamModel(vec3 position, float size) :Model(position, size) {};
	WilliamModel(const Model& model) :Model(model) {};
	virtual ~WilliamModel();
	virtual void draw(mat4 worldRotationUpdate);
	virtual void drawLetter(mat4 worldRotationUpdate);
	virtual void drawDigit(mat4 worldRotationUpdate);
};

