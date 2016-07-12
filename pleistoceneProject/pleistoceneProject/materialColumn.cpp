#include "materialColumn.h"
#include "materialLayer.h"
#include "globals.h"

MaterialColumn::MaterialColumn() {}

MaterialColumn::MaterialColumn(double landElevation, double initialTemperature):
	_landElevation(landElevation),
	_initialTemperature(initialTemperature)
{
	_earth.column.clear();
	_horizon.column.clear();
	_sea.column.clear();
	_air.column.clear();


	double baseElevation=buildEarth();
	baseElevation=buildHorizon(baseElevation);

	if (_landElevation < 0) {
		baseElevation = buildSea(baseElevation,0);
	}

	//buildAir(baseElevation);
}

double MaterialColumn::buildEarth()
{
	using namespace layers::earth;

	//build bedrock layer
	double bedrockElevation = _landElevation - bedrockDepth;

	double currentElevation;

	_earth.column.emplace_back(_landElevation, _initialTemperature, bedrockElevation, earthLayerHeights[0]);

	currentElevation = _earth.column.rbegin()->getTopElevation();


	//build remaining earth layers
	for (int i = 1; i < earthLayers; i++){
		double layerHeight = earthLayerHeights[i];
		_earth.column.emplace_back(_landElevation, _initialTemperature, bedrockElevation, earthLayerHeights[i]);
	}

	currentElevation = _earth.column.rbegin()->getTopElevation();
	return currentElevation;
}


double MaterialColumn::buildHorizon(double baseElevation)
{
	_horizon.column.emplace_back(_landElevation, _initialTemperature, baseElevation);
	double currentElevation =_horizon.column.rbegin()->getTopElevation();
	return currentElevation;
}

double MaterialColumn::buildSea(double baseElevation, double seaSurfaceElevation)
{

	using namespace layers::sea;

	double seaBottomElevation = baseElevation;

	int i=0;//position in seaLayerElevations array
	
	while (seaBottomElevation < seaLayerElevations[i] + seaSurfaceElevation) {
		i++;
	}
	if (i >= 6 || i == 0) { LOG("Inappropriate Sea Depth:"); LOG(seaBottomElevation); throw(2); return 0; }

	i--;

	//build sea bottom layer
	double topElevation = seaLayerElevations[i]+seaSurfaceElevation;
	_sea.column.emplace_back(_landElevation, _initialTemperature, baseElevation,topElevation);
	baseElevation = topElevation;
	i--;

	//build upper layers
	while (i >= 0) {
		double topElevation = seaLayerElevations[i]+seaSurfaceElevation;
		_sea.column.emplace_back(_landElevation, _initialTemperature, baseElevation, topElevation);
		baseElevation = topElevation;
		i--;
	}

	return topElevation;
}

void MaterialColumn::buildAir(double baseElevation)
{
	using namespace layers::air;

	//build boundary layer
	double boundaryLayerTopElevation = baseElevation + boundaryLayerHeight;

	_air.column.emplace_back(_landElevation, _initialTemperature, baseElevation, boundaryLayerTopElevation);

	baseElevation = boundaryLayerTopElevation;

	int i = 0;
	for (; baseElevation > airElevations[i]; i++);//adjusts to find first troposphere top height

	for (; i < 6; i++) {
		_air.column.emplace_back(_landElevation, _initialTemperature, baseElevation, airElevations[i]);
		baseElevation = airElevations[i];
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



void MaterialColumn::filterSolarRadiation(double incidentSolarRadiation)
{
	//STUB
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


double MaterialColumn::getLandElevation()const {return _horizon.column.rbegin()->getTopElevation();}
double MaterialColumn::getSurfaceTemperature()const {return _horizon.column.rbegin()->getTemperature();}//STUB (check if sea)
double MaterialColumn::getBoundaryLayerTemperature()const{return 0.0;}//STUB
