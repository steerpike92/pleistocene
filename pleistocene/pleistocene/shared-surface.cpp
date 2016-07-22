#include "shared-surface.h"
#include "material-layer.h"
#include "state-mixture.h"

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {


SharedSurface::SharedSurface() noexcept{}

//top surface constructor
SharedSurface::SharedSurface(MaterialLayer *ownerLayer, MaterialLayer *tenantLayer, double elevaton, LayerType layerType)  noexcept :
_ownerLayer(ownerLayer),
_tenantLayer(tenantLayer),
_spatialDirection(UP),
_midpointElevation(elevaton),
_area(100 * 1000 * 1000),
_tenantType(layerType)
{
	buildNormalVector();
}

//side shared surface constructor
SharedSurface::SharedSurface(MaterialLayer *ownerLayer, MaterialLayer *materialLayer, SpatialDirection spatialDirection,
	double bottomElevation, double topElevation, LayerType layerType) noexcept :
_ownerLayer(ownerLayer),
_tenantLayer(materialLayer),
_spatialDirection(spatialDirection),
_midpointElevation((topElevation + bottomElevation) / 2),
_area((topElevation - bottomElevation) * (6.2 * 1000)),//height*width
_tenantType(layerType)
{
	if (topElevation < bottomElevation) {
		LOG("Inverted Surface"); exit(EXIT_FAILURE);
	}

	buildNormalVector();
}

void SharedSurface::buildNormalVector() noexcept
{
	switch (_spatialDirection) {
	case(UP) :
		_normalVector = Eigen::Vector3d{ 0,0,1 };
		return;
	case(DOWN) :
		_normalVector = Eigen::Vector3d{ 0,0,-1 };
		return;
	case(NORTH_EAST):
		_normalVector = Eigen::Vector3d{ .5,-.5*pow(3,.5),0 };
		return;
	case(EAST):
		_normalVector = Eigen::Vector3d{ 1,0,0 };
		return;
	case(SOUTH_EAST):
		_normalVector = Eigen::Vector3d{ .5,.5*pow(3,.5),0 };
		return;
	case(SOUTH_WEST):
		_normalVector = Eigen::Vector3d{ -.5,.5*pow(3,.5),0 };
		return;
	case(WEST):
		_normalVector = Eigen::Vector3d{ -1,0,0 };
		return;
	case(NORTH_WEST):
		_normalVector = Eigen::Vector3d{ -.5,-.5*pow(3,.5),0 };
		return;
	}
}

double SharedSurface::getArea() const noexcept { return _area; }
double SharedSurface::getMidpointElevation() const noexcept { return _midpointElevation; }


void SharedSurface::performConduction() noexcept 
{
	elements::Mixture::conduction(*(_tenantLayer->getMixture()), *(_ownerLayer->getMixture()), _area);
}

void SharedSurface::buildPressureDifferential() noexcept
{

	_ownerPressure = _ownerLayer->getPressure(_midpointElevation);
	_tenantPressure = _tenantLayer->getPressure(_midpointElevation);

	_pressureDifferential = _ownerPressure - _tenantPressure;
	_pressureBuilt = true;

}

void SharedSurface::pressureFlow() noexcept 
{
	if (!_pressureBuilt) return;

	double flowConstant=6.2*pow(10,-14);

	double flowRate = _area * _pressureDifferential * flowConstant;

	bool backflow = signbit(flowRate);
	flowRate = abs(flowRate);
	flowRate = std::min(flowRate, 0.1);

	double givingTemperature;

	if (backflow) {
		_tenantLayer->getMixture()->transferMixture(_ownerLayer->getMixture(), flowRate);
	}
	else {
		_ownerLayer->getMixture()->transferMixture(_tenantLayer->getMixture(), flowRate);
	}

	_pressureBuilt = false;
}




}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene