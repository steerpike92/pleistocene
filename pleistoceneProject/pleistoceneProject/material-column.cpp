#include "material-column.h"
#include "material-layer.h"
#include "globals.h"
#include "world.h"

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {

//==================================
//INITIALIZATION
//==================================

MaterialColumn::MaterialColumn()  noexcept {}

MaterialColumn::MaterialColumn(double landElevation, double initialTemperature) noexcept :
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

//Layer Builders
//================
double MaterialColumn::buildEarth() noexcept
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

double MaterialColumn::buildHorizon(double baseElevation) noexcept
{
	_horizon.emplace_back(_landElevation-earth::bedrockDepth, _initialTemperature, baseElevation);
	double currentElevation = _horizon.back().getTopElevation();
	return currentElevation;
}

double MaterialColumn::buildSea(double baseElevation, double seaSurfaceElevation) noexcept
{

	using namespace layers::sea;

	double seaBottomElevation = baseElevation;

	int i = 0;//position in seaLayerElevations array

	while (seaBottomElevation < seaLayerElevations[i] + seaSurfaceElevation) { i++; } //determine number of layers

	if (i >= 6 || i == 0) { LOG("Inappropriate Sea Depth:"); LOG(seaBottomElevation); exit(EXIT_FAILURE); return 0; }

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

void MaterialColumn::buildAir(double baseElevation) noexcept
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

//Relation Builders
//=================
//need to be rebuilt after every insertion/deletion
void MaterialColumn::buildUniversalColumn() noexcept
{

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

void MaterialColumn::buildAdjacency(std::map<my::Direction, MaterialColumn*> &adjacientColumns) noexcept
{
	_adjacientColumns = adjacientColumns;

	buildMaterialLayerSurfaces();
	buildEarthLayerSurfaces();
	buildHorizonNeighborhood();
}

//Surface Builders
//=================

void MaterialColumn::buildMaterialLayerSurfaces() noexcept
{
	for (my::Direction direction : ownedDirections) {
		buildNeighborSurfaces(direction);
	}
}

void MaterialColumn::buildVerticalSurfaces() noexcept
{
	/*SharedSurface surface;
	for (MaterialLayer *layer : _column) {
		surface.area = climate::planetary::tileArea;
		surface.spatialDirection = UP;
		surface.materialLayer = layer->_up;
		layer->addSurface(surface);
	}*/
}

void MaterialColumn::buildNeighborSurfaces(my::Direction direction) noexcept
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


void MaterialColumn::buildEarthLayerSurfaces() noexcept
{
	//the jumps between elevations of adjacient tiles does not represent water flow through the crust very well.
	//equilize the bedrock layer elevations and build earth pair surfaces so corresponding earth layers flow into each other
	//cliffs and springs will happen through the horizon layer as an exception


}

void MaterialColumn::buildHorizonNeighborhood() noexcept
{
	//rivers, plants, animals are restricted to the horizon layer,
	//need neighboring horizons
}


void MaterialColumn::elevationChangeProcedure() noexcept
{
	//my primary concerns are:
	//a) basins filling with water and becoming seas
	//b) inland seas drying out
	//c) global sea level drop during ice ages

	//the tricky part is the possibility for layers to pop in and out of existance

	//another complication is the silliness of ~0 to ~2 meter deep seas we'll get in highly flat regions.

	//the best solution I think is to declare such a region a marsh and handle it as a special case in horizon layer;


}

//==================================
//SIMULATION
//==================================
void MaterialColumn::beginNewHour() noexcept {
	for (auto layer : _column) {
		layer->beginNewHour();
	}
}

void MaterialColumn::filterSolarRadiation(double energyKJ) noexcept
{
	_column.back()->filterSolarRadiation(energyKJ);
}

void MaterialColumn::simulateEvaporation() noexcept
{
	//STUB
}

void MaterialColumn::simulateInfraredRadiation() noexcept
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

void MaterialColumn::simulatePressure() noexcept
{
	//STUB
}

void MaterialColumn::simulateCondensation() noexcept
{
	//STUB
}

void MaterialColumn::simulatePrecipitation() noexcept
{
	//STUB
}

void MaterialColumn::simulateAirFlow()  noexcept {}
void MaterialColumn::simulateWaterFlow() noexcept {}
void MaterialColumn::simulatePlants()  noexcept {}

//==================================
//GETTERS
//==================================

double MaterialColumn::getLandElevation() const noexcept { return _horizon.back().getTopElevation(); }
double MaterialColumn::getSurfaceTemperature()const noexcept
{
	if (_submerged) { return _sea.back().getTemperature(); }

	else { return _horizon.back().getTemperature(); }
}
double MaterialColumn::getBoundaryLayerTemperature() const noexcept { return _air.front().getTemperature(); }

std::vector<std::string> MaterialColumn::getMessages(const StatRequest &statRequest) const noexcept
{
	std::vector<std::string> messages;
	std::vector<std::string> subMessages;
	std::stringstream stream;

	if (statRequest._statType == TEMPERATURE) {
		stream = std::stringstream();
		stream << "Back Radiation: " << int(_backRadiation) << " KJ";
		messages.push_back(stream.str());

		stream = std::stringstream();
		stream << "Escape Radiation: " << int(_escapeRadiation) << " KJ";
		messages.push_back(stream.str());
	}

	chooseLayer(statRequest);
	if (_chosenLayer != nullptr) {
		subMessages = _chosenLayer->getMessages(statRequest);
		messages.insert(messages.end(), subMessages.begin(), subMessages.end());
	}
	return messages;
}

double MaterialColumn::getStatistic(const StatRequest &statRequest) const noexcept {

	chooseLayer(statRequest);

	if (_chosenLayer == nullptr) return my::kFakeDouble;

	return _chosenLayer->getStatistic(statRequest);
}


void MaterialColumn::chooseLayer(const StatRequest &statRequest) const noexcept {

	_chosenLayer = nullptr;

	switch (statRequest._section) {

	case(SURFACE_) :

		if (statRequest._layer == 0) {
			if (_submerged) {
				auto layerReporting = &(_sea.back());
				_chosenLayer = const_cast<SeaLayer*>(layerReporting);
				return;
			}
			else {
				auto layerReporting = &(_horizon.back());
				_chosenLayer = const_cast<HorizonLayer*>(layerReporting);
				return;
			}
		}

		else {
			_chosenLayer = const_cast<AirLayer*>(&_air.front());
			return;
		}
		

	case(HORIZON_) : {

		if (_submerged) {
			return;//_chosenLayer stays nullptr;
		}

		auto layerReporting = &(_horizon.back());
		_chosenLayer = const_cast<HorizonLayer*>(layerReporting);
		return;
	}

	case(EARTH_) : {

		//5 bedrock
		//4 substratum 3
		//3 substratum 2
		//2 substratum 1
		//1 subsoil
		//0 horizon


		if (statRequest._layer == 0) {
			auto layerReporting = &(_horizon.back());
			_chosenLayer= const_cast<HorizonLayer*>(layerReporting);
			return;
		}

		auto layerReporting = _earth.rbegin();
		for (int i = 0; i < statRequest._layer; i++) {//advance downward to requested layer
			layerReporting++;
			if (layerReporting == _earth.rend()) {//below bedrock bottom. not valid
				return;
			}
		}
		//1) dereference iterator, 2) get pointer to object, 3) cast to non-const.
		_chosenLayer = const_cast<EarthLayer*>(&(*layerReporting));//this is a ridiculous line. 
		return;
	}

	case(SEA_) : {

		if (!_submerged) return;

		//5 does not exist
		//4 2000 - 20000 m
		//3 200-2000 m
		//2 20-200 m
		//1 2-20 m
		//0 sea surface 0-2 meters deep

		auto layerReporting = _sea.rbegin();//surface
		for (int i = 0; i < statRequest._layer; i++) {//advance downward to requested layer
			layerReporting++;
			if (layerReporting == _sea.rend()) {//hit sea bottom. not valid
				return;
			}
		}
		//1) dereference iterator, 2) get pointer to object, 3) cast to non-const.
		_chosenLayer = const_cast<SeaLayer*>(&(*layerReporting));
		return;
	}

	case(AIR_) : {

		//Boundary layer
		if (statRequest._layer == 0) {
			_chosenLayer = const_cast<AirLayer*>(&_air.front());
			return;
		}

		//other layers

		//start at stratosphere and work back to desired layer
		auto layerReporting = _air.rbegin();

		//5 stratosphere
		//4 trop 4
		//3 trop 3
		//2 trop 2  (this layer rarely does not exist)
		//1 trop 1  (this layer often does not exist)
		//0 boundary layer (this layer always exists, it morphs to the terrain)

		for (int i = 5; i > statRequest._layer; i--) {
			layerReporting++; //advance reverse iterator to go down
			if (layerReporting == _air.rend() - 1) {//kill the return to the boundary layer
				return;
			}
		}
		//layerReporting is now the desired layer
		//1) dereference iterator, 2) get pointer to object, 3) cast to non-const.
		_chosenLayer = const_cast<AirLayer*>(&(*layerReporting));
		return;
	}

	}//end switch
}


}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene