#pragma once
#include <vector>

namespace pleistocene {
namespace noise {

struct NoiseParameters {
	int octaves;
	double zoom, persistance, seed;
};

double Pseudorandom2D(double x, double y, double seed) noexcept;

double Interpolate(double a, double b, double x) noexcept;

double Noise2D(double x, double y, double seed) noexcept;

std::vector<double> PerlinNoise(std::vector<std::pair<double, double>> positions, NoiseParameters parameters) noexcept;

}//namespace noise
}//namespace pleistocene