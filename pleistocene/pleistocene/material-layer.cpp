#include "material-layer.h"
#include "world.h"


namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {


//////////==================================
//////////MATERIAL LAYER
//////////==================================

MaterialLayer::MaterialLayer() noexcept {}

MaterialLayer::MaterialLayer(double baseElevation, double bottomElevation,bool emittor) noexcept :
_baseElevation(baseElevation),
_bottomElevation(bottomElevation),
_bottomRelativeElevation(_bottomElevation - _baseElevation),
_emittor(emittor)
{
}

void MaterialLayer::addSurface(layers::SharedSurface &surface) noexcept
{
	_sharedSurfaces.push_back(surface);
}

void MaterialLayer::clearSurfaces() noexcept
{
	_sharedSurfaces.clear();
}

//SIMULATION
//==============================

void MaterialLayer::hourlyClear() noexcept 
{
	_mixture->hourlyClear();
}


void MaterialLayer::filterSolarRadiation(double energyKJ) noexcept
{
	if (_up == nullptr) {//stratosphere
		energyKJ= _mixture->filterSolarRadiation(3*energyKJ)/3;
	}
	else {
		energyKJ = _mixture->filterSolarRadiation(energyKJ);
	}

	//don't send down less than a joule
	if (energyKJ > 0.001) {
		//if down is nullptr the sunlight has reached the abyss and you have bigger problems
		if (_down == nullptr) { LOG("Sun to bedrock?"); exit(EXIT_FAILURE); }

		_down->filterSolarRadiation(energyKJ);	//chain downward
	}
}

double MaterialLayer::emitInfraredRadiation() noexcept
{
	return _mixture->emitInfrared();
}

double MaterialLayer::filterInfraredRadiation(double energyKJ) noexcept 
{
	return _mixture->filterInfrared(energyKJ);
}

void MaterialLayer::simulateConduction() noexcept
{
	for (SharedSurface &surface : _sharedSurfaces) {
		surface.performConduction();
	}
}

void MaterialLayer::computeSurfacePressures() noexcept {
	//TODO?
	//Probably just need the air layer function
}

double MaterialLayer::getPressure(double elevation) const noexcept {
	return 0.0; //I exist as STUB
}




//GETTERS
//==============================

double MaterialLayer::getBottomElevation() const noexcept { return _bottomElevation; }
double MaterialLayer::getTopElevation() const noexcept { return _topElevation; }
double MaterialLayer::getTemperature() const noexcept { return _mixture->getTemperature(); }
LayerType MaterialLayer::getType() const noexcept { return _layerType; }
elements::Mixture *MaterialLayer::getMixture() const noexcept { return _mixture; }

std::vector<std::string> MaterialLayer::getMessages(const StatRequest &statRequest) const noexcept
{
	std::vector<std::string> messages;
	std::vector<std::string> subMessages;

	std::stringstream stream;

	switch (statRequest._statType) {
	case(ELEVATION) :
		stream.str(std::string());
		stream << "Elevation: " << my::double2string(_bottomElevation);
		messages.push_back(stream.str());

		stream.str(std::string());
		stream << "Height: " << my::double2string(_height);
		messages.push_back(stream.str());

		return messages;

	case(TEMPERATURE) :

		messages = _mixture->getThermalMessages();

		//redundant
		/*stream.str(std::string());
		stream << "Temperature: " << my::double2string(getTemperature() - 273) << " �C";
		messages.push_back(stream.str());

		stream.str(std::string());
		stream << "Heat Capacity: " << my::double2string(_mixture->getHeatCapacity()) << " kJ/K";
		messages.push_back(stream.str());*/

		return messages;

	case(MATERIAL_PROPERTIES) :

		messages = _mixture->getElementMessages();

		stream.str(std::string());
		stream << "Albedo: " << my::double2string(_mixture->getAlbedo() * 100) << "%";
		messages.push_back(stream.str());

		stream.str(std::string());
		stream << "Mass: " << my::double2string(_mixture->getMass()) << " kg";
		messages.push_back(stream.str());

		stream.str(std::string());
		stream << "Height: " << my::double2string(_height) << " m";
		messages.push_back(stream.str());

		return messages;

	case(PRESSURE) :
	{
		int neighbors = 0;
		for (SharedSurface surface : _sharedSurfaces) {
			neighbors++;
		}



		stream.str(std::string());
		stream << "Neighbors: " << neighbors;
		messages.push_back(stream.str());

		if (_up == nullptr) {
			stream.str(std::string());
			stream << "No UP";
			messages.push_back(stream.str());
		}

		return messages;
	}
	case(FLOW) : return messages;
	}
	return messages;
}

double MaterialLayer::getStatistic(const StatRequest &statRequest) const noexcept
{
	switch (statRequest._statType) {
	case(ELEVATION) : return _bottomElevation;
	case(TEMPERATURE) : return _mixture->getTemperature();
	case(MATERIAL_PROPERTIES) : return _mixture->getAlbedo();
	case(PRESSURE) : return 0; //stub
	case(FLOW) : return -10; //stub
	default: LOG("Not a stat type"); exit(EXIT_FAILURE); return 1;
	}

}

Eigen::Vector2d MaterialLayer::getAdvection() const noexcept 
{
	return Eigen::Vector2d{ 0, 0 };
}

//////////==================================
//////////EARTH
//////////==================================

EarthLayer::EarthLayer() noexcept {}

EarthLayer::EarthLayer(double baseElevation, double temperature, double bottomElevation, double layerHeight, bool emittor) noexcept :
MaterialLayer(baseElevation, bottomElevation, emittor),
_solidPtr(new elements::SolidMixture())
{//normal constructor
	using namespace elements;
	

	_layerType = EARTH;

	std::vector<Element> elementVector;

	elementVector = generateSoil(earth::bedrockDepth-_bottomRelativeElevation, layerHeight);//-_bottomRelativeElevation is depth below surface

	//unique_ptr setup.
	std::unique_ptr<SolidMixture> temp(new SolidMixture(elementVector, temperature));
	_solidPtr = std::move(temp);
	temp.~unique_ptr();
	_mixture = _solidPtr.get();//raw (non-owning) base class pointer setup
	_mixture->_emittor = emittor;

	_height = layerHeight;
	_topElevation = _bottomElevation + _height;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}



std::vector<elements::Element> EarthLayer::generateSoil(double depth, double height) noexcept
{
	using namespace elements;
	using namespace layers::earth;

	double depthIndex = depth / bedrockDepth;

	Element element;
	std::vector<Element> elementVector;

	ElementType earthLayerType = determineEarthType(depthIndex);
	ElementType soilLayerType;

	switch (earthLayerType) {
	case(BEDROCK) ://impermeable bedrock layer 
		element = Element(VOLUME, BEDROCK, height, SOLID);
		elementVector.push_back(element);
		return elementVector;
	case(ROCK) ://permeable rock layer
		element = Element(VOLUME, ROCK, height, SOLID);
		elementVector.push_back(element);
		return elementVector;
	default://"CLAY" is stand in for soil layer

		//up to three types of soil mixed together
		for (int i = 0; i < 4; i++) {
			soilLayerType = determineSoilType(depthIndex);
			element = Element(VOLUME, soilLayerType, height/4.0, SOLID);
			elementVector.push_back(element);
		}
		return elementVector;
	}
}

elements::ElementType EarthLayer::determineEarthType(double depthIndex) noexcept
{
	using namespace elements;

	//depthIndex is from 0 to 1 (0=surface, 1=bedrockBottom)

	//randomDouble is from 0 to 1;
	double randomDouble = my::uniformRandom();
	//double randomDouble = 0.3;

	//soilRV : soil Random Variable from 0 to 1. 
	//low values correspond to near surface type soils (at surface, soilRV < 0.5)
	//high values correspond to bedrock types (at bedrock bottom, soilRV > 0.5)
	double soilRV = (depthIndex + randomDouble) / 2;

	if (soilRV > 0.65) { return BEDROCK; }
	if (soilRV > 0.51) { return ROCK; }
	return CLAY; //stand in for "SOIL".  Move on to soil determination
}

elements::ElementType EarthLayer::determineSoilType(double depthIndex) noexcept
{
	using namespace elements;

	//Semi STUB

	//depthIndex is from 0 to 1 (0=surface, 1=bedrockBottom)
	//Not used here yet but eventually probably

	//uniformRandom is from 0 to 1;
	double soilRV = my::uniformRandom();
	//double soilRV = 0.36;
	if (soilRV > 0.67) { return CLAY; }
	else if (soilRV > 0.33) { return SILT; }
	else return SAND;
}

elements::SolidMixture *EarthLayer::getSolidPtr() noexcept { return _solidPtr.get(); }

//SIMULATION
//===========================

void EarthLayer::simulateFlow() noexcept {}

//GETTERS
//===========================

std::vector<std::string> EarthLayer::getMessages(const StatRequest &statRequest) const noexcept
{
	std::vector<std::string> messages;
	std::stringstream stream;

	switch (statRequest._statType) {
	case(ELEVATION) : return MaterialLayer::getMessages(statRequest);
	case(TEMPERATURE) : return MaterialLayer::getMessages(statRequest);
	case(MATERIAL_PROPERTIES) :
	{
		messages = MaterialLayer::getMessages(statRequest);

		stream.str(std::string());
		stream << "Porosity: " << my::double2string(_solidPtr->getPorosity());
		messages.push_back(stream.str());

		stream.str(std::string());
		stream << "Permeability: " << my::double2string(_solidPtr->getPermeability());
		messages.push_back(stream.str());

		return messages;

	}
	case(PRESSURE) : return MaterialLayer::getMessages(statRequest);
	case(FLOW) : return MaterialLayer::getMessages(statRequest);

	default: LOG("Not a stat option"); exit(EXIT_FAILURE); return messages;

	}
}
double EarthLayer::getStatistic(const StatRequest &statRequest) const noexcept
{
	switch (statRequest._statType) {
	case(ELEVATION) : return _topElevation;
	case(TEMPERATURE) : return _solidPtr->getTemperature();
	case(MATERIAL_PROPERTIES) : return _solidPtr->getPermeability();
	case(PRESSURE) : return 0;
	case(FLOW) : return 0;
	default: LOG("Not a draw option"); exit(EXIT_FAILURE); return 1;
	}
}


////////////==================================
////////////HORIZON
////////////==================================

HorizonLayer::HorizonLayer() noexcept {}

HorizonLayer::HorizonLayer(double baseElevation, double temperature, double bottomElevation, bool emittor) noexcept :
EarthLayer(baseElevation, temperature, bottomElevation, layers::earth::topSoilHeight, emittor)
{
	_layerType = HORIZON;//overwrite
}

std::vector<std::string> HorizonLayer::getMessages(const StatRequest &statRequest) const noexcept
{
	std::vector<std::string> messages;
	std::stringstream stream;

	switch (statRequest._statType) {
	case(ELEVATION) : return MaterialLayer::getMessages(statRequest);
	case(TEMPERATURE) : return MaterialLayer::getMessages(statRequest);
	case(MATERIAL_PROPERTIES) :
	{
		messages=MaterialLayer::getMessages(statRequest);

		stream.str(std::string());
		stream << "Porousness: " << my::double2string(_solidPtr->getPorosity());
		messages.push_back(stream.str());

		stream.str(std::string());
		stream << "Permeability: " << my::double2string(_solidPtr->getPermeability());
		messages.push_back(stream.str());

		return messages;
	}
	case(PRESSURE) : return MaterialLayer::getMessages(statRequest);
	case(FLOW) : return MaterialLayer::getMessages(statRequest);
	}
	return messages;
}

double HorizonLayer::getStatistic(const StatRequest &statRequest) const noexcept
{
	switch (statRequest._statType) {
	case(ELEVATION) : return _topElevation;
	case(TEMPERATURE) : return _solidPtr->getTemperature();
	case(MATERIAL_PROPERTIES) : return  _solidPtr->getAlbedo();
	case(PRESSURE) : return 0;
	case(FLOW) : return 0;
	default: return 0;
	}
}

//////////////==================================
//////////////SEA
//////////////==================================

SeaLayer::SeaLayer() noexcept {}

SeaLayer::SeaLayer(double baseElevation, double temperature, double bottomElevation, double topElevation, bool emittor) noexcept :
MaterialLayer(baseElevation, bottomElevation, emittor),
_liquidPtr(new elements::LiquidMixture())
{
	using namespace elements;
	

	_layerType = layers::SEA;
	_topElevation = topElevation;
	_height = _topElevation - _bottomElevation;


	Element water = Element(VOLUME, WATER, _height, LIQUID);

	std::vector<Element> elementVector{ water };

	//unique_ptr setup.
	std::unique_ptr<LiquidMixture> temp(new LiquidMixture(elementVector, temperature));
	_liquidPtr = std::move(temp);
	temp.~unique_ptr();
	_mixture = _liquidPtr.get();//raw (non-owning) base class pointer setup
	_mixture->_emittor = emittor;

	_height = _topElevation - _bottomElevation;
	_topElevation = _bottomElevation + _height;
	_topRelativeElevation = _bottomRelativeElevation + _height;

}


elements::LiquidMixture *SeaLayer::getLiquidPtr() noexcept { return _liquidPtr.get(); }



//SIMULATION
//===========================

void SeaLayer::simulateFlow() noexcept {} //STUB

std::vector<std::string> SeaLayer::getMessages(const StatRequest &statRequest) const noexcept
{
	std::vector<std::string> messages=MaterialLayer::getMessages(statRequest);
	//std::stringstream stream;

	/*switch (statRequest._statType) {
	case(ELEVATION) : return MaterialLayer::getMessages(statRequest);
	case(TEMPERATURE) : return MaterialLayer::getMessages(statRequest);
	case(MATERIAL_PROPERTIES) : return MaterialLayer::getMessages(statRequest);
	case(PRESSURE) : return MaterialLayer::getMessages(statRequest);
	case(FLOW) : return MaterialLayer::getMessages(statRequest);
	default: LOG("Not a stat option"); exit(EXIT_FAILURE); return messages;
	}*/
	return messages;
}

double SeaLayer::getStatistic(const StatRequest &statRequest) const noexcept
{
	switch (statRequest._statType) {
	case(ELEVATION) : return _bottomElevation;
	case(TEMPERATURE) : return _mixture->getTemperature();
	case(MATERIAL_PROPERTIES) : return  _mixture->getAlbedo();
	case(PRESSURE) : return 1;
	case(FLOW) : return 1;
	default: LOG("Not a draw option"); exit(EXIT_FAILURE); return 1;
	}
}

//////////==================================
//////////AIR
//////////==================================


AirLayer::AirLayer() noexcept {}

AirLayer::AirLayer(double baseElevation, double temperature, double bottomElevation, double fixedTopElevation) noexcept :
MaterialLayer(baseElevation, bottomElevation, true),
_gasPtr(new elements::GaseousMixture)
{
	using namespace elements;
	

	_layerType = AIR;
	_topElevation = fixedTopElevation;

	std::vector<Element> air = generateAirElements(_bottomElevation, _topElevation);
	//unique_ptr setup.
	std::unique_ptr<GaseousMixture> temp(new GaseousMixture(air, temperature, _bottomElevation, _topElevation));
	_gasPtr = std::move(temp);
	temp.~unique_ptr();
	_mixture = _gasPtr.get();//raw (non-owning) base class pointer setup
	_mixture->_emittor = true;

	_height = _topElevation - _bottomElevation;
	_topRelativeElevation = _bottomRelativeElevation + _height;
}

std::vector<elements::Element> AirLayer::generateAirElements(double bottomElevation, double topElevation) noexcept
{
	using namespace elements;
	using namespace layers::air;
	double mols = expectedMolsCalculator(bottomElevation, topElevation);
	Element air = Element(MOLAR, DRY_AIR, mols, GAS);

	std::vector<Element> elementVector{ air };

	return elementVector;
}

void AirLayer::addSurface(SharedSurface &surface) noexcept 
{
	_sharedSurfaces.push_back(surface);
}

void AirLayer::addAirSurface(SharedAirSurface &airSurface) noexcept
{
	_sharedAirSurfaces.push_back(airSurface);
}

elements::GaseousMixture *AirLayer::getGasPtr() noexcept { return _gasPtr.get(); }

//SIMULATION
//=========================

void AirLayer::computeSurfacePressures() noexcept 
{
	for (SharedAirSurface &airSurface : _sharedAirSurfaces) {
		airSurface.buildPressureDifferential();
	}
}

void AirLayer::simulateFlow() noexcept 
{
	_gasPtr->_netFlow = 0;
	for (SharedAirSurface &airSurface : _sharedAirSurfaces) {
		airSurface.flow();
	}
}

double AirLayer::getPressure(double elevation) const noexcept {
	return truePressureCalculator(elevation);
}

//UTILITY
//=========================

double AirLayer::expectedHydrostaticPressureCalculator(double elevation) noexcept
{
	using namespace layers::air;

	//choose set of standard values based on elevation
	int i;
	if (elevation > StandardElevation[1]) { i = 1; }
	else { i = 0; }

	//set of parameters
	double Po = StandardPressure[i];
	double To = StandardTemperature[i];
	double L = StandardLapseRate[i];
	double h = elevation;
	double ho = StandardElevation[i];
	double M = Md;

	double Pressure;

	if (L != 0) {
		//calculation with lapse
		double exponent = (g*M) / (R*L);
		double base = To / (To + L*(h - ho));
		Pressure = Po * pow(base, exponent);
	}
	else {
		//calculation when lapse == 0
		double exponent = (-g*M*(h - ho)) / (R*To);
		Pressure = Po * exp(exponent);
	}



	return Pressure;
}

double AirLayer::expectedMolsCalculator(double bottomElevation, double topElevation) noexcept
{
	using namespace layers::air;
	double BottomPressure = expectedHydrostaticPressureCalculator(bottomElevation);
	double TopPressure = expectedHydrostaticPressureCalculator(topElevation);

	double PressureDifference = BottomPressure - TopPressure;
	double ExpectedMols = PressureDifference / (g*Md);

	return ExpectedMols;
}

double AirLayer::expectedTemperatureCalculator(double elevation) noexcept
{
	using namespace layers::air;

	//choose set of standard values based on elevation
	int i;
	if (elevation > StandardElevation[1]) { i = 1; }
	else { i = 0; }

	//parameters
	double To = StandardTemperature[i];
	double L = StandardLapseRate[i];
	double h = elevation;
	double ho = StandardElevation[i];

	//calculation
	double expectedTemperature = To + L*(h - ho);

	return expectedTemperature;
}

double AirLayer::lapsedTemperatureCalculator(double elevation) const noexcept
{
	using namespace layers::air;

	//choose set of standard values based on elevation
	int i;
	if (elevation > StandardElevation[0]) { i = 1; }
	else { i = 0; }

	//parameters
	double To = _gasPtr->getTemperature();
	double L = StandardLapseRate[i];//Alternatively we can calculate a moist adiabatic lapse rate
	double h = elevation;
	double ho = _bottomElevation;

	double LapsedTemperature = To + L*(h - ho);

	return LapsedTemperature;
}

double AirLayer::truePressureCalculator(double elevation) const noexcept
{
	double ExpectedMols = expectedMolsCalculator(_bottomElevation, _topElevation);
	double ExpectedTemperature = expectedTemperatureCalculator(elevation);
	double ExpectedPressure = expectedHydrostaticPressureCalculator(elevation);

	double TrueMols = _gasPtr->getMols();
	double TrueTemperature = lapsedTemperatureCalculator(elevation);

	if (ExpectedMols <= 0 || ExpectedTemperature <= 0) { LOG("pressure calculator divide by zero"); exit(EXIT_FAILURE);}

	double TruePressure = (TrueMols / ExpectedMols) * (TrueTemperature / ExpectedTemperature) * ExpectedPressure;

	return TruePressure;
}




//GETTERS
//=========================

double AirLayer::getTemperature() const noexcept { return _gasPtr->getTemperature(); }

std::vector<std::string> AirLayer::getMessages(const StatRequest &statRequest) const noexcept
{
	std::vector<std::string> messages;
	std::stringstream stream;

	switch (statRequest._statType) {
	case(ELEVATION) : return MaterialLayer::getMessages(statRequest);
	case(TEMPERATURE) : return MaterialLayer::getMessages(statRequest);
	case(MATERIAL_PROPERTIES) : return MaterialLayer::getMessages(statRequest);
	case(PRESSURE) : {
		stream << "Pressure: " << my::double2string(getPressure(_bottomElevation));
		messages.push_back(stream.str());
		

		int neighbors = 0;
		for (SharedAirSurface surface : _sharedAirSurfaces) {
			neighbors++;
		}
		stream.str(std::string());
		stream << "Owned Air Neighbors: " << neighbors;
		messages.push_back(stream.str());
		return messages;
	}
	case(FLOW) : {
		double UpFlux;
		if (_up != nullptr) { UpFlux=_sharedAirSurfaces.front().getNetFlux(); }
		else UpFlux=0;
		stream.str(std::string());
		stream << "Up flux: " <<UpFlux;
		messages.push_back(stream.str());
		return messages;
	}
	default: LOG("Not a draw option"); exit(EXIT_FAILURE); return messages;
	}
}

double AirLayer::getStatistic(const StatRequest &statRequest) const noexcept
{
	switch (statRequest._statType) {
	case(ELEVATION) : return _bottomElevation;
	case(TEMPERATURE) : return this->getTemperature();
	case(MATERIAL_PROPERTIES) : return _gasPtr->getMols()-expectedMolsCalculator(_bottomElevation,_topElevation);
	case(PRESSURE) : return (getPressure(_bottomElevation)-expectedHydrostaticPressureCalculator(_bottomElevation));
	case(FLOW) : {
		if(_up!=nullptr){return _sharedAirSurfaces.front().getNetFlux();}
		else return 0;
	}
	default: LOG("Not a stat option"); exit(EXIT_FAILURE); return 1;
	}
}

Eigen::Vector2d AirLayer::getAdvection() const noexcept
{	
	Eigen::Vector3d inertia=_gasPtr->getInertia();
	Eigen::Vector2d advection{ inertia[0],inertia[1] };
	
	return advection;
}

}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene