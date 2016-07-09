#include "water.h"
#include "graphics.h"
#include "solarRadiation.h"

Water::Water() {}
Water::~Water() {}

Water::Water(climate::water::BodyType waterType, double landElevation, double surfaceElevation, double localInitialTemperature) :
	_bodyType(waterType),
	_landElevation(landElevation),
	_surfaceElevation(surfaceElevation)
{

	_elevationShader = abs(double(_landElevation + 6 * climate::land::gaps)) / double(6 * climate::land::gaps);	

	using namespace elements;

	Element water = Element(VOLUME, WATER, 20);

	_waterSurface = LiquidMixture(water, localInitialTemperature);

}

void Water::bond(const WaterNeighbor &neighbor) {
	_waterNeighbors[neighbor.first] = neighbor.second;
}

double Water::filterSolarRadiation(double incidentSolarEnergyKJ) {
	_solarFraction = incidentSolarEnergyKJ / climate::earth::solarEnergyPerHour;
	//_waterSurface.calculateParameters();
	return _waterSurface.filterSolarRadiation(incidentSolarEnergyKJ);
}

double Water::filterInfraredRadiation(double incidentInfraredEnergyKJ){
	return _waterSurface.filterInfrared( incidentInfraredEnergyKJ);
}

double Water::emitInfraredRadiation() {
	return _waterSurface.emitInfrared();
}

bool Water::draw(Graphics & graphics, std::vector<SDL_Rect> onScreenPositions, climate::DrawType drawType)
{
	double solarShader = SolarRadiation::getRadiationShader(_solarFraction);

	double textureShader = solarShader*_elevationShader;
	textureShader = std::max(textureShader, 0.05);

	graphics.darkenTexture(_waterTextures[_bodyType], textureShader);

	return graphics.blitSurface(_waterTextures[_bodyType], NULL, onScreenPositions);
}

std::map<climate::water::BodyType, std::string> Water::_waterTextures;

void Water::setupTextures(Graphics &graphics) {
	_waterTextures[climate::water::DEEP] = "../../content/simpleTerrain/midOcean.png";
}



void Water::clearFlow() {
	_inputtileAddresses.clear();
}

void Water::calculateFlow() {

	//if (_directionalNeighbors.size() == 0) {
	//	LOG("No Neighbors Error");
	//	throw(5);
	//}

	//int minElevation = 10000;// i.e. + inf

	//if (_elevationType == climate::land::SUBMERGED) {//flow not meaningful to sea until currents are added if ever
	//	_outputtileAddress = this->_Address;
	//	//_coast = false;
	//	_basin = false;
	//	_feature = climate::land::feature::NONE;
	//	return;
	//}

	//for (std::pair<my::Direction, my::Address> pair : _directionalNeighbors) {
	//	if (_tiles[pair.second.i]._elevation < minElevation) {
	//		minElevation = _tiles[pair.second.i]._elevation;
	//		_outputtileAddress = pair.second;
	//	}
	//}

	//_tiles[_outputtileAddress.i]._inputtileAddresses.push_back(_Address);


	//if (minElevation > _elevation) {

	//	_feature = climate::land::feature::LAKE;

	//	_basin = true;

	//	//_basinDepth = minElevation - _elevation;
	//}
	//else {
	//	_feature = climate::land::feature::NONE;
	//	_basin = false;
	//	//_basinDepth = 0;
	//}
	/*
	if (_tiles[_outputtileAddress.i]._elevationType == climate::land::SUBMERGED) {
	_coast = true;
	}
	else {
	_coast = false;
	}*/

}


std::vector<std::string> Water::getMessages(climate::DrawType messageType) const {
	std::vector<std::string> messages;

	std::stringstream stream;
	stream << "Elevation: " << int(_landElevation) << " m";
	messages.push_back(stream.str());

	/*stream = std::stringstream();
	stream << "Solar Irradiance: " << int(getSolarFraction()*100)<<"%";
	messages.push_back(stream.str());*/

	std::vector<std::string> surfaceMessages = _waterSurface.getMessages();

	for (std::string &str : surfaceMessages) {
		messages.push_back(str);
	}

	return messages;
}


double Water::getSurfaceTemperature()const { return _waterSurface.getTemperature(); }

double Water::getSurfaceElevation()const { return _surfaceElevation; }

climate::water::BodyType Water::getBodyType()const { return _bodyType; }
