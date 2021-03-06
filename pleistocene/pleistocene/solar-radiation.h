#pragma once
#include "globals.h"

namespace pleistocene {
namespace simulation {
namespace climate {

class SolarRadiation {
public:
	SolarRadiation() noexcept;
	SolarRadiation(double latitude_deg, double longitude_deg) noexcept;

	static void setupSolarRadiation() noexcept;

	//returns a proportion of the max radiation ([0,1]) at the lat,lon at that time 
	double applySolarRadiation() noexcept;

	double _solarFraction;
	double getRadiationShader() noexcept;

private:
	double _latitude_rad;
	double _longitude_rad;


	//Vector direction of sun rays
	static Eigen::Vector3d _sunRayVector;


	//note, it might make more sense to just determine the hourly rotation matrix and then just keep applying it.

	static void buildRotationMatrix(double radiansRotation) noexcept;

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

}//namespace climate
}//namespace simulation
}//namespace pleistocene
