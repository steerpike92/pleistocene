#pragma once
#include <vector>

namespace pleistocene {
namespace noise {


inline double Pseudorandom2D(double x, double y, int seed) noexcept;

inline double Interpolate(double a, double b, double x) noexcept;

double Noise2D(double x, double y, int seed) noexcept;

struct NoiseParameters {
	int seed, octaves; 
	double zoom, persistance;
};

std::vector<double> PerlinNoise(std::vector<std::pair<double, double>> positions, NoiseParameters parameters) noexcept;

}//namespace noise
}//namespace pleistocene