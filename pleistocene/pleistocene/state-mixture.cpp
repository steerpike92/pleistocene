#include "state-mixture.h"
#include "mixture.h"

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {
namespace elements {
//===============================================================
//SOLID
//===============================================================

SolidMixture::SolidMixture() noexcept {}

SolidMixture::SolidMixture(Element element, double temperature) noexcept :
SolidMixture(std::vector<Element> {element}, temperature) {}

SolidMixture::SolidMixture(std::vector<Element> theElements, double temperature) noexcept :
	Mixture(theElements, temperature, elements::SOLID)
{
	SolidMixture::calculateParameters();
}


void SolidMixture::calculateParameters() noexcept {
	//Mixture::calculateParameters();
	calcualtePorosity();
	calculatePermeability();
	calculateGroundWaterFlow();
}

void SolidMixture::calcualtePorosity() noexcept {
	using namespace elements;
	_voidSpace = 0;
	for (const ElementPair &elementPair : _elements) {
		_voidSpace += elementPair.second.getVoidSpace();
	}
}

void SolidMixture::calculatePermeability() noexcept {
	using namespace elements;
	double totalPermeability = 0;
	for (const ElementPair &elementPair : _elements) {
		totalPermeability += elementPair.second.getPermeability()*elementPair.second.getVolume();
	}
	_permeability = totalPermeability / _totalVolume;
}
void SolidMixture::calculateGroundWaterFlow() noexcept {
}

double SolidMixture::getPermeability() const noexcept { return _permeability; }
double SolidMixture::getPorosity() const noexcept { return _voidSpace; }


//===============================================================
//PARTICULATE
//===============================================================

ParticulateMixture::ParticulateMixture() noexcept {}

ParticulateMixture::ParticulateMixture(std::vector<Element> theElements, double temperature) noexcept :
Mixture(theElements, temperature, elements::PARTICULATE)
{

}

Mixture ParticulateMixture::settle(double fluidViscosity, double fluidVelocity) noexcept
{
	return Mixture();
}


//===============================================================
//LIQUID
//===============================================================


LiquidMixture::LiquidMixture() noexcept {}

LiquidMixture::LiquidMixture(Element element, double temperature) noexcept :
LiquidMixture(std::vector<Element> {element}, temperature) {}

LiquidMixture::LiquidMixture(std::vector<Element> theElements, double temperature) noexcept :
	Mixture(theElements, temperature, elements::LIQUID) {}

//===============================================================
//DROPLET
//===============================================================

DropletMixture::DropletMixture() noexcept {}

DropletMixture::DropletMixture(Element element, double temperature) noexcept :
Mixture(element, temperature, elements::DROPLET) {}

//==============================================================
//GAS
//==============================================================

GaseousMixture::GaseousMixture() noexcept {}

GaseousMixture::GaseousMixture(Element element, double temperature, double bottomElevation, double topElevation) noexcept :
GaseousMixture(std::vector<Element>{element}, temperature, bottomElevation, topElevation)
{
}

GaseousMixture::GaseousMixture(std::vector<Element> elementVector, double temperature, double bottomElevation, double topElevation) noexcept :
Mixture(elementVector, temperature, elements::GAS, topElevation - bottomElevation),
_bottomElevation(bottomElevation),
_topElevation(topElevation)
{
	//calculateParameters();
}

void GaseousMixture::simulateCondensation() noexcept
{
}

DropletMixture GaseousMixture::filterPrecipitation(DropletMixture upperPrecipitation) noexcept
{
	return upperPrecipitation;
}

void GaseousMixture::calculateParameters() noexcept {
	//_clouds.calculateParameters();
	//Mixture::calculateParameters();

	/*calculateSpecificHeatCapacity();
	calculateSaturationDensity();
	calculateLapseRate();*/
}


void GaseousMixture::calculateSpecificHeatCapacity() noexcept {
	_specificHeatCapacity = _totalHeatCapacity / _totalMass;
}

void GaseousMixture::calculateLapseRate() noexcept {
	//stub. different from moist air
	_adiabaticLapseRate = 9.8 / _specificHeatCapacity;
}

void GaseousMixture::calculateSaturationDensity() noexcept {
	double Tc = _temperature - 273.15;
	double saturationDensity;//g/m3
	saturationDensity = 5.018 + 0.32321*Tc - 8.1841*pow(10, -3)*pow(Tc, 2) + 3.1243*pow(10, -4)*pow(Tc, 3);
	_saturationDensity = saturationDensity*0.001;//scale down to kg
	_saturationDensity = std::max(_saturationDensity, 0.0);
}


double GaseousMixture::getSaturationDensity() const noexcept {
	return _saturationDensity;
}

double GaseousMixture::getLapseRate() const noexcept {
	return _adiabaticLapseRate;
}

}//namespace elements
}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene
