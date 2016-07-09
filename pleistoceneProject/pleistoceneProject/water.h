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

	typedef std::pair<Direction, Water*> WaterNeighbor;
	std::map<Direction, Water*> _waterNeighbors;

public:
	Water();
	~Water();
	Water(climate::water::BodyType, double landElevation, double surfaceElevevation);

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
	Address _outputTileAddress;
	std::vector<Address> _inputTileAddresses;//Tiles feeding this tile

	//GETTERS
	//===========================================
public:
	std::vector<std::string> getMessages(climate::DrawType messageType) const;
	double getSurfaceTemperature()const;
	double getSurfaceElevation()const;
	climate::water::BodyType getBodyType()const;
};
