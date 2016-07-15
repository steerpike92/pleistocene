#include "map.h"
#include "gameOptions.h"
#include "graphics.h"
#include "noise.h"
#include "tile.h"

namespace pleistocene {

Map::Map() noexcept {}

Map::Map(graphics::Graphics &graphics, user_interface::Bios *bios, options::GameOptions &options) noexcept {
	srand((unsigned int)time(NULL));//seed random number generation

	//build tiles in memory and calls setup functions
	Tile::setupTiles(graphics);

	_exists = true;

	Tile::_biosPtr = bios;//give tile class a static bios reference

	//Map generating algorithm
	int seed = 21;//starting with a determined seed gives a pseudorandom intial map
	generateMap(seed);
}


void Map::generateMap(int seed) noexcept {
	Tile::generateTileElevation(seed);
	Tile::setupTileClimateAdjacency();
}


void Map::update(int elapsedTime) noexcept {
	Tile::updateTiles(elapsedTime);
}

void Map::simulate() noexcept {
	Tile::simulateTiles();
}

climate::DrawType Map::_drawType = climate::STANDARD_DRAW;

void Map::setDrawType(int drawNumber)  noexcept {
	using namespace climate;
	switch (drawNumber) {
	case(1) : _drawType = STANDARD_DRAW;
		break;
	case(2) : _drawType = SURFACE_TEMPERATURE_DRAW;
		break;
	case(3) : _drawType = SURFACE_AIR_TEMPERATURE_DRAW;
		break;
	default: _drawType = STANDARD_DRAW;
	}
}

climate::DrawType Map::getDrawType() noexcept {
	return _drawType;
}

void Map::draw(graphics::Graphics &graphics, bool cameraMovementFlag) noexcept {
	Tile::drawTiles(graphics, _drawType, cameraMovementFlag);
}
}//namespace pleistocene