#pragma once
#include "globals.h"
#include "solar-radiation.h"

#include "mixture.h"
#include "material-column.h"

namespace pleistocene {

//FORWARD DECLARATIONS
//=============================
namespace graphics { class Graphics; }

namespace simulation {

class Tile;
struct StatRequest;

namespace climate {

//CONSTANTS
//i mean... kConstants
//=============================
const double kTileArea = 100000000.0; //m^2

const double kG = 9.81;		//acceleration of gravity (m/s/s)
const int kMaxLatitude = 80;
const int kSolarYear_d = 60;	//Length of a solar year in days
const int kSolarDay_h = 24;	//length of a solar day in hours
const int kHour_s = 3600;

//longer hour allows for faster simulation of large time, but introduces problems where many assumptions are no longer true
//for instance, tiles baking in the sun only get to release radiation afterwards (although this might get changed)
//this leads to super hot tiles which (due to the T^4 scaling of radiation) then overestimates the emitted radiaion
//const int hour_s = 14400;

const double kSiderealDay_h = double(kSolarDay_h*kSolarYear_d) / double(kSolarYear_d + 1);//hours it takes earth to rotate through 2 pi radians
const double kTiltRad = 0.4101524;//radians of axial tilt
//const double kTiltRad = (M_PI / 2)*.6;

const double kSolarIntensity = 1.360;//kilo-watts per m2
const double kSolarEnergyPerHour = kSolarIntensity * kHour_s;// Kilo-Joules per hour per m2

const double kStephanBoltzmanConstant = 5.67*pow(10, -11);//kw per m2 per K4
const double kEmissionConstantPerHour = kStephanBoltzmanConstant * kHour_s;

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
	my::Address _address;
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


	bool elevationDraw(graphics::Graphics &graphics, std::vector<SDL_Rect> onscreenPositions, bool sunlit) noexcept;
private:

	static std::map<std::string, std::string> _climateTextures;
	static std::map<elevationType, std::string> _elevationTextures;

	//Standard Draw Subroutine
	void setElevationDrawSpecs(double elevation, double &computedElevationShader, elevationType &computedElevationType) noexcept;

public:
	//=================================================
	//SIMULATION
	//=================================================
	static void beginNewHour() noexcept;

	static int _simulationStep;
	static bool beginNextStep() noexcept;

	void simulateClimate() noexcept;
private:
	static const int kTotalSteps = 5;

	double simulateSolarRadiation() noexcept;


	



public:
	//GETTERS
	//===========================================
	double getStatistic(const StatRequest &statRequest) const noexcept;

	std::vector<std::string> getMessages(const StatRequest &statRequest) const noexcept;
};

}//namespace climate 
}//namespace simulation
}//namespace pleistocene