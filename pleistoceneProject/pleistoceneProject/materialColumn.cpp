#include "materialColumn.h"
#include "materialLayer.h"
#include "globals.h"

MaterialColumn::MaterialColumn() {}

MaterialColumn::~MaterialColumn()
{
	if (_earthRoot != nullptr) {
		MaterialLayer *head = _earthRoot;
		MaterialLayer *temp = head->getAbove();
		delete head;

		while (temp != nullptr) {
			head = temp;
			temp = head->getAbove();
			delete head;
		}

		//DELETE MATERIAL LAYERS
	}
}


MaterialColumn::MaterialColumn(double landElevation, double initialTemperature):
	_landElevation(landElevation),
	_initialTemperature(initialTemperature)

{
	MaterialLayer* currentLayer;

	currentLayer =buildEarth();
	currentLayer =buildHorizon(currentLayer);

	if (_landElevation < 0) {
		currentLayer = buildSea(currentLayer);
	}
	else { _seaRoot = nullptr; }

	buildAir(currentLayer);
}

MaterialLayer* MaterialColumn::buildEarth()
{
	using namespace layers::earth;
	EarthLayer *buildLayer, *oldLayer;
	//build bedrock layer
	double bedrockElevation = _landElevation - bedrockDepth;
	
	buildLayer= new EarthLayer(_landElevation, _initialTemperature, bedrockElevation, earthLayerHeights[0]);
	_elevationMarker = buildLayer->getTopElevation();

	_earthRoot = buildLayer;
	
	oldLayer = buildLayer;

	for (int i = 1; i < earthLayers; i++){
		double layerHeight = earthLayerHeights[i];
		buildLayer = new EarthLayer(_landElevation, _initialTemperature, oldLayer,layerHeight);
		_elevationMarker = buildLayer->getTopElevation();
		oldLayer = buildLayer;
	}
	
	
	//return pointer to last earth layer
	return buildLayer;
}


MaterialLayer* MaterialColumn::buildHorizon(MaterialLayer* previousLayer)
{
	_horizonRoot = new HorizonLayer(_landElevation, _initialTemperature, previousLayer);
	_elevationMarker = _horizonRoot->getTopElevation();
	return _horizonRoot;
}

MaterialLayer* MaterialColumn::buildSea(MaterialLayer* previousLayer)
{
	//build sea bottom layer

	using namespace layers;
	SeaLayer *buildLayer, *oldLayer;

	//build sea bottom layer

	buildLayer = new SeaLayer(_landElevation, _initialTemperature, previousLayer,0);
	_elevationMarker = buildLayer->getTopElevation();
	_seaRoot = buildLayer;
	oldLayer = buildLayer;

	//while (_elevationMarker < 0) {//build up to sea level
	//	buildLayer = new SeaLayer(_landElevation, _initialTemperature, oldLayer);
	//	_elevationMarker = buildLayer->getTopElevation();
	//	oldLayer = buildLayer;
	//}

	//return pointer to last sea layer
	return buildLayer;
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
	_airRoot = buildLayer;
	oldLayer = buildLayer;

	int i = 0;
	for (; _elevationMarker > airElevations[i]; i++);//adjusts to find first troposphere top height

	for (; i < 6; i++) {
		buildLayer = new AirLayer(_landElevation, _initialTemperature, oldLayer, airElevations[i]);
		oldLayer = buildLayer;
	}
	_airTail = buildLayer;
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