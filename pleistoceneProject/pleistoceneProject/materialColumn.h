#pragma once
#include "globals.h"
#include "materialLayer.h"
#include "stateMixture.h"
#include "element.h"

template <class layer>
class SubColumn {
private:
	vector<layer> _layers;

public:
	void push(layer const&);  // push element 
	void pop();               // pop element 
	layer top() const;            // return top element 

	bool empty() const {       // return true if empty.
		return elems.empty();
	}
};

class MaterialColumn {

	MaterialLayer *_bedrock = nullptr;//bottom Earth Layer (bottom overall layer)
	MaterialLayer *_horizon = nullptr;//Horizon Layer (sort of top earth layer
	MaterialLayer *_seaBottom = nullptr;//bottom sea Layer
	MaterialLayer *_seaSurface = nullptr;//top sea layer
	MaterialLayer *_boundaryLayer = nullptr;//bottom air layer
	MaterialLayer *_stratosphere = nullptr;//top air layer (top overall layer)

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
	MaterialLayer* buildHorizon(MaterialLayer* previousLayer);
	MaterialLayer* buildSea(MaterialLayer* previousLayer, double seaSurfaceElevation);
	void buildAir(MaterialLayer* previousLayer);


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