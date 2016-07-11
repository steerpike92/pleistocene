#include "materialLayer.h"

MaterialLayer::MaterialLayer(){}
MaterialLayer::~MaterialLayer(){
	if (_stateMixture != nullptr) {
		delete _stateMixture;
	}
}

MaterialLayer::MaterialLayer(double earthSurfaceElevation, MaterialLayer *layerBelow, double bottomElevation) :
	_earthSurfaceElevation(earthSurfaceElevation),
	_below(layerBelow)
{
	if (bottomElevation == my::FakeDouble) {
		_bottomElevation = _below->getTopElevation();
	}
	else {
		_bottomElevation = bottomElevation;
	}

	_bottomRelativeElevation = _bottomElevation - _earthSurfaceElevation;
}

MaterialLayer* MaterialLayer::getAbove()const { return _above; }
MaterialLayer* MaterialLayer::getBelow()const { return _below; }

double MaterialLayer::getBottomElevation()const { return _bottomElevation; }
double MaterialLayer::getTopElevation()const { return _topElevation; }


EarthLayer::EarthLayer(){}
EarthLayer::~EarthLayer(){}

EarthLayer::EarthLayer(double earthSurfaceElevation, double temperature, double bottomElevation) :
	MaterialLayer(earthSurfaceElevation,nullptr,bottomElevation)
{//bedrockConstructor
	using namespace elements;
	using namespace layers;

	_layerType = EARTH;

	std::vector<Element> ElementVector;
	
	Element bedrock = Element(VOLUME, BEDROCK, initialEarthLayerHeight, SOLID);

	_stateMixture = new SolidMixture(bedrock, temperature);
	_height = _stateMixture->getHeight();
	_topElevation = _bottomElevation + _height;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}

EarthLayer::EarthLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow) :
	MaterialLayer(earthSurfaceElevation, layerBelow)
{//normal constructor
	using namespace elements;
	using namespace layers;

	_layerType = EARTH;

	std::vector<Element> elementVector;

	
	elementVector = generateSoil(-_bottomRelativeElevation, initialEarthLayerHeight);//-_bottomRelativeElevation is depth below surface
	
	_stateMixture = new SolidMixture(elementVector, temperature);
	_height = _stateMixture->getHeight();
	_topElevation = _bottomElevation + _height;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}

EarthLayer::EarthLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow, bool horizonConstructor) :
	MaterialLayer(earthSurfaceElevation, layerBelow)
{//horizon constructor
	using namespace elements;
	using namespace layers;

	_layerType = EARTH;

	std::vector<Element> elementVector;


	elementVector = generateSoil(0,layers::topSoilHeight);//horizon depth is zero

	_stateMixture = new SolidMixture(elementVector, temperature);
	_height = _stateMixture->getHeight();



	_topElevation = _bottomElevation + _height;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}

std::vector<Element> EarthLayer::generateSoil(double depth, double height)
{
	using namespace elements;
	using namespace layers;

	int randomInt;
	double depthIndex = depth / bedrockDepth;

	Element element;
	std::vector<Element> elementVector;

	ElementType earthLayerType = determineEarthType(depthIndex);
	ElementType soilLayerType;

	switch (earthLayerType) {
	case(BEDROCK) ://impermeable bedrock layer 
		element = Element(VOLUME, BEDROCK, height, SOLID);
		elementVector.push_back(element);
		return elementVector;
	case(ROCK) ://permeable rock layer
		element = Element(VOLUME, ROCK, height, SOLID);
		elementVector.push_back(element);
		return elementVector;
	case(CLAY) ://"CLAY" is stand in for soil layer

		//up to three types of soil mixed together
		for (int i = 0; i < 3; i++) {
			soilLayerType = determineSoilType(depthIndex);
			element= Element(VOLUME, soilLayerType, height, SOLID);
			elementVector.push_back(element);
		}	
		return elementVector;
	}
}

elements::ElementType EarthLayer::determineEarthType(double depthIndex) 
{
	using namespace elements;

	//depthIndex is from 0 to 1 (0=surface, 1=bedrockBottom)

	//randomDouble is from 0 to 1;
	double randomDouble=uniformRandom();

	//soilRV : soil Random Variable from 0 to 1. 
	//low values correspond to near surface type soils (at surface, soilRV < 0.5)
	//high values correspond to bedrock types (at bedrock bottom, soilRV > 0.5)
	double soilRV = (depthIndex+randomDouble)/2;

	if (soilRV > 0.6) {return BEDROCK;}
	if (soilRV > 0.4) { return ROCK; }
	return CLAY; //stand in for "SOIL".  Move on to soil determination
}

elements::ElementType EarthLayer::determineSoilType(double depthIndex)
{
	using namespace elements;

	//Semi STUB

	//depthIndex is from 0 to 1 (0=surface, 1=bedrockBottom)
	//Not used here yet but eventually probably

	//randomDouble is from 0 to 1;
	double soilRV = uniformRandom();

	if (soilRV > 0.67) { return CLAY; }
	if (soilRV > 0.33) { return SILT; }
	return SAND;
}


HorizonLayer::HorizonLayer(){}
HorizonLayer::~HorizonLayer(){}

HorizonLayer::HorizonLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow) :
	EarthLayer(earthSurfaceElevation, temperature, layerBelow, true)//calls constructor specifically for horizon
{
	using namespace elements;
	using namespace layers;

	_layerType = HORIZON;//overwrite

	_height = _stateMixture->getHeight();
	_topElevation = _bottomElevation + _height;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}


SeaLayer::SeaLayer(){}
SeaLayer::~SeaLayer(){}

SeaLayer::SeaLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow, double fixedTopElevation) :
	MaterialLayer(earthSurfaceElevation,layerBelow)
{
	using namespace elements;
	using namespace layers;

	_layerType = SEA;

	std::vector<Element> elementVector;

	_stateMixture = new LiquidMixture(elementVector, temperature);
	_height = _stateMixture->getHeight();
	_topElevation = _bottomElevation + _height;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}


AirLayer::AirLayer(){}
AirLayer::~AirLayer(){}

AirLayer::AirLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow, double fixedTopElevation) :
	MaterialLayer(earthSurfaceElevation,layerBelow)
{
	using namespace elements;
	using namespace layers;

	_layerType = AIR;
	_topElevation = fixedTopElevation;

	std::vector<Element> air = generateAirElements(_bottomElevation, _topElevation);

	_stateMixture = new GaseousMixture(air, temperature, _bottomElevation, _topElevation);
	_height = _stateMixture->getHeight();
	_topRelativeElevation = _bottomRelativeElevation + _height;
}

std::vector<Element> AirLayer::generateAirElements(double bottomElevation, double topElevation) 
{
	using namespace elements;
	using namespace layers;

	std::vector<Element> elementVector;
	
	Element air = Element(MASS, DRY_AIR, 10, GAS);



	return elementVector;
}