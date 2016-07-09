#pragma once
#include "globals.h"
#include "mixture.h"
#include "stateMixture.h"

class Graphics;

class Water {
	double _landElevation;
	double _surfaceElevation;
	climate::water::BodyType _bodyType;
	static std::map<climate::water::BodyType, std::string> _waterTextures;
	double _solarFraction;
	double _elevationShader;
	LiquidMixture _waterSurface;

	typedef std::pair<my::Direction, Water*> WaterNeighbor;
	std::map<my::Direction, Water*> _waterNeighbors;

public:
	Water();
	~Water();
	Water(climate::water::BodyType, double landElevation, double surfaceElevevation, double localInitialTemperature);

	void bond(const WaterNeighbor &neighbor);

	double filterSolarRadiation(double incidentSolarEnergyKJ);
	double filterInfraredRadiation(double incidentInfraredEnergyKJ);
	double emitInfraredRadiation();

	bool draw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, climate::DrawType drawType);
	static void setupTextures(Graphics &graphics);

private:
	void clearFlow();
	void calculateFlow();

	//fresh water flows to neighbor tile with least elevation (watershed)
	my::Address _outputtileAddress;
	std::vector<my::Address> _inputtileAddresses;//Tiles feeding this tile

	//GETTERS
	//===========================================
public:
	std::vector<std::string> getMessages(climate::DrawType messageType) const;
	double getSurfaceTemperature()const;
	double getSurfaceElevation()const;
	climate::water::BodyType getBodyType()const;
};
