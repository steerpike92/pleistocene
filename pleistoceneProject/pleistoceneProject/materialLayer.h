#pragma once
#include "globals.h"
#include "mixture.h"

class MaterialLayer : public Mixture {

	double _bottomElevation;
	double _height;

	MaterialLayer* _layerAbove;
	MaterialLayer* _layerBelow;

public:
	MaterialLayer();
	~MaterialLayer();
	MaterialLayer(std::vector<Element> compositionElements, elements::State constructionState, 
		MaterialLayer layerBelow, double fixedTopElevation=my::FakeDouble);

private:
	
};
