#include "materialLayer.h"

MaterialLayer::MaterialLayer(){}
MaterialLayer::~MaterialLayer(){delete _stateMixture;}

MaterialLayer::MaterialLayer(std::vector<Element> compositionElements, double temperature, double bottomElevation) :
	_below(nullptr),
	_bottomElevation(bottomElevation)
{
	_stateMixture = new SolidMixture(compositionElements, temperature);
	_height = _stateMixture->getHeight();
	_topElevation = _bottomElevation + _height;
}

MaterialLayer::MaterialLayer(std::vector<Element> compositionElements, elements::State state,
	double temperature, MaterialLayer *layerBelow, double fixedTopElevation) :
	_below(layerBelow)
{
	_bottomElevation = _below->_topElevation;
	_below->_above = this;

	using namespace elements;
	switch (state) {
	case(SOLID) :
		_stateMixture = new SolidMixture(compositionElements, temperature);
		_height = _stateMixture->getHeight();
		_topElevation = _bottomElevation + _height;
		break;
	case(LIQUID) :
		_stateMixture = new LiquidMixture(compositionElements, temperature);
		_height = _stateMixture->getHeight();
		_topElevation = _bottomElevation + _height;
		break;
	case(GAS) :
		_stateMixture = new GaseousMixture(*compositionElements.begin(), temperature, _bottomElevation, _topElevation);
		break;
	}

}

MaterialLayer* MaterialLayer::getAbove()const { return _above; }
MaterialLayer* MaterialLayer::getBelow()const { return _below; }


MaterialColumn::MaterialColumn(){}

MaterialColumn::~MaterialColumn()
{
	if (_bedrockHead != nullptr) {
		MaterialLayer *head = _bedrockHead;
		MaterialLayer *temp = head->getAbove();
		delete head;

		while (temp != nullptr) {
			head = temp;
			temp = head->getAbove();
			delete head;
		}

		//DELETE MATERIAL LAYERS
	}
}


MaterialColumn::MaterialColumn(double landElevation)
{
	_elevationTop = landElevation;

}

void MaterialColumn::filterSolarRadiation(double incidentSolarRadiation)
{
}

void MaterialColumn::simulateInfraredRadiation()
{
}

void MaterialColumn::simulateEvaporation()
{
}

void MaterialColumn::simulateCondensation()
{
}

void MaterialColumn::simulatePrecipitation()
{
}

bool MaterialColumn::buildMaterialLayer()
{
	//returns true while it needs to be called again
	//returns false after creating the stratosphere


}
