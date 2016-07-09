#pragma once
#include "globals.h"
#include "mixture.h"
#include "stateMixture.h"

class Graphics;

class Land {
	double _elevation;
	double _elevationShader;
	double _solarFraction;
	climate::land::elevationType _elevationType;

	typedef std::pair<my::Direction, Land*> LandNeighbor;
	std::map<my::Direction, Land*> _landNeighbors;

	SolidMixture _topSoil;
	static std::map<climate::land::elevationType, std::string> _elevationTextures;
public:
	Land();
	~Land();
	Land(double elevation, double localInitialTemperature);

	void bond(const LandNeighbor &neighbor);


	static void setupTextures(Graphics &graphics);


	bool draw(Graphics &graphics, std::vector<SDL_Rect> onScreenPositions, climate::DrawType drawType);

	double filterSolarRadiation(double incidentSolarEnergyKJ);
	double filterInfraredRadiation(double incidentInfraredEnergyKJ);
	double emitInfraredRadiation();

private:
	void determineElevationType();

public:
	//GETTERS
	//=================================
	bool isSubmerged() const;
	double getLandElevation() const;
	double getSurfaceTemperature() const;
	//double getSolarFraction() const;

	std::vector<std::string> getMessages(climate::DrawType messageType) const;
};
