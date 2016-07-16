#pragma once
#include "globals.h"
#include "solarRadiation.h"

#include "mixture.h"
#include "materialColumn.h"

namespace pleistocene {

//FORWARD DECLARATIONS
//=============================
namespace graphics { class Graphics; }
namespace options { class GameOptions; }

namespace simulation {

class Tile;

namespace climate {

//CONSTANTS
//i mean... kConstants
//=============================
const double kTileArea = 100000000.0; //m^2

const double kG = 9.81;		//acceleration of gravity (m/s/s)
const int kMaxLatitude = 70;
const int kSolarYear_d = 40;	//Length of a solar year in days
const int kSolarDay_h = 24;	//length of a solar day in hours
const int kHour_s = 3600;
//const int hour_s = 14400;

const double kSiderealDay_h = double(kSolarDay_h*kSolarYear_d) / double(kSolarYear_d + 1);//hours it takes earth to rotate through 2 pi radians
const double kTiltRad = 0.4101524;//radians of axial tilt
//const double tilt_rad = (M_PI / 2)*.6;

const double kSolarIntensity = 1.360;//kilo-watts per m2
const double kSolarEnergyPerHour = kSolarIntensity * kHour_s;// Kilo-Joules per hour per m2

const double kStephanBoltzmanConstant = 5.67*pow(10, -11);//kw per m2 per K4
const double kEmmisionConstantPerHour = kStephanBoltzmanConstant * kHour_s;

const double kInitialTemperatureK = 295.0;



enum elevationType {
	SUBMERGED,
	LOW_LAND,
	MID_LAND,
	HIGH_LAND
};

const double kElevationGaps = 1000;

const double kLandCutoff = 0;
const double kMidCutoff = kElevationGaps;
const double kHighCutoff = 2 * kElevationGaps;

//if unititilized, -3000 meter depth (average sea depth)
const double kDefaultDepth = -3 * kElevationGaps;

//high point of 5000 meters above and below sea level
//rarely realized due to peaking adjustment to noise function
const double kElevationAmplitude = 5 * kElevationGaps;



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


	TileClimate(my::Address A, double noiseValue) noexcept;

	void buildAdjacency(std::map<my::Direction, TileClimate*> &adjacientTileClimates) noexcept;


	//=================================================
	//GRAPHICS
	//=================================================
	static void setupTextures(graphics::Graphics &graphics) noexcept;

	void updateClimate(int elapsedTime) noexcept;//animation Update


	bool drawClimate(graphics::Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, const options::GameOptions &options) noexcept;
private:

	static std::map<std::string, std::string> _climateTextures;
	static std::map<elevationType, std::string> _elevationTextures;

	bool elevationDraw(graphics::Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, const options::GameOptions &options)noexcept;

	//Standard Draw Subroutine
	void setElevationDrawSpecs(double elevation, double &computedElevationShader, elevationType &computedElevationType)noexcept;

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
	static const int kTotalSteps = 5;

	double simulateSolarRadiation()noexcept;


	static double _valueSum;
	static int _tileCount;//not always same as number of tiles (not all tiles sea layers to compare with)
	static double _valueMean;
	static double _standardDeviation;


	//TODO

	//static void clearStatistics();//reset for new data type
	//static void newStatisticRound();//reset for new hour
	//static void computeMean();//new mean value
	//static void computeStandardDeviation();//new standard deviation

	//static double sigmaOffMean(double dataValue);//how weird is THIS DATA?

	//void applyHeatMap(double sigmasOffMean); //color filter THIS DATA FOR ME!

public:
	//GETTERS
	//===========================================
	std::vector<std::string> getMessages(const options::GameOptions &options) const noexcept;
};

}//namespace climate 
}//namespace simulation
}//namespace pleistocene