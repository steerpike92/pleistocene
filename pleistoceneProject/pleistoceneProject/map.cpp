#include "map.h"
#include "graphics.h"
#include "noise.h"
#include "tile.h"

Map::Map(){}
Map::~Map() {}

Map::Map(Graphics &graphics, Bios *bios) {
	srand((unsigned int) time(NULL));//seed random number generation

	//build tiles in memory and calls setup functions
	Tile::setupTiles(graphics);

	Tile::_biosPtr = bios;//give tile class a static bios reference

	//Map generating algorithm
	int seed = 21;//starting with a determined seed gives a pseudorandom intial map
	generateMap(seed);
}


void Map::generateMap(int seed) {
	Tile::generateTileElevation(seed);
}

void Map::alterElevation(int deltaM) {
	//this->_totalElevationChange += deltaM;
	Tile::alterElevations(deltaM);
}

void Map::update(int elapsedTime) {
	Tile::updateTiles(elapsedTime);
}

void Map::simulate() {
	Tile::simulateTiles();
}

climate::DrawType Map::_drawType = climate::STANDARD_DRAW;

void Map::setDrawType(int drawNumber) {
	using namespace climate;
	switch (drawNumber) {
	case(2) : _drawType = SURFACE_TEMPERATURE_DRAW;
		break;
	case(3) : _drawType = SURFACE_AIR_TEMPERATURE_DRAW;
		break;
	default: _drawType = STANDARD_DRAW;
	}
}

climate::DrawType Map::getDrawType() {
	return _drawType;
}

void Map::draw(Graphics &graphics, bool cameraMovementFlag) {
	Tile::drawTiles(graphics, _drawType, cameraMovementFlag);
}