#pragma once
#include <vector>

namespace pleistocene {


class Statistics{
	double _mean;
	double _standardDeviation;
	double _sum;
	std::vector<double> _values;
public:
	Statistics() noexcept;

	void clear() noexcept;
	void contributeValue(double value) noexcept;
	void calculateStatistics() noexcept;
	double getSigmasOffMean(double value) const noexcept;
	double getHeatValue(double value) const noexcept;
};


}//namespace pleistocene

