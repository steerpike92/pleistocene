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
	MaterialLayer* layerHead;

	layerHead=buildEarth();
	layerHead=buildHorizon(layerHead);
	layerHead=buildSea(layerHead);
	buildAir(layerHead);
}

MaterialLayer* MaterialColumn::buildEarth()
{
	MaterialLayer* currentLayer;

	//build bedrock layer
	double bedrockElevation = _landElevation - layers::bedrockDepth;
	_earthRoot = new EarthLayer(_landElevation, _initialTemperature, bedrockElevation);
	_elevationMarker = _earthRoot->getTopElevation();

	currentLayer = _earthRoot;

	//build up to land elevation



	//return pointer to last earth layer
	return currentLayer;
}


MaterialLayer* MaterialColumn::buildHorizon(MaterialLayer* layerHead)
{



	return layerHead;
}

MaterialLayer* MaterialColumn::buildSea(MaterialLayer* layerHead)
{
	//build sea bottom layer


	//build up to sea surface elevation

	//return pointer to last sea layer
	return layerHead;
}

void MaterialColumn::buildAir(MaterialLayer* layerHead)
{
	//build boundary layer
	//build troposphere
	//build up to stratosphere
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