#pragma once
#include "globals.h"
#include "tileClimate.h"

class Map;
class Graphics;
class Bios;

//Tiles are hexagons
//organized into horizontal rows and vertical columns in MyVector2 _tileAddress(row,column)
//Holds terrain features->
//determins thegraphics of the tile, 

//Iterated through row by row to draw (so tiles can be drawn on top of each other)
class Tile {
public:
	Tile();
	~Tile();
	Tile(Address tileAddress, int elevation, Graphics &graphics);

	//=====================================================================
	//SETUP
	//=======================================================================

private:
	static std::vector <Tile> _tiles;

	static void buildTileVector(Graphics &graphics);

	static void buildTileNeighbors();//all tiles
	void buildNeighborhood();//individual tile

	static void setupTextures(Graphics &graphics);

	static std::vector<Address> _Addresses;

	//Rectangle with ingame tile dimensions
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
	static void drawTiles(Graphics &graphics, climate::DrawType drawType);//all
private: 
	void draw(Graphics &graphics, climate::DrawType drawType);//one

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
	Address _address;

public:
	//GETTERS
	//===================
	Address getAddress() const;
	SDL_Rect getGameRect() const;
	std::vector<std::string> sendMessages() const;//communicates with bios

protected:
	//==================================================================================
	//GAMEPLAY VARIABLES
	//==================================================================================

	//GEOGRAPHY
	//==============================
	//vector of neighboring tile addresses. Better to store addresses than pointers as pointers miiiight change
	//std::vector<Address> _neighbors;
	std::map<Direction, Address> _directionalNeighbors;

	//Latitude in degrees from equator
	double _latitude_deg;

	//0>= ... <360
	double _longitude_deg;


	TileClimate _tileClimate;
};
