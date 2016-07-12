#include "materialColumn.h"
#include "materialLayer.h"
#include "globals.h"

MaterialColumn::MaterialColumn() {}
MaterialColumn::~MaterialColumn(){}


MaterialColumn::MaterialColumn(double landElevation, double initialTemperature):
	_landElevation(landElevation),
	_initialTemperature(initialTemperature)
{
	MaterialLayer* currentLayer;

	currentLayer =buildEarth();
	currentLayer =buildHorizon(currentLayer);

	if (_landElevation < 0) {
		currentLayer = buildSea(currentLayer,0);
	}
	else { _seaBottom = nullptr; }

	buildAir(currentLayer);
}

MaterialLayer* MaterialColumn::buildEarth()
{
	using namespace layers::earth;
	MaterialLayer *buildLayer;
	//build bedrock layer
	double bedrockElevation = _landElevation - bedrockDepth;

	//ownedLayers.emplace_back(new EarthLayer(_landElevation, _initialTemperature, bedrockElevation, earthLayerHeights[0]));
	//auto iterator = ownedLayers.rbegin();
	//buildLayer=iterator->get();

	//_bedrock = buildLayer;

	for (int i = 1; i < earthLayers; i++){
		double layerHeight = earthLayerHeights[i];
		//ownedLayers.emplace_back(new EarthLayer(_landElevation, _initialTemperature, buildLayer,layerHeight));
		//auto iterator = ownedLayers.rbegin();
		//buildLayer = iterator->get();
	}
	//return pointer to last earth layer
	//return buildLayer;
	return nullptr;
}


MaterialLayer* MaterialColumn::buildHorizon(MaterialLayer* previousLayer)
{
	//ownedLayers.emplace_back(new HorizonLayer(_landElevation, _initialTemperature, previousLayer));
	//auto iterator = ownedLayers.rbegin();
	//_horizon = iterator->get();
	_elevationMarker = _horizon->getTopElevation();
	return _horizon;
}

MaterialLayer* MaterialColumn::buildSea(MaterialLayer* previousLayer, double seaSurfaceElevation)
{

	using namespace layers::sea;
	SeaLayer *buildLayer;

	double seaBottomElevation = previousLayer->getTopElevation();

	int i=0;//position in seaLayerElevations array
	
	while (seaBottomElevation < seaLayerElevations[i]+ seaSurfaceElevation) {
		i++;
	}
	if (i >= 6 || i == 0) { LOG("Inappropriate Sea Depth:"); LOG(seaBottomElevation); throw(2); return buildLayer; }

	i--;

	//build sea bottom layer
	double topElevation = seaLayerElevations[i]+seaSurfaceElevation;
	buildLayer = new SeaLayer(_landElevation, _initialTemperature, previousLayer,topElevation);
	_elevationMarker = buildLayer->getTopElevation();
	_seaBottom = buildLayer;
	i--;

	//build upper layers
	while (i >= 0) {
		double topElevation = seaLayerElevations[i]+seaSurfaceElevation;
		buildLayer = new SeaLayer(_landElevation, _initialTemperature, buildLayer, topElevation);
		_elevationMarker = buildLayer->getTopElevation();
		i--;
	}

	_seaSurface = buildLayer;

	return _seaSurface;
}

void MaterialColumn::buildAir(MaterialLayer* previousLayer)
{
	using namespace layers::air;
	//build troposphere
	//build up to stratosphere

	using namespace layers;
	AirLayer *buildLayer, *oldLayer;

	//build boundary layer
	double boundaryLayerTopElevation = _elevationMarker + boundaryLayerHeight;
	buildLayer = new AirLayer(_landElevation, _initialTemperature,previousLayer, boundaryLayerTopElevation);
	_elevationMarker = buildLayer->getTopElevation();
	_boundaryLayer = buildLayer;
	oldLayer = buildLayer;

	int i = 0;
	for (; _elevationMarker > airElevations[i]; i++);//adjusts to find first troposphere top height

	for (; i < 6; i++) {
		buildLayer = new AirLayer(_landElevation, _initialTemperature, oldLayer, airElevations[i]);
		oldLayer = buildLayer;
	}
	_stratosphere = buildLayer;
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
}

void MaterialColumn::simulateAirFlow(){}
void MaterialColumn::simulateWaterFlow(){}
void MaterialColumn::simulatePlants(){}