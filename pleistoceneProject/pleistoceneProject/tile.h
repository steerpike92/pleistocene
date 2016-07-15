#pragma once
#include "globals.h"
#include "tileClimate.h"
#include "noise.h"

namespace pleistocene {

class Map;

namespace graphics { class Graphics; }
namespace user_interface { class Bios; }
namespace options { class GameOptions; }

//Tiles are hexagons
//organized into horizontal rows and vertical columns in my::Vector2 _tileAddress(row,column)

//Iterated through row by row to draw (so tiles can be drawn on top of each other)
class Tile {
public:
	Tile()noexcept;
	Tile(my::Address tileAddress)noexcept;

	//=====================================================================
	//SETUP
	//=======================================================================

	//static void getOptions(GameOptions &options);

private:
	static std::vector <Tile> _tiles;

	static void buildTileVector()noexcept;

	static void buildTileNeighbors()noexcept;//all tiles
	void buildNeighborhood()noexcept;//individual tile

	static void setupTextures(graphics::Graphics &graphics)noexcept;

	static std::vector<my::Address> _Addresses;

	//my::Rectangle with ingame tile dimensions
	SDL_Rect _gameRectangle;

public:
	//Calls other constructors
	static void setupTiles(graphics::Graphics &graphics)noexcept;

	//Elevation noise creator
	static void generateTileElevation(int seed)noexcept;
private:
	static std::vector<double> buildNoiseTable(int Rows, int Cols, int seed) noexcept;

	static std::vector<double> blendNoiseTable(std::vector<double> noiseTable, int Rows, int Cols,
		int vBlendDistance, int  hBlendDistance)noexcept;


public:
	//construct surface relationships 
	static void setupTileClimateAdjacency()noexcept;



	//GRAPHICS
	//====================

	static void drawTiles(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options)noexcept;//all
private:
	void draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options)noexcept;//one
	std::vector<SDL_Rect> _onScreenPositions;

	//update animations
public:
	static void updateTiles(int elapsedTime)noexcept;
private:
	void update(int elapsedTime)noexcept;

	//run hour simulation
public:
	static void simulateTiles()noexcept;
private:
	void simulate()noexcept;

public:
	static user_interface::Bios* _biosPtr;

private:
	//(row,column)
	my::Address _Address;

public:
	//GETTERS
	//===================
	my::Address getAddress() const noexcept;
	SDL_Rect getGameRect() const noexcept;
	std::vector<std::string> sendMessages(const options::GameOptions &options) const noexcept;//communicates with bios

private:
	//==================================================================================
	//GAMEPLAY VARIABLES
	//==================================================================================

	//GEOGRAPHY
	//==============================
	//vector of neighboring tile my::Addresses. Better to store my::Addresses than pointers as pointers miiiight change
	//std::vector<my::Address> _neighbors;
	std::map<my::Direction, my::Address> _directionalNeighbors;

	//Latitude in degrees from equator
	double _latitude_deg;

	//0>= ... <360
	double _longitude_deg;

	//all simulation happens in TileClimate
	climate::TileClimate _tileClimate;
};

}//namespace pleistocene