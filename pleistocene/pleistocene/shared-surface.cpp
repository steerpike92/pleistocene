#include "shared-surface.h"
#include "material-layer.h"
#include "state-mixture.h"

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {

//////////////===============================
//////////////MIXED SURFACE
//////////////===============================


SharedSurface::SharedSurface() noexcept{}

//top surface constructor
SharedSurface::SharedSurface(MaterialLayer *ownerLayer, MaterialLayer *tenantLayer, double elevation, LayerType layerType)  noexcept :
_ownerLayer(ownerLayer),
_tenantLayer(tenantLayer),
_spatialDirection(UP),
_midpointElevation(elevation),
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

void SharedSurface::flow() noexcept 
{
	
}

MaterialLayer* SharedSurface::getTenant() noexcept { return _tenantLayer; }


//////////////===============================
//////////////AIR SURFACE
//////////////===============================


AirLayer *ownerLayer;
AirLayer *tenantLayer;


SharedAirSurface::SharedAirSurface() noexcept {}

SharedAirSurface::SharedAirSurface(AirLayer *ownerLayer, AirLayer *tenantLayer, SharedSurface &surface) noexcept:
SharedSurface(surface),
_ownerAirLayer(ownerLayer),
_tenantAirLayer(tenantLayer)
{

}

void SharedAirSurface::buildPressureDifferential() noexcept
{

	_ownerPressure = _ownerAirLayer->getPressure(_midpointElevation);
	_tenantPressure = _tenantAirLayer->getPressure(_midpointElevation);

	_pressureDifferential = _ownerPressure - _tenantPressure;
	_pressureBuilt = true;

}

//double SharedAirSurface::calculateEquilibriumExchange() const noexcept
//{
//	double ownerMols = _ownerAirLayer->getGasPtr()->getMols();
//	double tenantMols = _tenantAirLayer->getGasPtr()->getMols();
//
//	double ownerVolume  = _ownerAirLayer->getGasPtr()->getVolume();
//	double tenantVolume = _tenantAirLayer->getGasPtr()->getVolume();
//
//	double totalMols = ownerMols + tenantMols;
//	double totalVolume = ownerVolume + tenantVolume;
//
//	double totalDensity = totalMols / totalVolume;
//
//	double finalOwnerMols = totalDensity*ownerVolume;
//
//	double molExchange =  ownerMols-finalOwnerMols;
//
//	return molExchange;
//}

void SharedAirSurface::flow() noexcept 
{
	using namespace elements;
	if (!_pressureBuilt) { LOG("NO PRESSURE BUILT"); exit(EXIT_FAILURE); return; }

	double pressureConstant = 3e-14;
	double flowConstant = 0.2;

	Eigen::Vector3d ownerInertia = _ownerAirLayer->getGasPtr()->getInertia();
	Eigen::Vector3d tenantInertia = _tenantAirLayer->getGasPtr()->getInertia();

	ownerInertia += _normalVector*_pressureDifferential*pressureConstant;
	tenantInertia += _normalVector*_pressureDifferential*pressureConstant;

	Eigen::Vector3d average_inertia = (ownerInertia + tenantInertia)*0.5;

	_ownerAirLayer->getGasPtr()->setInertia(ownerInertia);
	_tenantAirLayer->getGasPtr()->setInertia(tenantInertia);

	Eigen::Vector3d flowVector = flowConstant*average_inertia + _normalVector*_pressureDifferential*pressureConstant;

	double flux = _area * (flowVector.dot(this->_normalVector));

	double flowRate = flux;
	
	bool backflow = signbit(flowRate);
	flowRate = abs(flowRate);

	
	/*double equilibriumExchange = calculateEquilibriumExchange();

	if (!backflow && equilibriumExchange>0) {
		flowRate = std::min(flowRate, equilibriumExchange / (_ownerAirLayer->getGasPtr()->getMols()) );
	}

	else if (backflow && equilibriumExchange < 0) {
		flowRate = std::min(flowRate, -equilibriumExchange / (_tenantAirLayer->getGasPtr()->getMols()) );
	}*/

	flowRate = std::min(flowRate, 0.05);

	if (backflow) {
		GaseousMixture::airFlow(*_ownerAirLayer->getGasPtr(), *_tenantAirLayer->getGasPtr(), flowRate, flowVector);
	}
	else {
		GaseousMixture::airFlow(*_tenantAirLayer->getGasPtr(), *_ownerAirLayer->getGasPtr(), flowRate, flowVector);
	}

	_pressureBuilt = false;
}


//////////////===============================
//////////////EARTH SURFACE
//////////////===============================

void SharedEarthSurface::flow() noexcept
{

}

//////////////===============================
//////////////HORIZON SURFACE
//////////////===============================

void SharedHorizonSurface::flow() noexcept
{

}

//////////////===============================
//////////////SEA SURFACE
//////////////===============================

void SharedSeaSurface::flow() noexcept
{

}


}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene