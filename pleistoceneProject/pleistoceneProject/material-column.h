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

	double _escapeRadiation;
	double _backRadiation;

	std::map<my::Direction, MaterialColumn*> _adjacientColumns;

	//====================================================
	//INITIALIZATION
	//====================================================
public:
	MaterialColumn() noexcept;
	MaterialColumn(double landElevation, double initialTemperature) noexcept;

	void buildAdjacency(std::map<my::Direction, MaterialColumn*> &adjacientColumns) noexcept;

private:
	//layer builders
	double buildEarth() noexcept;
	double buildHorizon(double baseElevation) noexcept;
	double buildSea(double baseElevation, double seaSurfaceElevation) noexcept;
	void buildAir(double baseElevation) noexcept;

	void buildUniversalColumn() noexcept;

	//layer relationship builders
	void buildMaterialLayerSurfaces() noexcept;

	void buildVerticalSurfaces() noexcept;
	void buildNeighborSurfaces(my::Direction) noexcept;

	void buildEarthLayerSurfaces() noexcept;
	void buildHorizonNeighborhood() noexcept;

	void elevationChangeProcedure() noexcept;

	//====================================================
	//SIMULATION
	//====================================================
public:
	void filterSolarRadiation(double incidentSolarRadiation) noexcept;
	void simulateEvaporation() noexcept;
	void simulateInfraredRadiation() noexcept;
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

private:
	void chooseLayer(const StatRequest &statRequest) const noexcept;
	mutable MaterialLayer *_chosenLayer;
};

}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene