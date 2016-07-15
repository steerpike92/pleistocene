#pragma once
#include "globals.h"

namespace pleistocene {
class Tile;
class Graphics;
class Bios;
class GameOptions;

class Map {
public:
	Map() noexcept;
	Map(Graphics &graphics, Bios *bios, GameOptions &options) noexcept;

	void generateMap(int seed) noexcept;

	void draw(Graphics &graphics, bool cameraMovementFlag) noexcept;
	void update(int elapsedTime) noexcept;

	void simulate() noexcept;

	Bios* _bioPtr;

	bool _exists = false;

	static climate::DrawType getDrawType() noexcept;
	void setDrawType(int drawNumber) noexcept;
private:

	static climate::DrawType _drawType;

	//tracks sea level rise and fall
	int _totalElevationChange = 0;
};
}//namespace pleistocene