#include "statistics.h"
#include "globals.h"

namespace pleistocene {

Statistics::Statistics() noexcept :
	_trackedMeans{},
	_trackedSigmas{},
	_mean(0),
	_sigma(0),
	_sum(0),
	_values{},
	_valid(false)
{
}

void Statistics::clear() noexcept
{
	_sigma = 0;
	_mean = 0;
	_sum = 0;
	_values.clear();
	_valid = false;
}

void Statistics::newStatistic() noexcept
{
	_trackedMeans.clear();
	_trackedSigmas.clear();
	this->clear();
}

void Statistics::contributeValue(double value) noexcept
{
	_sum += value;
	_values.push_back(value);
}

void Statistics::calculateStatistics() noexcept
{
	int N = _values.size();
	if (N == 0) {//no values for this statistic
		clear();
		return;
	}

	_valid = true;

	double currentMean = _sum / N;
	
	_trackedMeans.push_front(currentMean);

	int valuesTracked = _trackedMeans.size();
	if (valuesTracked > kTrackedFrames) { _trackedMeans.pop_back(); _trackedSigmas.pop_back(); valuesTracked--; }

	//Average tracked means to get mean
	_mean = 0;
	for (double mean : _trackedMeans) {
		_mean += mean;
	}
	_mean /= valuesTracked;

	//compute this data sets standard of deviation
	double deviation;
	double varianceSum = 0;

	for (double value : _values) {
		deviation = value - _mean;
		varianceSum += pow(deviation, 2);
	}

	double variance = varianceSum / N;

	double currentSigma= pow(variance, 0.5);

	_trackedSigmas.push_front(currentSigma);

	 //Average tracked sigmas to get sigma
	_sigma = 0;
	for (double sigma : _trackedSigmas) {
		_sigma += sigma;
	}
	_sigma /= double(valuesTracked);
}

//does what it says
double Statistics::getSigmasOffMean(double value) const noexcept
{
	if (!_valid || abs(_sigma)<0.000001) return 0;
	else return ((value - _mean) / _sigma);
}

//value between -1 and 1 representing above/below averageness
double Statistics::getHeatMapValue(double value) const noexcept
{
	double heatValue = getSigmasOffMean(value);
	heatValue = std::max(heatValue, -3.5);
	heatValue = std::min(heatValue, 3.5);
	return heatValue;
}

std::vector<std::string> Statistics::getMessages() const noexcept {
	std::vector<std::string> messages;
	std::stringstream stream;
	stream << "STATISTICS ";
	messages.push_back(stream.str());



	stream.str(std::string());

	stream << "Mean: " << my::double2string(_mean) <<" ";
	messages.push_back(stream.str());

	stream.str(std::string());
	stream << "Sigma: " << my::double2string(_sigma) <<" ";
	messages.push_back(stream.str());

	return messages;
}


}//namespace pleisocene