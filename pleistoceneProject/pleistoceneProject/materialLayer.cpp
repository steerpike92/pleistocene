#include "materialLayer.h"

MaterialLayer::MaterialLayer(){}
MaterialLayer::~MaterialLayer(){}

MaterialLayer::MaterialLayer(double earthSurfaceElevation, MaterialLayer *layerBelow, double bottomElevation) :
	_earthSurfaceElevation(earthSurfaceElevation),
	_below(layerBelow),
	_stateMixture(new Mixture)
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

EarthLayer::EarthLayer(double earthSurfaceElevation, double temperature, double bottomElevation, double layerHeight) :
	MaterialLayer(earthSurfaceElevation,nullptr,bottomElevation)
{//bedrockConstructor
	using namespace elements;
	using namespace layers;

	_layerType = EARTH;

	std::vector<Element> ElementVector;
	
	Element bedrock = Element(VOLUME, BEDROCK, layerHeight, SOLID);

	std::unique_ptr<SolidMixture> localMixture(new SolidMixture(bedrock, temperature));
	_stateMixture = std::move(localMixture);


	_height = layerHeight;
	_topElevation = _bottomElevation + _height;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}

EarthLayer::EarthLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow, double layerHeight) :
	MaterialLayer(earthSurfaceElevation, layerBelow)
{//normal constructor
	using namespace elements;
	using namespace layers;

	_layerType = EARTH;

	std::vector<Element> elementVector;
	
	elementVector = generateSoil(-_bottomRelativeElevation, layerHeight);//-_bottomRelativeElevation is depth below surface
	
	std::unique_ptr<SolidMixture> localMixture(new SolidMixture(elementVector, temperature));
	_stateMixture = std::move(localMixture);
	_height = layerHeight;
	_topElevation = _bottomElevation + _height;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}


std::vector<Element> EarthLayer::generateSoil(double depth, double height)
{
	using namespace elements;
	using namespace layers::earth;

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
	default:
		LOG("not a layer type"); throw(2); return elementVector;
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
	EarthLayer(earthSurfaceElevation, temperature, layerBelow, layers::earth::topSoilHeight )//calls constructor specifically for horizon
{
	using namespace elements;
	using namespace layers;

	_layerType = HORIZON;//overwrite
}


SeaLayer::SeaLayer(){}
SeaLayer::~SeaLayer(){}

SeaLayer::SeaLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow, double fixedTopElevation) :
	MaterialLayer(earthSurfaceElevation, layerBelow)
{
	using namespace elements;
	using namespace layers;

	_layerType = layers::SEA;
	_topElevation = fixedTopElevation;
	_height = _topElevation - _bottomElevation;

	
	Element water = Element(VOLUME, WATER, _height, LIQUID);
	
	std::vector<Element> elementVector{ water };

	std::unique_ptr<LiquidMixture> localMixture(new LiquidMixture(elementVector, temperature));
	_stateMixture = std::move(localMixture);

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

	std::unique_ptr<GaseousMixture> localMixture(new GaseousMixture(air, temperature, _bottomElevation, _topElevation));
	_stateMixture = std::move(localMixture);

	_height = _topElevation - _bottomElevation;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}

std::vector<Element> AirLayer::generateAirElements(double bottomElevation, double topElevation) 
{
	using namespace elements;
	using namespace layers::air;

	double mols = expectedMolsCalculator(bottomElevation, topElevation);

	Element air = Element(MOLAR, DRY_AIR, mols, GAS);

	std::vector<Element> elementVector{ air };

	return elementVector;
}

double AirLayer::expectedHydrostaticPressureCalculator(double elevation)
{
	using namespace layers::air;

	//choose set of standard values based on elevation
	int i;
	if (elevation > StandardElevation[0]){i = 1;}
	else { i = 0; }
	
	//set of parameters
	double Po = StandardPressure[i];
	double To = StandardTemperature[i];
	double L = StandardLapseRate[i];
	double h = elevation;
	double ho = StandardElevation[i];
	double M = Md;

	//calculation
	double exponent = (g*M) / (R*L);
	double base = To / (To + L*(h - ho));

	double Pressure = Po * pow(base, exponent);

	return Pressure;
}

double AirLayer::expectedMolsCalculator(double bottomElevation, double topElevation)
{
	using namespace layers::air;
	double BottomPressure = expectedHydrostaticPressureCalculator(bottomElevation);
	double TopPressure = expectedHydrostaticPressureCalculator(topElevation);
	double PressureDifference = BottomPressure - TopPressure;
	double ExpectedMols = PressureDifference / (g*Md);

	return ExpectedMols;
}

double AirLayer::expectedTemperatureCalculator(double elevation)
{
	using namespace layers::air;

	//choose set of standard values based on elevation
	int i;
	if (elevation > StandardElevation[0]) { i = 1; }
	else { i = 0; }

	//parameters
	double To = StandardTemperature[i];
	double L = StandardLapseRate[i];
	double h = elevation;
	double ho = StandardElevation[i];

	//calculation
	double expectedTemperature = To + L*(h - ho);

	return expectedTemperature;
}

double AirLayer::lapsedTemperatureCalculator(double elevation) const
{
	using namespace layers::air;

	//choose set of standard values based on elevation
	int i;
	if (elevation > StandardElevation[0]) { i = 1; }
	else { i = 0; }

	//parameters
	double To = _stateMixture->getTemperature();
	double L = StandardLapseRate[i];//Alternatively we can calculate a moist adiabatic lapse rate
	double h = elevation;
	double ho = _bottomElevation;

	double LapsedTemperature = To + L*(h - ho);

	return LapsedTemperature;
}

double AirLayer::truePressureCalculator(double elevation) const
{
	double ExpectedMols = expectedMolsCalculator(_bottomElevation, _topElevation);
	double ExpectedTemperature = expectedTemperatureCalculator(elevation);
	double ExpectedPressure = expectedHydrostaticPressureCalculator(elevation);

	double TrueMols = _stateMixture->getMols();
	double TrueTemperature = lapsedTemperatureCalculator(elevation);

	if (ExpectedMols <= 0 || ExpectedTemperature <= 0) {LOG("pressure calculator divide by zero"); throw(2); return 0;}

	double TruePressure = (TrueMols / ExpectedMols) * (TrueTemperature / ExpectedTemperature) * ExpectedPressure;

	return TruePressure;
}
