#pragma once
#include "globals.h"
#include <set>

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {
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
	Element(ConstructorType constructorType, ElementType elementType,
		double value, State state = NO_STATE) noexcept;

	void combineLike(Element like) noexcept;
	void addMass(double mass) noexcept;
	double pullMass(double massRequested) noexcept;
	void resizeBy(double proportion) noexcept;

	//getters
	//================================
	ElementType getElementType() const noexcept;
	State getState() const noexcept;

	std::vector<std::string> getMessages() const noexcept;

	double getAlbedo() const noexcept;
	double getSolarAbsorptivity() const noexcept;
	double getInfraredAbsorptivity() const noexcept;
	double getHeatCapacity() const noexcept;
	double getVolume()const noexcept;
	double getMass()const noexcept;
	double getMols()const noexcept;
	double getVoidSpace()const noexcept;
	double getPermeability()const noexcept;
	bool getStateConflict(State state)const noexcept;

private:
	//======================================================
	//PROPERTY MAPS
	//======================================================

	//KJ/(kg*K)
	static const ElementPropertyMap _specificHeatMap;
	static ElementPropertyMap buildSpecificHeatMap() noexcept;

	//KJ/kg
	//heat released(+) or consumed(-) in transition from "first" to "second"
	static const std::map<ElementCoupling, double> _latentHeatMap;
	static std::map<ElementCoupling, double> buildLatentHeatMap() noexcept;



	//kg/m3
	static const ElementPropertyMap _densityMap;//maybe add specific volume map (m3/kg)
	static ElementPropertyMap buildDensityMap() noexcept;

	//kg/mol
	static const ElementPropertyMap _molarMassMap;//maybe add specific... mols? (mols/kg)
	static ElementPropertyMap buildMolarMassMap() noexcept;

	//solid/liquid/gas (natural state for element)
	static const std::map<ElementType, State> _stateMap;
	static std::map<ElementType, State> buildStateMap() noexcept;

	//permeability. Meters per hour per unit pressure gradient
	static const ElementPropertyMap _permeabilityMap;
	static ElementPropertyMap buildPermeabilityMap() noexcept;

	//porousness. Void Space per total volume
	static const ElementPropertyMap _porosityMap;
	static ElementPropertyMap buildPorosityMap() noexcept;

	//particle density, assuming spherical volume (so snow gets super low density) kg/m3
	static const ElementPropertyMap _particleDensityMap;
	static ElementPropertyMap buildParticleDensityMap() noexcept;

	//particle radius (m)
	static const ElementPropertyMap _particleRadiusMap;
	static ElementPropertyMap buildParticleRadiusMap() noexcept;

	//Viscosity
	static const ElementPropertyMap _dynamicViscosityMap;
	static ElementPropertyMap buildDynamicViscosityMap() noexcept;

	//accepted type mixtures (can't add rocks to a gas)
	static const std::map<State, std::set<ElementType>> _acceptedTypesMap;
	static std::map<State, std::set<ElementType>> buildAcceptedTypesMap() noexcept;


	//reflective index for solids/water surface 
	static const ElementPropertyMap _albedoMap;
	static ElementPropertyMap buildAlbedoMap() noexcept;

	//reflective index for diffuse elements (reflected/kg)
	static const ElementPropertyMap _reflectivityMap;
	static ElementPropertyMap buildReflectivityMap() noexcept;

	//solar absorption rate (proportion absorbed/kg)
	static const ElementPropertyMap _solarAbsorptivityMap;
	static ElementPropertyMap buildSolarAbsorptivityMap() noexcept;

	//Infrared absorption rate (proportion absorbed/kg)
	static const ElementPropertyMap _infraredAbsorptivityMap;
	static ElementPropertyMap buildInfraredAbsorptivityMap() noexcept;

	static const std::map<ElementType, std::string> _elementNameMap;
	static std::map<ElementType, std::string> buildElementNameMap() noexcept;


};

}//namespace elements
}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene
