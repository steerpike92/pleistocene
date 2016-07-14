#pragma once
#include "math.h"

inline double findnoise2(double x, double y, int seed) noexcept;


inline double interpolate1(double a, double b, double x) noexcept;

double noise2(double x, double y, int seed) noexcept;


double** perlinNoise(int Cols, int Rows, double zoom, double p, int octaves, int seed) noexcept;
