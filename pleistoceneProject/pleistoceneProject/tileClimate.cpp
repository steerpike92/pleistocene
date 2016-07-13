#include "tileClimate.h"
#include "graphics.h"

//======================================
//INITIALIALIZATION
//======================================

TileClimate::TileClimate() {}

TileClimate::TileClimate(my::Address A, double landElevation){
	
	_Address = A;

	my::Vector2d latLonDeg = _Address.getLatLonDeg();
	_latitude_deg = latLonDeg.x;
	_longitude_deg = latLonDeg.y;
	_solarRadiation = SolarRadiation(_latitude_deg, _longitude_deg);

	double initialTemperature= calculateLocalInitialtemperature();

	_materialColumn = MaterialColumn(landElevation, initialTemperature);
	
}

double TileClimate::calculateLocalInitialtemperature() {
	double localInitialTemperature = climate::planetary::initialTemperatureK;
	localInitialTemperature -= pow((_latitude_deg / 90.0), 2) * 40;
	return localInitialTemperature;
}

void TileClimate::buildAdjacency(std::map<my::Direction, TileClimate*> adjacientTileClimates) {

	for (auto &climatePair : adjacientTileClimates) {
		//STUB
	}

}



//======================================
//SIMULATION
//======================================

int TileClimate::_simulationStep;

void TileClimate::beginNewHour() {
	_simulationStep = 0;
	SolarRadiation::setupSolarRadiation();
	//i.e. create earth rotation matrix for current time and set sun ray vector
}

bool TileClimate::beginNextStep() {
	_simulationStep++;
	return (_simulationStep <= _totalSteps);//check if simulation hour complete
}

void TileClimate::simulateClimate(){
	
	double solarEnergyPerHour;

	switch (_simulationStep) {
	case(1) :
		solarEnergyPerHour=simulateSolarRadiation();
		if (solarEnergyPerHour > 0) {_materialColumn.filterSolarRadiation(solarEnergyPerHour);}
		_materialColumn.simulateEvaporation();
		_materialColumn.simulateInfraredRadiation();
		break;
	case(2) :
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
		LOG("Error: Simulation step out of bounds");
		throw(2);
		break;
	}
}

double TileClimate::simulateSolarRadiation() {
	double solarFraction=_solarRadiation.applySolarRadiation();
	double incidentSolarEnergyPerHour = solarFraction*climate::planetary::solarEnergyPerHour;
	return incidentSolarEnergyPerHour;
}



//======================================
//GRAPHICS
//======================================

void TileClimate::updateClimate(int elapsedTime) {
}



bool TileClimate::drawClimate(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, climate::DrawType drawType) {

	using namespace climate;
	bool selectionFlag = false;

	switch (drawType) {
	case(STANDARD_DRAW) :
		return standardDraw(graphics, onScreenPositions);
	case(SURFACE_TEMPERATURE_DRAW) :
		return surfaceTemperatureDraw(graphics, onScreenPositions);
	case(SURFACE_AIR_TEMPERATURE_DRAW) :
		return surfaceAirTemperatureDraw(graphics, onScreenPositions);
	default:
		LOG("NO DRAW TYPE");
		throw(2);
		return false;
	}
}

bool TileClimate::standardDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions) {
	using namespace climate;

	double elevation = _materialColumn.getLandElevation();

	double elevationShader;
	climate::land::elevationType elevationDrawType;

	setElevationDrawSpecs(elevation, elevationShader, elevationDrawType);
	
	double solarShader = _solarRadiation.getRadiationShader();

	double textureShader = solarShader*elevationShader;
	textureShader = std::max(textureShader, 0.05);

	graphics.darkenTexture(_elevationTextures[elevationDrawType], textureShader);

	return graphics.blitSurface(_elevationTextures[elevationDrawType], nullptr, onScreenPositions);
}


void TileClimate::setElevationDrawSpecs(double elevation, double &computedElevationShader, 
	climate::land::elevationType &computedElevationType) {

	if (elevation< climate::land::landCutoff) {
		computedElevationType = climate::land::SUBMERGED;
		computedElevationShader = abs(double(elevation + 6 * climate::land::gaps)) / double(6 * climate::land::gaps);
		return;
	}
	if (elevation <climate::land::midCutoff) {
		computedElevationType = climate::land::LOW_LAND;
		computedElevationShader = abs(0.6 + 2 * double(climate::land::gaps - elevation) / double(5 * climate::land::gaps));
		return;
	}
	if (elevation < climate::land::highCutoff) {
		computedElevationType = climate::land::MID_LAND;
		computedElevationShader = abs(0.6 + 2 * double(2 * climate::land::gaps - elevation) / double(5 * climate::land::gaps));
		return;
	}

	computedElevationType = climate::land::HIGH_LAND;
	computedElevationShader = abs(0.4 + 2 * double(elevation - 2 * climate::land::gaps) / double(10 * climate::land::gaps));
	return;
}


bool TileClimate::surfaceTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions) {
	using namespace climate;
	const double landFudge = 10;

	double surfaceTemperature = _materialColumn.getSurfaceTemperature();

	double colorIntensity = std::min(abs(landFudge + surfaceTemperature - planetary::initialTemperatureK) / 40.0, 1.0);
	double filter = 1.0 - colorIntensity;

	if (landFudge + surfaceTemperature < planetary::initialTemperatureK) {//Cold
		graphics.colorFilter(_climateTextures["whiteTile"], filter, filter, 1.0);
	}
	else {//Hot
		graphics.colorFilter(_climateTextures["whiteTile"], 1.0, filter, filter);
	}

	return graphics.blitSurface(_climateTextures["whiteTile"], NULL, onScreenPositions);
}

bool TileClimate::surfaceAirTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions) {
	using namespace climate;

	double temperature = _materialColumn.getBoundaryLayerTemperature();
	const double airFudge = 20;

	double colorIntensity = std::min(abs(airFudge+temperature - planetary::initialTemperatureK) / 20.0, 1.0);
	double filter = 1.0 - colorIntensity;

	if (airFudge+temperature < planetary::initialTemperatureK) {//Cold
		graphics.colorFilter(_climateTextures["whiteTile"], filter, filter, 1.0);
	}
	else {//Hot
		graphics.colorFilter(_climateTextures["whiteTile"], 1.0, filter, filter);
	}

	return graphics.blitSurface(_climateTextures["whiteTile"], NULL, onScreenPositions);
}

std::map<std::string, std::string> TileClimate::_climateTextures;
std::map<climate::land::elevationType, std::string> TileClimate::_elevationTextures;

void TileClimate::setupTextures(Graphics &graphics) {

	_climateTextures["whiteTile"] = "../../content/simpleTerrain/whiteTile.png";


	_elevationTextures[climate::land::SUBMERGED] = "../../content/simpleTerrain/midOcean.png";
	_elevationTextures[climate::land::LOW_LAND] = "../../content/simpleTerrain/lowLand.png";
	_elevationTextures[climate::land::MID_LAND] = "../../content/simpleTerrain/midLand.png";
	_elevationTextures[climate::land::HIGH_LAND] = "../../content/simpleTerrain/highLand.png";


	graphics.loadImage(_climateTextures["whiteTile"]);

	for (std::pair< climate::land::elevationType, std::string> P : _elevationTextures) {
		graphics.loadImage(P.second);
	}
}


//===========================================
//GETTERS
//===========================================

std::vector<std::string> TileClimate::getMessages(climate::DrawType messageType) const {
	using namespace climate;

	std::vector<std::string> messages;



	std::vector<std::string> SubMessages = _materialColumn.getMessages(messageType);

	for (std::string &str : SubMessages) {
		messages.push_back(str);
	}

	return messages;

}
