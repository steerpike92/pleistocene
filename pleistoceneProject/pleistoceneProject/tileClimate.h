#pragma once
#include "globals.h"
#include "solarRadiation.h"

#include "mixture.h"
#include "materialColumn.h"

class Tile;
class Graphics;

class TileClimate {
	my::Address _Address;
	double _longitude_deg;
	double _latitude_deg;
	double calculateLocalInitialtemperature();

	SolarRadiation _solarRadiation;//local incident radiation
	MaterialColumn _materialColumn;

	bool _submerged = true;

	std::map<my::Direction, TileClimate> _adjacientTileClimates;

public:
	//INIITIALIZATION
	//==============================================

	TileClimate();
	TileClimate(my::Address A, double landElevation);

	void buildAdjacency(std::map<my::Direction, TileClimate> adjacientTileClimates);
	

	//=================================================
	//GRAPHICS
	//=================================================
	static void setupTextures(Graphics &graphics);

	bool drawClimate(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, climate::DrawType drawType);
private:
	static std::map<std::string, std::string> _climateDrawTextures;
	bool standardDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions);
	bool surfaceTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions);
	bool surfaceAirTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions);
public:
	void updateClimate(int elapsedTime);//animation Update



	//=================================================
	//SIMULATION
	//=================================================
	static void beginNewHour();

	static int _simulationStep;
	static bool beginNextStep();

	void simulateClimate();
private:
	static const int _totalSteps = 5;

	//step (-1)
	double simulateSolarRadiation();
	//void filterSolarRadiation(double solarEnergyPerHour);
	//void simulateEvaporation();//including transpiration
	//void simulateInfraredRadiation();

	////step(1)
	//void simulatePressure();

	////step(2)
	//void simulateAirflow();

	////step(3)
	//void simulateCondensation();
	//void simulatePrecipitation();

	//step(4)
	void simulateWaterFlow();
	void simulatePlants();	

public:
	//GETTERS
	//===========================================
	double getSurfaceTemperature() const;

	std::vector<std::string> getMessages(climate::DrawType messageType) const;

};



