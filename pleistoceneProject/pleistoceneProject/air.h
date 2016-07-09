#pragma once
#include "globals.h"
#include "mixture.h"
#include "stateMixture.h"


class Graphics;

class Air {
	double _surfaceElevation;

	GaseousMixture _boundaryLayer;
	std::vector<GaseousMixture> _troposphere;
	GaseousMixture _stratosphere;
	int _layerCount;

	//up/down radiation
	//radiation incident upon layer
	//0 corresponds to surface, 1 to boundary layer, ... n-1 to stratosphere, n to space. 
	double _upRadiation[climate::air::maxLayers+2];
	double _downRadiation[climate::air::maxLayers+2];
	

	typedef std::pair<my::Direction, Air*> AirNeighbor;
	std::map<my::Direction, Air*> _airNeighbors;

	//===============================================
	//INITIALIZATION
	//===============================================
public:
	Air();
	~Air();
	Air(double surfaceElevation, double surfaceTemperature);
private:
	void buildTroposphere(double bottomElevation, double topElevation);
	void initializePressureAtElevation(double bottomElevation, double topElevation, GaseousMixture &stubMixture, climate::air::airType type);

public:
	void bond(const AirNeighbor &neighbor);

	//===============================================
	//SIMULATION
	//===============================================
	double filterSolarRadiation(double incidentSolarEnergyKJ);
	double filterAndComputeBackRadiation(double incidentInfraredEnergyKJ);
private:
	void fillRadiationArrays(double incidentInfraredEnergyKJ);
	double filterUpRadiationArray();
	double filterDownRadiationArray();

	double _backRadiation;
	double _escapedRadiation;
public:
	void buildHydroStaticPressure();
	void simulateAirflow();

	//void changeSurfaceElevation(double surfaceElevation);



	//GETTERS
	//===========================

	double getSurfaceAirPressure()const;
	double getSurfaceAirTemperature()const;

	std::vector<std::string> getMessages(climate::DrawType messageType) const;

};
