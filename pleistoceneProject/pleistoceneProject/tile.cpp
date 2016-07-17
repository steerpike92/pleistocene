#include "tile.h"
#include "graphics.h"
#include "world.h"
#include "bios.h"

namespace pleistocene {
namespace simulation {

Tile::Tile() noexcept {}

Tile::Tile(my::Address tileAddress) noexcept {

	_address = tileAddress;

	_gameRectangle.x = _address.getGamePos().x;
	_gameRectangle.y = _address.getGamePos().y;
	_gameRectangle.w = globals::kTileWidth;
	_gameRectangle.h = globals::kTileHeight;


	my::Vector2d latLonDeg = _address.getLatLonDeg();

	_latitude_deg = latLonDeg.x;
	_longitude_deg = latLonDeg.y;
}


void Tile::buildNeighborhood() noexcept {
	my::Address neighborAddress;
	for (int j = 0; j < 6; j++) {
		neighborAddress = this->_address.adjacent(j);
		if (neighborAddress.i != -1) {
			this->_directionalNeighbors[static_cast<my::Direction>(j)] = neighborAddress;
		}
	}
}



void Tile::update(int elapsedTime) noexcept {

}

//SIMULATION
//============================


void Tile::simulate() noexcept {
	_tileClimate.simulateClimate();
}


//GRAPHICS
//=======================


bool Tile::draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options) noexcept {
	_gameRectangle.x = (globals::kTileWidth / 2) * (_address.r % 2) + globals::kTileWidth * _address.c;
	_gameRectangle.w = globals::kTileWidth;
	_gameRectangle.y = _address.r * globals::kEffectiveTileHeight;
	_gameRectangle.h = globals::kTileWidth;

	bool selectionBool = false;//selection flag

	if (cameraMovementFlag) {//only update positions if camera has moved
		_onScreenPositions = graphics.getOnScreenPositions(&_gameRectangle);
	}

	if (_onScreenPositions.empty()) {
		return false;
	}

	//draw and check selection
	selectionBool = _tileClimate.drawClimate(graphics, _onScreenPositions);
	return selectionBool;
}


//GETTERS
//=================

my::Address Tile::getAddress() const noexcept { return _address; }

SDL_Rect Tile::getGameRect() const noexcept { return _gameRectangle; }

std::vector<std::string> Tile::sendMessages(const StatRequest &statRequest) const noexcept {



	std::vector<std::string> messages;

	std::stringstream stream;
	stream << "(Lat,Lon): (" << int(this->_latitude_deg) << "," << int(this->_longitude_deg) << ")";
	messages.push_back(stream.str());

	std::vector<std::string> climateMessages = _tileClimate.getMessages(statRequest);

	for (std::string &str : climateMessages) {
		messages.push_back(str);
	}

	return messages;
}

double Tile::getStatistic(const StatRequest &statRequest) noexcept {
	

	_tileClimate.getStatistic(statRequest);

}

}//namespace simulation
}//namespace pleistocene
