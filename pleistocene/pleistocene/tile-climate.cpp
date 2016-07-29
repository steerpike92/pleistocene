#include "tile-climate.h"
#include "graphics.h"
#include "world.h"

namespace pleistocene {
namespace simulation {
namespace climate {
//======================================
//INITIALIALIZATION
//======================================

TileClimate::TileClimate() noexcept {}

TileClimate::TileClimate(my::Address A, double noiseValue) noexcept
{
	double landElevation = noiseValue * kElevationAmplitude;

	_address = A;

	my::Vector2d latLonDeg = _address.getLatLonDeg();
	_latitude_deg = latLonDeg.x;
	_longitude_deg = latLonDeg.y;
	_solarRadiation = SolarRadiation(_latitude_deg, _longitude_deg);

	double initialTemperature = calculateLocalInitialtemperature();

	_materialColumn = layers::MaterialColumn(landElevation, initialTemperature);
}

double TileClimate::calculateLocalInitialtemperature() noexcept
{
	double localInitialTemperature = kInitialTemperatureK;
	localInitialTemperature -= pow((_latitude_deg / 70), 2) * (kInitialTemperatureK - 273);
	return localInitialTemperature;
}

void TileClimate::buildAdjacency(std::map<my::Direction, TileClimate*> &adjacientTileClimates) noexcept
{
	_adjacientTileClimates = adjacientTileClimates;

	std::map<my::Direction, layers::MaterialColumn*> adjacientColumns;
	my::Direction direction;
	layers::MaterialColumn* column;

	for (auto &climate : _adjacientTileClimates) {
		direction = climate.first;
		column = &(climate.second->_materialColumn);
		adjacientColumns[direction] = column;
	}

	_materialColumn.buildAdjacency(adjacientColumns);
}


//======================================
//SIMULATION
//======================================

int TileClimate::_simulationStep;

void TileClimate::beginNewHour() noexcept
{
	_simulationStep = 0;
	SolarRadiation::setupSolarRadiation();
	//i.e. create earth rotation matrix for current time and set sun ray vector
}

bool TileClimate::beginNextStep() noexcept
{
	_simulationStep++;
	return (_simulationStep <= kTotalSteps);//check if simulation hour complete
}

void TileClimate::simulateClimate() noexcept
{
	double solarEnergyPerHour;

	switch (_simulationStep) {
	case(1) :
		_materialColumn.beginNewHour();
		solarEnergyPerHour = simulateSolarRadiation();
		if (solarEnergyPerHour > 0) { _materialColumn.filterSolarRadiation(solarEnergyPerHour); }
		_materialColumn.simulateEvaporation();
		_materialColumn.simulateInfraredRadiation();
		break;
	case(2) :
		_materialColumn.simulateConduction();
		break;
	case(3) :
		_materialColumn.simulatePressure();
		break;
	case(4) :
		_materialColumn.simulateAirFlow();
		_materialColumn.simulateCondensation();
		_materialColumn.simulatePrecipitation();
		break;

	case(5) :
		_materialColumn.simulateWaterFlow();
		_materialColumn.simulatePlants();
		break;
	default:
		LOG("Error: Simulation step out of bounds"); exit(EXIT_FAILURE);
	}
}

double TileClimate::simulateSolarRadiation() noexcept
{
	double solarFraction = _solarRadiation.applySolarRadiation();
	double incidentSolarEnergyPerHour = solarFraction*kSolarEnergyPerHour;
	return incidentSolarEnergyPerHour;
}

//======================================
//GRAPHICS
//======================================

bool TileClimate::elevationDraw(graphics::Graphics &graphics, std::vector<SDL_Rect> onscreenPositions, bool sunlit) noexcept
{

	double elevation = _materialColumn.getLandElevation();

	double elevationShader;
	elevationType elevationDrawType;

	setElevationDrawSpecs(elevation, elevationShader, elevationDrawType);

	double solarShader = 1;
	if (sunlit) solarShader = _solarRadiation.getRadiationShader(); //TODO control of shading....

	double textureShader = solarShader*elevationShader;
	textureShader = std::max(textureShader, 0.05);

	graphics.darkenTexture(_elevationTextures[elevationDrawType], textureShader);

	return graphics.blitTexture(_elevationTextures[elevationDrawType], nullptr, onscreenPositions);
}

void TileClimate::setElevationDrawSpecs(double elevation, double &computedElevationShader, elevationType &computedElevationType) noexcept
{
	if (elevation < kLandCutoff) {
		computedElevationType = SUBMERGED;
		computedElevationShader = abs(double(elevation + 6 * kElevationGaps)) / double(6 * kElevationGaps);
		return;
	}
	if (elevation < kMidCutoff) {
		computedElevationType = LOW_LAND;
		computedElevationShader = abs(0.6 + 2 * double(kElevationGaps - elevation) / double(5 * kElevationGaps));
		return;
	}
	if (elevation < kHighCutoff) {
		computedElevationType = MID_LAND;
		computedElevationShader = abs(0.6 + 2 * double(2 * kElevationGaps - elevation) / double(5 * kElevationGaps));
		return;
	}

	computedElevationType = HIGH_LAND;
	computedElevationShader = abs(0.4 + 2 * double(elevation - 2 * kElevationGaps) / double(10 * kElevationGaps));
	return;
}

void TileClimate::advectionDraw(graphics::Graphics &graphics, std::vector<SDL_Rect> onscreenPositions, const StatRequest &statRequest) noexcept
{
	Eigen::Vector2d advectionVector = _materialColumn.getAdvection(statRequest);

	double norm = advectionVector.norm()*1e11;

	bool null_barb = true;

	//select barb
	size_t barbSelected = 0;//calm
	if (norm < 5 && norm > 2) {//slight wind
		barbSelected = 1; 
		null_barb = false;
	} 
	else if (norm>=5) {//steady wind
		barbSelected = size_t(norm / 5)+1;
		barbSelected = std::min(barbSelected, 17u);
		null_barb = false;
	}

	//determine angle
	double angle = 0;
	if (!null_barb) {
		advectionVector.normalize();
		angle = atan2(advectionVector[1], advectionVector[0]) * (180/M_PI);
	}

	//shift and shrink onscreen positions
	
	for (SDL_Rect &rect : onscreenPositions) {

		if (null_barb) {
			rect.y += rect.h / 3;
		}
		else {
			rect.y += rect.h / 3;
		}

		rect.h = int(double(rect.h*(_windBarbRects[barbSelected].h))/70.0);
		rect.w =  int(double(rect.w*(_windBarbRects[barbSelected].w))/70.0);
	} 



	graphics.blitTexture(_windBarbTexture, &_windBarbRects[barbSelected], onscreenPositions, angle);
}


std::map<elevationType, std::string> TileClimate::_elevationTextures;
std::string TileClimate::_windBarbTexture;
std::vector<SDL_Rect> TileClimate::_windBarbRects;

void TileClimate::setupTextures(graphics::Graphics &graphics)  noexcept 
{
	_elevationTextures[SUBMERGED] = "../../content/simpleTerrain/midOcean.png";
	_elevationTextures[LOW_LAND] = "../../content/simpleTerrain/lowLand.png";
	_elevationTextures[MID_LAND] = "../../content/simpleTerrain/midLand.png";
	_elevationTextures[HIGH_LAND] = "../../content/simpleTerrain/highLand.png";

	for (std::pair< elevationType, std::string> P : _elevationTextures) {
		graphics.loadImage(P.second);
	}

	_windBarbTexture = "../../content/sprites/barbs.png";
	graphics.loadImage(_windBarbTexture);

	SDL_Rect rectangle;
	rectangle.x = 0;
	rectangle.y = 0;
	rectangle.w = 59;
	rectangle.h = 22;
	_windBarbRects.push_back(rectangle);


	rectangle.x = 0;
	rectangle.y = 37;
	rectangle.w = 59;
	rectangle.h = 17;
	int offset = 25;
	for (int i = 0; i < 17; i++) {
		rectangle.y = 37+i*(offset + rectangle.h);
		_windBarbRects.push_back(rectangle);
	}
}

//===========================================
//GETTERS
//===========================================

double TileClimate::getStatistic(const StatRequest &statRequest) const noexcept
{
	return _materialColumn.getStatistic(statRequest);
}

std::vector<std::string> TileClimate::getMessages(const StatRequest &statRequest) const noexcept 
{
	using namespace climate;

	std::vector<std::string> messages;

	std::vector<std::string> SubMessages = _materialColumn.getMessages(statRequest);

	for (std::string &str : SubMessages) {
		messages.push_back(str);
	}

	return messages;

}

}//namespace climate
}//namespace simulation
}//namespace pleistocene
