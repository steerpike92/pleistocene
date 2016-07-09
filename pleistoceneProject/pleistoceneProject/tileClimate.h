#pragma once
#include "globals.h"
#include "solarRadiation.h"
#include "air.h"
#include "water.h"
#include "land.h"
#include "mixture.h"

class Tile;
class Graphics;

class TileClimate {
	Address _address;
	double _longitude_deg;
	double _latitude_deg;
	double _surfaceTemperature;
	double _surfaceElevation;
	bool _submerged;

	double _upRadiation = 0.0;
	double _backRadiation = 0.0;

	SolarRadiation _solarRadiation;//local incident radiation
	Air _air;//atmosphere
	Water _water;//oceans, seas, lakes, ponds, rivers, marshes, swamps. You name it
	Land _land;//terrestrial or submerged earth, along with rooted plant life
	std::map<Direction, TileClimate> _adjacientTileClimates;

public:
	//INIITIALIZATION
	//==============================================

	TileClimate();
	~TileClimate();
	TileClimate(Address A, double landElevation);

	void buildAdjacency(std::map<Direction, TileClimate> adjacientTileClimates);
	

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
	void simulateSolarRadiation();

	//step(0)
	void filterSolarRadiation(double solarEnergyPerHour);
	void simulateEvaporation();//including transpiration
	void simulateInfraredRadiation();

	//step(1)
	void simulatePressure();

	//step(2)
	void simulateAirflow();

	//step(3)
	void simulateCondensation();
	void simulatePrecipitation();

	//step(4)
	void simulateWaterFlow();
	void simulatePlants();
	void measureSurfaceTemperature();
	

public:
	//GETTERS
	//===========================================
	double getSurfaceTemperature() const;

	std::vector<std::string> getMessages(climate::DrawType messageType) const;

};

