#pragma once
#include <Eigen/Dense>	//linear algebra

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {


class MaterialLayer;
class AirLayer;
class SeaLayer;
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
private:
	MaterialLayer *_tenantLayer;
	MaterialLayer *_ownerLayer;
protected:
	double _area;
	double _midpointElevation;
	
	double _ownerPressure;
	double _tenantPressure;

	double _pressureDifferential;

	Eigen::Vector3d _normalVector;
	void buildNormalVector() noexcept;

	bool _pressureBuilt = false;

public:
	SharedSurface() noexcept;

	//top shared surface constructor
	SharedSurface(MaterialLayer *ownerLayer, MaterialLayer *tenantLayer, double elevation, LayerType tenantType) noexcept;

	//side shared surface constructor
	SharedSurface(MaterialLayer *ownerLayer, MaterialLayer *tenantLayer, SpatialDirection spatialDirection,
		double bottomElevation, double topElevation, LayerType tenantType) noexcept;

	double getArea() const noexcept;

	double getMidpointElevation() const noexcept;

	SpatialDirection _spatialDirection;

	LayerType _tenantType;

	void performConduction() noexcept;

	MaterialLayer* getTenant() noexcept;

	virtual void buildPressureDifferential() noexcept;
	
	virtual void flow() noexcept;
};

class SharedAirSurface : public SharedSurface {

	AirLayer* _ownerAirLayer;
	AirLayer* _tenantAirLayer;

	double calculateEquilibriumExchange() const noexcept;

public:
	SharedAirSurface() noexcept;
	SharedAirSurface(AirLayer *ownerLayer, AirLayer *tenantLayer, SharedSurface &surface) noexcept;

	void buildPressureDifferential() noexcept;

	void flow() noexcept;
};



class SharedEarthSurface : public SharedSurface {
public:
	SpatialDirection _spatialDirection;
	EarthLayer *_earthLayer;
	double _heightGradient;
	void flow() noexcept;
};

class SharedHorizonSurface : public SharedSurface {
	HorizonLayer *_ownerHorizonLayer;
	HorizonLayer *_tenantHorizonLayer;
public:
	double heightGradient;
	void flow() noexcept;
};

class SharedSeaSurface : public SharedSurface {
public:
	void flow() noexcept;
};


}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene