#pragma once
#include "globals.h"
#include "tileClimate.h"

class Map;
class Graphics;
class Bios;

//Tiles are hexagons
//organized into horizontal rows and vertical columns in my::Vector2 _tileAddress(row,column)
//Holds terrain features->
//determins thegraphics of the tile, 

//Iterated through row by row to draw (so tiles can be drawn on top of each other)
class Tile {
public:
	Tile();
	~Tile();
	Tile(my::Address tileAddress, int elevation, Graphics &graphics);

	//=====================================================================
	//SETUP
	//=======================================================================

private:
	static std::vector <Tile> _tiles;

	static void buildTileVector(Graphics &graphics);

	static void buildTileNeighbors();//all tiles
	void buildNeighborhood();//individual tile

	static void setupTextures(Graphics &graphics);

	static std::vector<my::Address> _Addresses;

	//my::Rectangle with ingame tile dimensions
	SDL_Rect _gameRectangle; 

public:
	//Calls other constructors
	static void setupTiles(Graphics &graphics);

	//Elevation noise creator
	static void generateTileElevation(int seed);
private:
	static std::vector<double> buildNoiseTable(int seed, double zoom, double persistance, int octaves, 
		int Rows, int Cols);

	static std::vector<double> blendNoiseTable(std::vector<double> noiseTable,int Rows, int Cols, int vBlendDistance, int  hBlendDistance);

	//Draw to renderer
public:
	static void drawTiles(Graphics &graphics, climate::DrawType drawType, bool cameraMovementFlag);//all
private: 
	void draw(Graphics &graphics, climate::DrawType drawType, bool cameraMovementFlag);//one
	std::vector<SDL_Rect> _onScreenPositions;

	//update animations
public:
	static void updateTiles(int elapsedTime);
private:
	void update(int elapsedTime);

	//run hour simulation
public:
	static void simulateTiles();
private:
	void simulate();

public:
	static Bios* _biosPtr;


public:
	static void alterElevations(int deltaM);
private:
	void alterElevation(int deltaM);

private:
	//(row,column)
	my::Address _Address;

public:
	//GETTERS
	//===================
	my::Address getAddress() const;
	SDL_Rect getGameRect() const;
	std::vector<std::string> sendMessages() const;//communicates with bios

protected:
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


	TileClimate _tileClimate;
};
