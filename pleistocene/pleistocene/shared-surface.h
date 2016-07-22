#pragma once
#include <Eigen/Dense>	//linear algebra

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {

class MaterialLayer;
class EarthLayer;
class HorizonLayer;
enum LayerType;

enum SpatialDirection {
	NORTH_EAST,
	EAST,
	SOUTH_EAST,
	SOUTH_WEST,
	WEST,
	NORTH_WEST,
	UP,
	DOWN
};


class SharedSurface {
	double _area;
	double _midpointElevation;
	
	double _ownerPressure;
	double _tenantPressure;

	double _pressureDifferential;

	MaterialLayer *_tenantLayer;
	MaterialLayer *_ownerLayer;

	Eigen::Vector3d _normalVector;
	void buildNormalVector() noexcept;

	bool _pressureBuilt = false;

public:
	
	SharedSurface() noexcept;

	//top shared surface constructor
	SharedSurface(MaterialLayer *ownerLayer, MaterialLayer *tenantLayer, double elevaton, LayerType tenantType) noexcept;

	//side shared surface constructor
	SharedSurface(MaterialLayer *ownerLayer, MaterialLayer *tenantLayer, SpatialDirection spatialDirection,
		double bottomElevation, double topElevation, LayerType tenantType) noexcept;

	double getArea() const noexcept;

	double getMidpointElevation() const noexcept;

	SpatialDirection _spatialDirection;

	LayerType _tenantType;

	void performConduction() noexcept;

	void buildPressureDifferential() noexcept;
	
	void pressureFlow() noexcept;

};


struct SharedEarthSurface {
	SpatialDirection _spatialDirection;
	EarthLayer *_earthLayer;
	double _area;
	double _heightGradient;
};

struct NeighborHorizon {
	HorizonLayer *neighbor;
	double heightGradient;
};


}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene