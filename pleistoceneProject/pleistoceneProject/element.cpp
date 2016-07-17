#include "element.h"
#include "material-layer.h"

namespace pleistocene {
namespace simulation {
namespace climate {
namespace elements {

//=====================================================================================================================
//ELEMENT
//=====================================================================================================================

Element::Element() noexcept {}

Element::Element(elements::ConstructorType constructorType, elements::ElementType elementType, double value, elements::State state) noexcept {
	using namespace elements;

	//if (value < 0) {LOG("inappropriately small element construction value"); LOG(value);exit(EXIT_FAILURE);}

	_elementType = elementType;

	if (state == NO_STATE) { _state = _stateMap.at(_elementType); }
	else { _state = state; }

	if (_elementType == WATER || _elementType == ICE || _elementType == SNOW || _elementType == WATER_VAPOR || _elementType == CLOUD) {
		_waterForm = true;
	}

	switch (constructorType) {
	case(VOLUME) :
		_volume = value;
		//if (_densityMap.count(_elementType) == 0) { LOG("Not a Volume substance"); exit(EXIT_FAILURE); } //NOEXCEPT
		if (_state == PARTICULATE) { _mass = _volume*_particleDensityMap.at(_elementType); }
		else { _mass = _volume*_densityMap.at(_elementType); }
		break;
	case(MOLAR) :
		_mols = value;
		//if (_molarMassMap.count(_elementType) == 0) { LOG("Not a Mol substance"); exit(EXIT_FAILURE); } //NOEXCEPT
		_mass = _mols*_molarMassMap.at(_elementType);
		break;
	case(MASS) :
		_mass = value;
		switch (_state) {
		case(SOLID) :
			_volume = _mass / _densityMap.at(_elementType);
			break;
		case(PARTICULATE) :
			_volume = _mass / _particleDensityMap.at(_elementType);
			break;
		case(LIQUID) :
			_volume = _mass / _densityMap.at(_elementType);
			break;
		case(DROPLET) :
			_volume = _mass / _densityMap.at(_elementType);
			break;
		case(GAS) :
			_mols = _mass / _molarMassMap.at(_elementType);
			break;
		}
		break;
		//default:
			//LOG("No mixture constructor type"); exit(EXIT_FAILURE); return; //NOEXCEPT
	}
}

void Element::combineLike(Element like) noexcept {

	//NOEXCEPT if (_elementType != like._elementType) { LOG("DIFFERENT ELEMENT"); exit(EXIT_FAILURE); return; }

	_mass += like._mass;

	if (_densityMap.count(_elementType)) {//if density defined
		_volume = _mass / _densityMap.at(_elementType);
	}

	if (_molarMassMap.count(_elementType)) {//if molar mass defined
		_mols = _mass / _molarMassMap.at(_elementType);
	}
}
void Element::addMass(double mass) noexcept {
	_mass += abs(mass);

	if (_densityMap.count(_elementType)) {//if density defined
		_volume = _mass / _densityMap.at(_elementType);
	}

	if (_molarMassMap.count(_elementType)) {//if molar mass defined
		_mols = _mass / _molarMassMap.at(_elementType);
	}
}
double Element::pullMass(double massRequested) noexcept {
	double massContained = _mass;
	_mass -= massRequested;
	if (_mass <= 0) {
		_mass = 0;
		_mols = 0;
		_volume = 0;

		return massContained;
	}
	else {
		if (_densityMap.count(_elementType)) {//if density defined
			_volume = _mass / _densityMap.at(_elementType);
		}

		if (_molarMassMap.count(_elementType)) {//if molar mass defined
			_mols = _mass / _molarMassMap.at(_elementType);
		}

		return massRequested;
	}
}
void Element::resizeBy(double proportion) noexcept {
	if (proportion <= 0) {
		_mass = 0;
		_volume = 0;
		_mols = 0;
		return;
	}

	_mass *= proportion;

	if (_densityMap.count(_elementType)) {//if density defined
		_volume = _mass / _densityMap.at(_elementType);
	}

	if (_molarMassMap.count(_elementType)) {//if molar mass defined
		_mols = _mass / _molarMassMap.at(_elementType);
	}
}

//=====================================================================================
//GETTER CALCULATORS
//=====================================================================================

elements::ElementType Element::getElementType() const noexcept { return _elementType; }
elements::State Element::getState() const noexcept { return _state; }
double Element::getHeatCapacity() const noexcept { return _mass*_specificHeatMap.at(_elementType); }

double Element::getAlbedo() const noexcept {
	using namespace elements;
	if (_state == SOLID || _state == LIQUID) { return _albedoMap.at(_elementType); }
	else { return std::min(_reflectivityMap.at(_elementType)*_mass, 1.0); }
}
double Element::getSolarAbsorptivity() const noexcept {
	using namespace elements;
	if (_state == SOLID) { return 1; }
	else { return std::min(_solarAbsorptivityMap.at(_elementType)*_mass, 1.0); }
}
double Element::getInfraredAbsorptivity() const noexcept {
	using namespace elements;
	if (_state == SOLID) { return 1; }
	else { return std::min(_infraredAbsorptivityMap.at(_elementType)*_mass, 1.0); }
}

double Element::getMass() const noexcept { return _mass; }
double Element::getVolume() const noexcept { return _volume; }
double Element::getMols() const noexcept { return _mols; }
double Element::getVoidSpace() const noexcept { return _volume*_porosityMap.at(_elementType); }
double Element::getPermeability() const noexcept { return _permeabilityMap.at(_elementType); }

bool Element::getStateConflict(elements::State state) const noexcept {
	//check if this type is in the "state" list of accepted types 
	if (_acceptedTypesMap.at(state).count(_elementType)) { return false; }//no conflict}
	else { return true; }//conflict}
}


//=====================================================================================================================
//PROPERTY MAPS
//=====================================================================================================================

elements::ElementPropertyMap Element::buildSpecificHeatMap() noexcept {
	using namespace elements;
	ElementPropertyMap specificHeat;
	specificHeat[DRY_AIR] = 1.00;
	specificHeat[WATER_VAPOR] = 3.985;
	specificHeat[CLOUD] = 4.186;

	specificHeat[WATER] = 4.186;
	specificHeat[SNOW] = 2.05;
	specificHeat[ICE] = 2.05;

	specificHeat[BEDROCK] = 0.84;//say...unfractured metamorphic rock (or basalt)
	specificHeat[ROCK] = 0.92;//say...sandstone
	specificHeat[SAND] = 0.80;//(dry)
	specificHeat[SILT] = 0.80;//(dry)
	specificHeat[CLAY] = 0.92;//(dry)

	return specificHeat;
}
const elements::ElementPropertyMap Element::_specificHeatMap = Element::buildSpecificHeatMap();

std::map<elements::ElementCoupling, double> Element::buildLatentHeatMap() noexcept {
	using namespace elements;
	std::map<ElementCoupling, double> latentHeat;

	latentHeat[ElementCoupling(ICE, SNOW)] = 0;
	latentHeat[ElementCoupling(ICE, WATER)] = 334;
	latentHeat[ElementCoupling(ICE, CLOUD)] = 334;
	latentHeat[ElementCoupling(ICE, WATER_VAPOR)] = 2599;

	latentHeat[ElementCoupling(SNOW, WATER)] = 334;
	latentHeat[ElementCoupling(SNOW, CLOUD)] = 334;
	latentHeat[ElementCoupling(SNOW, WATER_VAPOR)] = 2599;

	latentHeat[ElementCoupling(WATER, CLOUD)] = 0;
	latentHeat[ElementCoupling(WATER, WATER_VAPOR)] = 2265;

	latentHeat[ElementCoupling(CLOUD, WATER_VAPOR)] = 2265;


	std::map<ElementCoupling, double> fullLatentHeat;


	for (auto const &element : latentHeat) {
		fullLatentHeat[element.first] = element.second;//forward transformations
		fullLatentHeat[ElementCoupling(element.first.second, element.first.first)] =
			-element.second;//backward transformations
	}

	return fullLatentHeat;
}
const std::map<elements::ElementCoupling, double> Element::_latentHeatMap = Element::buildLatentHeatMap();

elements::ElementPropertyMap Element::buildDensityMap() noexcept {
	using namespace elements;
	ElementPropertyMap density;

	density[CLOUD] = 1000;
	density[WATER] = 1000;
	density[SNOW] = 400;
	density[ICE] = 961;

	density[BEDROCK] = 2700;
	density[ROCK] = 2400;
	density[SAND] = 1520;
	density[SILT] = 1280;
	density[CLAY] = 1200;

	return density;
}
const elements::ElementPropertyMap Element::_densityMap = Element::buildDensityMap();

elements::ElementPropertyMap Element::buildMolarMassMap() noexcept {
	using namespace elements;
	using namespace layers::air;
	ElementPropertyMap molarMass;

	molarMass[DRY_AIR] = Md;
	molarMass[WATER_VAPOR] = Mv;
	molarMass[CLOUD] = Mv;
	molarMass[WATER] = Mv;
	molarMass[SNOW] = Mv;
	molarMass[ICE] = Mv;

	return molarMass;
}
const elements::ElementPropertyMap Element::_molarMassMap = Element::buildMolarMassMap();

std::map<elements::ElementType, elements::State> Element::buildStateMap() noexcept {
	using namespace elements;
	std::map<ElementType, State> stateMap;

	//now somewhat redundant as I like accepted types map more

	stateMap[DRY_AIR] = GAS;
	stateMap[WATER_VAPOR] = GAS;

	stateMap[CLOUD] = DROPLET;

	stateMap[WATER] = LIQUID;

	//solid can also be used as particulate (except for rocks)
	stateMap[SNOW] = SOLID;
	stateMap[ICE] = SOLID;
	stateMap[BEDROCK] = SOLID;
	stateMap[ROCK] = SOLID;
	stateMap[SAND] = SOLID;
	stateMap[SILT] = SOLID;
	stateMap[CLAY] = SOLID;
	return stateMap;
}
const std::map<elements::ElementType, elements::State> Element::_stateMap = Element::buildStateMap();

elements::ElementPropertyMap Element::buildPermeabilityMap() noexcept {
	using namespace elements;
	ElementPropertyMap permeabilityMap;

	//meters per hour
	permeabilityMap[ICE] = 0;
	permeabilityMap[SNOW] = 0;
	permeabilityMap[BEDROCK] = pow(10, -7);
	permeabilityMap[ROCK] = pow(10, -4);
	permeabilityMap[SAND] = pow(10, -1);
	permeabilityMap[SILT] = pow(10, -3);
	permeabilityMap[CLAY] = pow(10, -5);

	return permeabilityMap;
}
const elements::ElementPropertyMap Element::_permeabilityMap = Element::buildPermeabilityMap();

elements::ElementPropertyMap Element::buildPorosityMap() noexcept {
	using namespace elements;
	ElementPropertyMap porosityMap;

	//Porosity is defined as the void space of a rock or unconsolidated material
	//n=(V_void)/(V_total)
	porosityMap[ICE] = 0;
	porosityMap[SNOW] = 0;
	porosityMap[BEDROCK] = 0.1;
	porosityMap[ROCK] = 0.3;
	porosityMap[SAND] = 0.4;
	porosityMap[SILT] = 0.48;
	porosityMap[CLAY] = 0.47;

	return porosityMap;
}
const elements::ElementPropertyMap Element::_porosityMap = Element::buildPorosityMap();

elements::ElementPropertyMap Element::buildParticleDensityMap() noexcept {
	using namespace elements;
	ElementPropertyMap particleDensityMap;

	particleDensityMap[ICE] = 961;
	particleDensityMap[SNOW] = 200;//?
	particleDensityMap[SAND] = 2659;
	particleDensityMap[SILT] = 2798;
	particleDensityMap[CLAY] = 2837;

	return particleDensityMap;
}
const elements::ElementPropertyMap Element::_particleDensityMap = Element::buildParticleDensityMap();

elements::ElementPropertyMap Element::buildParticleRadiusMap() noexcept {
	using namespace elements;
	ElementPropertyMap particleRadiusMap;

	particleRadiusMap[ICE] = 5 * pow(10, -3);//hail
	particleRadiusMap[SNOW] = 2 * pow(10, -3);//falling snow
	particleRadiusMap[SAND] = 1 * pow(10, -3);
	particleRadiusMap[SILT] = 2 * pow(10, -5);
	particleRadiusMap[CLAY] = 1 * pow(10, -6);

	return particleRadiusMap;
}
const elements::ElementPropertyMap Element::_particleRadiusMap = Element::buildParticleRadiusMap();

elements::ElementPropertyMap Element::buildDynamicViscosityMap() noexcept {
	using namespace elements;
	ElementPropertyMap dynamicViscosityMap;

	//This should really depend on temperature
	dynamicViscosityMap[DRY_AIR] = 1.8 * pow(10, -5);
	dynamicViscosityMap[WATER_VAPOR] = 1.8 * pow(10, -5);
	dynamicViscosityMap[WATER] = 1 * pow(10, -3);

	return dynamicViscosityMap;
}
const elements::ElementPropertyMap Element::_dynamicViscosityMap = Element::buildDynamicViscosityMap();

std::map<elements::State, std::set<elements::ElementType>> Element::buildAcceptedTypesMap() noexcept {
	using namespace elements;
	std::map<State, std::set<ElementType>> acceptedTypesMap;

	std::set<ElementType> gasTypes{ DRY_AIR, WATER_VAPOR };
	acceptedTypesMap[GAS] = gasTypes;

	std::set<ElementType> dropletTypes{ CLOUD };
	acceptedTypesMap[DROPLET] = dropletTypes;

	std::set<ElementType> liquidTypes{ WATER };
	acceptedTypesMap[LIQUID] = liquidTypes;

	std::set<ElementType> particulateTypes{ ICE,SNOW,SAND,SILT,CLAY };
	acceptedTypesMap[PARTICULATE] = particulateTypes;

	std::set<ElementType> solidTypes{ ICE,SNOW,SAND,SILT,CLAY,ROCK,BEDROCK };
	acceptedTypesMap[SOLID] = solidTypes;

	return acceptedTypesMap;
}
const std::map<elements::State, std::set<elements::ElementType>> Element::_acceptedTypesMap = Element::buildAcceptedTypesMap();

elements::ElementPropertyMap Element::buildAlbedoMap() noexcept {
	using namespace elements;
	ElementPropertyMap albedo;

	albedo[WATER] = 0.06;
	albedo[SNOW] = 0.85;
	albedo[ICE] = 0.6;

	albedo[BEDROCK] = 0.1;
	albedo[ROCK] = 0.3;
	albedo[SAND] = 0.4;
	albedo[SILT] = 0.2;
	albedo[CLAY] = 0.2;
	return albedo;
}
const elements::ElementPropertyMap Element::_albedoMap = Element::buildAlbedoMap();

elements::ElementPropertyMap Element::buildReflectivityMap() noexcept {
	using namespace elements;
	ElementPropertyMap reflectivityMap;

	//Cloudless:
	reflectivityMap[DRY_AIR] = 2.6*pow(10, -5);

	//reflectivityMap[DRY_AIR] = 2.2*pow(10, -5);
	reflectivityMap[WATER_VAPOR] = 2.2*pow(10, -5);

	reflectivityMap[CLOUD] = 1 * pow(10, -1);//stub

	reflectivityMap[SNOW] = 1 * pow(10, -1);//while particulate
	reflectivityMap[ICE] = 1 * pow(10, -1);//while particulate

	reflectivityMap[SAND] = 1 * pow(10, -5);//while particulate
	reflectivityMap[SILT] = 1 * pow(10, -5);//while particulate
	reflectivityMap[CLAY] = 1 * pow(10, -5);//while particulate

	return reflectivityMap;
}
const elements::ElementPropertyMap Element::_reflectivityMap = Element::buildReflectivityMap();

elements::ElementPropertyMap Element::buildSolarAbsorptivityMap() noexcept {
	using namespace elements;
	ElementPropertyMap solarAbsorptivityMap;

	solarAbsorptivityMap[DRY_AIR] = 2.2*pow(10, -5);
	solarAbsorptivityMap[WATER_VAPOR] = 2.2*pow(10, -5);
	solarAbsorptivityMap[CLOUD] = 2.2*pow(10, -5);

	solarAbsorptivityMap[WATER] = 2.2*pow(10, -4);//eh

	solarAbsorptivityMap[SNOW] = 2.2*pow(10, -5);//while particulate
	solarAbsorptivityMap[ICE] = 2.2*pow(10, -5);//while particulate

	solarAbsorptivityMap[SAND] = 1 * pow(10, -3);//while particulate
	solarAbsorptivityMap[SILT] = 1 * pow(10, -3);//while particulate
	solarAbsorptivityMap[CLAY] = 1 * pow(10, -3);//while particulate

	return solarAbsorptivityMap;
}
const elements::ElementPropertyMap Element::_solarAbsorptivityMap = Element::buildSolarAbsorptivityMap();

elements::ElementPropertyMap Element::buildInfraredAbsorptivityMap() noexcept {
	using namespace elements;
	ElementPropertyMap infraredAbsorptivityMap;

	//stub
	infraredAbsorptivityMap[DRY_AIR] = 1.6*pow(10, -4);
	infraredAbsorptivityMap[WATER_VAPOR] = 1 * pow(10, -3);

	infraredAbsorptivityMap[CLOUD] = 1 * pow(10, -3);

	infraredAbsorptivityMap[WATER] = 1 * pow(10, -2);

	infraredAbsorptivityMap[SNOW] = 2.2*pow(10, -5);//while particulate
	infraredAbsorptivityMap[ICE] = 2.2*pow(10, -5);//while particulate

	infraredAbsorptivityMap[SAND] = 1 * pow(10, -3);//while particulate
	infraredAbsorptivityMap[SILT] = 1 * pow(10, -3);//while particulate
	infraredAbsorptivityMap[CLAY] = 1 * pow(10, -3);//while particulate

	return infraredAbsorptivityMap;
}
const elements::ElementPropertyMap Element::_infraredAbsorptivityMap = Element::buildInfraredAbsorptivityMap();

}//namespace elements
}//namespace climate
}//namespace simulation
}//namespace pleistocene
