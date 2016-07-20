#pragma once

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {

class MaterialLayer;

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
public:

	SpatialDirection _spatialDirection;
	MaterialLayer *_materialLayer;
	double _area;
	double _midpointElevation;


	SharedSurface() noexcept;

	//top shared surface constructor
	SharedSurface(MaterialLayer *materialLayer, double elevaton)  noexcept;

	//side shared surface constructor
	SharedSurface(SpatialDirection spatialDirection, MaterialLayer *materialLayer, double bottomElevation, double topElevation) noexcept;
};

//
//struct SharedEarthSurface {
//	SpatialDirection _spatialDirection;
//	EarthLayer *_earthLayer;
//	double _area;
//	double _heightGradient;
//};
//
//struct NeighborHorizon {
//	HorizonLayer *neighbor;
//	double heightGradient;
//};


}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene