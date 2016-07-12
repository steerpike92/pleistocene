#include "solarRadiation.h"


SolarRadiation::SolarRadiation() {}

SolarRadiation::SolarRadiation(double latitude_deg, double longitude_deg) {
	using namespace climate::planetary;

	_latitude_rad=(my::degToRad(latitude_deg));
	_longitude_rad = (my::degToRad(longitude_deg));

	//Longitude=0 normalVector setup
	_normalVector(0) = cos(_latitude_rad + tilt_rad);
	_normalVector(1)=0;
	_normalVector(2)= sin(_latitude_rad + tilt_rad);

	//longitude shift
	buildRotationMatrix(_longitude_rad);

	_normalVector = _rotationMatrix*_normalVector;

}


bool SolarRadiation::_axisExists = false;

//Rotation matrix from sidereal angle
void SolarRadiation::buildRotationMatrix(double angle_rad){
	using namespace climate::planetary;

	if (!_axisExists) {//First time setup
		_earthAxis << -sin(tilt_rad),
			0,
			cos(tilt_rad);

		_intermediateMatrix << 0, -_earthAxis(2), _earthAxis(1),
			_earthAxis(2), 0, -_earthAxis(0),
			-_earthAxis(1), _earthAxis(0), 0;

		_axisExists = true;
	}

	if (angle_rad == _oldRotation) {
		return;//this is already the right matrix;
	}

	//Rotation Matrix calculation
	_rotationMatrix = Eigen::Matrix3d::Identity() + sin(angle_rad)*_intermediateMatrix+(1.0-cos(angle_rad))*(_intermediateMatrix*_intermediateMatrix);
	_oldRotation = angle_rad;

	//LOG("ROTATION MATRIX");
	//LOG(_rotationMatrix);
}

double SolarRadiation::_oldRotation = -1.0;

void SolarRadiation::setupSolarRadiation() {
	using namespace climate::planetary;

	//Setup Rotation Matrix
	//take total hours in this year and divide it by the length of a sidereal day (hours)
	double sDays = (my::SimulationTime::_globalTime.getHour() + my::SimulationTime::_globalTime.getDay()*solarDay_h) / siderealDay_h;

	//take the floor of the number of sidereal days to determine our progress through the current siderial day
	double sDayFloor = floor(sDays);
	double sTime = sDays - sDayFloor;//portion of current siderial day

	double angle_rad = sTime*M_PI * 2;//multiply by 2pi to get rad position

	buildRotationMatrix(angle_rad);

	//Setup Sun Ray Vector
	//sun vector rotates in circle once per solar year
	angle_rad = 2 * M_PI*(double(my::SimulationTime::_globalTime.getDay()+
		double(my::SimulationTime::_globalTime.getHour())/double(solarDay_h)) /(double)solarYear_d);

	_sunRayVector(0) = cos(angle_rad);
	_sunRayVector(1) = sin(angle_rad);
	_sunRayVector(2)=0;
}

double SolarRadiation::applySolarRadiation(){
	//setupRadiation needs to get called for the hour before these get called.

	//rotated normal vector
	Eigen::Vector3d _rotatedNormalVector = _rotationMatrix*_normalVector;

	_solarFraction = _sunRayVector.dot(_rotatedNormalVector);

	if (_solarFraction < 0) {//night
		_solarFraction = 0;
	}

	return _solarFraction;
}

Eigen::Vector3d SolarRadiation::_sunRayVector;//Vector direction of sun rays

Eigen::Vector3d SolarRadiation::_earthAxis;//earth axis of rotation
Eigen::Matrix3d SolarRadiation::_intermediateMatrix;//Setup Matrix (constant)
Eigen::Matrix3d SolarRadiation::_rotationMatrix;//Rotation matrix from sidereal angle



double SolarRadiation::getRadiationShader() {
	double solarShader = _solarFraction;
	if (!SOLAR_SHADE) { solarShader = 1; }
	return solarShader;
}