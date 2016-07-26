#pragma once
#include "globals.h"
#include "mixture.h"

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {
namespace elements {

//===============================================================
//SOLID
//===============================================================

class SolidMixture : public Mixture {
	//Mixture _suspendedLiquid;//ground water

	double _saturation;//portion of void space occupied
	double _voidSpace;
	double _permeability;

public:
	SolidMixture() noexcept;
	SolidMixture(Element element, double temperature) noexcept;
	SolidMixture(std::vector<Element> theElements, double temperature) noexcept;

	void calculateParameters() noexcept;
private:
	void calcualtePorosity() noexcept;
	void calculatePermeability() noexcept;
	void calculateGroundWaterFlow() noexcept;

public:
	double getPorosity() const noexcept;
	double getPermeability() const noexcept;
};

//===============================================================
//PARTICULATE
//===============================================================

class ParticulateMixture : public Mixture {
public:
	ParticulateMixture() noexcept;
	ParticulateMixture(std::vector<Element> theElements, double temperature) noexcept;

	Mixture settle(double fluidViscosity, double fluidVelocity) noexcept;
};


//===============================================================
//LIQUID
//===============================================================

class LiquidMixture : public Mixture {
	//ParticulateMixture _suspendedSolids;//silt/sand/organics
	//std::vector<Element> dissolvedElements;//dissolved oxygen/salt

public:
	LiquidMixture() noexcept;
	LiquidMixture(Element element, double temperature) noexcept;
	LiquidMixture(std::vector<Element> theElements, double temperature) noexcept;
	//GETTERS
	//===========================
};

//===============================================================
//DROPLET
//===============================================================

//just clouds. So I'm not going to worry about making this too abstract
class DropletMixture : public Mixture {
	double _dropletRadius;
public:
	DropletMixture() noexcept;
	DropletMixture(Element element, double temperature) noexcept;



};

//===============================================================
//GAS
//===============================================================

class GaseousMixture : public Mixture {
	//ParticulateMixture _suspendedSolid; //snow/ice crystals/aerosals (maybe) (someday)
	DropletMixture _clouds;//clouds

	double _bottomElevation;
	double _topElevation;

	double _specificHeatCapacity = 0;

	double _adiabaticLapseRate = 0;

	double _saturationDensity = 0;

public:
	GaseousMixture() noexcept;
	GaseousMixture(Element element, double temperature, double bottomElevation, double topElevation)  noexcept;
	GaseousMixture(std::vector<Element> elementVector, double temperature, double bottomElevation, double topElevation)  noexcept;

	double _netFlow = 0;

	//=======================================
	//MIXING GAS
	//=======================================

	static void airFlow(GaseousMixture &receivingGas, GaseousMixture &givingGas, double proportion, Eigen::Vector3d flowVector) noexcept;

private:
	void push(GaseousMixture &addedGas) noexcept;
	//Mixture pull(Mixture &subtractedMixture) noexcept;

	GaseousMixture copyProportion(double proportion) const noexcept;

	void lapseTemperature(double deltaElevation) noexcept;

	void simulateCondensation() noexcept;
	DropletMixture filterPrecipitation(DropletMixture upperPrecipitation) noexcept;

	void calculateParameters() noexcept;

	


	void calculateSpecificHeatCapacity() noexcept;
	void calculateLapseRate() noexcept;
	void calculateSaturationDensity() noexcept;
public:
	//GETTERS
	//===========================
	/*double getLapseRate() const noexcept;
	double getSaturationDensity() const noexcept;*/
};

}//namespace elements
}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene