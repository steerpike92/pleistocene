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
	//SubColumn <SeaLayer> seaColumn;
	//SubColumn <AirLayer> earthColumn;

	//MaterialLayer *_bedrock = nullptr;//bottom Earth Layer (bottom overall layer)
	//MaterialLayer *_horizon = nullptr;//Horizon Layer (sort of top earth layer
	//MaterialLayer *_seaBottom = nullptr;//bottom sea Layer
	//MaterialLayer *_seaSurface = nullptr;//top sea layer
	//MaterialLayer *_boundaryLayer = nullptr;//bottom air layer
	//MaterialLayer *_stratosphere = nullptr;//top air layer (top overall layer)

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
	//MaterialLayer* buildSea(MaterialLayer* previousLayer, double seaSurfaceElevation);
	//void buildAir(MaterialLayer* previousLayer);

	//====================================================
	//SIMULATION
	//====================================================
public:
//Simulation steps
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