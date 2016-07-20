#pragma once
#include <vector>
#include <list>
#include <string>
#include "tile-climate.h"

namespace pleistocene {


class Statistics{

	std::vector<double> _values;//current statistical frame's data values
	double _sum;//the sum of _values
	bool _valid;//Are there any values in _values? Guard against divide by zero.

	//24 so that we have a full day of data to compare with.
	//Larger multiples of 24 also may be appropriate.
	//const int kTrackedFrames=(simulation::climate::kSolarDay_h)*(simulation::climate::kSolarYear_d);
	const int kTrackedFrames = (simulation::climate::kSolarDay_h);

	std::list<double> _trackedMeans;//list of previously computed mean values for this statistic
	std::list<double> _trackedSigmas;//list of previously computed standards of deviation for this statistic

	double _mean;//mean of _trackedMeans. A mean of means
	double _sigma;//mean of _trackedSigmas. A mean of sigmas.

public:
	Statistics() noexcept;
	void clear() noexcept;//Prepare for new set of data values
	void newStatistic() noexcept;//Clear all data for totally new statistic
	void contributeValue(double value) noexcept;//contribute a data value


	//compute current data set's mean and sigma; 
	//add these to the tracked lists;
	//compute average mean/sigmas
	void calculateStatistics() noexcept;

	//compute difference between given value and the mean in terms of standards of deviation
	double getSigmasOffMean(double value) const noexcept;

	//scales "getSigmasOffMean" into roughly [-1, 1] for heat mapping 
	double getHeatMapValue(double value) const noexcept;

	std::vector<std::string> getMessages() const noexcept;
};


}//namespace pleistocene

