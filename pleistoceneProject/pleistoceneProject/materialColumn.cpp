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

	_earth.emplace_back(_landElevation, _initialTemperature, bedrockElevation, earthLayerHeights[0]);

	currentElevation = _earth.back().getTopElevation();

	//build remaining earth layers
	for (int i = 1; i < earthLayers; i++){
		double layerHeight = earthLayerHeights[i];
		_earth.emplace_back(_landElevation, _initialTemperature, bedrockElevation, earthLayerHeights[i]);
	}

	currentElevation = _earth.back().getTopElevation();
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
	
	//build boundary layer
	_air.emplace_back(baseElevation, _initialTemperature, layerBottomElevation, layerTopElevation);
	layerBottomElevation = layerTopElevation;

	int i = 0;
	for (; layerBottomElevation > airElevations[i]; i++);//adjusts to find first troposphere top height

	for (; i < 6; i++) {
		layerTopElevation = airElevations[i];
		_air.emplace_back(baseElevation, _initialTemperature, layerBottomElevation, layerTopElevation);
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
	//STUB
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

void MaterialColumn::simulateAirFlow(){}
void MaterialColumn::simulateWaterFlow(){}
void MaterialColumn::simulatePlants(){}



////////////==================================
////////////GETTERS
////////////==================================

double MaterialColumn::getLandElevation()const {return _horizon.back().getTopElevation();}
double MaterialColumn::getSurfaceTemperature()const {return _horizon.back().getTemperature();}//STUB (check if sea)
double MaterialColumn::getBoundaryLayerTemperature()const{return 1.0;}//STUB
