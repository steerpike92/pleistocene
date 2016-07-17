#include "world.h"
#include "game-options.h"
#include "graphics.h"
#include "noise.h"
#include "tile.h"
#include "input.h"

namespace pleistocene {
namespace simulation {

World::World() noexcept {}


World::World(graphics::Graphics &graphics, user_interface::Bios *bios, const options::GameOptions &options) noexcept 
{
	srand((unsigned int)time(NULL));//seed random number generation

	//build tiles in memory and calls setup functions
	Tile::setupTiles(graphics);

	_exists = true;

	Tile::_biosPtr = bios;//give tile class a static bios reference

	//World generating algorithm
	int seed = 21;//starting with a determined seed gives a pseudorandom intial map
	generateWorld(seed, options);
}


void World::generateWorld(int seed, const options::GameOptions &options) noexcept 
{
	Tile::generateTileElevation(seed);
	Tile::setupTileClimateAdjacency();
}


void World::update(int elapsedTime) noexcept 
{
	Tile::updateTiles(elapsedTime);
}

void World::simulate(const options::GameOptions & options) noexcept 
{
	Tile::simulateTiles();
}

void World::processInput(const Input & input, const options::GameOptions & options) noexcept
{
	//New map (resets all simulation data and generates new tile elevations with a random seed
	if (input.wasKeyPressed(SDL_SCANCODE_G)) {
		generateWorld(rand(), options);
		my::SimulationTime::resetGlobalTime();
		simulate(options);
	}

	//simulation
	if (input.wasKeyPressed(SDL_SCANCODE_RETURN) ||	//Press enter for one hour of simulation
		input.wasKeyHeld(SDL_SCANCODE_BACKSLASH) ||	//Hold backslash for continuous simulation
		options._continuousSimulation)			//Press spacebar to toggle continuous simulation
	{
		my::SimulationTime::updateGlobalTime();
		simulate(options);
	}

}


void World::draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options) noexcept {
	Tile::drawTiles(graphics, cameraMovementFlag, options);
}

}//namespace simulation
}//namespace pleistocene