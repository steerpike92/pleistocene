#include "noise.h"
#include <math.h>
#include <limits>
#include "utility.h"

namespace pleistocene {
namespace noise {

//not written by me
double Pseudorandom2D(double x, double y, int seed) noexcept
{
	int n = (int)x*seed + (int)y * 57;
	n = (n << 13) ^ n;
	int nn = (n*(n*n * 60493 + 19990303) + 1376312589) & INT_MAX;
	return 1.0 - ((double)nn / 1073741824.0);
}

//not written by me
double Interpolate(double a, double b, double x) noexcept
{
	double ft = x * 3.1415927;
	double f = (1.0 - cos(ft))* 0.5;
	return a*(1.0 - f) + b*f;
}

//not written by me
double Noise2D(double x, double y, int seed) noexcept
{
	int floor_x = int(x);
	int floor_y = int(y);

	//Box for interpolation
	double bottom_left, bottom_right, upper_left, upper_right;
	bottom_left = Pseudorandom2D(floor_x, floor_y, seed);
	bottom_right = Pseudorandom2D(floor_x + 1, floor_y, seed);
	upper_left = Pseudorandom2D(floor_x, floor_y + 1, seed);
	upper_right = Pseudorandom2D(floor_x + 1, floor_y + 1, seed);

	//Interpolate between the values.
	double interpolation1 = Interpolate(bottom_left, bottom_right, x - floor_x);
	double interpolation2 = Interpolate(upper_left, upper_right, x - floor_x);
	return Interpolate(interpolation1, interpolation2, y - floor_y);
}


std::vector<double> PerlinNoise(std::vector<std::pair<double, double>> positions, NoiseParameters parameters) noexcept 
{
	using namespace utility;

	//returned noise vector
	std::vector<double> noise_table;

	//parameter unpacking
	int seed = parameters.seed;
	int octave_count= parameters.octaves;
	double zoom= parameters.zoom;
	double persistance = parameters.persistance;


	//build frequency and amplitude vectors
	std::vector<double> frequencies;
	std::vector<double> amplitudes;

	for (int octave : range( 0, octave_count)) {
		frequencies.push_back(pow(2, octave));		//double frequency with each octave.

		amplitudes.push_back(pow(persistance, octave)); //scale down amplitude with each octave.

	}

	//helps determine noise amplitude (this is a dummy value to be updated)
	double maximum = 0.01;

	double X;  //x position variable (related to, but not the same as the literal tile position)
	double Y;  //y position variable
	

	for (auto position : positions) {
		double getNoise = 0;

		//loop through octaves
		for (int octave : range(0, octave_count)) {
			double frequency = frequencies[octave];
			double amplitude = amplitudes[octave];

			X = double(position.first)*frequency / zoom;
			Y = double(position.second)*frequency / zoom;

			getNoise += noise::Noise2D(X, Y, seed)*amplitude;
		}

		noise_table.emplace_back(getNoise);

		//update maximum
		if (abs(getNoise) > maximum) maximum = abs(getNoise);
	}

	//Rescale with maximum so ranges in noiseTable are from -1 to 1. 
	for (double &noise_value : noise_table) {
		noise_value /= maximum;
	}

	return noise_table;
}


}//namespace noise
}//namespace pleistocene