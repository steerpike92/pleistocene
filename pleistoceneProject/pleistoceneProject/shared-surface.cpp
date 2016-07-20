#include "shared-surface.h"
#include "material-layer.h"

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {


SharedSurface::SharedSurface() noexcept{}


SharedSurface::SharedSurface(MaterialLayer *materialLayer, double elevaton)  noexcept :
_spatialDirection(UP),
_materialLayer(materialLayer),
_midpointElevation(elevaton),
_area(100 * 1000 * 1000)
{
}

//side shared surface constructor
SharedSurface::SharedSurface(SpatialDirection spatialDirection, MaterialLayer *materialLayer, double bottomElevation, double topElevation) noexcept :
_spatialDirection(spatialDirection),
_materialLayer(materialLayer),
_midpointElevation((topElevation + bottomElevation) / 2),
_area((topElevation - bottomElevation) * (6.2 * 1000))//height*width
{
}

}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene