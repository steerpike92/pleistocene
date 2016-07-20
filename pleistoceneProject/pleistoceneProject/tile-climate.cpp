#include "tile-climate.h"
#include "graphics.h"
#include "world.h"

namespace pleistocene {
namespace simulation {
namespace climate {
//======================================
//INITIALIALIZATION
//======================================

TileClimate::TileClimate() noexcept {}

TileClimate::TileClimate(my::Address A, double noiseValue) noexcept {

	double landElevation=noiseValue * kElevationAmplitude;

	_address = A;

	my::Vector2d latLonDeg = _address.getLatLonDeg();
	_latitude_deg = latLonDeg.x;
	_longitude_deg = latLonDeg.y;
	_solarRadiation = SolarRadiation(_latitude_deg, _longitude_deg);

	double initialTemperature = calculateLocalInitialtemperature();

	_materialColumn = layers::MaterialColumn(landElevation, initialTemperature);
}



double TileClimate::calculateLocalInitialtemperature() noexcept {
	double localInitialTemperature = kInitialTemperatureK;
	localInitialTemperature -= pow((_latitude_deg / 90.0), 2) * 40;
	return localInitialTemperature;
}

void TileClimate::buildAdjacency(std::map<my::Direction, TileClimate*> &adjacientTileClimates) noexcept {
	_adjacientTileClimates = adjacientTileClimates;

	std::map<my::Direction, layers::MaterialColumn*> adjacientColumns;
	my::Direction direction;
	layers::MaterialColumn* column;

	for (auto &climate : _adjacientTileClimates) {
		direction = climate.first;
		column = &(climate.second->_materialColumn);
		adjacientColumns[direction] = column;
	}

	_materialColumn.buildAdjacency(adjacientColumns);
}



//======================================
//SIMULATION
//======================================

int TileClimate::_simulationStep;

void TileClimate::beginNewHour() noexcept {
	_simulationStep = 0;
	SolarRadiation::setupSolarRadiation();
	//i.e. create earth rotation matrix for current time and set sun ray vector
}

bool TileClimate::beginNextStep() noexcept {
	_simulationStep++;
	return (_simulationStep <= kTotalSteps);//check if simulation hour complete
}

void TileClimate::simulateClimate() noexcept {

	double solarEnergyPerHour;

	switch (_simulationStep) {
	case(1) :
		_materialColumn.beginNewHour();
		solarEnergyPerHour = simulateSolarRadiation();
		if (solarEnergyPerHour > 0) { _materialColumn.filterSolarRadiation(solarEnergyPerHour); }
		_materialColumn.simulateEvaporation();
		_materialColumn.simulateInfraredRadiation();
		break;
	case(2) :
		_materialColumn.simulateConduction();
		_materialColumn.simulatePressure();
		break;
	case(3) :
		_materialColumn.simulateAirFlow();
		break;

	case(4) :
		_materialColumn.simulateCondensation();
		_materialColumn.simulatePrecipitation();
		break;

	case(5) :
		_materialColumn.simulateWaterFlow();
		_materialColumn.simulatePlants();
		break;
	default:
		LOG("Error: Simulation step out of bounds");exit(EXIT_FAILURE);
	}
}

double TileClimate::simulateSolarRadiation() noexcept {
	double solarFraction = _solarRadiation.applySolarRadiation();
	double incidentSolarEnergyPerHour = solarFraction*kSolarEnergyPerHour;
	return incidentSolarEnergyPerHour;
}



//======================================
//GRAPHICS
//======================================



bool TileClimate::elevationDraw(graphics::Graphics &graphics, std::vector<SDL_Rect> onscreenPositions, bool sunlit) noexcept {

	double elevation = _materialColumn.getLandElevation();

	double elevationShader;
	elevationType elevationDrawType;

	setElevationDrawSpecs(elevation, elevationShader, elevationDrawType);

	double solarShader = 1;
	if (sunlit) solarShader = _solarRadiation.getRadiationShader(); //TODO control of shading....

	double textureShader = solarShader*elevationShader;
	textureShader = std::max(textureShader, 0.05);

	graphics.darkenTexture(_elevationTextures[elevationDrawType], textureShader);

	return graphics.blitSurface(_elevationTextures[elevationDrawType], nullptr, onscreenPositions);
}


void TileClimate::setElevationDrawSpecs(double elevation, double &computedElevationShader, elevationType &computedElevationType) noexcept {

	if (elevation < kLandCutoff) {
		computedElevationType = SUBMERGED;
		computedElevationShader = abs(double(elevation + 6 * kElevationGaps)) / double(6 * kElevationGaps);
		return;
	}
	if (elevation < kMidCutoff) {
		computedElevationType = LOW_LAND;
		computedElevationShader = abs(0.6 + 2 * double(kElevationGaps - elevation) / double(5 * kElevationGaps));
		return;
	}
	if (elevation < kHighCutoff) {
		computedElevationType = MID_LAND;
		computedElevationShader = abs(0.6 + 2 * double(2 * kElevationGaps - elevation) / double(5 * kElevationGaps));
		return;
	}

	computedElevationType = HIGH_LAND;
	computedElevationShader = abs(0.4 + 2 * double(elevation - 2 * kElevationGaps) / double(10 * kElevationGaps));
	return;
}


std::map<std::string, std::string> TileClimate::_climateTextures;
std::map<elevationType, std::string> TileClimate::_elevationTextures;

void TileClimate::setupTextures(graphics::Graphics &graphics)  noexcept {

	_elevationTextures[SUBMERGED] = "../../content/simpleTerrain/midOcean.png";
	_elevationTextures[LOW_LAND] = "../../content/simpleTerrain/lowLand.png";
	_elevationTextures[MID_LAND] = "../../content/simpleTerrain/midLand.png";
	_elevationTextures[HIGH_LAND] = "../../content/simpleTerrain/highLand.png";

	for (std::pair< elevationType, std::string> P : _elevationTextures) {
		graphics.loadImage(P.second);
	}
}


//===========================================
//GETTERS
//===========================================

double TileClimate::getStatistic(const StatRequest &statRequest) const noexcept
{
	return _materialColumn.getStatistic(statRequest);
}

std::vector<std::string> TileClimate::getMessages(const StatRequest &statRequest) const noexcept 
{
	using namespace climate;

	std::vector<std::string> messages;

	std::vector<std::string> SubMessages = _materialColumn.getMessages(statRequest);

	for (std::string &str : SubMessages) {
		messages.push_back(str);
	}

	return messages;

}

}//namespace climate
}//namespace simulation
}//namespace pleistocene


