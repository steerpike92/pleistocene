#pragma once
#include "globals.h"
#include "material-layer.h"
#include "state-mixture.h"
#include "element.h"

namespace pleistocene {

namespace simulation {

struct StatRequest;

namespace climate {
namespace layers {



class MaterialColumn {
	//Object owning vectors
	std::vector<EarthLayer> _earth;
	std::vector<HorizonLayer> _horizon;
	std::vector<SeaLayer> _sea;
	std::vector<AirLayer> _air;

	//conveniently assignable during const calls

	//non owning vector with pointers to all layers
	std::vector<MaterialLayer*> _column;

	double _landElevation;
	bool _submerged;
	double _initialTemperature;

	double _latitude_rad;

	double _escapeRadiation;
	double _backRadiation;

	std::map<my::Direction, MaterialColumn*> _adjacientColumns;

	//====================================================
	//INITIALIZATION
	//====================================================
public:
	MaterialColumn() noexcept;
	MaterialColumn(double landElevation, double initialTemperature, double latitude_rad) noexcept;


private:
	//layer builders
	//================
	double buildEarth() noexcept;
	double buildHorizon(double baseElevation) noexcept;
	double buildSea(double baseElevation, double seaSurfaceElevation) noexcept;
	void buildAir(double baseElevation) noexcept;

	//Relation Builders
	//=================
public: 
	void buildAdjacency(std::map<my::Direction, MaterialColumn*> &adjacientColumns) noexcept; 
private:
	void buildUniversalColumn() noexcept;

	//Surface Builders
	//=================
	void buildNeighborSurfaces() noexcept;

	void buildTopSurfaces() noexcept;
	void buildGeneralNeighborSurfaces(my::Direction ownedDirection) noexcept;

	void buildEarthSurfaces(my::Direction ownedDirection) noexcept;
	void buildHorizonSurfaces(my::Direction ownedDirection) noexcept;
	void buildSeaSurfaces(my::Direction ownedDirection) noexcept;
	void buildAirSurfaces(my::Direction ownedDirection) noexcept;

	void elevationChangeProcedure() noexcept;

	//====================================================
	//SIMULATION
	//====================================================
public:
	void beginNewHour() noexcept;

	void filterSolarRadiation(double incidentSolarRadiation) noexcept;
	void simulateEvaporation() noexcept;
	void simulateInfraredRadiation() noexcept;
	void simulateConduction() noexcept;
	void simulatePressure() noexcept;
	void simulateCondensation() noexcept;
	void simulatePrecipitation() noexcept;
	void simulateAirFlow() noexcept;
	void simulateWaterFlow() noexcept;
	void simulatePlants() noexcept;


	//====================================================
	//GETTERS
	//====================================================

	double getLandElevation() const noexcept;
	double getSurfaceTemperature() const noexcept;
	double getBoundaryLayerTemperature() const noexcept;

	std::vector<std::string> getMessages(const StatRequest &statRequest) const noexcept;

	double getStatistic(const StatRequest &statRequest) const noexcept;

	Eigen::Vector2d getAdvection(const StatRequest &statRequest) const noexcept;

	//Getter helpers
private:
	void chooseLayer(const StatRequest &statRequest) const noexcept;
	mutable MaterialLayer *_chosenLayer;
};

}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene