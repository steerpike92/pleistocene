#pragma once
#include "globals.h"
#include "tile-climate.h"
#include "noise.h"

namespace pleistocene {

class Statistics;

namespace graphics { class Graphics; }
namespace simulation {


struct StatRequest;

//Tiles are hexagons
//organized into horizontal rows and vertical columns in my::Vector2 _tileAddress(row,column)

//Iterated through row by row to draw (so tiles can be drawn on top of each other)

class Tile {
public:
	Tile() noexcept;
	Tile(my::Address tileAddress) noexcept;

	
	void buildNeighborhood() noexcept;
	//Map to adjacient tile addresses
	std::map<my::Direction, my::Address> _directionalNeighbors;




	//GRAPHICS
	//====================
	bool statDraw(graphics::Graphics &graphics, bool cameraMovementFlag, const Statistics &statistics, const StatRequest &statRequest) noexcept;
	bool elevationDraw(graphics::Graphics &graphics, bool cameraMovementFlag, bool sunlit) noexcept;

	static std::map<int, std::string> _colorTextures;
	static void setupTextures(graphics::Graphics &graphics) noexcept;
private:
	bool onscreenPositionUpdate(graphics::Graphics &graphics, bool cameraMovementFlag) noexcept;
	std::vector<SDL_Rect> _onscreenPositions;
	//my::Rectangle with ingame tile dimensions
	SDL_Rect _gameRectangle;


public:
	//run hour simulation
	void simulate() noexcept;

	//(row,column)
	my::Address _address;

	SDL_Rect getGameRect() const noexcept;
	std::vector<std::string> sendMessages(const StatRequest &statRequest) const noexcept;//communicates with bios


	//all simulation happens in TileClimate
	climate::TileClimate _tileClimate;


	double _statValue;//e.g. temperature
	double _heatMapValue;//e.g. sigmas off mean scaled to -1 to 1

	double getStatistic(const StatRequest &statRequest) noexcept;

private:

	//Latitude in degrees from equator
	double _latitude_deg;

	//0>= ... <360
	double _longitude_deg;
};



}//namespace simulation
}//namespace pleistocene