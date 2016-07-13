#pragma once
#include "globals.h"
#include "materialLayer.h"
#include "stateMixture.h"
#include "element.h"


class MaterialColumn {
	//Object owning vectors
	std::vector<EarthLayer> _earth;
	std::vector<HorizonLayer> _horizon;
	std::vector<SeaLayer> _sea;
	std::vector<AirLayer> _air;

	//non owning vector with pointers to all layers
	std::vector<MaterialLayer*> _column;
	

	double _landElevation;
	bool _submerged;
	double _initialTemperature;

	std::map<my::Direction, MaterialColumn*> _adjacientColumns;

	//====================================================
	//INITIALIZATION
	//====================================================
public:
	MaterialColumn();
	MaterialColumn(double landElevation, double initialTemperature);

	void buildAdjacency(std::map<my::Direction, MaterialColumn*> &adjacientColumns);

private:
	//layer builders
	double buildEarth();
	double buildHorizon(double baseElevation);
	double buildSea(double baseElevation, double seaSurfaceElevation);
	void buildAir(double baseElevation);

	void buildUniversalColumn();

	//layer relationship builders
	void buildMaterialLayerSurfaces();

	void buildVerticalSurfaces();
	void buildNeighborSurfaces(my::Direction);

	void buildEarthLayerSurfaces();
	void buildHorizonNeighborhood();

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