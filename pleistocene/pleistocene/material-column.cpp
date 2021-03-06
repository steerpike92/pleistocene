#include "material-column.h"
#include "tile-climate.h"
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
_initialTemperature(initialTemperature),
_submerged(_landElevation < -2)
{
	double baseElevation = buildEarth();

	baseElevation = buildHorizon(baseElevation);

	if (_submerged) {//TODO. sand bars? reefs? marshes? lagoons?
		baseElevation = buildSea(baseElevation, 0);
	}

	buildAir(baseElevation);

	buildUniversalColumn();
}

//Layer Builders
//================
double MaterialColumn::buildEarth() noexcept
{
	using namespace layers::earth;

	double bedrockElevation = _landElevation - bedrockDepth;
	double currentElevation = bedrockElevation;

	for (int i = 0; i < earthLayers; i++) {
		double layerHeight = earthLayerHeights[i];
		//_earth.emplace_back(bedrockElevation, 280, currentElevation, layerHeight);
		_earth.emplace_back(bedrockElevation, _initialTemperature, currentElevation, layerHeight, false);
		currentElevation = _earth.back().getTopElevation();
	}
	return currentElevation;
}

double MaterialColumn::buildHorizon(double baseElevation) noexcept
{
	_horizon.emplace_back(_landElevation - earth::bedrockDepth, _initialTemperature, baseElevation, !_submerged);
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
		bool emittor = (i == 0);
		topElevation = seaLayerElevations[i] + seaSurfaceElevation;
		_sea.emplace_back(_landElevation, _initialTemperature, baseElevation, topElevation, emittor);
		//_sea.emplace_back(_landElevation, 288, baseElevation, topElevation);
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
	//buildLayer = AirLayer(baseElevation, 288, layerBottomElevation, layerTopElevation);
	_air.push_back(std::move(buildLayer));
	layerBottomElevation = layerTopElevation;

	int i = 0;
	for (; layerBottomElevation > airElevations[i]; i++);//adjusts to find first troposphere top height

	for (; i < 6; i++) {
		layerTopElevation = airElevations[i];
		//buildLayer = AirLayer(baseElevation, _initialTemperature, layerBottomElevation, layerTopElevation);
		buildLayer = AirLayer(baseElevation, 288, layerBottomElevation, layerTopElevation);
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

	for (MaterialLayer *layer : _column) {
		layer->_down = previous;
		if (previous) previous->_up = layer;

		previous = layer;
	}

}

void MaterialColumn::buildAdjacency(std::map<my::Direction, MaterialColumn*> &adjacientColumns) noexcept
{
	_adjacientColumns = adjacientColumns;

	buildTopSurfaces();

	buildNeighborSurfaces();

}

//Surface Builders
//=================

void MaterialColumn::buildNeighborSurfaces() noexcept
{
	for (my::Direction ownedDirection : ownedDirections) {
		if (this->_adjacientColumns.count(ownedDirection)) {
			buildGeneralNeighborSurfaces(ownedDirection);
			buildAirSurfaces(ownedDirection);
			buildEarthSurfaces(ownedDirection);
			buildHorizonSurfaces(ownedDirection);
			buildSeaSurfaces(ownedDirection);
		}
	}
}

void MaterialColumn::buildTopSurfaces() noexcept
{
	SharedSurface surface;
	//GENERAL TOP SURFACES
	//iterate through general column
	for (MaterialLayer *layer : _column) {
		if (layer->_up != nullptr) {
			surface = SharedSurface(layer, layer->_up, layer->getTopElevation(), layer->getType());
			layer->addSurface(surface);
		}
	}



	SharedAirSurface airSurface;

	//iterate through air column without hitting top layer (stratosphere)
	for (auto A = _air.begin(); A != _air.end() - 1; A++) {
		surface = SharedSurface(&(*A), &(*(A+1)), A->getTopElevation(), AIR);
		airSurface = SharedAirSurface(&(*A), &(*(A+1)), surface);
		A->addAirSurface(airSurface);
	}
	
	//TODO
	//Sea internal
	//Earth internal
	//(horizon internal?)
}

void MaterialColumn::buildGeneralNeighborSurfaces(my::Direction ownedDirection) noexcept
{
	SharedSurface surface;

	MaterialLayer *A = _column.front();//this column's layer
	MaterialLayer *B = _adjacientColumns[ownedDirection]->_column.front();//neighbor column's layer

	double A_bot, A_top, B_bot, B_top;
	double top, bot;
	const SpatialDirection sDirection = static_cast<SpatialDirection>(ownedDirection);

	do {
		A_bot = A->getBottomElevation();
		A_top = A->getTopElevation();

		B_bot = B->getBottomElevation();
		B_top = B->getTopElevation();

		if (A_top > B_top) {//A above
			if (B_top > A_bot) {//in contact

				top = B_top;
				bot = std::max(A_bot, B_bot);

				//failure check
				if (bot > top) { LOG("A_bot: " << A_bot << " A_top: " << A_top); LOG("B_bot: " << B_bot << " B_top: " << B_top); exit(EXIT_FAILURE); }

				surface = SharedSurface(A, B, sDirection, bot, top, B->getType());
				A->addSurface(surface);

			}
			if (B->_up == nullptr) return;
			B = B->_up; //increment B
		}
		else { //B above
			if (A_top > B_bot) {//in contact

				top = A_top;
				bot = std::max(A_bot, B_bot);

				//failure check
				if (bot > top) { LOG("A_bot: " << A_bot << " A_top: " << A_top); LOG("B_bot: " << B_bot << " B_top: " << B_top); exit(EXIT_FAILURE); }


				surface = SharedSurface(A, B, sDirection, bot, top, B->getType());
				A->addSurface(surface);

			}
			if (A->_up == nullptr) return;
			A = A->_up; //increment A
		}

	} while (true);
}

void MaterialColumn::buildEarthSurfaces(my::Direction ownedDirection) noexcept
{
	//the jumps between elevations of adjacient tiles does not represent water flow through the crust very well.
	//equilize the bedrock layer elevations and build earth pair surfaces so corresponding earth layers flow into each other
	//cliffs and springs will happen through the horizon layer as an exception
	//STUB
}
void MaterialColumn::buildHorizonSurfaces(my::Direction ownedDirection) noexcept
{
	//rivers, plants, animals are restricted to the horizon layer,
	//need neighboring horizons
	//STUB
}
void MaterialColumn::buildSeaSurfaces(my::Direction ownedDirection) noexcept
{
	//STUB
}

void MaterialColumn::buildAirSurfaces(my::Direction ownedDirection) noexcept
{
	SharedAirSurface airSurface;
	SharedSurface buildSurface;

	auto A = _air.begin();//this column's layer
	auto B = _adjacientColumns[ownedDirection]->_air.begin();//neighbor column's layer

	double A_bot, A_top, B_bot, B_top;
	double top, bot;
	const SpatialDirection sDirection = static_cast<SpatialDirection>(ownedDirection);

	do {
		A_bot = A->getBottomElevation();
		A_top = A->getTopElevation();

		B_bot = B->getBottomElevation();
		B_top = B->getTopElevation();

		if (A_top > B_top) {//A above
			if (B_top > A_bot) {//in contact

				top = B_top;
				bot = std::max(A_bot, B_bot);

				//failure check
				if (bot > top) { LOG("A_bot: " << A_bot << " A_top: " << A_top); LOG("B_bot: " << B_bot << " B_top: " << B_top); exit(EXIT_FAILURE); }


				buildSurface = SharedSurface(&(*A), &(*B), sDirection, bot, top, AIR);
				airSurface = SharedAirSurface(&(*A), &(*B), buildSurface);
				A->addAirSurface(airSurface);

			}
			B++;
			if (B == _adjacientColumns[ownedDirection]->_air.end()) { return; }
		}
		else { //B above
			if (A_top > B_bot) {//in contact

				top = A_top;
				bot = std::max(A_bot, B_bot);

				//failure check
				if (bot > top) { LOG("A_bot: " << A_bot << " A_top: " << A_top); LOG("B_bot: " << B_bot << " B_top: " << B_top); exit(EXIT_FAILURE); }


				buildSurface = SharedSurface(&(*A), &(*B), sDirection, bot, top, AIR);
				airSurface = SharedAirSurface(&(*A), &(*B), buildSurface);
				A->addAirSurface(airSurface);

			}
			A++;
			if (A == _air.end()) return;
		}

	} while (true);
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
	_backRadiation = 0;
	_escapeRadiation = 0;
	
	for (MaterialLayer* layer : _column) {
		layer->hourlyClear();
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

	double fraction = 1.0;
	
	upRadiation = surfaceLayer->emitInfraredRadiation();
	downRadiation.clear();

	//filter/emit upwards
	for (AirLayer &air : _air) {
		upRadiation = air.filterInfraredRadiation(upRadiation);
		emittedEnergy = air.emitInfraredRadiation();
		downRadiation.push_back(emittedEnergy / 2.0);
		upRadiation+= emittedEnergy / 2.0;
	}
	_escapeRadiation = upRadiation;


	double currentDownRadiation;
	auto radiation_rit = downRadiation.rbegin();//end of down radiation vector
	
	//filter downwards
	for (auto air_rit = _air.rbegin() + 1; air_rit != _air.rend(); ++air_rit) {
		currentDownRadiation = *radiation_rit;
		++radiation_rit;//advance down radiation reverse iterator
		*radiation_rit += air_rit->filterInfraredRadiation(currentDownRadiation);//filter radiation
	}
		
	_backRadiation = *radiation_rit;

	surfaceLayer->filterInfraredRadiation(*radiation_rit);
}

void MaterialColumn::simulateConduction() noexcept{
	for (MaterialLayer *layer : _column) {
		layer->simulateConduction();
	}
}

void MaterialColumn::simulatePressure() noexcept
{
	//build pressure on surfaces:
	for (AirLayer &air : _air) {
		air.computeSurfacePressures();
	}


}

void MaterialColumn::simulateAirFlow()  noexcept 
{
	//FLOW
	for (AirLayer &air : _air) {
		air.simulateFlow();
	}

	//bottom boundary condition
	Eigen::Vector3d horizonInertia= _air.begin()->getGasPtr()->getInertia();
	horizonInertia[2] = std::min(horizonInertia[2], 0.0);
	_air.front().getGasPtr()->setInertia(horizonInertia);

	//top boundary condition
	Eigen::Vector3d stratInertia = _air.rbegin()->getGasPtr()->getInertia();
	stratInertia[2] = std::max(stratInertia[2], 0.0);
	_air.rbegin()->getGasPtr()->setInertia(stratInertia);

	//polar filter?
	if (_adjacientColumns.count(my::NORTH_EAST) == 0) {
		for (auto &air : _air) {
			Eigen::Vector3d inertia = air.getGasPtr()->getInertia();
			if (inertia[1] < 0) {
				inertia[1] = 0;
				air.getGasPtr()->setInertia(inertia);
			}
		}
	}

	//antipolar filter
	if (_adjacientColumns.count(my::SOUTH_EAST) == 0) {
		for (auto &air : _air) {
			Eigen::Vector3d inertia = air.getGasPtr()->getInertia();
			if (inertia[1] > 0) {
				inertia[1] = 0;
				air.getGasPtr()->setInertia(inertia);
			}
		}
	}
}

void MaterialColumn::simulateCondensation() noexcept
{
	//STUB
}

void MaterialColumn::simulatePrecipitation() noexcept
{
	//STUB
}

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
		stream.str(std::string());
		stream << "Back Radiation: " << my::double2string(_backRadiation) << " KJ";
		messages.push_back(stream.str());

		stream.str(std::string());
		stream << "Escape Radiation: " << my::double2string(_escapeRadiation) << " KJ";
		messages.push_back(stream.str());
	}

	if ((statRequest._statType == ELEVATION) & (statRequest._section == SURFACE_)) {
		return _horizon.back().getMessages(statRequest);
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

	case(HORIZON_) : {

		if (_submerged) {
			return;//_chosenLayer stays nullptr;
		}

		auto layerReporting = &(_horizon.back());
		_chosenLayer = const_cast<HorizonLayer*>(layerReporting);
		return;
	}

	case(EARTH_) : {


		//6 horizon
		//5 subsoil
		//4 substratum 1
		//3 substratum 2
		//2 substratum 3
		//1 substratum 4
		//0 bedrock

		if (statRequest._layer == 6) {
			auto layerReporting = &(_horizon.back());
			_chosenLayer= const_cast<HorizonLayer*>(layerReporting);
			return;
		}

		auto layerReporting = _earth.rbegin();
		for (int i = 5; i > statRequest._layer; i--) {//advance downward to requested layer
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

		//0 2000 - 20000 m
		//1 200-2000 m
		//2 20-200 m
		//3 2-20 m
		//4 sea surface 0-2 meters deep

		auto layerReporting = _sea.rbegin();//surface
		for (int i = 4; i > statRequest._layer; i--) {//advance downward to requested layer
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



Eigen::Vector2d MaterialColumn::getAdvection(const StatRequest &statRequest) const noexcept
{
	chooseLayer(statRequest);//maybe redundant, but don't want to depend on previous call to set _chosenLayer

	return _chosenLayer->getAdvection();
}



}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene