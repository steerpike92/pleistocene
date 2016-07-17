#include "statistics.h"
#include "math.h"
#include "algorithm"

namespace pleistocene {

Statistics::Statistics() noexcept :
	_mean(0),
	_standardDeviation(0),
	_sum(0),
	_values{}
{
}

void Statistics::clear() noexcept
{
	_sum = 0;
	_values.clear();
}

void Statistics::contributeValue(double value) noexcept
{
	_sum += value;
	_values.push_back(value);
}

void Statistics::calculateStatistics() noexcept
{
	int N = _values.size();
	
	if (N == 0) return;

	_mean = _sum / N;
	
	double standardDeviationSum = 0;
	double deviation;

	for (double value : _values) {
		deviation = value - _mean;
		standardDeviationSum += pow(deviation, 2);
	}

	_standardDeviation = pow(standardDeviationSum, 0.5) / N;

}

//does what it says
double Statistics::getSigmasOffMean(double value) const noexcept
{
	if (_standardDeviation == 0) return 0;
	else return (value - _mean) / _standardDeviation;
}

//value between -1 and 1 representing above/below averageness
double Statistics::getHeatValue(double value) const noexcept
{
	if (_standardDeviation == 0) return 0;
	double heatValue = getSigmasOffMean(value) / 3;
	heatValue = std::max(heatValue, -1.0);
	heatValue = std::min(heatValue, 1.0);
	return heatValue;
}


}//namespace pleisocene