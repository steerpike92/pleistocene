#pragma once
#include "globals.h"

class Tile;
class Graphics;
class Bios;
class GameOptions;

class Map {
public:
	Map();
	Map(Graphics &graphics, Bios *bios, GameOptions &options);

	void generateMap(int seed);

	void draw(Graphics &graphics, bool cameraMovementFlag);
	void update(int elapsedTime);

	void simulate();

	Bios* _bioPtr;

	bool _exists = false;

	static climate::DrawType getDrawType();
	void setDrawType(int drawNumber);
private:

	static climate::DrawType _drawType;

	//tracks sea level rise and fall
	int _totalElevationChange = 0;
};
