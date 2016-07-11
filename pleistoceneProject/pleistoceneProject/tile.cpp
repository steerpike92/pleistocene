#include "tile.h"
#include "graphics.h"
#include "map.h"
#include "bios.h"
#include "noise.h"

Tile::Tile() {}
Tile::~Tile() {}

Tile::Tile(my::Address tileAddress, double elevation, Graphics &graphics) {

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

void Tile::setupTiles(Graphics &graphics) {
	buildTileVector(graphics);
	setupTextures(graphics);
	buildTileNeighbors();
}

void Tile::buildTileVector(Graphics &graphics) {
	//Tile constructor
	for (int row = 0; row < globals::TILE_ROWS; row++) {
		for (int col = 0; col < globals::TILE_COLUMNS; col++) {
			_tiles.emplace_back(my::Address(row, col), climate::land::defaultDepth, graphics);
			_Addresses.emplace_back(row, col);
			if (_Addresses.back().i == my::FakeIndex) {
				LOG("NOT A VALID Address");
			}
		}
	}
}

void Tile::buildTileNeighbors() {
	for (Tile &T : _tiles) {
		T.buildNeighborhood();
	}
}


std::vector<double> Tile::buildNoiseTable(int seed, double zoom, double persistance, int octaves, int Rows, int Cols){

	std::vector<double> noiseTable;

	//helps determine noise amplitude (this is a dummy value to be updated)
	double maximum = 0.01;

	double X;//x position noise variable
	double Y;//y position noise variable

	my::Address A;//spurious my::Address

	//Build noise table
	for (int row = 0; row<Rows; row++) {
		for (int col = 0; col<Cols; col++) {

			bool spurious = true;
			A = my::Address(row, col, spurious);
			double getNoise = 0;

			//loop through octaves
			for (int a = 0; a<octaves - 1; a++)
			{
				//double frequency with each octave.
				double frequency = pow(2, a);

				//scale down amplitude with each octave.
				double amplitude = pow(persistance, a);

				X = double(A.getGamePos().x)*frequency / zoom;
				Y = double(A.getGamePos().y)*frequency / zoom;

				//call noise function at (X,Y)
				getNoise += noise2(X, Y, seed)*amplitude;

			}//end octave loop

			 //update noiseTable
			noiseTable.emplace_back(getNoise);

			//update maximum
			if (abs(getNoise) > maximum) maximum = abs(getNoise);

		}
	}


	//Rescale with maximum so ranges in noiseTable are from -1 to 1. 
	for (int row = 0; row < Rows; row++) {
		for (int col = 0; col < Cols; col++) {
			noiseTable[row*Cols + col] /= maximum;
		}
	}
	
	return noiseTable;
}

std::vector<double> Tile::blendNoiseTable(std::vector<double> noiseTable, int Rows, int Cols, int vBlendDistance, int  hBlendDistance) {

	//blend map east/west edge together
	double blend;

	for (int row = 0; row < globals::TILE_ROWS; row++) {
		for (int col = 0; col < hBlendDistance; col++) {
			blend = (col / std::max(double(hBlendDistance), 1.0)) * noiseTable[row*Cols + col] +
				((double(hBlendDistance) - col) / std::max(double(hBlendDistance), 1.0))*noiseTable[row*Cols + col + globals::TILE_COLUMNS];

			noiseTable[row*Cols + col] = blend;
		}
	}

	//blend north/south pole into water. Walking into a black barrier is kinda lame

	for (int row = 0; row < vBlendDistance; row++) {
		for (int col = 0; col < globals::TILE_COLUMNS; col++) {
			blend = (row / std::max(double(vBlendDistance), 1.0)) * noiseTable[row*Cols + col] +
				(double(vBlendDistance - row) / std::max(double(vBlendDistance), 1.0))*(-.5 + .5*noiseTable[(row + vBlendDistance)*Cols + col]);

			noiseTable[row*Cols + col] = blend;
		}
	}

	for (int row = globals::TILE_ROWS - vBlendDistance; row < globals::TILE_ROWS; row++) {
		for (int col = 0; col < globals::TILE_COLUMNS; col++) {
			blend = (double(globals::TILE_ROWS - row) / std::max(double(vBlendDistance), 1.0)) * noiseTable[row*Cols + col] +
				(double(vBlendDistance + row - globals::TILE_ROWS) / std::max(double(vBlendDistance), 1.0))*(-.5 + .5*noiseTable[(row - vBlendDistance)*Cols + col]);

			noiseTable[row*Cols + col] = blend;
		}
	}

	return noiseTable;

}


void Tile::generateTileElevation(int seed) {

	//Noise building parameters
	double zoom = 4000;
	double persistance = .55;
	int octaves = 8;
	const int hBlendDistance = globals::TILE_COLUMNS / 10;
	const int vBlendDistance = std::min(10,globals::TILE_ROWS/10);
	int Cols = globals::TILE_COLUMNS + hBlendDistance;
	int Rows = globals::TILE_ROWS;

	//NOISE GENERATOR
	//===================
	std::vector<double> noiseTable = buildNoiseTable(seed, zoom, persistance, octaves, Rows, Cols);
	noiseTable = blendNoiseTable(noiseTable, Rows, Cols, vBlendDistance, hBlendDistance);

	//SET ELEVATION
	//=====================

	double shelfPower = 1.5;
	double slopePower = 1;
	double abyssPower = .5;

	double landPower = 2;

	my::Address A;
	double noiseValue;

	for (int row = 0; row < globals::TILE_ROWS; row++) {
		for (int col = 0; col < globals::TILE_COLUMNS; col++) {
			noiseValue = noiseTable[row*Cols + col];
			if (noiseValue>0) {
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

			//Final elevation set
			A = my::Address(row, col);
			if (A.i == my::FakeIndex) {
				LOG("waht happened?");
				throw (2);
			}

			double elevation = noiseValue * climate::land::amplitude;
			_tiles[A.i]._tileClimate = TileClimate(A, elevation);
		}
	}
}

void Tile::alterElevations(int deltaM) {
	//
	//	for (my::Address A : _Addresses) {
	//		_tiles[A.i].alterElevation(deltaM);
	//	}
	//
	//	calculateTileFlows();
}

void Tile::alterElevation(int deltaM) {
	//
	//	setElevation(_elevation + deltaM);
	//
}

Bios* Tile::_biosPtr;

void Tile::setupTextures(Graphics &graphics) {

	TileClimate::setupTextures(graphics);

	//selection graphics
	graphics.loadImage("../../content/simpleTerrain/whiteOutline.png");
	graphics.loadImage("../../content/simpleTerrain/blackOutline.png");
}


void Tile::updateTiles(int elapsedTime) {
	for (Tile &T : _tiles) {
		T.update(elapsedTime);
	}
}

void Tile::update(int elapsedTime) {

}

void Tile::simulateTiles() {

	TileClimate::beginNewHour();

	while (TileClimate::beginNextStep()) {
		for (Tile &tile : _tiles) {
			tile.simulate();
		}
	}
}

void Tile::simulate() { _tileClimate.simulateClimate(); }

void Tile::drawTiles(Graphics &graphics, climate::DrawType drawType, bool cameraMovementFlag) {
	for (Tile &tile : _tiles) {
		tile.draw(graphics, drawType, cameraMovementFlag);
	}
}

void Tile::draw(Graphics &graphics, climate::DrawType drawType, bool cameraMovementFlag) {
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
	selectionValue=_tileClimate.drawClimate(graphics, _onScreenPositions, drawType);
	if (selectionValue) {_biosPtr->selectTile(this);}
}

my::Address Tile::getAddress()const {return _Address;}

SDL_Rect Tile::getGameRect() const {return _gameRectangle;}

std::vector<std::string> Tile::sendMessages() const {

	climate::DrawType messageType = Map::getDrawType();

	std::vector<std::string> messages;

	std::stringstream stream;
	stream << "(Lat,Lon): (" << int(this->_latitude_deg) << "," << int(this->_longitude_deg) << ")";
	messages.push_back(stream.str());

	std::vector<std::string> climateMessages=_tileClimate.getMessages(messageType);

	for (std::string &str : climateMessages) {
		messages.push_back(str);
	}

	return messages;
}


void Tile::buildNeighborhood() {
	my::Address neighborAddress;
	for (int j = 0; j < 6; j++) {
		neighborAddress = this->_Address.adjacent(j);
		if (neighborAddress.i != -1) {
			this->_directionalNeighbors[static_cast<my::Direction>(j)] = neighborAddress;
		}
	}
}