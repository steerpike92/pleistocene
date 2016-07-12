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

	void updateClimate(int elapsedTime);//animation Update


	bool drawClimate(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, climate::DrawType drawType);
private:

	static std::map<std::string, std::string> _climateTextures;
	static std::map<climate::land::elevationType, std::string> _elevationTextures;

	bool standardDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions);

	//Standard Draw Subroutine
	void setElevationDrawSpecs(double elevation, double &computedElevationShader, climate::land::elevationType &computedElevationType);

	bool surfaceTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions);
	bool surfaceAirTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions);
public:
	//=================================================
	//SIMULATION
	//=================================================
	static void beginNewHour();

	static int _simulationStep;
	static bool beginNextStep();

	void simulateClimate();
private:
	static const int _totalSteps = 5;

	double simulateSolarRadiation();


public:
	//GETTERS
	//===========================================
	std::vector<std::string> getMessages(climate::DrawType messageType) const;
};



