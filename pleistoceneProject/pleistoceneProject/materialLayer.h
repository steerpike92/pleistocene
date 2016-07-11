#pragma once
#include "globals.h"
#include "stateMixture.h"

class MaterialLayer;
class EarthLayer;
class HorizonLayer;

namespace layers {
	enum LayerType {
		EARTH,
		SEA,
		HORIZON,
		AIR
	};

	enum SpatialDirection {
		UP,
		DOWN,
		NORTH_EAST,
		EAST,
		SOUTH_EAST,
		SOUTH_WEST,
		WEST,
		NORTH_WEST
	};

	struct SharedSurface {
		SpatialDirection spatialDirection;
		MaterialLayer *materialLayer;
		double area;
	};

	struct SharedEarthSurface {
		SpatialDirection spatialDirection;
		EarthLayer *earthLayer;
		double area;
		double heightGradient;
	};

	struct NeighborHorizon {
		HorizonLayer *neighbor;
		double heightGradient;
	};

	const double bedrockDepth = 200; //200 m of simulated subterranian activity

	const double initialEarthLayerHeight = 20;

	const double topSoilHeight = 0.2; //20 cm of topsoil in horizon

}

//==================================
//MATERIAL LAYER
//==================================

class MaterialLayer {
protected:
	
	Mixture *_stateMixture;

	double _bottomElevation;//Elevation above sea level (of bottom of layer)
	double _height;
	double _topElevation;//Elevation above sea level (of top of layer)

	double _earthSurfaceElevation;

	double _bottomRelativeElevation;//Elevation relative to earth's surface (negative if below) (of bottom of layer)
	double _topRelativeElevation;//Elevation relative to earth's surface (negative if below) (of top of layer)


	layers::LayerType _layerType;

	MaterialLayer* _above = nullptr;//i.e. next node
	MaterialLayer* _below = nullptr;//i.e. previous node

	std::vector<layers::SharedSurface> _sharedSurfaces;

public:
	MaterialLayer();
	~MaterialLayer();
	MaterialLayer(double earthSurfaceElevation, MaterialLayer *layerBelow = nullptr, double bottomElevation = my::FakeDouble);
	MaterialLayer* getAbove() const;
	MaterialLayer* getBelow() const;

	double getTopElevation()const;
	double getBottomElevation()const;

};

//==================================
//EARTH
//================================

class EarthLayer : public MaterialLayer {
protected:
	//unique earth member variables
	std::vector<layers::SharedEarthSurface> _sharedEarthSurfaces;

public:
	EarthLayer();
	~EarthLayer();
	EarthLayer(double earthSurfaceElevation, double temperature, double bottomElevation);//bedrockConstructor
	EarthLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow);
	EarthLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow, bool horizonConstructor);//horizon constructor
protected:
	//unique earth member functions

	//buildAlignedEarthSurfaces

	//Groundwater flow

	//Earth Material constructors
	static std::vector<Element> generateSoil(double depth, double height);
	static elements::ElementType determineEarthType(double depthIndex);
	static elements::ElementType determineSoilType(double depthIndex);
};


//==================================
//HORIZON
//==================================

class HorizonLayer : public EarthLayer{
	//unique horizon member variables
	std::map<my::Direction, layers::NeighborHorizon> neighbors;

	//TO DO

	//Transient water:
		//Rivers
		//Wetlands
		//Ponds
		//SnowPack
		//Glacier

	//Plants


public:
	HorizonLayer();
	~HorizonLayer();
	HorizonLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow);

private:
	//unique horizon member functions
	//void buildNeighbors();

	//TO DO

	//Soil:
		//Evolution (organic deposits, nutrients)
		//Erosion/sedimentation height changes

	//Water:

		//Surface water flow
		//Glacial flow

	//Plants:
		//Plant light filtration and photosynthesis
		//Plant water suction
		//Plant transpiration
		//Plant growth


	//Getters for tile will mostly be directed here.
	//Will want to know:
		//harvestable plants
		//fresh water sources
		//temperature
		//elevation gradients

};

//==================================
//SEA
//==================================

class SeaLayer : public MaterialLayer {
	//unique sea member variables
	Eigen::Vector3d inertialCurrentVector;

public:
	SeaLayer();
	~SeaLayer();
	SeaLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow, double fixedTopElevation = my::FakeDouble);
};

//==================================
//AIR
//==================================

class AirLayer : public MaterialLayer {
	//unique air member variables
	Eigen::Vector3d inertialWindVector;

public:
	AirLayer();
	~AirLayer();
	AirLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow, double fixedTopElevation);

private:
	//unique air member functions

	static std::vector<Element> generateAirElements(double bottomElevation, double topElevation);

	static double expectedHydrostaticPressureCalculator(double elevation);
	static double expectedMolsCalculator(double bottomElevation, double topElevation);

	double truePressureCalculator(double elevation);
};