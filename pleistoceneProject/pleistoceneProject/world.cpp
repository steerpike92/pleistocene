#include "world.h"
#include "game-options.h"
#include "bios.h"
#include "graphics.h"
#include "noise.h"
#include "input.h"

namespace pleistocene {
namespace simulation {

World::World() noexcept {}


World::World(graphics::Graphics &graphics, const options::GameOptions &options) noexcept :
_statRequest(StatRequest()),
_selectedTile(nullptr)
{
	srand((unsigned int)time(NULL));//seed random number generation

	//build tiles in memory and calls setup functions
	setupTiles(graphics);
	
	//World generating algorithm
	int seed = 21;//starting with a determined seed gives a pseudorandom intial map
	generateWorld(seed, options);
}

void World::setupTiles(graphics::Graphics &graphics) noexcept {
	buildTileVector();
	setupTextures(graphics);
	buildTileNeighbors();
}

//initializes each tile at a default depth
void World::buildTileVector() noexcept {
	//Tile constructor
	for (int row = 0; row < my::Address::GetRows(); row++) {
		for (int col = 0; col < my::Address::GetCols(); col++) {
			_tiles.emplace_back(my::Address(row, col));
			if (my::Address(row, col).i == my::kFakeIndex) { LOG("not a valid Address");  exit(EXIT_FAILURE); }
		}
	}
}

void World::generateWorld(int seed, const options::GameOptions &options) noexcept 
{
	generateTileElevations(seed);
	setupTileClimateAdjacency();
}

void World::buildTileNeighbors() noexcept {
	for (Tile &T : _tiles) {
		T.buildNeighborhood();
	}
}


std::vector<double> World::buildNoiseTable(int Rows, int Cols, int seed) noexcept {

	noise::NoiseParameters noise_parameters;
	noise_parameters.octaves = 8;			//number of noise octaves. (each octave has twice the frequency of the previous octave, and (persistance) the amplitude
	noise_parameters.seed = seed;			//seed for pseudorandom number generation
	noise_parameters.zoom = 4000;			//determines wavelength of first octave
	noise_parameters.persistance = 0.55;		//amplitude lost acending each octave

							//create vector containing position pairs for each tile
	std::vector<std::pair<double, double>> positions;

	my::Address A;
	my::Vector2 V;
	std::pair<double, double> position;

	for (int row = 0; row < Rows; row++) {
		for (int col = 0; col < Cols; col++) {
			A = my::Address(row, col, true);
			V = A.getGamePos();
			position.first = V.x;
			position.second = V.y;
			positions.push_back(position);
		}
	}

	//return noise for each position
	return noise::PerlinNoise(positions, noise_parameters);
}

std::vector<double> World::blendNoiseTable(std::vector<double> noiseTable, int Rows, int Cols, int vBlendDistance, int  hBlendDistance) noexcept {

	//blend map east/west edge together
	double blend;

	int TileRows = my::Address::GetRows();
	int TileCols = my::Address::GetCols();

	for (int row = 0; row < TileRows; row++) {
		for (int col = 0; col < hBlendDistance; col++) {
			blend = (col / std::max(double(hBlendDistance), 1.0)) * noiseTable[row*Cols + col] +
				((double(hBlendDistance) - col) / std::max(double(hBlendDistance), 1.0))*noiseTable[row*Cols + col + TileCols];

			noiseTable[row*Cols + col] = blend;
		}
	}

	//blend north/south pole into water. Walking into a black barrier is kinda lame

	for (int row = 0; row < vBlendDistance; row++) {
		for (int col = 0; col < TileCols; col++) {
			blend = (row / std::max(double(vBlendDistance), 1.0)) * noiseTable[row*Cols + col] +
				(double(vBlendDistance - row) / std::max(double(vBlendDistance), 1.0))*(-.5 + .5*noiseTable[(row + vBlendDistance)*Cols + col]);

			noiseTable[row*Cols + col] = blend;
		}
	}

	for (int row = TileRows - vBlendDistance; row < TileRows; row++) {
		for (int col = 0; col < TileCols; col++) {
			blend = (double(TileRows - row) / std::max(double(vBlendDistance), 1.0)) * noiseTable[row*Cols + col] +
				(double(vBlendDistance + row - TileRows) / std::max(double(vBlendDistance), 1.0))*(-.5 + .5*noiseTable[(row - vBlendDistance)*Cols + col]);

			noiseTable[row*Cols + col] = blend;
		}
	}

	return noiseTable;

}

void World::generateTileElevations(int seed) noexcept {

	int TileRows = my::Address::GetRows();
	int TileCols = my::Address::GetCols();


	const int		hBlendDistance = TileCols / 10;			//horizontal blend distance for east west map edge blending
	const int		vBlendDistance = std::min(10, TileRows / 10);	//vertical blend distance for blending poles into sea
	int Cols = TileCols + hBlendDistance;					//columns needed in noise table
	int Rows = TileRows;							//rows needed in noise table




										//noise generator;
	std::vector<double> noiseTable = buildNoiseTable(Rows, Cols, seed);

	//noise edge blender
	noiseTable = blendNoiseTable(noiseTable, Rows, Cols, vBlendDistance, hBlendDistance);



	//Elevation shape parameters
	const double shelfPower = 1.5;
	const double slopePower = 1;
	const double abyssPower = .5;
	const double landPower = 2;

	my::Address A;
	double noiseValue;

	//SET ELEVATION
	for (int row = 0; row < TileRows; row++) {
		for (int col = 0; col < TileCols; col++) {
			noiseValue = noiseTable[row*Cols + col];
			if (noiseValue > 0) {
				noiseValue = pow(noiseValue, landPower);
			}
			else {
				if (noiseValue > -.15) {
					noiseValue = -pow(abs(noiseValue), shelfPower);
				}
				else if (noiseValue > -.3) {
					noiseValue = -pow(abs(noiseValue), slopePower);
				}
				else {
					noiseValue = -pow(abs(noiseValue), abyssPower);
				}
			}

			//determine tile to set
			A = my::Address(row, col);
			if (A.i == my::kFakeIndex) { LOG("address index out of bounds"); exit(EXIT_FAILURE); }

			//Finally initialize TileClimate
			_tiles[A.i]._tileClimate = climate::TileClimate(A, noiseValue);
		}
	}
}


void World::setupTileClimateAdjacency() noexcept {

	std::map<my::Direction, climate::TileClimate*>		adjacientTileClimates;
	my::Direction						direction;
	climate::TileClimate					*climatePtr;

	for (Tile & tile : _tiles) {
		//build adjacient climate map for tile
		for (auto neighbor : tile._directionalNeighbors) {
			direction = neighbor.first;
			climatePtr = &(_tiles[neighbor.second.i]._tileClimate);//grab neighbor _tileClimate ptr
			adjacientTileClimates[direction] = climatePtr;
		}

		//pass map to tile's tileClimate
		tile._tileClimate.buildAdjacency(adjacientTileClimates);

		//clear and restart for next tile
		adjacientTileClimates.clear();
	}
}


void World::simulate(const options::GameOptions &options) noexcept 
{
	climate::TileClimate::beginNewHour();

	while (climate::TileClimate::beginNextStep()) {
		for (Tile &tile : _tiles) {
			tile.simulate();
		}
	}

}

void World::processInput(const Input & input, const options::GameOptions &options) noexcept
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


	//draw type selection
	if (input.wasKeyPressed(SDL_SCANCODE_1)) { _statRequest._statType = ELEVATION; }
	if (input.wasKeyPressed(SDL_SCANCODE_2)) { _statRequest._statType = TEMPERATURE; }
	if (input.wasKeyPressed(SDL_SCANCODE_3)) { _statRequest._statType = MATERIAL_PROPERTIES;  }
	if (input.wasKeyPressed(SDL_SCANCODE_4)) { _statRequest._statType = FLOW; }
	if (input.wasKeyPressed(SDL_SCANCODE_5)) { _statRequest._statType = MOISTURE; }
	//draw section selector
	if (input.wasKeyPressed(SDL_SCANCODE_6)) { _statRequest._section = SURFACE_; _statRequest._layer = 0; }
	if (input.wasKeyPressed(SDL_SCANCODE_7)) { _statRequest._section = HORIZON_; _statRequest._layer = 0;  }
	if (input.wasKeyPressed(SDL_SCANCODE_8)) { _statRequest._section = EARTH_; _statRequest._layer = 0; }
	if (input.wasKeyPressed(SDL_SCANCODE_9)) { _statRequest._section = SEA_; _statRequest._layer = 0; }
	if (input.wasKeyPressed(SDL_SCANCODE_0)) { _statRequest._section = AIR_; _statRequest._layer = 0; }

	//layer selection
	if (input.wasKeyPressed(SDL_SCANCODE_LEFTBRACKET)) { _statRequest._layer--; }
	if (input.wasKeyPressed(SDL_SCANCODE_RIGHTBRACKET)) { _statRequest._layer++; }
	_statRequest._layer %= 8;

	
}

void World::setupTextures(graphics::Graphics &graphics) noexcept {

	climate::TileClimate::setupTextures(graphics);

	//selection graphics
	graphics.loadImage("../../content/simpleTerrain/whiteOutline.png");
	graphics.loadImage("../../content/simpleTerrain/blackOutline.png");

	Tile::setupTextures(graphics);

}

void World::draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options, user_interface::Bios &bios) noexcept 
{
	if (_statRequest._statType == ELEVATION && _statRequest._section == SURFACE_) {
		for (Tile &tile : _tiles) {
			if (tile.elevationDraw(graphics, cameraMovementFlag, options._sunlit)) {
				_selectedTile = &tile;
			}
		}
	}

	else {
		_statistics.clear();
		double tileStatValue;

		for (Tile &tile : _tiles) {
			tileStatValue = tile.getStatistic(_statRequest);
			_statistics.contributeValue(tileStatValue);
		}

		_statistics.calculateStatistics();

		for (Tile &tile : _tiles) {
			if (tile.statDraw(graphics, cameraMovementFlag)) {
				_selectedTile = &tile;
			}
		}
	}

	if (_selectedTile) {
		std::vector<SDL_Rect> onscreenPositions = graphics.getOnscreenPositions(&_selectedTile->getGameRect());
		if (onscreenPositions.empty()) {
			return;
		}
		graphics.blitSurface("../../content/simpleTerrain/blackOutline.png", NULL, onscreenPositions);
	}

}

std::vector<std::string> World::getMessages() const noexcept 
{
	if (_selectedTile != nullptr) {
		return _selectedTile->sendMessages(_statRequest);
	}
	else return std::vector<std::string> {};
}

void World::clearSelected() noexcept { _selectedTile = nullptr; }

std::vector<std::string> World::getReadout() const noexcept
{
	std::vector<std::string> readout;

	std::string word;

	switch (_statRequest._section) {
	case(SURFACE_) : word="SURFACE "; break;
	case(HORIZON_) :  word = "HORIZON "; break;
	case(EARTH_) :  word = "EARTH "; break;
	case(SEA_) :  word = "SEA "; break;
	case(AIR_) :   word = "AIR "; break;
	}

	readout.push_back(word);

	switch (_statRequest._statType) {
	case(ELEVATION) : word = "ELEVATION "; break;
	case(TEMPERATURE) :  word = "TEMPERATURE "; break;
	case(MATERIAL_PROPERTIES) :  word = "MATERIAL PROPERTIES "; break;
	case(FLOW) : word = "FLOW "; break;
	case(MOISTURE) : word = "MOISTURE "; break;
	}

	readout.push_back(word);
	return readout;
}


}//namespace simulation
}//namespace pleistocene