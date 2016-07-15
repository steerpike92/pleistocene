#pragma once
#include "globals.h"
#include "element.h"

namespace pleistocene {

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
	Mixture() noexcept;
	Mixture(Element element, double temperature, elements::State state, double fixedVolume = my::FakeDouble) noexcept;
	Mixture(std::vector<Element> compositionElements, double temperature, elements::State state, double fixedVolume = my::FakeDouble) noexcept;

	//PARAMETER CALCULATIONS
	//=====================================================================================================================
	virtual void calculateParameters() noexcept;
protected:
	virtual void calculateVolume() noexcept;
	virtual void calculateMass() noexcept;
	virtual void calculateMols() noexcept;
	virtual void calculateHeatCapacity() noexcept;

	virtual void calculateAlbedoIndex() noexcept;
	virtual void calculateSolarAbsorptionIndex() noexcept;
	virtual void calculateInfraredAbsorptionIndex() noexcept;

public:
	//MIXING MIXTURES
	//=====================================================================================================================
	static void transferMixture(Mixture &receivingMixture, Mixture &givingMixture, double proportion) noexcept;
	void resizeBy(double proportion) noexcept;

protected:
	void push(Mixture &addedMixture) noexcept;
	//Mixture pull(Mixture &subtractedMixture) noexcept;

	Mixture copyProportion(double proportion) const noexcept;
	void pushSpecific(Element addedSpecificElement, double temperature = 0.0, bool tempSpecified = false) noexcept;
	double pullSpecific(Element subtractedSpecificElement) noexcept;

public:
	double filterSolarRadiation(double incidentSolarEnergyKJ) noexcept;
	double emitInfrared() noexcept;
	double filterInfrared(double infraredEnergyKJ) noexcept;
	static void conduction(Mixture &mixture1, Mixture &mixture2) noexcept;

	//=======================================
	//GETTERS
	//=======================================

	std::vector<std::string> getMessages() const noexcept;
	double getTemperature() const noexcept;
	double getHeight() const noexcept;
	double getAlbedo() const noexcept;
	double getVolume() const noexcept;
	double getMass() const noexcept;
	double getMols() const noexcept;
};

}//namespace pleistocene