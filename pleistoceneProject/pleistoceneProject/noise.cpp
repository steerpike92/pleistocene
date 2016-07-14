#include "noise.h"


inline double findnoise2(double x, double y, int seed) noexcept
{
	int n = (int)x*seed + (int)y * 57;
	n = (n << 13) ^ n;
	int nn = (n*(n*n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0 - ((double)nn / 1073741824.0);
}


inline double interpolate1(double a, double b, double x) noexcept
{
	double ft = x * 3.1415927;
	double f = (1.0 - cos(ft))* 0.5;
	return a*(1.0 - f) + b*f;
}


double noise2(double x, double y, int seed) noexcept
{
	double floorx = (double)((int)x);//This is kinda a cheap way to floor a double integer.
	double floory = (double)((int)y);
	double s, t, u, v;//Integer declaration
	s = findnoise2(floorx, floory, seed);
	t = findnoise2(floorx + 1, floory, seed);
	u = findnoise2(floorx, floory + 1, seed);//Get the surrounding pixels to calculate the transition.
	v = findnoise2(floorx + 1, floory + 1, seed);
	double int1 = interpolate1(s, t, x - floorx);//Interpolate between the values.
	double int2 = interpolate1(u, v, x - floorx);//Here we use x-floorx, to get 1st dimension. Don't mind the x-floorx thingie, it's part of the cosine formula.
	return interpolate1(int1, int2, y - floory);//Here we use y-floory, to get the 2nd dimension.
}


double** perlinNoise(int Cols, int Rows, double zoom, double p, int octaves, int seed) noexcept
//w and h speak for themselves, zoom wel zoom in and out on it, I usually
{// use 75. P stands for persistence,

	double** noiseTable;

	noiseTable = new double *[Rows];
	for (int row = 0; row < Rows; row++) {
		noiseTable[row] = new double[Cols];
	}

	for (int r = 0; r<Rows; r++)
	{//Loops to loop trough all the pixels
		for (int c = 0; c<Cols; c++)
		{
			double getnoise = 0;
			for (int a = 0; a<octaves - 1; a++)//This loops trough the octaves.
			{
				double frequency = pow(2, a);//This increases the frequency with every loop of the octave.
				double amplitude = pow(p, a);//This decreases the amplitude with every loop of the octave.

											 //This uses our perlin noise functions. It calculates all our zoom and frequency and amplitude
				getnoise += noise2(((double)c)*frequency / zoom, ((double)r) / zoom*frequency, seed)*amplitude;
			}//	
			noiseTable[r][c] = getnoise;
		}//														   
	}

	return noiseTable;

}