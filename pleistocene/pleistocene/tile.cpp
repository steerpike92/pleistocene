#include "tile.h"
#include "graphics.h"
#include "world.h"
#include "bios.h"
#include "statistics.h"

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
		if (neighborAddress.i != my::kFakeIndex) {
			this->_directionalNeighbors[static_cast<my::Direction>(j)] = neighborAddress;
		}
	}
}

//SIMULATION
//============================


void Tile::simulate() noexcept {
	_tileClimate.simulateClimate();
}


//GRAPHICS
//=======================


bool Tile::statDraw(graphics::Graphics &graphics, bool cameraMovementFlag, const Statistics &statistics, const StatRequest &statRequest) noexcept
{
	//onscreen guard against wasting time. Also updates onscreen position.
	if (!onscreenPositionUpdate(graphics, cameraMovementFlag)) {
		return false;
	}
	
	if (_statValue == my::kFakeDouble) {//no legitimate value
		graphics.colorFilter(_colorTextures[0], 0, 0, 0);//filter to black
		return graphics.blitTexture(_colorTextures[0], NULL, _onscreenPositions);
	}
	

	//determine draw color
	my::RGB rgb = statistics.getColor(this->_statValue);
	graphics.colorFilter(_colorTextures[0], rgb.r, rgb.g, rgb.b);


	//old heat map coloring

	//if (_heatMapValue <= 0) {//Cold (blue)
	//	graphics.colorFilter(_colorTextures[0], filter, filter, 1.0);
	//}
	//else {//Hot (red)
	//	graphics.colorFilter(_colorTextures[0], 1.0, filter, filter);
	//}

	bool returnValue = false;

	returnValue= graphics.blitTexture(_colorTextures[0], NULL, _onscreenPositions);


	if (statRequest._statType == FLOW) {
		_tileClimate.advectionDraw(graphics, _onscreenPositions, statRequest);
	}


	return returnValue;
}


bool Tile::elevationDraw(graphics::Graphics &graphics, bool cameraMovementFlag, bool sunlit) noexcept 
{
	if (!onscreenPositionUpdate(graphics, cameraMovementFlag)) { //checks 
		return false;
	}
	return _tileClimate.elevationDraw(graphics, _onscreenPositions, sunlit);
}


bool Tile::onscreenPositionUpdate(graphics::Graphics &graphics, bool cameraMovementFlag) noexcept
{
	if (cameraMovementFlag) {//only update positions if camera has moved
		_onscreenPositions = graphics.getOnscreenPositions(&_gameRectangle);
	}
	//return true if tile is onscreen
	if (_onscreenPositions.empty()) { return false; }
	else { return true; }
}

std::map<int, std::string> Tile::_colorTextures;

void Tile::setupTextures(graphics::Graphics &graphics) noexcept
{
	_colorTextures[0] = "../../content/simpleTerrain/whiteTile.png";
	graphics.loadImage(_colorTextures[0]);
}

//GETTERS
//=================

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
	_statValue= _tileClimate.getStatistic(statRequest);
	return _statValue;
}

}//namespace simulation
}//namespace pleistocene
