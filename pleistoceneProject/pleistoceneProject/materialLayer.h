#pragma once
#include "globals.h"
#include "stateMixture.h"

class MaterialLayer {

	Mixture *_stateMixture;

	double _bottomElevation;
	double _height;
	double _topElevation;

	MaterialLayer* _above = nullptr;//i.e. next node
	MaterialLayer* _below = nullptr;//i.e. previous node

public:
	MaterialLayer();
	~MaterialLayer();
	MaterialLayer(std::vector<Element> Elements, double temperature, double bottomElevation);//bedrockConstructor
	MaterialLayer(std::vector<Element> Elements, elements::State state, double temperature, MaterialLayer *layerBelow,
		double fixedTopElevation=my::FakeDouble);
	MaterialLayer* getAbove() const;
	MaterialLayer* getBelow() const;
private:


	
};


class MaterialColumn {

	MaterialLayer *_bedrockHead = nullptr;//bottom Earth Layer
	MaterialLayer *_topSoilHead = nullptr;//bottom Horizon Layer
	MaterialLayer *_seaBottomHead = nullptr;//bottom sea Layer
	MaterialLayer *_boundaryLayerHead = nullptr;//bottom air layer

	double _elevationTop;//used in construction

public:
	MaterialColumn();
	~MaterialColumn();
	MaterialColumn(double landElevation);

	void filterSolarRadiation(double incidentSolarRadiation);
	void simulateInfraredRadiation();
	void simulateEvaporation();
	void simulateCondensation();
	void simulatePrecipitation();

private:
	//returns true while it needs to be called again
	//returns false after creating the stratosphere
	bool buildMaterialLayer();
};