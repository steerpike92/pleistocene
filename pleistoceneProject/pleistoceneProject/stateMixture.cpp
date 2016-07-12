#include "stateMixture.h"
#include "mixture.h"


//===============================================================
//SOLID
//===============================================================

SolidMixture::SolidMixture(){}

SolidMixture::SolidMixture(Element element, double temperature) : 
	SolidMixture(std::vector<Element> {element}, temperature){}

SolidMixture::SolidMixture(std::vector<Element> theElements, double temperature) :
	Mixture(theElements, temperature, elements::SOLID) 
{
	SolidMixture::calculateParameters();
}


void SolidMixture::calculateParameters() {
	Mixture::calculateParameters();
	calcualtePorosity();
	calculatePermeability();
	calculateGroundWaterFlow();
}

void SolidMixture::calcualtePorosity() {
	using namespace elements;
	_voidSpace = 0;
	for (const ElementPair &elementPair : _elements) {
		_voidSpace += elementPair.second.getVoidSpace();
	}
}

void SolidMixture::calculatePermeability() {
	using namespace elements;
	double totalPermeability = 0;
	for (const ElementPair &elementPair : _elements) {
		totalPermeability += elementPair.second.getPermeability()*elementPair.second.getVolume();
	}
	_permeability = totalPermeability / _totalVolume;
}
void SolidMixture::calculateGroundWaterFlow() {

}

//===============================================================
//PARTICULATE
//===============================================================

ParticulateMixture::ParticulateMixture(){}

ParticulateMixture::ParticulateMixture(std::vector<Element> theElements, double temperature) :
	Mixture(theElements,temperature,elements::PARTICULATE)
{

}

Mixture ParticulateMixture::settle(double fluidViscosity, double fluidVelocity)
{
	return Mixture();
}


//===============================================================
//LIQUID
//===============================================================


LiquidMixture::LiquidMixture() {}

LiquidMixture::LiquidMixture(Element element, double temperature) :
	LiquidMixture(std::vector<Element> {element}, temperature) {}

LiquidMixture::LiquidMixture(std::vector<Element> theElements, double temperature) :
	Mixture(theElements, temperature, elements::LIQUID) {}

//===============================================================
//DROPLET
//===============================================================

DropletMixture::DropletMixture() {}

DropletMixture::DropletMixture(Element element, double temperature) :
	Mixture(element, temperature, elements::DROPLET){}

//==============================================================
//GAS
//==============================================================

GaseousMixture::GaseousMixture() {}

GaseousMixture::GaseousMixture(Element element, double temperature, double bottomElevation, double topElevation) :
	Mixture(element, temperature, elements::GAS, topElevation-bottomElevation),
	_bottomElevation(bottomElevation),
	_topElevation(topElevation)
{
	calculateParameters();
}



GaseousMixture::GaseousMixture(std::vector<Element> elementVector, double temperature, double bottomElevation, double topElevation) :
	Mixture(elementVector, temperature, elements::GAS, topElevation - bottomElevation),
	_bottomElevation(bottomElevation),
	_topElevation(topElevation)
{
	calculateParameters();
}

void GaseousMixture::simulateCondensation()
{

}

DropletMixture GaseousMixture::filterPrecipitation(DropletMixture upperPrecipitation)
{
	return upperPrecipitation;
}

void GaseousMixture::calculateParameters() {
	//_clouds.calculateParameters();
	Mixture::calculateParameters();

	calculateSpecificHeatCapacity();
	calculateSaturationDensity();
	calculateLapseRate();
}


void GaseousMixture::calculateSpecificHeatCapacity() {
	_specificHeatCapacity = _totalHeatCapacity / _totalMass;
}

void GaseousMixture::calculateLapseRate() {
	//stub. different from moist air
	_adiabaticLapseRate = 9.8 / _specificHeatCapacity;
}

void GaseousMixture::calculateSaturationDensity(){
	double Tc = _temperature - 273.15;
	double saturationDensity;//g/m3
	saturationDensity = 5.018 + 0.32321*Tc - 8.1841*pow(10, -3)*pow(Tc, 2) + 3.1243*pow(10, -4)*pow(Tc, 3);
	_saturationDensity = saturationDensity*0.001;//scale down to kg
	_saturationDensity = std::max(_saturationDensity, 0.0);
}


double GaseousMixture::getSaturationDensity() const{
	return _saturationDensity;
}

double GaseousMixture::getLapseRate() const {
	return _adiabaticLapseRate;
}