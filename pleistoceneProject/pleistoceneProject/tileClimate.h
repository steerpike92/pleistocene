#pragma once
#include "globals.h"
#include "solarRadiation.h"

#include "mixture.h"
#include "materialColumn.h"

namespace pleistocene {

namespace graphics { class Graphics; }
namespace options { class GameOptions; }

class Tile;

namespace climate {



class TileClimate {
	my::Address _Address;
	double _longitude_deg;
	double _latitude_deg;
	double calculateLocalInitialtemperature() noexcept;

	SolarRadiation _solarRadiation;//local incident radiation
	layers::MaterialColumn _materialColumn;

	std::map<my::Direction, TileClimate*> _adjacientTileClimates;

public:
	//INIITIALIZATION
	//==============================================

	TileClimate() noexcept;


	TileClimate(my::Address A, double landElevation) noexcept;

	void buildAdjacency(std::map<my::Direction, TileClimate*> &adjacientTileClimates) noexcept;


	//=================================================
	//GRAPHICS
	//=================================================
	static void setupTextures(graphics::Graphics &graphics) noexcept;

	void updateClimate(int elapsedTime) noexcept;//animation Update


	bool drawClimate(graphics::Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, const options::GameOptions &options) noexcept;
private:

	static std::map<std::string, std::string> _climateTextures;
	static std::map<climate::land::elevationType, std::string> _elevationTextures;

	bool elevationDraw(graphics::Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, const options::GameOptions &options)noexcept;

	//Standard Draw Subroutine
	void setElevationDrawSpecs(double elevation, double &computedElevationShader, climate::land::elevationType &computedElevationType)noexcept;

	bool temperatureDraw(graphics::Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, const options::GameOptions &options)noexcept;
	//bool surfaceAirTemperatureDraw(graphics::Graphics &graphics, std::vector<SDL_Rect> onScreenPositions)noexcept;

	//bool materialDraw(graphics::Graphics &graphics, std::vector<SDL_Rect> onScreenPositions)noexcept;


public:
	//=================================================
	//SIMULATION
	//=================================================
	static void beginNewHour()noexcept;

	static int _simulationStep;
	static bool beginNextStep()noexcept;

	void simulateClimate()noexcept;
private:
	static const int _totalSteps = 5;

	double simulateSolarRadiation()noexcept;


	static double _valueSum;
	static int _tileCount;//not always same as number of tiles (not all tiles sea layers to compare with)
	static double _valueMean;
	static double _standardDeviation;



	static void clearStatistics();//reset for new data type
	static void newStatisticRound();//reset for new hour
	static void computeMean();//new mean value
	static void computeStandardDeviation();//new standard deviation
	
	static double sigmaOffMean(double dataValue);//how weird is THIS DATA?

	void applyHeatMap(double sigmasOffMean); //color filter THIS DATA FOR ME!

public:
	//GETTERS
	//===========================================
	std::vector<std::string> getMessages(const options::GameOptions &options) const noexcept;
};

}//namespace climate
}//namespace pleistocene