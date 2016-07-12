#pragma once
#include "globals.h"


class Tile;
class Graphics;
class Bios;

class Map {
public:
	Map();
	Map(Graphics &graphics, Bios *bios);

	void generateMap(int seed);

	void draw(Graphics &graphics, bool cameraMovementFlag);
	void update(int elapsedTime);

	void simulate();

	void alterElevation(int deltaM);

	Bios* _bioPtr;

	static climate::DrawType getDrawType();
	void setDrawType(int drawNumber);
private:

	static climate::DrawType _drawType;

	//tracks sea level rise and fall
	int _totalElevationChange = 0;
};
