#pragma once
#include "globals.h"
#include "mixture.h"

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

	double _specificHeatCapacity=0;

	double _adiabaticLapseRate=0;

	double _saturationDensity=0;

public:
	GaseousMixture() noexcept;
	GaseousMixture(Element element, double temperature, double bottomElevation, double topElevation)  noexcept;
	GaseousMixture(std::vector<Element> elementVector, double temperature, double bottomElevation, double topElevation)  noexcept;

	void simulateCondensation() noexcept;
	DropletMixture filterPrecipitation(DropletMixture upperPrecipitation) noexcept;

	void calculateParameters() noexcept;
	
private:
	void calculateSpecificHeatCapacity() noexcept;
	void calculateLapseRate() noexcept;
	void calculateSaturationDensity() noexcept;
public:
	//GETTERS
	//===========================
	double getLapseRate() const noexcept;
	double getSaturationDensity() const noexcept;
};


