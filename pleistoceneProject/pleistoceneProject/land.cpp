#include "land.h"
#include "graphics.h"
#include "solarRadiation.h"

//================================================================================
//SETUP
//================================================================================

Land::Land() {}
Land::~Land() {}

Land::Land(double elevation, double localInitialTemperature) {

	_elevation = elevation;
	determineElevationType();

	using namespace elements;
	std::vector<Element> composition;
	Element clay = Element(VOLUME, CLAY, 0.4);
	Element sand = Element(VOLUME, SAND, 0.2);
	Element silt = Element(VOLUME, SILT, 0.4);
	composition.push_back(clay);
	composition.push_back(sand);
	composition.push_back(silt);
	_topSoil = SolidMixture(composition, localInitialTemperature);

}

void Land::bond(const LandNeighbor &neighbor) {
	_landNeighbors[neighbor.first] = neighbor.second;
}

void Land::determineElevationType() {
	if (_elevation < climate::earth::landCutoff) {
		_elevationType = climate::earth::SUBMERGED;
		_elevationShader = abs(double(_elevation + 6*climate::earth::gaps)) / double(6*climate::earth::gaps);
		return;
	}
	if (_elevation <climate::earth::midCutoff) {
		_elevationType = climate::earth::LOW_LAND;
		_elevationShader = abs(0.6+ 2*double(climate::earth::gaps-_elevation) / double(5*climate::earth::gaps));
		return;
	}
	if (_elevation < climate::earth::highCutoff) {
		_elevationType = climate::earth::MID_LAND;
		_elevationShader = abs(0.6+ 2*double(2*climate::earth::gaps-_elevation) / double(5 * climate::earth::gaps));
		return;
	}

	_elevationType = climate::earth::HIGH_LAND;
	_elevationShader = abs(0.4 + 2*double(_elevation-2*climate::earth::gaps) / double(10 * climate::earth::gaps));
	return;
}



//================================================================================
//SIMULATION
//================================================================================

double Land::filterSolarRadiation(double incidentSolarEnergyKJ) {
	_solarFraction = incidentSolarEnergyKJ / climate::planetary::solarEnergyPerHour;
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

std::map<climate::earth::elevationType, std::string> Land::_elevationTextures;

void Land::setupTextures(Graphics &graphics) {

	//Elevation graphics
	_elevationTextures[climate::earth::SUBMERGED] = "../../content/simpleTerrain/midOcean.png";
	_elevationTextures[climate::earth::LOW_LAND] = "../../content/simpleTerrain/lowLand.png";
	_elevationTextures[climate::earth::MID_LAND] = "../../content/simpleTerrain/midLand.png";
	_elevationTextures[climate::earth::HIGH_LAND] = "../../content/simpleTerrain/highLand.png";

	//load graphics
	for (std::pair< climate::earth::elevationType, std::string> P : _elevationTextures) {
		graphics.loadImage(P.second);
	}


	//Terrain graphics
	//"../../content/simpleTerrain/lakeMod.png";
}

//==========================================================================
//GETTERS
//==========================================================================

double Land::getLandElevation() const { return this->_elevation; }

bool Land::isSubmerged() const {return (this->_elevationType == climate::earth::SUBMERGED);}

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


