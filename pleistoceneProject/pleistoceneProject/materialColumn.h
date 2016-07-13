#pragma once
#include "globals.h"
#include "materialLayer.h"
#include "stateMixture.h"
#include "element.h"


class MaterialColumn {

	std::vector<EarthLayer> _earth;
	std::vector<HorizonLayer> _horizon;
	std::vector<SeaLayer> _sea;
	std::vector<AirLayer> _air;

	double _landElevation;
	bool _submerged;
	double _initialTemperature;

	//====================================================
	//INITIALIZATION
	//====================================================
public:
	MaterialColumn();
	MaterialColumn(double landElevation, double initialTemperature);

	void buildEarthLayerSurfaces();
	void buildHorizonNeighborhood();
	void buildMaterialLayerSurfaces();

private:
	double buildEarth();
	double buildHorizon(double baseElevation);
	double buildSea(double baseElevation, double seaSurfaceElevation);
	void buildAir(double baseElevation);

	//====================================================
	//SIMULATION
	//====================================================
public:
	void filterSolarRadiation(double incidentSolarRadiation);
	void simulateEvaporation();
	void simulateInfraredRadiation();
	void simulatePressure();
	void simulateCondensation();
	void simulatePrecipitation();
	void simulateAirFlow();
	void simulateWaterFlow();
	void simulatePlants();



	
	//====================================================
	//GETTERS
	//====================================================

	double getLandElevation()const;
	double getSurfaceTemperature()const;
	double getBoundaryLayerTemperature()const;

	std::vector<std::string> getMessages(climate::DrawType messageType)const;

};