#pragma once
#include "globals.h"

class SolarRadiation {
public:
	SolarRadiation();
	SolarRadiation(double latitude_deg, double longitude_deg);

	static void setupSolarRadiation();

	//returns a proportion of the max radiation ([0,1]) at the lat,lon at that time 
	double applySolarRadiation();

	double _solarFraction;
	double getRadiationShader();

private:
	double _latitude_rad;
	double _longitude_rad;


	//Vector direction of sun rays
	static Eigen::Vector3d _sunRayVector;


	//note, it might make more sense to just determine the hourly rotation matrix and then just keep applying it.

	static void buildRotationMatrix(double radiansRotation);

	static double _oldRotation;//check if we can reuse the old rotation matrix

	static bool _axisExists;//Setup flag
	static Eigen::Vector3d _earthAxis;//earth axis of rotation
	static Eigen::Matrix3d _intermediateMatrix;//Setup Matrix (constant)
	static Eigen::Matrix3d _rotationMatrix;//Rotation matrix from sidereal angle


	//Normalized vector orthogonal to tile surface
	//Depends strictly on longitude and latitude
	//Transformed by _rotationMatrix to determine hourly position before dotted with _sunRayVector
	Eigen::Vector3d _normalVector;
};
