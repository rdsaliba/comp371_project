#include "Model.h"
class TaqiModel :public Model {
public:
	TaqiModel() :Model() {};
	TaqiModel(vec3 position, float size) :Model(position, size) {};
	TaqiModel(const Model& model) :Model(model) {};
	virtual ~TaqiModel();
	virtual void draw(mat4 worldRotationUpdate);
	virtual void drawLetter(mat4 worldRotationUpdate);
	virtual void drawDigit(mat4 worldRotationUpdate);
};
