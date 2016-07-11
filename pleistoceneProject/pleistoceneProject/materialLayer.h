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

	namespace earth {
		const double bedrockDepth = 201; //201 m of simulated subterranian activity

		const int earthLayers = 6;//if changed, update earthLayerHeights
		const double earthLayerHeight = (bedrockDepth - 1) / (earthLayers - 1);
		const double subSoilHeight = 0.8; //80 cm of subsoil
		const double topSoilHeight = 0.2; //20 cm of topsoil in horizon

		const double earthLayerHeights[] = { earthLayerHeight, earthLayerHeight,
			earthLayerHeight, earthLayerHeight, earthLayerHeight, subSoilHeight,topSoilHeight };
	}

	
	namespace sea {

		const double seaLayerDepths[] = { 2,20,200,2000,20000 };

	}



	namespace air {

		const double boundaryLayerHeight = 200;
		const double tropopauseElevation = 11000;

		const double troposphereLayerHeight = tropopauseElevation/4.0;

		const double stratopauseElevation = 32000;

		const double airElevations[] = { 0, troposphereLayerHeight, 2 * troposphereLayerHeight,
			3 * troposphereLayerHeight, 4 * troposphereLayerHeight, stratopauseElevation };


		const double R = 8.31432;	//universal gas constant (J/(k*mol))
		const double Md = 0.0289644;	//Molar mass of dry air
		const double Mv = 0.0180153;	//Molar mass of water vapor
		const double g = 9.80665;

		const double StandardElevation[2] = { 0,11000 };
		const double StandardPressure[2] = { 101325, 22632 };
		const double StandardTemperature[2] = { 288.15, 216.65 };
		const double StandardLapseRate[2] = { -0.0065, 0 };
	}




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
	EarthLayer(double earthSurfaceElevation, double temperature, double bottomElevation, double layerHeight);//bedrockConstructor
	EarthLayer(double earthSurfaceElevation, double temperature, MaterialLayer *layerBelow, double layerHeight);
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
	static double expectedTemperatureCalculator(double elevation);

	double lapsedTemperatureCalculator(double elevation)const;
	double truePressureCalculator(double elevation)const;
};