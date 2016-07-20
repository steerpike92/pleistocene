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
_selectedTile(nullptr),
_seed(9919),
_statisticsUpToDate(false)
{
	srand((unsigned int)time(NULL));//seed random number generation

	//build tiles in memory and calls setup functions
	setupTiles(graphics);
	
	//World generating algorithm
	generateWorld(options);
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

void World::generateWorld(const options::GameOptions &options) noexcept 
{
	generateTileElevations();
	setupTileClimateAdjacency();
}

void World::buildTileNeighbors() noexcept {
	for (Tile &T : _tiles) {
		T.buildNeighborhood();
	}
}


std::vector<double> World::buildNoiseTable(int Rows, int Cols) noexcept {

	noise::NoiseParameters noise_parameters;
	noise_parameters.octaves = 8;			//number of noise octaves. (each octave has twice the frequency of the previous octave, and (persistance) the amplitude
	noise_parameters.seed = _seed;			//seed for pseudorandom number generation
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
	double weightedAverage;

	int TileRows = my::Address::GetRows();
	int TileCols = my::Address::GetCols();

	double nonZeroHBlendDistance = std::max(double(hBlendDistance), 1.0);

	double weight1;//for weighted averages
	double weight2;

	for (int row = 0; row < TileRows; row++) {
		for (int col = 0; col < hBlendDistance; col++) {

			weight1 = double(col) / nonZeroHBlendDistance;
			weight2 = double(hBlendDistance - col) / nonZeroHBlendDistance;

			weightedAverage = 
				weight1 * noiseTable[row*Cols + col] +
				weight2 * noiseTable[row*Cols + col + TileCols];

			noiseTable[row*Cols + col] = weightedAverage;
		}
	}

	//blend north/south pole into water. Walking into a black barrier is kinda lame
	//this dramatically improves the feel of the map

	double nonZeroVBlendDistance = std::max(double(vBlendDistance), 1.0);

	for (int row = 0; row < vBlendDistance; row++) {
		for (int col = 0; col < TileCols; col++) {
			weight1 = double(row) / nonZeroVBlendDistance;
			weight2 = double(vBlendDistance - row) / nonZeroVBlendDistance;

			weightedAverage =  
				weight1 * noiseTable[row*Cols + col] +
				weight2 * (-.5 + .5*noiseTable[(row + vBlendDistance)*Cols + col]);

			noiseTable[row*Cols + col] = weightedAverage;
		}
	}

	for (int row = TileRows - vBlendDistance; row < TileRows; row++) {
		for (int col = 0; col < TileCols; col++) {
			weight1 = double(TileRows - row) / nonZeroVBlendDistance;
			weight2 = double(vBlendDistance + row - TileRows) / nonZeroVBlendDistance;

			weightedAverage = 
				weight1 * noiseTable[row*Cols + col] +
				weight2 *(-.5 + .5*noiseTable[(row - vBlendDistance)*Cols + col]);

			noiseTable[row*Cols + col] = weightedAverage;
		}
	}

	return noiseTable;

}

void World::generateTileElevations() noexcept {

	int TileRows = my::Address::GetRows();
	int TileCols = my::Address::GetCols();


	const int hBlendDistance = TileCols / 6;	//horizontal blend distance for east west map edge blending
	const int vBlendDistance = std::min(10, TileRows / 10);	//vertical blend distance for blending poles into sea
	int Cols = TileCols + hBlendDistance;					//columns needed in noise table
	int Rows = TileRows;							//rows needed in noise table




										//noise generator;
	std::vector<double> noiseTable = buildNoiseTable(Rows, Cols);

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

	_statisticsUpToDate = false;
}

void World::performStatistics() noexcept 
{
	_statistics.clear();

	double tileStatValue;

	for (Tile &tile : _tiles) {//must be reference as the tile stores and needs the result of getStatistic
		tileStatValue = tile.getStatistic(_statRequest);
		if (tileStatValue != my::kFakeDouble) {//don't contribute fake values.
			_statistics.contributeValue(tileStatValue);
		}
	}

	_statistics.calculateStatistics();
	_statisticsUpToDate = true;
}


void World::processInput(const Input & input, const options::GameOptions &options) noexcept
{
	//New map (resets all simulation data and generates new tile elevations with a random seed
	if (input.wasKeyPressed(SDL_SCANCODE_G)) {
		_seed = rand();
		LOG("Seed = " << _seed);
		generateWorld(options);
		my::SimulationTime::resetGlobalTime();
		_statistics.newStatistic();
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


	bool newStatistic = false;

	//draw type selection
	if (input.wasKeyPressed(SDL_SCANCODE_1)) { _statRequest._statType = ELEVATION; newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_2)) { _statRequest._statType = TEMPERATURE; newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_3)) { _statRequest._statType = MATERIAL_PROPERTIES; newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_4)) { _statRequest._statType = FLOW; newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_5)) { _statRequest._statType = MOISTURE; newStatistic = true;}
	//draw section selector
	if (input.wasKeyPressed(SDL_SCANCODE_6)) { _statRequest._section = SURFACE_; _statRequest._layer = 0; newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_7)) { _statRequest._section = HORIZON_; _statRequest._layer = 0;  newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_8)) { _statRequest._section = EARTH_; _statRequest._layer = 0;  newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_9)) { _statRequest._section = SEA_; _statRequest._layer = 0;  newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_0)) { _statRequest._section = AIR_; _statRequest._layer = 0;  newStatistic = true;}

	//layer selection
	if (input.wasKeyPressed(SDL_SCANCODE_LEFTBRACKET)) { _statRequest._layer--;  newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_RIGHTBRACKET)) { _statRequest._layer++;  newStatistic = true;}

	//return to default
	if (input.wasKeyPressed(SDL_SCANCODE_Q)) {
		_statRequest._section = SURFACE_;
		_statRequest._statType = ELEVATION; 
		_statRequest._layer = 0;  
		newStatistic = true;
	}

	////ascend above surface to air
	if (_statRequest._section == SURFACE_ && _statRequest._layer == 1) {
		_statRequest._section = AIR_; 
		_statRequest._layer = 0;
	}

	if (_statRequest._section == AIR_ && _statRequest._layer == -1) {
		_statRequest._section = SURFACE_;
		_statRequest._layer = 0;
	}

	if (_statRequest._section == AIR_ && _statRequest._layer == 6) {
		_statRequest._layer = 5;
	}

	if (_statRequest._section == SURFACE_ && _statRequest._layer == -1) {
		_statRequest._section = HORIZON_;
		_statRequest._layer = 0;
	}

	if (_statRequest._section == HORIZON_ && _statRequest._layer == 1) {
		_statRequest._section = SURFACE_;
		_statRequest._layer = 0;
	}


	if (_statRequest._layer < 0)  _statRequest._layer = 0;
	if (_statRequest._layer > 6)  _statRequest._layer = 6;

	
	if (newStatistic) {
		_statistics.newStatistic();
		_statisticsUpToDate = false;
	}
	
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

	if (!_statisticsUpToDate) {
		performStatistics();
	}


	if (_statRequest._statType == ELEVATION && _statRequest._section == SURFACE_) {
		for (Tile &tile : _tiles) {
			if (tile.elevationDraw(graphics, cameraMovementFlag, options._sunlit)) {
				_selectedTile = &tile;
			}
		}
	}

	else {
		for (Tile &tile : _tiles) {
			if (tile.statDraw(graphics, cameraMovementFlag, _statistics)) {
				_selectedTile = &tile;
			}
		}
	}

	if (_selectedTile) {
		std::vector<SDL_Rect> onscreenPositions = graphics.getOnscreenPositions(&_selectedTile->getGameRect());
		if (onscreenPositions.empty()) {
			return;
		}
		graphics.blitTexture("../../content/simpleTerrain/blackOutline.png", NULL, onscreenPositions);
	}
}

std::vector<std::string> World::getMessages() const noexcept 
{
	if (_selectedTile != nullptr) {
		double sigmas=_statistics.getSigmasOffMean(_selectedTile->getStatistic(_statRequest));

		std::stringstream stream;
		stream << "Sigmas off mean: " << my::double2string(sigmas);
		std::vector<std::string> messages;
		messages.push_back(stream.str());

		std::vector<std::string> subMessages;
		subMessages= _selectedTile->sendMessages(_statRequest);
		messages.insert(messages.end(), subMessages.begin(), subMessages.end());
		return messages;
	}
	else return std::vector<std::string> {};
}

void World::clearSelected() noexcept { _selectedTile = nullptr; }

std::vector<std::string> World::getReadout() const noexcept
{
	std::vector<std::string> readout;

	std::string statement;
	std::stringstream stream;

	switch (_statRequest._section) {
	case(SURFACE_) : stream<<"Surface "; break;
	case(HORIZON_) : stream<< "Horizon "; break;
	case(EARTH_) : stream<< "Earth "; break;
	case(SEA_) : stream<< "Sea "; break;
	case(AIR_) : stream<< "Air "; break;
	}

	switch (_statRequest._statType) {
	case(ELEVATION) : stream<< "elevation. "; break;
	case(TEMPERATURE) : stream<< "temperature. "; break;
	case(MATERIAL_PROPERTIES) : stream<< "material properties. "; break;
	case(FLOW) : stream<< "flow. "; break;
	case(MOISTURE) : stream<< "moisture. "; break;
	}

	readout.push_back(stream.str());

	stream.str(std::string());
	stream << "Layer: " << _statRequest._layer << ". ";
	
	readout.push_back(stream.str());

	stream.str(std::string());
	stream << "Seed: "<< _seed<< ". ";

	readout.push_back(stream.str());

	std::vector<std::string> subReadout = _statistics.getMessages();

	readout.insert(readout.end(), subReadout.begin(), subReadout.end());

	return readout;
}


}//namespace simulation
}//namespace pleistocene