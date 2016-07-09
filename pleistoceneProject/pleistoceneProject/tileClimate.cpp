#include "tileClimate.h"
#include "graphics.h"

TileClimate::TileClimate() {}
TileClimate::~TileClimate() {}

TileClimate::TileClimate(Address A, double landElevation){
	
	_address = A;

	MyVector2d latLonDeg = _address.getLatLonDeg();
	_latitude_deg = latLonDeg.x;
	_longitude_deg = latLonDeg.y;
	_solarRadiation = SolarRadiation(_latitude_deg, _longitude_deg);

	_land = Land(landElevation);

	_submerged = _land.isSubmerged();

	if (_submerged) {//water surface
		_water = Water(climate::water::DEEP, landElevation, 0);
		_surfaceElevation = _water.getSurfaceElevation();
		_surfaceTemperature = _water.getSurfaceTemperature();
	}
	else {//land surface
		_water = Water(climate::water::TRANSIENT, landElevation, landElevation);
		_surfaceElevation = _land.getLandElevation();
		_land.getSurfaceTemperature();
	}

	_air = Air(_surfaceElevation,_surfaceTemperature);
}


void TileClimate::updateClimate(int elapsedTime){
}

std::map<std::string, std::string> TileClimate::_climateDrawTextures;

void TileClimate::setupTextures(Graphics &graphics) {
	Land::setupTextures(graphics);
	Water::setupTextures(graphics);

	_climateDrawTextures["whiteTile"] = "../../content/simpleTerrain/whiteTile.png";
	graphics.loadImage(_climateDrawTextures["whiteTile"]);
}

bool TileClimate::drawClimate(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, climate::DrawType drawType){

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
	if (_submerged) { return _water.draw(graphics, onScreenPositions, STANDARD_DRAW); }
	else { return _land.draw(graphics, onScreenPositions, STANDARD_DRAW); }
}

bool TileClimate::surfaceTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions) {
	using namespace climate;
	double colorIntensity = std::min(abs(_surfaceTemperature - earth::initialTemperatureK) / 40.0, 1.0);
	double filter = 1.0 - colorIntensity;

	if (_surfaceTemperature < earth::initialTemperatureK) {//Cold
		graphics.colorFilter(_climateDrawTextures["whiteTile"], filter, filter, 1.0);
	}
	else {//Hot
		graphics.colorFilter(_climateDrawTextures["whiteTile"], 1.0, filter, filter);
	}

	return graphics.blitSurface(_climateDrawTextures["whiteTile"], NULL, onScreenPositions);
}

bool TileClimate::surfaceAirTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions) {
	using namespace climate;

	double temperature = _air.getSurfaceAirTemperature();

	double colorIntensity = std::min(abs(temperature - earth::initialTemperatureK) / 40.0, 1.0);
	double filter = 1.0 - colorIntensity;

	if (temperature < earth::initialTemperatureK) {//Cold
		graphics.colorFilter(_climateDrawTextures["whiteTile"], filter, filter, 1.0);
	}
	else {//Hot
		graphics.colorFilter(_climateDrawTextures["whiteTile"], 1.0, filter, filter);
	}

	return graphics.blitSurface(_climateDrawTextures["whiteTile"], NULL, onScreenPositions);
}


void TileClimate::buildAdjacency(std::map<Direction, TileClimate> adjacientTileClimates){

	//give air land and water references(pointers) to neighboring respective climate elements so tile climate doesn't have
	//to care about intra air actions. (or land or water)
	
	//deliberately don't give them inter type pointers to preserve encapsulation

	for (auto &climatePair : adjacientTileClimates) {
		std::pair<Direction, Air*>  airPair(climatePair.first, &(climatePair.second._air));
		_air.bond(airPair);

		std::pair<Direction, Water*>  waterPair(climatePair.first, &(climatePair.second._water));
		_water.bond(waterPair);

		std::pair<Direction, Land*>  landPair(climatePair.first, &(climatePair.second._land));
		_land.bond(landPair);
	}

	

}

int TileClimate::_simulationStep;

void TileClimate::beginNewHour() {
	_simulationStep = -1;
	SolarRadiation::setupSolarRadiation();
	//i.e. create earth rotation matrix for current time and set sun ray vector
}

bool TileClimate::beginNextStep() {
	_simulationStep++;
	return (_simulationStep < _totalSteps);//check if simulation hour complete
}

void TileClimate::simulateClimate(){

	switch (_simulationStep) {
	case(0) :
		simulateSolarRadiation();
		simulateEvaporation();
		simulateInfraredRadiation();
		break;
	case(1) :
		simulatePressure();
		break;
	case(2) :
		simulateAirflow();
		break;

	case(3) :
		simulateCondensation();
		simulatePrecipitation();
		break;

	case(4) :
		simulateWaterFlow();
		simulatePlants();
		measureSurfaceTemperature();
		break;
	default:
		LOG("Error: Simulation step out of bounds");
		throw(2);
		break;
	}
}

void TileClimate::simulateSolarRadiation() {
	double solarFraction=_solarRadiation.applySolarRadiation();
	double incidentSolarEnergyPerHour = solarFraction*climate::earth::solarEnergyPerHour;
	filterSolarRadiation(incidentSolarEnergyPerHour);
}

void TileClimate::filterSolarRadiation(double incidentSolarEnergyPerHour) {
	
	incidentSolarEnergyPerHour = _air.filterSolarRadiation(incidentSolarEnergyPerHour);
	if (_submerged) {
		incidentSolarEnergyPerHour = _water.filterSolarRadiation(incidentSolarEnergyPerHour);
		_land.filterSolarRadiation(incidentSolarEnergyPerHour);
	}
	else {
		_land.filterSolarRadiation(incidentSolarEnergyPerHour);
	}
}

void TileClimate::simulateEvaporation(){//stub
}

void TileClimate::simulateInfraredRadiation(){
	if (_submerged) { _upRadiation =_water.emitInfraredRadiation();}
	else { _upRadiation =_land.emitInfraredRadiation();}
	_backRadiation = _air.filterAndComputeBackRadiation(_upRadiation);
	if (_land.isSubmerged()) { _water.filterInfraredRadiation(_backRadiation); }
	else { _land.filterInfraredRadiation(_backRadiation);}
}

void TileClimate::simulatePressure() {
	_air.buildHydroStaticPressure();
}

void TileClimate::simulateAirflow(){//stub
}

void TileClimate::simulateCondensation(){//stub
}

void TileClimate::simulatePrecipitation(){//stub
}

void TileClimate::simulateWaterFlow(){//stub
}

void TileClimate::simulatePlants() {//stub
}


double TileClimate::getSurfaceTemperature() const { return _surfaceTemperature; }

std::vector<std::string> TileClimate::getMessages(climate::DrawType messageType) const {
	using namespace climate;

	std::vector<std::string> messages;

	/*std::stringstream stream;
	stream << "Up Radiation: " << int(_upRadiation);
	messages.push_back(stream.str());

	stream = std::stringstream();
	stream << "Back Radiation: " << int(_backRadiation);
	messages.push_back(stream.str());*/



	std::vector<std::string> SubMessages;


	switch (messageType) {
	case(SURFACE_TEMPERATURE_DRAW) : 
		if (_land.isSubmerged()) { SubMessages= _water.getMessages(SURFACE_TEMPERATURE_DRAW); }
		else { SubMessages= _land.getMessages(SURFACE_TEMPERATURE_DRAW); }
		break;
	case(SURFACE_AIR_TEMPERATURE_DRAW) :
		SubMessages= _air.getMessages(SURFACE_AIR_TEMPERATURE_DRAW);
		break;
	default : 
		if (_land.isSubmerged()) { SubMessages= _water.getMessages(STANDARD_DRAW);}
		else { SubMessages= _land.getMessages(STANDARD_DRAW);}
		break;
	}

	for (std::string &str : SubMessages) {
		messages.push_back(str);
	}

	return messages;

}

void TileClimate::measureSurfaceTemperature() {
	if (_land.isSubmerged()) { _surfaceTemperature = _water.getSurfaceTemperature(); }
	else{ _surfaceTemperature = _land.getSurfaceTemperature(); }
}