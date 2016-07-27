#pragma once
#include "globals.h"
#include "element.h"

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {
namespace elements {

class Mixture {
protected:
	std::map<elements::ElementType, Element> _elements;//main constituent elements
	elements::State _state;


	double _temperature = 0;//Kelvin

	//m^2 column values
	double _totalVolume = 0;//m3
	double _fixedVolume = 0;//m3. Use fixed volume for gas cells
	bool _volumeIsFixed = false;//for gases
	double _totalMass = 0;//kg
	double _totalHeatCapacity = 0;//kJ/(kg K)
	double _totalMols = 0;

	//values scaled up by incorporating tile surface area
	double _trueVolume = 0;
	double _trueMass = 0;
	double _trueMols = 0;
	double _trueHeatCapacity = 0;

	double _albedo = 0;//reflective index (proportion reflected passing through this material)
	double _solarAbsorptionIndex = 0;//proportion absorbed passing through this material
	double _infraredAbsorptionIndex = 0;//proportion absorbed passing through this material

	double _hourlySolarInput = 0;
	//double _dailySolarInput = 0;

	double _hourlyInfraredInput = 0;
	double _hourlyInfraredInputDisplay = 0;

	double _hourlyOutputRadiation = 0;
	//double _dailyOutputRadiation = 0;

	double _netConductiveExchange = 0;

	double _equilibriumTemperature = 0;
	
	Eigen::Vector3d _velocity { 0,0,0 };

	double handleInOutRadiation() noexcept;
	double calculateEmissions(double temperature) const noexcept;
	double calculateEquilibriumTemperature(double inputRadiation) const noexcept;

public:
	Mixture() noexcept;
	Mixture(Element element, double temperature, elements::State state, double fixedVolume = my::kFakeDouble) noexcept;
	Mixture(std::vector<Element> compositionElements, double temperature, elements::State state, double fixedVolume = my::kFakeDouble) noexcept;

	void hourlyClear();

	bool _emittor = false;

	//=======================================
	//PARAMETER CALCULATIONS
	//=======================================
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
	//=======================================
	//MIXING MIXTURES
	//=======================================
	static void transferMixture(Mixture &receivingMixture, Mixture &givingMixture, double proportion) noexcept;
	void resizeBy(double proportion) noexcept;
	
protected:
	void push(Mixture &addedMixture) noexcept;
	//Mixture pull(Mixture &subtractedMixture) noexcept;

	Mixture copyProportion(double proportion) const noexcept;
	void pushSpecific(Element addedSpecificElement, double temperature = 0.0, bool tempSpecified = false) noexcept;
	double pullSpecific(Element subtractedSpecificElement) noexcept;

public:
	//=======================================
	//SIMULATION
	//=======================================

	double filterSolarRadiation(double incidentSolarEnergyKJ) noexcept;
	double emitInfrared() noexcept;
	double filterInfrared(double infraredEnergyKJ) noexcept;
	static void conduction(Mixture &mixture1, Mixture &mixture2, double area) noexcept;

	//=======================================
	//GETTERS
	//=======================================

	std::vector<std::string> getThermalMessages() const noexcept;
	std::vector <std::string> getElementMessages() const noexcept;
	double getTemperature() const noexcept;
	double getHeatCapacity() const noexcept;
	double getHeight() const noexcept;
	double getAlbedo() const noexcept;
	double getVolume() const noexcept;
	double getMass() const noexcept;
	double getMols() const noexcept;

	void applyForce(Eigen::Vector3d force) noexcept;
	
	virtual void setVelocity(Eigen::Vector3d velocity) noexcept;
	virtual Eigen::Vector3d getVelocity() const noexcept;

};

}//namespace elements
}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene