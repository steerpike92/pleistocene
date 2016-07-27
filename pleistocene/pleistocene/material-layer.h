#pragma once
#include "globals.h"
#include "state-mixture.h"
#include "shared-surface.h"

namespace pleistocene {

namespace simulation {

struct StatRequest;

namespace climate {
namespace layers {

enum LayerType {
	EARTH,
	SEA,
	HORIZON,
	AIR
};



const std::vector<my::Direction> ownedDirections{ my::NORTH_EAST, my::EAST, my::SOUTH_EAST };


namespace earth {
const double bedrockDepth = 201; //201 m of simulated subterranian activity

const int earthLayers = 6;//if changed, update earthLayerHeights
const double earthLayerHeight = (bedrockDepth - 1) / (double(earthLayers - 1));
const double subSoilHeight = 0.8; //80 cm of subsoil
const double topSoilHeight = 0.2; //20 cm of topsoil in horizon

const double earthLayerHeights[] = { earthLayerHeight, earthLayerHeight,
	earthLayerHeight, earthLayerHeight, earthLayerHeight, subSoilHeight };
}


namespace sea {

const int kMaxSeaLayers = 5;
const double seaLayerElevations[kMaxSeaLayers+1] = { 0,-2,-20,-200,-2000,-20000 };

}


namespace air {

const int kMaxAirLayers = 6;//IF CHANGED, UPDATE airElevations!!!!!

const double boundaryLayerHeight = 1000;
const double tropopauseElevation = 11000;

const double troposphereLayerHeight = tropopauseElevation / (kMaxAirLayers-2); //-boundary layer, -stratosphere gives -2

const double stratopauseElevation = 18000;

//Rather ineligant, but I don't anticipate many changes to the number of max layers
const double airElevations[kMaxAirLayers] = { 0, troposphereLayerHeight, 2 * troposphereLayerHeight,
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



//////////==================================
//////////MATERIAL LAYER
//////////==================================


class MaterialLayer {
protected:
	//MaterialLayer is basically just a wrapper for a Mixture
	elements::Mixture *_mixture;

	double _bottomElevation;//Elevation above sea level (of bottom of layer)
	double _height;
	double _topElevation;//Elevation above sea level (of top of layer)

	double _baseElevation;

	double _bottomRelativeElevation;//Elevation relative to earth's surface (negative if below) (of bottom of layer)
	double _topRelativeElevation;//Elevation relative to earth's surface (negative if below) (of top of layer)



	layers::LayerType _layerType;


	std::vector<SharedSurface> _sharedSurfaces;

public:
	//INITIALIZATION
	//============================
	MaterialLayer() noexcept;
	MaterialLayer(double baseElevation, double bottomElevation, bool emittor) noexcept;

	MaterialLayer *_up = nullptr;
	MaterialLayer *_down = nullptr;

	bool _emittor;

	virtual void addSurface(SharedSurface &surface) noexcept;
	void clearSurfaces() noexcept;

	//SIMULATION
	//============================

	virtual void hourlyClear() noexcept;

	//chains downward
	void filterSolarRadiation(double energyKJ) noexcept;

	double emitInfraredRadiation() noexcept;
	double filterInfraredRadiation(double energyKJ) noexcept;

	void simulateConduction() noexcept;

	virtual void computeSurfacePressures() noexcept;

	virtual void simulateFlow() = 0;

	//GETTERS
	//============================
	double getTopElevation() const noexcept;
	double getBottomElevation() const noexcept;
	virtual double getTemperature() const noexcept;
	LayerType getType() const noexcept;
	elements::Mixture *getMixture() const noexcept;

	virtual double getPressure(double elevation) const noexcept;
	virtual Eigen::Vector2d getAdvection() const noexcept;

	virtual std::vector<std::string> getMessages(const struct StatRequest &statRequest) const noexcept;
	virtual double getStatistic(const struct StatRequest &statRequest) const noexcept;

};


////////////================================
////////////EARTH
////////////================================

class EarthLayer : public MaterialLayer {
protected:
	//unique earth member variables
	std::vector<layers::SharedEarthSurface> _sharedEarthSurfaces;

	std::unique_ptr<elements::SolidMixture> _solidPtr;
	elements::SolidMixture _tempSolid;

public:
	EarthLayer() noexcept;
	EarthLayer(double baseElevation, double temperature, double bottomElevation, double layerHeight, bool emittor) noexcept;

	//void addEarthSurface(SharedEarthSurface &earthSurface) noexcept;

	void simulateFlow() noexcept;

	elements::SolidMixture *getSolidPtr() noexcept;

	//Message getter
	virtual std::vector<std::string> getMessages(const struct StatRequest &statRequest) const noexcept;
	virtual double getStatistic(const struct StatRequest &statRequest) const noexcept;

private:
	//unique earth member functions

	//buildAlignedEarthSurfaces

	//Groundwater flow

	//Earth Material constructors
	static std::vector<elements::Element> generateSoil(double depth, double height) noexcept;
	static elements::ElementType determineEarthType(double depthIndex) noexcept;
	static elements::ElementType determineSoilType(double depthIndex) noexcept;
};


//////////==================================
//////////HORIZON
//////////==================================

class HorizonLayer : public EarthLayer {
	//unique horizon member variables

	std::vector<layers::SharedHorizonSurface> _sharedHorizonSurfaces;

	//TO DO

	//Transient water:
		//Rivers
		//Wetlands
		//Ponds
		//SnowPack
		//Glacier

	//Plants


public:
	HorizonLayer() noexcept;
	//~HorizonLayer() noexcept;
	HorizonLayer(double baseElevation, double temperature, double bottomElevation, bool emittor) noexcept;

	//Message getter
	std::vector<std::string> getMessages(const struct StatRequest &statRequest) const noexcept;
	double getStatistic(const struct StatRequest &statRequest) const noexcept;

	//void addHorizonSurface(SharedHorizonSurface &horizonSurface) noexcept;

private:
	//unique horizon member functions
	//void buildNeighbors() noexcept;

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


//////////==================================
//////////SEA
//////////==================================

class SeaLayer : public MaterialLayer {
	//unique sea member variables
	Eigen::Vector3d inertialCurrentVector;

	std::vector<layers::SharedSeaSurface> _sharedSeaSurfaces;

	std::unique_ptr<elements::LiquidMixture> _liquidPtr;
	elements::LiquidMixture _tempLiquid;


public:
	SeaLayer() noexcept;
	SeaLayer(double baseElevation, double temperature, double bottomElevation, double topElevation, bool emittor) noexcept;

	void simulateFlow() noexcept;

	//void addSeaSurface(SharedSeaSurface &seaSurface) noexcept;

	elements::LiquidMixture *getLiquidPtr() noexcept;

	//Message getter
	std::vector<std::string> getMessages(const struct StatRequest &statRequest) const noexcept;
	double getStatistic(const struct StatRequest &statRequest) const noexcept;
};


//////////==================================
//////////AIR
//////////==================================

class AirLayer : public MaterialLayer {
	//unique air member variables
	Eigen::Vector3d inertialWindVector;

	std::vector<layers::SharedAirSurface> _sharedAirSurfaces;

	std::unique_ptr<elements::GaseousMixture> _gasPtr;
	elements::GaseousMixture _tempGas;


	double incidentUpRadiation;
	double incidentDownRadiation;

public:
	AirLayer() noexcept;
	AirLayer(double baseElevation, double temperature, double bottomElevation, double fixedTopElevation) noexcept;

	void simulateFlow() noexcept;

	void addSurface(SharedSurface &surface) noexcept;
	void addAirSurface(SharedAirSurface &airSurface) noexcept;

	void computeSurfacePressures() noexcept;

	void applyCoriolisForce(double latitude_rad) noexcept;

	elements::GaseousMixture *getGasPtr() noexcept;
	elements::GaseousMixture *getTempPtr() noexcept;

private:
	//unique air member functions

	static std::vector<elements::Element> generateAirElements(double bottomElevation, double topElevation) noexcept;

	static double expectedHydrostaticPressureCalculator(double elevation) noexcept;
	static double expectedMolsCalculator(double bottomElevation, double topElevation) noexcept;
	static double expectedTemperatureCalculator(double elevation) noexcept;

	double lapsedTemperatureCalculator(double elevation) const noexcept;
	double truePressureCalculator(double elevation) const noexcept;

public:

	void hourlyClear() noexcept;

	double getPressure(double elevation) const noexcept;

	double getTemperature() const noexcept;

	//Message getter
	std::vector<std::string> getMessages(const struct StatRequest &statRequest) const noexcept;
	double getStatistic(const struct StatRequest &statRequest) const noexcept;

	Eigen::Vector2d getAdvection() const noexcept;

};


}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene