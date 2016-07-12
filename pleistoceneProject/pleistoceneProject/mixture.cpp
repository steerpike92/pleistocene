#include "mixture.h"

//=====================================================================================================================
//MIXTURE
//=====================================================================================================================

//Constructors
//============
Mixture::Mixture(){}

Mixture::Mixture(Element element, double temperature, elements::State state, double fixedVolume):
	Mixture(std::vector<Element> {element}, temperature, state, fixedVolume){}

Mixture::Mixture(std::vector<Element> compositionElements, double temperature, elements::State state, double fixedVolume) :
	_temperature(temperature),
	_state(state),
	_fixedVolume(fixedVolume)
{
	if (_temperature <= 0) { LOG("ERROR: Inappropriately low temperature (this is kelvin): "); LOG(temperature); throw(2); }

	if (_fixedVolume == my::FakeDouble) {_volumeIsFixed = false;}
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

void Mixture::calculateParameters() {
	calculateMass();
	calculateVolume();
	calculateMols();
	calculateHeatCapacity();
	calculateAlbedoIndex();
	calculateSolarAbsorptionIndex();
	calculateInfraredAbsorptionIndex();
}

void Mixture::calculateMass() {
	using namespace elements;
	_totalMass = 0;
	for (const ElementPair &elementPair : _elements) {
		_totalMass += elementPair.second.getMass();
	}
	//AUX
}

void Mixture::calculateVolume() {
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

void Mixture::calculateMols() {
	using namespace elements;

	if (_state != GAS) {_totalMols = -1;return;}

	_totalMols = 0;
	for (const ElementPair &elementPair : _elements) {
		_totalMols += elementPair.second.getMols();
	}
}

void Mixture::calculateHeatCapacity() {
	using namespace elements;
	_totalHeatCapacity = 0;
	for (const ElementPair &elementPair : _elements) {
		_totalHeatCapacity += elementPair.second.getHeatCapacity();
	}
	//AUX
}

void Mixture::calculateAlbedoIndex() {
	using namespace elements;

	if (_state == SOLID || _state==LIQUID) {//for solids/liquids albedo is a volume average of the surface elements
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

void Mixture::calculateSolarAbsorptionIndex() {
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
		//AUX
		_solarAbsorptionIndex = std::min(_solarAbsorptionIndex, 1.0);
	}
}

void Mixture::calculateInfraredAbsorptionIndex() {
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

void Mixture::transferMixture(Mixture &receivingMixture, Mixture &givingMixture, double proportion){
	Mixture pushMix = givingMixture.copyProportion(proportion);
	givingMixture.resizeBy(1 - proportion);
	receivingMixture.push(pushMix);
}

Mixture Mixture::copyProportion(double proportion) const{
	Mixture copiedMixture = *this;
	copiedMixture.resizeBy(proportion);
	return copiedMixture;
}

void Mixture::resizeBy(double proportion) {
	for (elements::ElementPair &elementPair : _elements) {
		elementPair.second.resizeBy(proportion);
	}
}

void Mixture::push(Mixture &addedMixture){
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

void Mixture::pushSpecific(Element addedSpecificElement, double temperature, bool temperatureSpecified) {
	using namespace elements;

	ElementType eType = addedSpecificElement.getElementType();

	//check state conflict
	if (addedSpecificElement.getStateConflict(_state)) {LOG("Element type not compatible with state");throw(2);}

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

double Mixture::pullSpecific(Element pulledElement) {
	using namespace elements;
	ElementType eType = pulledElement.getElementType();
	double massPulled=0.0;
	double massRequested = pulledElement.getMass();
	if (_elements.count(eType)) {
		massPulled = _elements[eType].pullMass(massRequested);
		
		if (_elements[eType].getMass()<=0) {//you took everything
			auto iterator = _elements.find(eType);
			_elements.erase(iterator);
		}
	}
	return massPulled;
}

//========================================================================================================
//SIMULATION
//========================================================================================================

double Mixture::filterSolarRadiation(double solarEnergyKJ){
	_totalSolarAbsorbed = 0;
	_totalInfraredAbsorbed = 0;
	_totalInfraredEmitted = 0;
	
	int mixtureCount = _elements.size();
	if (mixtureCount== 0) {LOG("NO COMPONENTS"); throw(2); return solarEnergyKJ;}
	if (_totalHeatCapacity <= 0) { LOG("ZERO HEAT CAPACITY"); throw(2); return solarEnergyKJ; }
	if (_albedo < 0 || _albedo>1) { LOG("WEIRD ALBEDO"); throw(2); return solarEnergyKJ; }

	//reflection
	solarEnergyKJ *= (1 - _albedo);

	_totalSolarAbsorbed = _solarAbsorptionIndex*solarEnergyKJ;

	double outputEnergyKJ = (1 - _solarAbsorptionIndex)*solarEnergyKJ;

	//TEMPERATURE CHANGE!!!!	
	_temperature += _totalSolarAbsorbed / _totalHeatCapacity;

	return outputEnergyKJ;
}

double Mixture::emitInfrared() {

	int mixtureCount = _elements.size();
	if (mixtureCount == 0) { LOG("NO COMPONENTS"); throw(2); return 0; }
	if (_totalHeatCapacity <= 0) { LOG("ZERO HEAT CAPACITY");  throw(2); return 0; }

	double emissionEnergy;
	if (_state == elements::GAS) {
		emissionEnergy = climate::planetary::emmisionConstantPerHour * pow(_temperature, 4)*_totalMass * 5 * pow(10, -4);
	}
	else {
		emissionEnergy = climate::planetary::emmisionConstantPerHour * pow(_temperature, 4);
	}

	_totalInfraredEmitted += emissionEnergy;
	
	//TEMPERATURE CHANGE!!!!
	_temperature -= emissionEnergy / _totalHeatCapacity; //emmision cooling

	return emissionEnergy;
}


double Mixture::filterInfrared(double infraredEnergy){
	int mixtureCount = _elements.size();
	if (mixtureCount == 0) { LOG("NO COMPONENTS");  throw(2); return infraredEnergy; }
	if (_totalHeatCapacity <= 0) { LOG("ZERO HEAT CAPACITY");  throw(2); return infraredEnergy; }

	//absorption heating
	double infraredAbsorbed = _infraredAbsorptionIndex*infraredEnergy;
	_totalInfraredAbsorbed += infraredAbsorbed;

	//TEMPERATURE CHANGE!!!!! (previously this was done twice as a bug)
	_temperature += infraredAbsorbed / _totalHeatCapacity;


	infraredEnergy -= infraredAbsorbed;
	return infraredEnergy;
}


void Mixture::conduction(Mixture &mixture1, Mixture &mixture2){//STUB
}

double Mixture::getTemperature()const {return _temperature;}
double Mixture::getHeight()const { return _totalVolume; }
double Mixture::getAlbedo()const { return _albedo; }
double Mixture::getVolume()const { return _totalVolume; }
double Mixture::getMass()const { return _totalMass; }
double Mixture::getMols()const { return _totalMols; }

std::vector<std::string> Mixture::getMessages() const {
	std::vector<std::string> messages;

	std::stringstream stream;
	stream << "Temperature: " << round(_temperature-273.15) << " °C";
	messages.push_back(stream.str());

	/*stream = std::stringstream();
	stream << "Albedo: " << int(_albedo * 100) << "%";
	messages.push_back(stream.str());

	stream = std::stringstream();
	stream << "Mass: " << int(_totalMass);
	messages.push_back(stream.str());*/
	
	stream = std::stringstream();
	stream << "Heat Capacity: " << int(_totalHeatCapacity);
	messages.push_back(stream.str());

	stream = std::stringstream();
	stream << "Absorbed Solar Energy: " << int(this->_totalSolarAbsorbed);
	messages.push_back(stream.str());

	stream = std::stringstream();
	stream << "Absorbed Infrared Energy: " << int(this->_totalInfraredAbsorbed);
	messages.push_back(stream.str());

	stream = std::stringstream();
	stream << "Emitted Energy (KJ): " << int(this->_totalInfraredEmitted);
	messages.push_back(stream.str());

	stream = std::stringstream();
	stream << " ";
	messages.push_back(stream.str());

	return messages;

}

