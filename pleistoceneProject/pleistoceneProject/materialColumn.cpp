#include "materialColumn.h"
#include "materialLayer.h"
#include "globals.h"

namespace layers {
////////////==================================
////////////INITIALIZATION
////////////==================================

MaterialColumn::MaterialColumn() {}

MaterialColumn::MaterialColumn(double landElevation, double initialTemperature) :
	_landElevation(landElevation),
	_initialTemperature(initialTemperature)
{

	double baseElevation = buildEarth();

	baseElevation = buildHorizon(baseElevation);

	if (_landElevation < 0) {
		baseElevation = buildSea(baseElevation, 0);
		_submerged = true;
	}
	else { _submerged = false; }

	buildAir(baseElevation);

	buildUniversalColumn();
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
	for (int i = 1; i < earthLayers; i++) {
		double layerHeight = earthLayerHeights[i];
		_earth.emplace_back(bedrockElevation, _initialTemperature, currentElevation, layerHeight);
		currentElevation = _earth.back().getTopElevation();
	}
	return currentElevation;
}


double MaterialColumn::buildHorizon(double baseElevation)
{
	_horizon.emplace_back(_landElevation, _initialTemperature, baseElevation);
	double currentElevation = _horizon.back().getTopElevation();
	return currentElevation;
}

double MaterialColumn::buildSea(double baseElevation, double seaSurfaceElevation)
{

	using namespace layers::sea;

	double seaBottomElevation = baseElevation;

	int i = 0;//position in seaLayerElevations array

	while (seaBottomElevation < seaLayerElevations[i] + seaSurfaceElevation) { i++; } //determine number of layers

	if (i >= 6 || i == 0) { LOG("Inappropriate Sea Depth:"); LOG(seaBottomElevation); throw(2); return 0; }

	i--;

	double topElevation;

	//build sea layers
	while (i >= 0) {
		topElevation = seaLayerElevations[i] + seaSurfaceElevation;
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
	double layerTopElevation = layerBottomElevation + boundaryLayerHeight;

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

//needs to be rebuilt after every insertion/deletion
void MaterialColumn::buildUniversalColumn() {

	_column.clear();
	for (auto &layer : _earth) {
		_column.push_back(&layer);
	}
	for (auto &layer : _horizon) {
		_column.push_back(&layer);
	}
	for (auto &layer : _sea) {
		_column.push_back(&layer);
	}
	for (auto &layer : _air) {
		_column.push_back(&layer);
	}

	//build vertical ptrs
	MaterialLayer* previous = nullptr;

	for (auto layer : _column) {
		layer->_down = previous;
		if (previous) previous->_up = layer;

		previous = layer;
	}

}

void MaterialColumn::buildAdjacency(std::map<my::Direction, MaterialColumn*> &adjacientColumns)
{
	_adjacientColumns = adjacientColumns;

	buildMaterialLayerSurfaces();
	buildEarthLayerSurfaces();
	buildHorizonNeighborhood();
}

void MaterialColumn::buildMaterialLayerSurfaces()
{
	for (my::Direction direction : ownedDirections) {
		buildNeighborSurfaces(direction);
	}
}

void MaterialColumn::buildVerticalSurfaces() {
	/*SharedSurface surface;
	for (MaterialLayer *layer : _column) {
		surface.area = climate::planetary::tileArea;
		surface.spatialDirection = UP;
		surface.materialLayer = layer->_up;
		layer->addSurface(surface);
	}*/
}

void MaterialColumn::buildNeighborSurfaces(my::Direction direction)
{

	//SharedSurface surface;

	//MaterialLayer *A = _column.front();//this column's layer
	//MaterialLayer *B = _adjacientColumns[direction]->_column.front();//neighbors column's layer

	//double height;

	//double A_bot, A_top, B_bot, B_top;

	//const SpatialDirection sDirection = static_cast<SpatialDirection>(direction);

	//while (A != nullptr) {
	//	A_bot = A->getBottomElevation();
	//	A_top = A->getTopElevation();

	//	B_bot = B->getBottomElevation();
	//	B_top = B->getTopElevation();

	//	while (A_top > B_bot) {

	//		height = A_top - std::max(A_bot, B_bot);
	//		surface.area = width*height;
	//		surface.materialLayer = B;
	//		surface

	//			A->addSurface()
	//	}
	//}



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


void MaterialColumn::elevationChangeProcedure() {
	//my primary concerns are:
	//a) basins filling with water and becoming seas
	//b) inland seas drying out
	//c) global sea level drop during ice ages

	//the tricky part is the possibility for layers to pop in and out of existance

	//another complication is the silyness of ~0 to ~2 meter deep seas we'll get in highly flat regions.

	//the best solution I think is to declare such a region a marsh and handle it as a special case in horizon layer;


}

////////////==================================
////////////SIMULATION
////////////==================================

void MaterialColumn::filterSolarRadiation(double energyKJ)
{
	_column.back()->filterSolarRadiation(energyKJ);
}

void MaterialColumn::simulateEvaporation()
{
	//STUB
}

void MaterialColumn::simulateInfraredRadiation()
{
	MaterialLayer* surfaceLayer;

	//surface radiation
	if (_submerged) { surfaceLayer = &_sea.back(); }
	else { surfaceLayer = &_horizon.back(); }


	double upRadiation;//radiation incident upwards upon layer
	double emittedEnergy;
	std::vector<double> downRadiation;//radiation incident downwards upon layer

	upRadiation = surfaceLayer->emitInfraredRadiation();

	//filter/emit upwards
	for (AirLayer &air : _air) {
		emittedEnergy = air.emitInfraredRadiation();
		downRadiation.push_back(emittedEnergy / 2.0);
		upRadiation = air.filterInfraredRadiation(upRadiation) + emittedEnergy / 2.0;
	}
	_escapeRadiation = upRadiation;

	//this may be the most unnecessarily complex thing I've ever done in c++
	double currentDownRadiation;
	auto radiation_rit = downRadiation.rbegin();//end of down radiation vector
	//filter downwards
	for (auto air_rit = _air.rbegin() + 1; air_rit != _air.rend(); ++air_rit) {
		currentDownRadiation = *radiation_rit;
		radiation_rit++;//advance down radiation reverse iterator
		*radiation_rit += air_rit->filterInfraredRadiation(currentDownRadiation);//filter radiation
	}
	_backRadiation = *radiation_rit; //I think

	surfaceLayer->filterInfraredRadiation(_backRadiation);

}

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

void MaterialColumn::simulateAirFlow() {}
void MaterialColumn::simulateWaterFlow() {}
void MaterialColumn::simulatePlants() {}



////////////==================================
////////////GETTERS
////////////==================================

double MaterialColumn::getLandElevation()const { return _horizon.back().getTopElevation(); }
double MaterialColumn::getSurfaceTemperature()const
{
	if (_submerged) { return _sea.back().getTemperature(); }

	else { return _horizon.back().getTemperature(); }
}
double MaterialColumn::getBoundaryLayerTemperature()const { return _air.front().getTemperature(); }

std::vector<std::string> MaterialColumn::getMessages(climate::DrawType messageType) const
{
	std::vector<std::string> messages;

	std::stringstream stream;
	stream << "Land Elevation: " << int(_landElevation);
	messages.push_back(stream.str());

	stream = std::stringstream();
	stream << "Back Radiation: " << int(_backRadiation) << " KJ";
	messages.push_back(stream.str());

	stream = std::stringstream();
	stream << "Escape Radiation: " << int(_escapeRadiation) << " KJ";
	messages.push_back(stream.str());


	switch (messageType) {
	case(climate::STANDARD_DRAW) :

		stream = std::stringstream();
		stream << "Bedrock Elevation: " << int(_earth.front().getBottomElevation());
		messages.push_back(stream.str());

		stream = std::stringstream();
		stream << "Horizon Elevation: " << int(_earth.back().getTopElevation());
		messages.push_back(stream.str());



		break;
	case(climate::SURFACE_TEMPERATURE_DRAW) :
		if (_submerged) {
			stream = std::stringstream();
			stream << "Water Surface Temp: " << int(_sea.back().getTemperature() - 273) << " °C";
			messages.push_back(stream.str());
		}
		else {
			stream = std::stringstream();
			stream << "Land Surface Temp: " << int(_horizon.back().getTemperature() - 273) << " °C";
			messages.push_back(stream.str());
		}
		break;
	case(climate::SURFACE_AIR_TEMPERATURE_DRAW) :
		stream = std::stringstream();
		stream << "Air Surface Temp: " << int(_air.front().getTemperature() - 273) << "°C";
		messages.push_back(stream.str());
		break;
	}


	return messages;
}

}