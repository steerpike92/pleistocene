#include "map.h"
#include "gameOptions.h"
#include "graphics.h"
#include "noise.h"
#include "tile.h"

namespace pleistocene {

Map::Map() noexcept {}

Map::Map(graphics::Graphics &graphics, user_interface::Bios *bios, const options::GameOptions &options) noexcept {
	srand((unsigned int)time(NULL));//seed random number generation

	//build tiles in memory and calls setup functions
	Tile::setupTiles(graphics);

	_exists = true;

	Tile::_biosPtr = bios;//give tile class a static bios reference

	//Map generating algorithm
	int seed = 21;//starting with a determined seed gives a pseudorandom intial map
	generateMap(seed, options);
}


void Map::generateMap(int seed, const options::GameOptions &options) noexcept {
	Tile::generateTileElevation(seed);
	Tile::setupTileClimateAdjacency();
}


void Map::update(int elapsedTime) noexcept {
	Tile::updateTiles(elapsedTime);
}

void Map::simulate(const options::GameOptions &options) noexcept {

	Tile::simulateTiles();
}


void Map::draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options) noexcept {
	Tile::drawTiles(graphics, cameraMovementFlag, options);
}
}//namespace pleistocene