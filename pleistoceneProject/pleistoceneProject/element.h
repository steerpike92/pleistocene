#pragma once
#include "globals.h"
#include <set>

namespace pleistocene {
namespace simulation {
namespace climate {
namespace elements {

class Element;

enum ElementType {
	DRY_AIR,//GAS
	WATER_VAPOR,//GAS

	CLOUD,//DROPLET

	WATER,//LIQUID

	ICE,//LIQUID/ SOLID / PARTICULATE (glacial flow, ice pack, hail)

	SNOW,//SOLID / PARTICULATE (snow pack, suspended ice crystals)

	SAND,//SOLID / PARTICULATE
	SILT,//SOLID / PARTICULATE
	CLAY,//SOLID / PARTICULATE

	ROCK,//SOLID
	BEDROCK//SOLID

};

typedef std::map<const ElementType, double> ElementPropertyMap;
typedef std::pair<const ElementType, const ElementType> ElementCoupling;
typedef std::pair<const ElementType, Element> ElementPair;
typedef std::map<const ElementType, Element> ElementMap;


enum ConstructorType {
	VOLUME,
	MOLAR,
	MASS
};


enum State {
	NO_STATE,//for initialization with inferred state
	SOLID,
	PARTICULATE,//suspended solid (silt in river, dust in air). fixed particle size
	LIQUID,
	DROPLET,//suspended liquid (cloud). variable particle size
	GAS
};



//=====================================================================================================================
//ELEMENT
//=====================================================================================================================

class Element {

	ElementType _elementType;
	double _volume = 0;
	double _mols = 0;
	double _mass = 0;

	bool _waterForm = false;
	State _state;

public:
	Element() noexcept;
	Element(elements::ConstructorType constructorType, elements::ElementType elementType,
		double value, elements::State state = elements::NO_STATE) noexcept;

	void combineLike(Element like) noexcept;
	void addMass(double mass) noexcept;
	double pullMass(double massRequested) noexcept;
	void resizeBy(double proportion) noexcept;

	//getters
	//================================
	elements::ElementType getElementType() const noexcept;
	elements::State getState() const noexcept;

	double getAlbedo() const noexcept;
	double getSolarAbsorptivity() const noexcept;
	double getInfraredAbsorptivity() const noexcept;
	double getHeatCapacity() const noexcept;
	double getVolume()const noexcept;
	double getMass()const noexcept;
	double getMols()const noexcept;
	double getVoidSpace()const noexcept;
	double getPermeability()const noexcept;
	bool getStateConflict(elements::State state)const noexcept;

private:
	//======================================================
	//PROPERTY MAPS
	//======================================================

	//KJ/(kg*K)
	static const elements::ElementPropertyMap _specificHeatMap;
	static elements::ElementPropertyMap buildSpecificHeatMap() noexcept;

	//KJ/kg
	//heat released(+) or consumed(-) in transition from "first" to "second"
	static const std::map<elements::ElementCoupling, double> _latentHeatMap;
	static std::map<elements::ElementCoupling, double> buildLatentHeatMap() noexcept;



	//kg/m3
	static const elements::ElementPropertyMap _densityMap;//maybe add specific volume map (m3/kg)
	static elements::ElementPropertyMap buildDensityMap() noexcept;

	//kg/mol
	static const elements::ElementPropertyMap _molarMassMap;//maybe add specific... mols? (mols/kg)
	static elements::ElementPropertyMap buildMolarMassMap() noexcept;

	//solid/liquid/gas (natural state for element)
	static const std::map<elements::ElementType, elements::State> _stateMap;
	static std::map<elements::ElementType, elements::State> buildStateMap() noexcept;

	//permeability. Meters per hour per unit pressure gradient
	static const elements::ElementPropertyMap _permeabilityMap;
	static elements::ElementPropertyMap buildPermeabilityMap() noexcept;

	//porousness. Void Space per total volume
	static const elements::ElementPropertyMap _porosityMap;
	static elements::ElementPropertyMap buildPorosityMap() noexcept;

	//particle density, assuming spherical volume (so snow gets super low density) kg/m3
	static const elements::ElementPropertyMap _particleDensityMap;
	static elements::ElementPropertyMap buildParticleDensityMap() noexcept;

	//particle radius (m)
	static const elements::ElementPropertyMap _particleRadiusMap;
	static elements::ElementPropertyMap buildParticleRadiusMap() noexcept;

	//Viscosity
	static const elements::ElementPropertyMap _dynamicViscosityMap;
	static elements::ElementPropertyMap buildDynamicViscosityMap() noexcept;

	//accepted type mixtures (can't add rocks to a gas)
	static const std::map<elements::State, std::set<elements::ElementType>> _acceptedTypesMap;
	static std::map<elements::State, std::set<elements::ElementType>> buildAcceptedTypesMap() noexcept;


	//reflective index for solids/water surface 
	static const elements::ElementPropertyMap _albedoMap;
	static elements::ElementPropertyMap buildAlbedoMap() noexcept;

	//reflective index for diffuse elements (reflected/kg)
	static const elements::ElementPropertyMap _reflectivityMap;
	static elements::ElementPropertyMap buildReflectivityMap() noexcept;

	//solar absorption rate (proportion absorbed/kg)
	static const elements::ElementPropertyMap _solarAbsorptivityMap;
	static elements::ElementPropertyMap buildSolarAbsorptivityMap() noexcept;

	//Infrared absorption rate (proportion absorbed/kg)
	static const elements::ElementPropertyMap _infraredAbsorptivityMap;
	static elements::ElementPropertyMap buildInfraredAbsorptivityMap() noexcept;


};

}//namespace elements
}//namespace climate
}//namespace simulation
}//namespace pleistocene
