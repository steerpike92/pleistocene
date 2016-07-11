#pragma once
#include "globals.h"
#include "materialLayer.h"
#include "stateMixture.h"
#include "element.h"

class MaterialColumn {

	MaterialLayer *_earthRoot = nullptr;//bottom Earth Layer (bottom overall layer
	MaterialLayer *_horizonRoot = nullptr;//bottom Horizon Layer
	MaterialLayer *_seaBottomRoot = nullptr;//bottom sea Layer
	MaterialLayer *_airRoot = nullptr;//bottom air layer
	MaterialLayer *_airTail = nullptr;//top air layer (top overall layer)

	double _landElevation;
	double _initialTemperature;

	double _elevationMarker;


//initialization
public:
	MaterialColumn();
	~MaterialColumn();
	MaterialColumn(double landElevation, double initialTemperature);

	void buildEarthLayerSurfaces();
	void buildHorizonNeighborhood();
	void buildMaterialLayerSurfaces();

private:
	MaterialLayer* buildEarth();
	MaterialLayer* buildHorizon(MaterialLayer* layerHead);
	MaterialLayer* buildSea(MaterialLayer* layerHead);
	void buildAir(MaterialLayer* layerHead);


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

};