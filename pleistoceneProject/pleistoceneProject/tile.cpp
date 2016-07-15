#include "tile.h"
#include "graphics.h"
#include "map.h"
#include "bios.h"

namespace pleistocene {

Tile::Tile() noexcept {}

Tile::Tile(my::Address tileAddress) noexcept {

	_Address = tileAddress;

	_gameRectangle.x = _Address.getGamePos().x;
	_gameRectangle.y = _Address.getGamePos().y;
	_gameRectangle.w = globals::TILE_WIDTH;
	_gameRectangle.h = globals::TILE_HEIGHT;


	my::Vector2d latLonDeg = _Address.getLatLonDeg();

	_latitude_deg = latLonDeg.x;
	_longitude_deg = latLonDeg.y;
}


std::vector<Tile> Tile::_tiles;

std::vector<my::Address> Tile::_Addresses;


void Tile::setupTiles(graphics::Graphics &graphics) noexcept {
	buildTileVector();
	setupTextures(graphics);
	buildTileNeighbors();
}

//initializes each tile at a default depth
void Tile::buildTileVector() noexcept {
	//Tile constructor
	for (int row = 0; row < my::Address::GetRows(); row++) {
		for (int col = 0; col < my::Address::GetCols(); col++) {
			_tiles.emplace_back(my::Address(row, col));
			_Addresses.emplace_back(row, col);
			if (_Addresses.back().i == my::FakeIndex) {
				LOG("not a valid Address");
			}
		}
	}
}

void Tile::buildTileNeighbors() noexcept {
	for (Tile &T : _tiles) {
		T.buildNeighborhood();
	}
}

void Tile::buildNeighborhood() noexcept {
	my::Address neighborAddress;
	for (int j = 0; j < 6; j++) {
		neighborAddress = this->_Address.adjacent(j);
		if (neighborAddress.i != -1) {
			this->_directionalNeighbors[static_cast<my::Direction>(j)] = neighborAddress;
		}
	}
}

user_interface::Bios* Tile::_biosPtr;

std::vector<double> Tile::buildNoiseTable(int Rows, int Cols, int seed) noexcept {

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

std::vector<double> Tile::blendNoiseTable(std::vector<double> noiseTable, int Rows, int Cols, int vBlendDistance, int  hBlendDistance) noexcept {

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

void Tile::generateTileElevation(int seed) noexcept {

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
			//NOEXCEPT if (A.i == my::FakeIndex) {LOG("address index out of bounds");throw (2);}

			//set elevation
			double elevation = noiseValue * climate::land::amplitude;
			_tiles[A.i]._tileClimate = climate::TileClimate(A, elevation);
		}
	}
}


void Tile::setupTileClimateAdjacency() noexcept {

	std::map<my::Direction, climate::TileClimate*>		adjacientTileClimates;
	my::Direction						direction;
	climate::TileClimate					*climatePtr;

	for (Tile &tile : _tiles) {
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


void Tile::updateTiles(int elapsedTime) noexcept {
	for (Tile &T : _tiles) {
		T.update(elapsedTime);
	}
}

void Tile::update(int elapsedTime) noexcept {

}

//SIMULATION
//============================

void Tile::simulateTiles() noexcept {

	climate::TileClimate::beginNewHour();

	while (climate::TileClimate::beginNextStep()) {
		for (Tile &tile : _tiles) {
			tile.simulate();
		}
	}
}

void Tile::simulate() noexcept {
	_tileClimate.simulateClimate();
}


//GRAPHICS
//=======================

void Tile::setupTextures(graphics::Graphics &graphics) noexcept {

	climate::TileClimate::setupTextures(graphics);

	//selection graphics
	graphics.loadImage("../../content/simpleTerrain/whiteOutline.png");
	graphics.loadImage("../../content/simpleTerrain/blackOutline.png");
}

void Tile::drawTiles(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options) noexcept {
	for (Tile &tile : _tiles) {
		tile.draw(graphics, cameraMovementFlag, options);
	}
}

void Tile::draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options) noexcept {
	_gameRectangle.x = (globals::TILE_WIDTH / 2) * (_Address.r % 2) + globals::TILE_WIDTH * _Address.c;
	_gameRectangle.w = globals::TILE_WIDTH;
	_gameRectangle.y = _Address.r * globals::EFFECTIVE_HEIGHT;
	_gameRectangle.h = globals::TILE_WIDTH;

	bool selectionValue = false;//selection flag

	if (cameraMovementFlag) {//only update positions if camera has moved
		_onScreenPositions = graphics.getOnScreenPositions(&_gameRectangle);
	}

	if (_onScreenPositions.empty()) {
		return;
	}

	//draw and check selection
	selectionValue = _tileClimate.drawClimate(graphics, _onScreenPositions, options);
	if (selectionValue) { _biosPtr->selectTile(this); }
}


//GETTERS
//=================

my::Address Tile::getAddress() const noexcept { return _Address; }

SDL_Rect Tile::getGameRect() const noexcept { return _gameRectangle; }

std::vector<std::string> Tile::sendMessages(const options::GameOptions &options) const noexcept {

	

	std::vector<std::string> messages;

	std::stringstream stream;
	stream << "(Lat,Lon): (" << int(this->_latitude_deg) << "," << int(this->_longitude_deg) << ")";
	messages.push_back(stream.str());

	std::vector<std::string> climateMessages = _tileClimate.getMessages(options);

	for (std::string &str : climateMessages) {
		messages.push_back(str);
	}

	return messages;
}

}//namespace pleistocene
