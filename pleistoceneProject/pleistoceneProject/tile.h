#pragma once
#include "globals.h"
#include "tile-climate.h"
#include "noise.h"

namespace pleistocene {


namespace graphics { class Graphics; }
namespace user_interface { class Bios; }

namespace simulation {

struct StatRequest;

//Tiles are hexagons
//organized into horizontal rows and vertical columns in my::Vector2 _tileAddress(row,column)

//Iterated through row by row to draw (so tiles can be drawn on top of each other)

class Tile {
public:
	Tile() noexcept;
	Tile(my::Address tileAddress) noexcept;

	//=====================================================================
	//SETUP
	//=======================================================================

	//static void getOptions(GameOptions &options);
	void buildNeighborhood() noexcept;

	//GRAPHICS
	//====================
	bool draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options) noexcept;
private:
	std::vector<SDL_Rect> _onScreenPositions;
	//my::Rectangle with ingame tile dimensions
	SDL_Rect _gameRectangle;

public:
	
	//update animations
	void update(int elapsedTime) noexcept;

	//run hour simulation
	void simulate() noexcept;

	//(row,column)
	my::Address _address;


	
	my::Address getAddress() const noexcept;
	SDL_Rect getGameRect() const noexcept;
	std::vector<std::string> sendMessages(const StatRequest &statRequest) const noexcept;//communicates with bios

												  //all simulation happens in TileClimate

	climate::TileClimate _tileClimate;

	std::map<my::Direction, my::Address> _directionalNeighbors;




	double _statistic; 


private:

	
	//vector of neighboring tile my::Addresses. Better to store my::Addresses than pointers as pointers miiiight change
	//std::vector<my::Address> _neighbors;

	//Latitude in degrees from equator
	double _latitude_deg;

	//0>= ... <360
	double _longitude_deg;



};



}//namespace simulation
}//namespace pleistocene