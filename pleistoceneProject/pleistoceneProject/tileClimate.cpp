#include "tileClimate.h"
#include "graphics.h"

TileClimate::TileClimate() {}//STUB

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

void TileClimate::updateClimate(int elapsedTime){
}

std::map<std::string, std::string> TileClimate::_climateDrawTextures;

void TileClimate::setupTextures(Graphics &graphics) {

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
	/*if (_submerged) { return _water.draw(graphics, onScreenPositions, STANDARD_DRAW); }
	else { return _land.draw(graphics, onScreenPositions, STANDARD_DRAW); }*/
	return false;
}

bool TileClimate::surfaceTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions) {
	using namespace climate;
	const double landFudge = 10;

	double surfaceTemperature = 10;

	double colorIntensity = std::min(abs(landFudge+ surfaceTemperature - planetary::initialTemperatureK) / 40.0, 1.0);
	double filter = 1.0 - colorIntensity;

	if (landFudge+ surfaceTemperature < planetary::initialTemperatureK) {//Cold
		graphics.colorFilter(_climateDrawTextures["whiteTile"], filter, filter, 1.0);
	}
	else {//Hot
		graphics.colorFilter(_climateDrawTextures["whiteTile"], 1.0, filter, filter);
	}

	return graphics.blitSurface(_climateDrawTextures["whiteTile"], NULL, onScreenPositions);
}

bool TileClimate::surfaceAirTemperatureDraw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions) {
	//using namespace climate;

	//double temperature = _air.getSurfaceAirTemperature();
	//const double airFudge = 20;

	//double colorIntensity = std::min(abs(airFudge+temperature - planetary::initialTemperatureK) / 20.0, 1.0);
	//double filter = 1.0 - colorIntensity;

	//if (airFudge+temperature < planetary::initialTemperatureK) {//Cold
	//	graphics.colorFilter(_climateDrawTextures["whiteTile"], filter, filter, 1.0);
	//}
	//else {//Hot
	//	graphics.colorFilter(_climateDrawTextures["whiteTile"], 1.0, filter, filter);
	//}

	//return graphics.blitSurface(_climateDrawTextures["whiteTile"], NULL, onScreenPositions);
	return false;
}


void TileClimate::buildAdjacency(std::map<my::Direction, TileClimate> adjacientTileClimates){

	for (auto &climatePair : adjacientTileClimates) {
		//STUB
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
	
	double solarEnergyPerHour;

	switch (_simulationStep) {
	case(0) :
		solarEnergyPerHour=simulateSolarRadiation();
		_materialColumn.filterSolarRadiation(solarEnergyPerHour);
		_materialColumn.simulateEvaporation();
		_materialColumn.simulateInfraredRadiation();
		break;
	case(1) :
		_materialColumn.simulatePressure();
		break;
	case(2) :
		_materialColumn.simulateAirFlow();
		break;

	case(3) :
		_materialColumn.simulateCondensation();
		_materialColumn.simulatePrecipitation();
		break;

	case(4) :
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


	/*switch (messageType) {
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
	}*/

	for (std::string &str : SubMessages) {
		messages.push_back(str);
	}

	return messages;

}
