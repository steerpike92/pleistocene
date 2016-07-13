#include "materialColumn.h"
#include "materialLayer.h"
#include "globals.h"


////////////==================================
////////////INITIALIZATION
////////////==================================

MaterialColumn::MaterialColumn() {}

MaterialColumn::MaterialColumn(double landElevation, double initialTemperature):
	_landElevation(landElevation),
	_initialTemperature(initialTemperature)
{

	double baseElevation=buildEarth();

	baseElevation=buildHorizon(baseElevation);

	if (_landElevation < 0) {
		baseElevation = buildSea(baseElevation,0);
		_submerged = true;
	}
	else {_submerged = false;}

	buildAir(baseElevation);
}

double MaterialColumn::buildEarth()
{
	using namespace layers::earth;

	//build bedrock layer
	double bedrockElevation = _landElevation - bedrockDepth;

	double currentElevation;

	_earth.emplace_back(bedrockElevation, _initialTemperature, bedrockElevation, earthLayerHeights[0]);

	currentElevation = _earth.back().getTopElevation();

	//build remaining earth layers
	for (int i = 1; i < earthLayers; i++){
		double layerHeight = earthLayerHeights[i];
		_earth.emplace_back(bedrockElevation, _initialTemperature, currentElevation, layerHeight);
		currentElevation = _earth.back().getTopElevation();
	}
	return currentElevation;
}


double MaterialColumn::buildHorizon(double baseElevation)
{
	_horizon.emplace_back(_landElevation, _initialTemperature, baseElevation);
	double currentElevation =_horizon.back().getTopElevation();
	return currentElevation;
}

double MaterialColumn::buildSea(double baseElevation, double seaSurfaceElevation)
{

	using namespace layers::sea;

	double seaBottomElevation = baseElevation;

	int i=0;//position in seaLayerElevations array
	
	while (seaBottomElevation < seaLayerElevations[i] + seaSurfaceElevation) { i++; } //determine number of layers

	if (i >= 6 || i == 0) { LOG("Inappropriate Sea Depth:"); LOG(seaBottomElevation); throw(2); return 0; }

	i--;

	double topElevation;

	//build sea layers
	while (i >= 0) {
		topElevation = seaLayerElevations[i]+seaSurfaceElevation;
		_sea.emplace_back(_landElevation, _initialTemperature, baseElevation, topElevation);
		baseElevation = topElevation;
		i--;
	}

	return topElevation;
}

void MaterialColumn::buildAir(double baseElevation)
{
	using namespace layers::air;

	//for air, baseElevation corresponds to the base of the air column
	//layerBbottomElevation corresponds to the bottom of that layer
	double layerBottomElevation = baseElevation;
	double layerTopElevation= layerBottomElevation + boundaryLayerHeight;
	
	AirLayer buildLayer;

	//build boundary layer
	buildLayer = AirLayer(baseElevation, _initialTemperature, layerBottomElevation, layerTopElevation);
	_air.push_back(std::move(buildLayer));
	layerBottomElevation = layerTopElevation;

	int i = 0;
	for (; layerBottomElevation > airElevations[i]; i++);//adjusts to find first troposphere top height

	for (; i < 6; i++) {
		layerTopElevation = airElevations[i];
		buildLayer = AirLayer(baseElevation, _initialTemperature, layerBottomElevation, layerTopElevation);
		_air.push_back(std::move(buildLayer));
		layerBottomElevation = layerTopElevation;
	}
}



void MaterialColumn::buildEarthLayerSurfaces()
{
	//the jumps between elevations of adjacient tiles does not represent water flow through the crust very well.
	//equilize the bedrock layer elevations and build earth pair surfaces so corresponding earth layers flow into each other
	//cliffs and springs will happen through the horizon layer as an exception


}

void MaterialColumn::buildHorizonNeighborhood()
{
	//rivers, plants, animals are restricted to the horizon layer,
	//need neighboring horizons
}

void MaterialColumn::buildMaterialLayerSurfaces()
{
	//fluxing air and sea layers. Earth layers included as they can block passage of air/sea



}

////////////==================================
////////////SIMULATION
////////////==================================

void MaterialColumn::filterSolarRadiation(double energyKJ)
{

	//stratosphere absorbs solar radiation at a greater rate (ozone absorption of UV)
	double ozoneBoost = 2;
	energyKJ = _air.back().filterSolarRadiation(energyKJ*ozoneBoost) / ozoneBoost;

	//filter through rest of atmosphere
	for (auto rit = _air.rbegin()+1; rit != _air.rend(); ++rit) {//starts one below stratosphere
		energyKJ = rit->filterSolarRadiation(energyKJ);
	}

	//filter through sea
	if (_submerged) {
		for (auto rit = _sea.rbegin() + 1; rit != _sea.rend(); ++rit) {//starts one below stratosphere
			energyKJ = rit->filterSolarRadiation(energyKJ);
		}
	}

	if (energyKJ > 0.01) {//don't needlessly call filter on horizon
		_horizon.back().filterSolarRadiation(energyKJ);
	}

}

void MaterialColumn::simulateEvaporation()
{
	//STUB
}

void MaterialColumn::simulateInfraredRadiation()
{
	double upRadiation;
	if (_submerged) { upRadiation = _sea.back().emitInfraredRadiation(); }
	else { upRadiation = _horizon.back().emitInfraredRadiation(); }



}



	//double Air::filterAndComputeBackRadiation(double incidentInfraredEnergyKJ) {

	//	fillRadiationArrays(incidentInfraredEnergyKJ);

	//	_backRadiation = filterDownRadiationArray();
	//	_escapedRadiation = filterUpRadiationArray();

	//	return _backRadiation;
	//}

	//void Air::fillRadiationArrays(double incidentInfraredEnergyKJ) {

	//	//Step 0: 0 initialize radiation arrays
	//	for (int i = 0; i < _layerCount + 2; i++) {
	//		_upRadiation[i] = 0.0;
	//		_downRadiation[i] = 0.0;
	//	}


	//	//EX (troposphere may be different number of layers)

	//	//index, layer

	//	// 0 -- surface		(downRadiation[0] = back radiation)(upRadiation[0] = 0)
	//	// 1 -- _boundaryLayer	(upRadiation[1] is from surface)
	//	// 2 -- _troposphere[0]
	//	// 3 -- _troposphere[1]
	//	// 4 -- _troposphere[2]
	//	// 5 -- _troposphere[3]
	//	// 6 -- _stratosphere	(downRadiation[6] = 0)
	//	// 7 -- space		(downRadiation[7] = 0)(upRadiation[7] escapes system)

	//	//Radiation arrays give the direction and KJ of energy INCIDENT upon THAT layer


	//	//Step 1: Fill radiaiton arrays with emitted energy
	//	_upRadiation[1] = incidentInfraredEnergyKJ;

	//	double emittedRadiation;

	//	int layerIndex = 1;

	//	//1.a boundary layer
	//	emittedRadiation = _boundaryLayer.emitInfrared();
	//	_upRadiation[layerIndex + 1] = emittedRadiation / 2.0;
	//	_downRadiation[layerIndex - 1] = emittedRadiation / 2.0;//this radiation goes to surface
	//	layerIndex++;

	//	//1.b troposphere
	//	for (GaseousMixture &gasMix : _troposphere) {
	//		emittedRadiation = gasMix.emitInfrared();
	//		_upRadiation[layerIndex + 1] = emittedRadiation / 2.0;
	//		_downRadiation[layerIndex - 1] = emittedRadiation / 2.0;
	//		layerIndex++;
	//	}

	//	//1.c stratosphere
	//	emittedRadiation = _stratosphere.emitInfrared();
	//	_upRadiation[layerIndex + 1] = emittedRadiation / 2.0;//this radiation goes to space
	//	_downRadiation[layerIndex - 1] = emittedRadiation / 2.0;
	//}

	//double Air::filterUpRadiationArray() {
	//	/*std::cout << "\nupRad: ";
	//	for (int i = 0; i < _layerCount + 2; i++) {
	//	std::cout << int(_upRadiation[i])<<", ";
	//	}*/

	//	int layerIndex = 1;

	//	_upRadiation[layerIndex + 1] += _boundaryLayer.filterInfrared(_upRadiation[layerIndex]);//filter up to cell above
	//	layerIndex++;

	//	for (GaseousMixture &gasMix : _troposphere) {
	//		_upRadiation[layerIndex + 1] += gasMix.filterInfrared(_upRadiation[layerIndex]);//filter up to cell above
	//		layerIndex++;
	//	}
	//	//std::cout << "Before Stratosphere: " << int(_upRadiation[layerIndex]) << std::endl;
	//	_upRadiation[layerIndex + 1] += _stratosphere.filterInfrared(_upRadiation[layerIndex]);//escapes to space
	//											       //std::cout << "After Stratosphere: " << int(_upRadiation[layerIndex+1]) << std::endl;

	//	return _upRadiation[layerIndex + 1];
	//}

	//double Air::filterDownRadiationArray() {
	//	/*std::cout << "\ndownRad: ";
	//	for (int i = 0; i < _layerCount + 2; i++) {
	//	std::cout << int(_downRadiation[i]) << ", ";
	//	}*/

	//	int layerIndex = _layerCount - 1;

	//	for (std::vector<GaseousMixture>::reverse_iterator &gasPtr = _troposphere.rbegin(); gasPtr != _troposphere.rend(); ++gasPtr) {//reverse iterator
	//		_downRadiation[layerIndex - 1] += gasPtr->filterInfrared(_downRadiation[layerIndex]);
	//		layerIndex--;
	//	}
	//	//std::cout << "Before Boundary: " << int(_downRadiation[layerIndex]) << std::endl;
	//	_downRadiation[layerIndex - 1] += _boundaryLayer.filterInfrared(_downRadiation[layerIndex]);//back to surface
	//												    //std::cout << "After Boundary: " << int(_downRadiation[layerIndex-1]) << std::endl;

	//	return _downRadiation[0];
	//}

void MaterialColumn::simulatePressure() 
{
	//STUB
}

void MaterialColumn::simulateCondensation()
{
	//STUB
}

void MaterialColumn::simulatePrecipitation()
{
	//STUB
}

void MaterialColumn::simulateAirFlow(){}
void MaterialColumn::simulateWaterFlow(){}
void MaterialColumn::simulatePlants(){}



////////////==================================
////////////GETTERS
////////////==================================

double MaterialColumn::getLandElevation()const {return _horizon.back().getTopElevation();}
double MaterialColumn::getSurfaceTemperature()const
{
	if (_submerged) { return _sea.back().getTemperature(); }
	
	else { return _horizon.back().getTemperature(); }
}
double MaterialColumn::getBoundaryLayerTemperature()const{return _air.front().getTemperature();}

std::vector<std::string> MaterialColumn::getMessages(climate::DrawType messageType) const
{
	std::vector<std::string> messages;

	std::stringstream stream;
	stream << "Land Elevation: " << int(_landElevation);
	messages.push_back(stream.str());

	switch (messageType) {
	case(climate::STANDARD_DRAW) :

		stream=std::stringstream();
		stream << "Bedrock Elevation: " << int(_earth.front().getBottomElevation());
		messages.push_back(stream.str());

		stream = std::stringstream();
		stream << "Horizon Elevation: " << int(_earth.back().getTopElevation());
		messages.push_back(stream.str());



		break;
	case(climate::SURFACE_TEMPERATURE_DRAW) :
		if (_submerged) {
			stream = std::stringstream();
			stream << "Water Surface Temp: " << int(_sea.back().getTemperature());
			messages.push_back(stream.str());
		}
		else {
			stream = std::stringstream();
			stream << "Land Surface Temp: " << int(_horizon.back().getTemperature());
			messages.push_back(stream.str());
		}
		break;
	case(climate::SURFACE_AIR_TEMPERATURE_DRAW) :
		stream = std::stringstream();
		stream << "Air Surface Temp: " << int(_air.front().getTemperature());
		messages.push_back(stream.str());
		break;
	}


	return messages;
}
 