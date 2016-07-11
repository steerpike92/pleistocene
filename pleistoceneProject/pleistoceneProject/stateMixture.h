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
	SolidMixture();
	~SolidMixture();
	SolidMixture(Element element, double temperature);
	SolidMixture(std::vector<Element> theElements, double temperature);

	void calculateParameters();
private:
	void calcualtePorosity();
	void calculatePermeability();
	void calculateGroundWaterFlow();

};

//===============================================================
//PARTICULATE
//===============================================================

class ParticulateMixture : public Mixture {
public:
	ParticulateMixture();
	~ParticulateMixture();
	ParticulateMixture(std::vector<Element> theElements, double temperature);
	
	Mixture settle(double fluidViscosity, double fluidVelocity);
};


//===============================================================
//LIQUID
//===============================================================

class LiquidMixture : public Mixture {
	//ParticulateMixture _suspendedSolids;//silt/sand/organics
	//std::vector<Element> dissolvedElements;//dissolved oxygen/salt

public:
	LiquidMixture();
	~LiquidMixture();
	LiquidMixture(Element element, double temperature);
	LiquidMixture(std::vector<Element> theElements, double temperature);
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
	DropletMixture();
	~DropletMixture();
	DropletMixture(Element element, double temperature);
	
	

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
	GaseousMixture();
	~GaseousMixture();
	GaseousMixture(Element element, double temperature, double bottomElevation, double topElevation);
	GaseousMixture(std::vector<Element> elementVector, double temperature, double bottomElevation, double topElevation);

	void simulateCondensation();
	DropletMixture filterPrecipitation(DropletMixture upperPrecipitation);

	void calculateParameters();
	
private:
	void calculateSpecificHeatCapacity();
	void calculateLapseRate();
	void calculateSaturationDensity();
public:
	//GETTERS
	//===========================
	double getLapseRate()const;
	double getSaturationDensity()const;
};


