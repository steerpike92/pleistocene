#pragma once
#include "globals.h"
#include <set>

class Element;

namespace elements {
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
}



//=====================================================================================================================
//ELEMENT
//=====================================================================================================================

class Element {

	elements::ElementType _elementType;
	double _volume = 0;
	double _mols = 0;
	double _mass = 0;

	bool _waterForm = false;
	elements::State _state;

public:
	Element();
	Element(elements::ConstructorType constructorType, elements::ElementType elementType, double value, elements::State state = elements::NO_STATE);

	void combineLike(Element like);
	void addMass(double mass);
	double pullMass(double massRequested);
	void resizeBy(double proportion);

	//getters
	//================================
	elements::ElementType getElementType() const;
	elements::State getState() const;

	double getAlbedo() const;
	double getSolarAbsorptivity() const;
	double getInfraredAbsorptivity() const;
	double getHeatCapacity() const;
	double getVolume()const;
	double getMass()const;
	double getMols()const;
	double getVoidSpace()const;
	double getPermeability()const;
	bool getStateConflict(elements::State state)const;

private:
	//======================================================
	//PROPERTY MAPS
	//======================================================

	//KJ/(kg*K)
	static const elements::ElementPropertyMap _specificHeatMap;
	static elements::ElementPropertyMap buildSpecificHeatMap();

	//KJ/kg
	//heat released(+) or consumed(-) in transition from "first" to "second"
	static const std::map<elements::ElementCoupling, double> _latentHeatMap;
	static std::map<elements::ElementCoupling, double> buildLatentHeatMap();



	//kg/m3
	static const elements::ElementPropertyMap _densityMap;//maybe add specific volume map (m3/kg)
	static elements::ElementPropertyMap buildDensityMap();

	//kg/mol
	static const elements::ElementPropertyMap _molarMassMap;//maybe add specific... mols? (mols/kg)
	static elements::ElementPropertyMap buildMolarMassMap();

	//solid/liquid/gas (natural state for element)
	static const std::map<elements::ElementType, elements::State> _stateMap;
	static std::map<elements::ElementType, elements::State> buildStateMap();

	//permeability. Meters per hour per unit pressure gradient
	static const elements::ElementPropertyMap _permeabilityMap;
	static elements::ElementPropertyMap buildPermeabilityMap();

	//porousness. Void Space per total volume
	static const elements::ElementPropertyMap _porosityMap;
	static elements::ElementPropertyMap buildPorosityMap();

	//particle density, assuming spherical volume (so snow gets super low density) kg/m3
	static const elements::ElementPropertyMap _particleDensityMap;
	static elements::ElementPropertyMap buildParticleDensityMap();

	//particle radius (m)
	static const elements::ElementPropertyMap _particleRadiusMap;
	static elements::ElementPropertyMap buildParticleRadiusMap();

	//Viscosity
	static const elements::ElementPropertyMap _dynamicViscosityMap;
	static elements::ElementPropertyMap buildDynamicViscosityMap();

	//accepted type mixtures (can't add rocks to a gas)
	static const std::map<elements::State, std::set<elements::ElementType>> _acceptedTypesMap;
	static std::map<elements::State, std::set<elements::ElementType>> buildAcceptedTypesMap();


	//reflective index for solids/water surface 
	static const elements::ElementPropertyMap _albedoMap;
	static elements::ElementPropertyMap buildAlbedoMap();

	//reflective index for diffuse elements (reflected/kg)
	static const elements::ElementPropertyMap _reflectivityMap;
	static elements::ElementPropertyMap buildReflectivityMap();

	//solar absorption rate (proportion absorbed/kg)
	static const elements::ElementPropertyMap _solarAbsorptivityMap;
	static elements::ElementPropertyMap buildSolarAbsorptivityMap();

	//Infrared absorption rate (proportion absorbed/kg)
	static const elements::ElementPropertyMap _infraredAbsorptivityMap;
	static elements::ElementPropertyMap buildInfraredAbsorptivityMap();


};
