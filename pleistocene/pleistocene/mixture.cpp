#include "mixture.h"
#include "tile-climate.h"

namespace pleistocene {
namespace simulation {
namespace climate {
namespace layers {
namespace elements {

//=====================================================================================================================
//MIXTURE
//=====================================================================================================================

//Constructors
//======================

Mixture::Mixture() noexcept {}

Mixture::Mixture(Element element, double temperature, elements::State state, double fixedVolume) noexcept :
Mixture(std::vector<Element> {element}, temperature, state, fixedVolume) {}

Mixture::Mixture(std::vector<Element> compositionElements, double temperature, elements::State state, double fixedVolume) noexcept :
	_temperature(temperature),
	_state(state),
	_fixedVolume(fixedVolume)
{
	if (_temperature <= 0) { LOG("ERROR: Inappropriately low temperature (this is kelvin) noexcept : "); LOG(temperature); exit(EXIT_FAILURE); }

	if (_fixedVolume == my::kFakeDouble) { _volumeIsFixed = false; }
	else { _volumeIsFixed = true; }

	_elements.clear();

	for (Element &element : compositionElements) {
		pushSpecific(element);
	}
	calculateParameters();
}


//=====================================================================================================================
//CALCULATIONS
//=====================================================================================================================

void Mixture::calculateParameters() noexcept {
	calculateMass();
	calculateVolume();
	calculateMols();
	calculateHeatCapacity();
	calculateAlbedoIndex();
	calculateSolarAbsorptionIndex();
	calculateInfraredAbsorptionIndex();
}

void Mixture::calculateMass() noexcept {
	using namespace elements;
	_totalMass = 0;
	for (const ElementPair &elementPair : _elements) {
		_totalMass += elementPair.second.getMass();
	}
	//AUX
}

void Mixture::calculateVolume() noexcept {
	using namespace elements;
	if (_volumeIsFixed) {
		_totalVolume = _fixedVolume;
		return;
	}
	else {
		_totalVolume = 0;
		for (const ElementPair &elementPair : _elements) {
			_totalVolume += elementPair.second.getVolume();
		}
	}
	//AUX
}

void Mixture::calculateMols() noexcept {
	using namespace elements;

	if (_state != GAS) { _totalMols = my::kFakeDouble; return; }//Throw? //TODO

	_totalMols = 0;
	for (const ElementPair &elementPair : _elements) {
		_totalMols += elementPair.second.getMols();
	}
}

void Mixture::calculateHeatCapacity() noexcept {
	using namespace elements;
	_totalHeatCapacity = 0;
	for (const ElementPair &elementPair : _elements) {
		_totalHeatCapacity += elementPair.second.getHeatCapacity();
	}
	//AUX
}

void Mixture::calculateAlbedoIndex() noexcept {
	using namespace elements;

	if (_state == SOLID || _state == LIQUID) {//for solids/liquids albedo is a volume average of the surface elements
		double weightedAlbedo = 0;
		for (const ElementPair &elementPair : _elements) {
			weightedAlbedo += elementPair.second.getAlbedo()*elementPair.second.getVolume();
		}
		if (_totalVolume <= 0) { _albedo = 0; return; }
		_albedo = weightedAlbedo / _totalVolume;
		return;
	}
	else {//add the albedos
		_albedo = 0;
		for (const ElementPair &elementPair : _elements) {
			_albedo += elementPair.second.getAlbedo();
		}
		//AUX
		_albedo = std::min(_albedo, 1.0);
	}
}

void Mixture::calculateSolarAbsorptionIndex() noexcept {
	using namespace elements;

	if (_state == SOLID) {//opaque
		_solarAbsorptionIndex = 1;
		return;
	}
	else {
		_solarAbsorptionIndex = 0;
		for (const ElementPair &elementPair : _elements) {
			_solarAbsorptionIndex += elementPair.second.getSolarAbsorptivity();
		}
		_solarAbsorptionIndex = std::min(_solarAbsorptionIndex, 1.0);
		//LOG("Non-Solid solar absorption: " << _solarAbsorptionIndex);
	}
}

void Mixture::calculateInfraredAbsorptionIndex() noexcept {
	using namespace elements;

	if (_state == SOLID) {//opaque
		_infraredAbsorptionIndex = 1.0;
		return;
	}
	else {
		_infraredAbsorptionIndex = 0;
		for (const ElementPair &elementPair : _elements) {
			_infraredAbsorptionIndex += elementPair.second.getInfraredAbsorptivity();
		}
		//AUX
		_infraredAbsorptionIndex = std::min(_infraredAbsorptionIndex, 1.0);
	}
}


//=====================================================================================================================
//MIXING MIXTURES
//=====================================================================================================================

void Mixture::transferMixture(Mixture &receivingMixture, Mixture &givingMixture, double proportion) noexcept {
	Mixture pushMix = givingMixture.copyProportion(proportion);
	givingMixture.resizeBy(1 - proportion);
	receivingMixture.push(pushMix);
}

Mixture Mixture::copyProportion(double proportion) const noexcept {
	Mixture copiedMixture = *this;
	copiedMixture.resizeBy(proportion);
	return copiedMixture;
}

void Mixture::resizeBy(double proportion) noexcept {
	for (elements::ElementPair &elementPair : _elements) {
		elementPair.second.resizeBy(proportion);
	}
}

void Mixture::push(Mixture &addedMixture) noexcept {
	using namespace elements;
	double totalHeat = this->_totalHeatCapacity*_temperature +
		addedMixture._totalHeatCapacity*addedMixture._temperature;
	double newTotalHeatCapacity = this->_totalHeatCapacity + addedMixture._totalHeatCapacity;
	_temperature = totalHeat / newTotalHeatCapacity;

	for (ElementPair &elementPair : addedMixture._elements) {
		pushSpecific(elementPair.second);
	}
	calculateParameters();
}

void Mixture::pushSpecific(Element addedSpecificElement, double temperature, bool temperatureSpecified) noexcept {
	using namespace elements;

	ElementType eType = addedSpecificElement.getElementType();

	//check state conflict
	if (addedSpecificElement.getStateConflict(_state)) { LOG("Element type not compatible with state"); exit(EXIT_FAILURE); }

	if (temperatureSpecified) {
		Mixture temporaryMixture = Mixture(addedSpecificElement, temperature, _state);
		this->push(temporaryMixture);
		return;
	}
	else {
		if (_elements.count(eType)) {
			_elements[eType].combineLike(addedSpecificElement);
			return;
		}
		else {
			_elements[eType] = addedSpecificElement;
			return;
		}
	}
}

double Mixture::pullSpecific(Element pulledElement) noexcept {
	using namespace elements;
	ElementType eType = pulledElement.getElementType();
	double massPulled = 0.0;
	double massRequested = pulledElement.getMass();
	if (_elements.count(eType)) {
		massPulled = _elements[eType].pullMass(massRequested);

		if (_elements[eType].getMass() <= 0) {//you took everything
			auto iterator = _elements.find(eType);
			_elements.erase(iterator);
		}
	}
	return massPulled;
}

//========================================================================================================
//SIMULATION
//========================================================================================================

double Mixture::filterSolarRadiation(double solarEnergyKJ) noexcept {
	if (solarEnergyKJ <= 0) { return 0; }

	int mixtureCount = _elements.size();
	if (mixtureCount == 0) { LOG("NO COMPONENTS"); exit(EXIT_FAILURE); return solarEnergyKJ; }
	if (_totalHeatCapacity <= 0) { LOG("ZERO HEAT CAPACITY"); exit(EXIT_FAILURE); return solarEnergyKJ; }
	if (_albedo < 0 || _albedo>1) { LOG("WEIRD ALBEDO"); exit(EXIT_FAILURE); return solarEnergyKJ; }

	//reflection
	solarEnergyKJ *= (1 - _albedo);

	//Water and air transmit a lot of energy
	double solarAbsorbed = _solarAbsorptionIndex*solarEnergyKJ;

	double outputEnergyKJ = solarEnergyKJ- solarAbsorbed;

	//TEMPERATURE CHANGE!!!!
	_temperature += solarAbsorbed / _totalHeatCapacity;

	return outputEnergyKJ;
}

double Mixture::emitInfrared() noexcept {
	int mixtureCount = _elements.size();
	if (mixtureCount == 0) { LOG("NO COMPONENTS"); exit(EXIT_FAILURE); return 0; }
	if (_totalHeatCapacity <= 0) { LOG("ZERO HEAT CAPACITY");  exit(EXIT_FAILURE); return 0; }
	if (_temperature <= 0) { LOG("BELOW ABSOLUTE ZERO"); exit(EXIT_FAILURE); return 0; }
	double emissionEnergy;
	if (_state == elements::GAS) {
		emissionEnergy = kEmissionConstantPerHour * pow(_temperature, 4)*_totalMass * 4 * pow(10, -4);
	}
	else {
		emissionEnergy = kEmissionConstantPerHour * pow(_temperature, 4);
	}

	_temperature -= emissionEnergy / _totalHeatCapacity; //emmision cooling

	return emissionEnergy;
}

double Mixture::filterInfrared(double infraredEnergy) noexcept {
	int mixtureCount = _elements.size();
	if (mixtureCount == 0) { LOG("NO COMPONENTS");  exit(EXIT_FAILURE); return infraredEnergy; }//dummy return
	if (_totalHeatCapacity <= 0) { LOG("ZERO HEAT CAPACITY");  exit(EXIT_FAILURE); return infraredEnergy; }//dummy return

	//absorption heating
	double infraredAbsorbed = _infraredAbsorptionIndex*infraredEnergy;

	//TEMPERATURE CHANGE
	_temperature += infraredAbsorbed / _totalHeatCapacity;

	infraredEnergy -= infraredAbsorbed;
	return infraredEnergy;
}

void Mixture::conduction(Mixture &mixture1, Mixture &mixture2, double area) noexcept 
{
	double deltaT = mixture2._temperature - mixture1._temperature;

	double conductivity=pow(10, -6);

	if (area<0) { LOG("Negative Area");  exit(EXIT_FAILURE); return; }

	//Shitty conductivity estimate
	if (mixture1._state == GAS || mixture2._state == GAS) {
		conductivity *= 0.01;
	}

	//Calculate heat exchanged in conduction
	double heatExchanged = deltaT*conductivity*area;
	bool sign = signbit(heatExchanged);
	heatExchanged = abs(heatExchanged);

	//Check overstep
	double totalHeat = mixture1._totalHeatCapacity*mixture1._temperature + mixture2._totalHeatCapacity*mixture2._temperature;
	double equalizationTemp = totalHeat / (mixture1._totalHeatCapacity + mixture2._totalHeatCapacity);

	//heat exchange required to equilize temperatures
	double maxHeatExchange = abs(equalizationTemp - mixture1._temperature)*mixture1._totalHeatCapacity;

	//minimum
	heatExchanged = std::min(maxHeatExchange, heatExchanged);
	if (sign) heatExchanged = -heatExchanged;

	mixture1._temperature += heatExchanged / mixture1._totalHeatCapacity;
	mixture2._temperature -= heatExchanged / mixture2._totalHeatCapacity;

	if(mixture1._temperature<=0 || mixture2._temperature <= 0){ LOG("BELOW ABSOLUTE ZERO"); exit(EXIT_FAILURE); }
}

double Mixture::getTemperature() const noexcept { return _temperature; }
double Mixture::getHeatCapacity() const noexcept { return _totalHeatCapacity; }
double Mixture::getHeight() const noexcept { return _totalVolume; }
double Mixture::getAlbedo() const noexcept { return _albedo; }
double Mixture::getVolume() const noexcept { return _totalVolume; }
double Mixture::getMass() const noexcept { return _totalMass; }
double Mixture::getMols() const noexcept { return _totalMols; }

std::vector<std::string> Mixture::getThermalMessages() const noexcept {
	std::vector<std::string> messages;

	std::stringstream stream;
	stream << "Temperature: " << my::double2string(_temperature - 273.15) << " °C";
	messages.push_back(stream.str());

	stream.str(std::string());
	stream << "Heat Capacity: " << my::double2string(_totalHeatCapacity);
	messages.push_back(stream.str());

	/*stream.str(std::string());
	stream << "Absorbed Solar Energy: " << my::double2string(this->_totalSolarAbsorbed);
	messages.push_back(stream.str());*/

	/*stream.str(std::string());
	stream << "Absorbed Infrared Energy: " << my::double2string(this->_totalInfraredAbsorbed);
	messages.push_back(stream.str());*/

	/*stream.str(std::string());
	stream << "Emitted Energy (KJ): " << my::double2string(this->_infraredEmitted);
	messages.push_back(stream.str());*/

	stream.str(std::string());
	stream << " ";
	messages.push_back(stream.str());

	return messages;
}

std::vector<std::string> Mixture::getElementMessages() const noexcept 
{
	std::vector<std::string> messages;
	std::vector<std::string> subMessages;

	for (const ElementPair &elementPair : _elements) {
		subMessages=elementPair.second.getMessages();
		messages.insert(messages.begin(), subMessages.begin(), subMessages.end());
	}

	std::stringstream stream;
	stream << "Solar Absorption Index: " << my::double2string(this->_solarAbsorptionIndex);
	messages.push_back(stream.str());

	stream.str(std::string());
	stream << "Infrared Absorption Index: " << my::double2string(this->_infraredAbsorptionIndex);
	messages.push_back(stream.str());

	return messages;
}


}//namespace elements
}//namespace layers
}//namespace climate
}//namespace simulation
}//namespace pleistocene