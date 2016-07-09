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

public:
	//GETTERS
	//===========================

};

//===============================================================
//PARTICULATE
//===============================================================

class ParticulateMixture : public Mixture {
	
public:
	ParticulateMixture();
	~ParticulateMixture();
	ParticulateMixture(std::vector<Element> theElements, double temperature);
	//GETTERS
	//===========================
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
	//GETTERS
	//===========================
};

//===============================================================
//GAS
//===============================================================

class GaseousMixture : public Mixture {
	//ParticulateMixture _suspendedSolid; //snow/ice crystals/aerosals (maybe) (someday)
	//DropletMixture _clouds;//clouds

	double _bottomElevation;
	double _topElevation;

	double _upPressure=0;//pressure acting on this parcel from below
	double _downPressure=0;//pressure acting on this parcel from above

	double _bottomPressure=0;//this parcel's pressure at the bottom
	double _topPressure=0;//this parcel's pressure at the top

	double _specificHeatCapacity=0;

	double _thermalEnergy=0;
	double _adiabaticLapseRate=0;
	double _columnWeight=0;

	double _saturationDensity=0;

public:
	GaseousMixture();
	~GaseousMixture();
	GaseousMixture(Element element, double temperature, double volume, double bottomElevation, double topElevation);
	//GaseousMixture(std::vector<Element> theElements, double temperature, double volume);

	void calculateParameters();
	double calculateBottomPressure(double downPressure);
private:
	void calculateSpecificHeatCapacity();
	void calculateColumnWeight();
	void calculateLapseRate();
	void calculateSaturationDensity();

	//Eigen::Vector3d _momentum;
	//Eigen::Vector3d _velocity;
public:
	//GETTERS
	//===========================
	double getLapseRate()const;
	double getBottomPressure()const;
	double getSaturationDensity()const;
};


