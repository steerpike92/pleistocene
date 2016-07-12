#include "land.h"
#include "graphics.h"
#include "solarRadiation.h"

//================================================================================
//SETUP
//================================================================================

Land::Land() {}
Land::~Land() {}

Land::Land(double elevation) {

	_elevation = elevation;
	determineElevationType();

	double surfaceTemperature = climate::earth::initialTemperatureK;

	using namespace elements;
	std::vector<Element> composition;
	Element clay = Element(VOLUME, CLAY, 0.1);
	Element sand = Element(VOLUME, SAND, 0.0);
	Element silt = Element(VOLUME, SILT, 0.0);
	composition.push_back(clay);
	composition.push_back(sand);
	composition.push_back(silt);
	_topSoil = SolidMixture(composition, surfaceTemperature);

}

void Land::bond(const LandNeighbor &neighbor) {
	_landNeighbors[neighbor.first] = neighbor.second;
}

void Land::determineElevationType() {
	if (_elevation < climate::land::landCutoff) {
		_elevationType = climate::land::SUBMERGED;
		_elevationShader = abs(double(_elevation + 6*climate::land::gaps)) / double(6*climate::land::gaps);
		return;
	}
	if (_elevation <climate::land::midCutoff) {
		_elevationType = climate::land::LOW_LAND;
		_elevationShader = abs(0.6+ 2*double(climate::land::gaps-_elevation) / double(5*climate::land::gaps));
		return;
	}
	if (_elevation < climate::land::highCutoff) {
		_elevationType = climate::land::MID_LAND;
		_elevationShader = abs(0.6+ 2*double(2*climate::land::gaps-_elevation) / double(5 * climate::land::gaps));
		return;
	}

	_elevationType = climate::land::HIGH_LAND;
	_elevationShader = abs(0.4 + 2*double(_elevation-2*climate::land::gaps) / double(10 * climate::land::gaps));
	return;
}



//================================================================================
//SIMULATION
//================================================================================

double Land::filterSolarRadiation(double incidentSolarEnergyKJ) {
	_solarFraction = incidentSolarEnergyKJ / climate::earth::solarEnergyPerHour;
	//_topSoil.calculateParameters();
	return _topSoil.filterSolarRadiation(incidentSolarEnergyKJ);
}

double Land::filterInfraredRadiation(double incidentInfraredEnergyKJ) {
	return _topSoil.filterInfrared(incidentInfraredEnergyKJ);
}

double Land::emitInfraredRadiation() {
	return _topSoil.emitInfrared();
}


//================================================================================
//GRAPHICS
//================================================================================


bool Land::draw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, climate::DrawType drawType) {

	double solarShader = SolarRadiation::getRadiationShader(_solarFraction);

	double textureShader = solarShader*_elevationShader;
	textureShader = std::max(textureShader, 0.05);

	graphics.darkenTexture(_elevationTextures[_elevationType],textureShader);

	return graphics.blitSurface(_elevationTextures[_elevationType], NULL, onScreenPositions);
}

std::map<climate::land::elevationType, std::string> Land::_elevationTextures;

void Land::setupTextures(Graphics &graphics) {

	//Elevation graphics
	_elevationTextures[climate::land::SUBMERGED] = "../../content/simpleTerrain/midOcean.png";
	_elevationTextures[climate::land::LOW_LAND] = "../../content/simpleTerrain/lowLand.png";
	_elevationTextures[climate::land::MID_LAND] = "../../content/simpleTerrain/midLand.png";
	_elevationTextures[climate::land::HIGH_LAND] = "../../content/simpleTerrain/highLand.png";

	//load graphics
	for (std::pair< climate::land::elevationType, std::string> P : _elevationTextures) {
		graphics.loadImage(P.second);
	}


	//Terrain graphics
	//"../../content/simpleTerrain/lakeMod.png";
}

//==========================================================================
//GETTERS
//==========================================================================

double Land::getLandElevation() const { return this->_elevation; }

bool Land::isSubmerged() const {return (this->_elevationType == climate::land::SUBMERGED);}

double Land::getSurfaceTemperature() const {return _topSoil.getTemperature();}

//double Land::getSolarFraction() const { return _solarFraction; }

std::vector<std::string> Land::getMessages(climate::DrawType messageType) const {
	std::vector<std::string> messages;

	std::stringstream stream;
	stream << "Elevation: " << int(_elevation)<< " m";
	messages.push_back(stream.str());

	/*stream = std::stringstream();
	stream << "Solar Irradiance: " << int(_solarFraction*100)<<"%";
	messages.push_back(stream.str());*/

	std::vector<std::string> surfaceMessages = _topSoil.getMessages();

	for (std::string &str : surfaceMessages) {
		messages.push_back(str);
	}

	return messages;
}


