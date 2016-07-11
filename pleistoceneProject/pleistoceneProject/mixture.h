#pragma once
#include "globals.h"
#include "element.h"

class Mixture;

//======================================================================================
//MIXTURE
//======================================================================================

class Mixture {
protected:
	std::map<elements::ElementType, Element> _elements;//main constituent elements
	elements::State _state;


	double _temperature = 0;//Kelvin
	double _totalVolume = 0;//m3
	double _fixedVolume = 0;//m3. Use fixed volume for gas cells
	bool _volumeIsFixed = false;//for gases
	double _totalMass = 0;//kg
	double _totalHeatCapacity = 0;//kJ/(kg K)
	double _totalMols = 0;

	double _albedo = 0;//reflective index (proportion reflected passing through this material)
	double _solarAbsorptionIndex = 0;//proportion absorbed passing through this material
	double _infraredAbsorptionIndex = 0;//proportion absorbed passing through this material

	double _totalSolarAbsorbed = 0;
	double _totalInfraredAbsorbed = 0;
	double _totalInfraredEmitted = 0;

public:
	Mixture();
	~Mixture();
	Mixture(Element element, double temperature, elements::State state, double fixedVolume=my::FakeDouble);
	Mixture(std::vector<Element> compositionElements, double temperature, elements::State state, double fixedVolume = my::FakeDouble);

	//PARAMETER CALCULATIONS
	//=====================================================================================================================
	virtual void calculateParameters();
protected:
	virtual void calculateVolume();
	virtual void calculateMass();
	virtual void calculateMols();
	virtual void calculateHeatCapacity();

	virtual void calculateAlbedoIndex();
	virtual void calculateSolarAbsorptionIndex();
	virtual void calculateInfraredAbsorptionIndex();

public:
	//MIXING MIXTURES
	//=====================================================================================================================
	static void transferMixture(Mixture &receivingMixture, Mixture &givingMixture, double proportion);
	void resizeBy(double proportion);

protected:
	void push(Mixture &addedMixture);
	//Mixture pull(Mixture &subtractedMixture);

	Mixture copyProportion(double proportion) const;	
	void pushSpecific(Element addedSpecificElement, double temperature=0.0, bool tempSpecified=false );
	double pullSpecific(Element subtractedSpecificElement);

public:
	double filterSolarRadiation(double incidentSolarEnergyKJ);
	double emitInfrared();
	double filterInfrared(double infraredEnergyKJ);
	static void conduction(Mixture &mixture1, Mixture &mixture2);

	//=======================================
	//GETTERS
	//=======================================

	std::vector<std::string> getMessages()const;
	double getTemperature()const;
	double getHeight()const;
	double getAlbedo()const;
	double getVolume()const;
	double getMass()const;
};

