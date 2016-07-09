#include "air.h"
#include "graphics.h"
#include "solarRadiation.h"
#include "tileClimate.h"

Air::Air() {}
Air::~Air() {}

Air::Air(double surfaceElevation, double surfaceTemperature) :
	_surfaceElevation(surfaceElevation)
{
	using namespace elements;
	using namespace climate::air;

	initializePressureAtElevation(surfaceElevation, surfaceElevation + boundaryLayerHeight, _boundaryLayer, BOUNDARY_LAYER);

	buildTroposphere(surfaceElevation + boundaryLayerHeight, tropopauseElevation);

	initializePressureAtElevation(tropopauseElevation, stratopauseElevation, _stratosphere, STRATOSPHERE);

	_layerCount = 2 + _troposphere.size();
}

void Air::buildTroposphere(double bottomElevation, double topElevation) {
	using namespace climate::air;

	_troposphere.clear();
	GaseousMixture stubMix;
	double layerTopElevation;

	int index = 1;
	while (index*troposphereLayerHeight < bottomElevation) {
		index++;
	}

	while (bottomElevation < topElevation) {
		layerTopElevation = index*troposphereLayerHeight;
		initializePressureAtElevation(bottomElevation, layerTopElevation, stubMix, TROPOSPHERE);
		_troposphere.emplace_back(stubMix);
		bottomElevation = layerTopElevation;
		index++;
	}
}

void Air::initializePressureAtElevation(double bottomElevation, double topElevation, GaseousMixture &stubMixture, climate::air::airType type) {

	using namespace elements;
	using namespace climate::air;
	using namespace climate::earth;

	double lapseRate;

	if (type == STRATOSPHERE) { lapseRate = 0; }
	else { lapseRate = 0.01; }

	double height = topElevation - bottomElevation;


	double bottomTemperature = initialTemperatureK - bottomElevation*approximateLapseRate;
	double bottomPressure = seaLevelAveragePressure* exp(-Md*g*bottomElevation / (bottomTemperature*R));

	double topTemperature = bottomTemperature - height*lapseRate;
	double topPressure = seaLevelAveragePressure*exp(-Md*g*topElevation / (topTemperature*R));

	double pressureDifference = bottomPressure - topPressure;
	double mass = pressureDifference / g;

	Element air = Element(MASS, DRY_AIR, mass);
	stubMixture = GaseousMixture(air, bottomTemperature, height, bottomElevation, topElevation);
}

void Air::bond(const AirNeighbor &neighbor) {
	_airNeighbors[neighbor.first] = neighbor.second;
}

double Air::filterSolarRadiation(double incidentSolarEnergyKJ) {

	//stratosphere absorbs solar radiation at a greater rate
	double ozoneBoost = 2;
	incidentSolarEnergyKJ = _stratosphere.filterSolarRadiation(incidentSolarEnergyKJ*ozoneBoost)/ozoneBoost;

	for (std::vector<GaseousMixture>::reverse_iterator &i = _troposphere.rbegin();i != _troposphere.rend(); ++i) {//reverse iterator
		incidentSolarEnergyKJ = i->filterSolarRadiation(incidentSolarEnergyKJ);
	}

	incidentSolarEnergyKJ = _boundaryLayer.filterSolarRadiation(incidentSolarEnergyKJ);

	return incidentSolarEnergyKJ;
}

double Air::filterAndComputeBackRadiation(double incidentInfraredEnergyKJ) {

	fillRadiationArrays(incidentInfraredEnergyKJ);

	_backRadiation = filterDownRadiationArray();
	_escapedRadiation = filterUpRadiationArray();

	return _backRadiation;
}

void Air::fillRadiationArrays(double incidentInfraredEnergyKJ) {

	//Step 0: 0 initialize radiation arrays
	for (int i = 0; i < _layerCount+2; i++) {
		_upRadiation[i] = 0.0;
		_downRadiation[i] = 0.0;
	}


	//EX (troposphere may be different number of layers)

	//index, layer

	// 0 -- surface		(downRadiation[0] = back radiation)(upRadiation[0] = 0)
	// 1 -- _boundaryLayer	(upRadiation[1] is from surface)
	// 2 -- _troposphere[0]
	// 3 -- _troposphere[1]
	// 4 -- _troposphere[2]
	// 5 -- _troposphere[3]
	// 6 -- _stratosphere	(downRadiation[6] = 0)
	// 7 -- space		(downRadiation[7] = 0)(upRadiation[7] escapes system)

	//Radiation arrays give the direction and KJ of energy INCIDENT upon THAT layer


	//Step 1: Fill radiaiton arrays with emitted energy
	_upRadiation[1] = incidentInfraredEnergyKJ;

	double emittedRadiation;

	int layerIndex = 1;

	//1.a boundary layer
	emittedRadiation = _boundaryLayer.emitInfrared();
	_upRadiation[layerIndex + 1] = emittedRadiation / 2.0;
	_downRadiation[layerIndex-1] = emittedRadiation / 2.0;//this radiation goes to surface
	layerIndex++;

	//1.b troposphere
	for (GaseousMixture &gasMix : _troposphere) {
		emittedRadiation = gasMix.emitInfrared();
		_upRadiation[layerIndex+1] = emittedRadiation / 2.0;
		_downRadiation[layerIndex-1] = emittedRadiation / 2.0;
		layerIndex++;
	}

	//1.c stratosphere
	emittedRadiation = _stratosphere.emitInfrared();
	_upRadiation[layerIndex+1] = emittedRadiation / 2.0;//this radiation goes to space
	_downRadiation[layerIndex-1] = emittedRadiation / 2.0;
}

double Air::filterUpRadiationArray() {
	/*std::cout << "\nupRad: ";
	for (int i = 0; i < _layerCount + 2; i++) {
		std::cout << int(_upRadiation[i])<<", ";
	}*/

	int layerIndex = 1;

	_upRadiation[layerIndex + 1] += _boundaryLayer.filterInfrared(_upRadiation[layerIndex]);//filter up to cell above
	layerIndex++;

	for (GaseousMixture &gasMix : _troposphere) {
		_upRadiation[layerIndex + 1] += gasMix.filterInfrared(_upRadiation[layerIndex]);//filter up to cell above
		layerIndex++;
	}
	//std::cout << "Before Stratosphere: " << int(_upRadiation[layerIndex]) << std::endl;
	_upRadiation[layerIndex+1] +=_stratosphere.filterInfrared(_upRadiation[layerIndex]);//escapes to space
	//std::cout << "After Stratosphere: " << int(_upRadiation[layerIndex+1]) << std::endl;

	return _upRadiation[layerIndex +1];
}

double Air::filterDownRadiationArray() {
	/*std::cout << "\ndownRad: ";
	for (int i = 0; i < _layerCount + 2; i++) {
		std::cout << int(_downRadiation[i]) << ", ";
	}*/

	int layerIndex = _layerCount-1;

	for (std::vector<GaseousMixture>::reverse_iterator &gasPtr = _troposphere.rbegin(); gasPtr != _troposphere.rend(); ++gasPtr) {//reverse iterator
		_downRadiation[layerIndex - 1] += gasPtr->filterInfrared(_downRadiation[layerIndex]);
		layerIndex--;
	}
	//std::cout << "Before Boundary: " << int(_downRadiation[layerIndex]) << std::endl;
	_downRadiation[layerIndex - 1] += _boundaryLayer.filterInfrared(_downRadiation[layerIndex]);//back to surface
	//std::cout << "After Boundary: " << int(_downRadiation[layerIndex-1]) << std::endl;

	return _downRadiation[0];
}

void Air::buildHydroStaticPressure() {
	double bottomPressure = _stratosphere.calculateBottomPressure(0);
	for (std::vector<GaseousMixture>::reverse_iterator &i = _troposphere.rbegin();
	i != _troposphere.rend(); ++i) {
		bottomPressure=i->calculateBottomPressure(bottomPressure);
	}
	_boundaryLayer.calculateBottomPressure(bottomPressure);
}

void Air::simulateAirflow() {

}




//void Air::changeSurfaceElevation(double surfaceElevation) {
//	//stub
//	_surfaceElevation = surfaceElevation;
//}


double Air::getSurfaceAirPressure() const{
	return _boundaryLayer.getBottomPressure();
}

double Air::getSurfaceAirTemperature() const{
	return _boundaryLayer.getTemperature();
}

std::vector<std::string> Air::getMessages(climate::DrawType messageType) const {
	std::vector<std::string> messages;
	std::stringstream stream;
	stream << "Back Radiation: " << int(_backRadiation);
	messages.push_back(stream.str());

	stream=std::stringstream();
	stream << "Escaped Radiation: " << int(_escapedRadiation);
	messages.push_back(stream.str());


	std::vector<std::string> gasMessages;

	gasMessages = _boundaryLayer.getMessages();
	for (std::string &str : gasMessages) {
		messages.push_back(str);
	}

	for (GaseousMixture const &gas : _troposphere) {
		gasMessages.clear();
		gasMessages = gas.getMessages();
		for (std::string &str : gasMessages) {
			messages.push_back(str);
		}
	}
	gasMessages.clear();
	gasMessages = _stratosphere.getMessages();
	for (std::string &str : gasMessages) {
		messages.push_back(str);
	}

	return messages;
}

