#pragma once
#include "globals.h"
#include "materialLayer.h"
#include "stateMixture.h"
#include "element.h"

template <class T>
struct SubColumn {
	std::vector<T> column;
};

class MaterialColumn {

	SubColumn <EarthLayer> _earth;
	SubColumn <HorizonLayer> _horizon;
	SubColumn <SeaLayer> _sea;
	SubColumn <AirLayer> _air;

	double _landElevation;
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


};