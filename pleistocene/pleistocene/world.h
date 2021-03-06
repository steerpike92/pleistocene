#pragma once
#include "globals.h"
#include "statistics.h"
#include "tile.h"

namespace pleistocene {
 
namespace graphics { class Graphics; }
namespace user_interface { class Bios; }
class Input;
namespace options { class GameOptions; }

namespace simulation {



enum StatType {
	ELEVATION,		//1
	TEMPERATURE,		//2
	MATERIAL_PROPERTIES,	//3. Component names, Albedo, Heat Capacity, Porousness, Permeability, Salinity,
	PRESSURE,			//4. Surface water PRESSURE, groundwater PRESSURE, currents, airPRESSURE
	FLOW			//5. 
};


enum Section {
	SURFACE_,		//6
	HORIZON_,		//7
	EARTH_,			//8
	SEA_,			//9
	AIR_			//0
};

struct StatRequest {
	StatType _statType;
	Section _section;
	int _layer;

	StatRequest():
		_statType(ELEVATION),
		_section(SURFACE_),
		_layer(0)
	{}

	StatRequest(StatType statType, Section section, int layer) :
		_statType(statType),
		_section(section),
		_layer(layer)
	{}

};


class World {
public:
	World() noexcept;
	World(graphics::Graphics &graphics, const options::GameOptions &options) noexcept;

private:

	void setupTiles(graphics::Graphics & graphics) noexcept;
	void buildTileVector() noexcept;
	void setupTextures(graphics::Graphics & graphics) noexcept;
	void buildTileNeighbors() noexcept;

	void generateTileElevations() noexcept;
	void setupTileClimateAdjacency() noexcept;

	std::vector<double> World::buildNoiseTable(int Rows, int Cols) noexcept;

	std::vector<double> World::blendNoiseTable(std::vector<double> noiseTable, int Rows, int Cols, int vBlendDistance, int  hBlendDistance) noexcept;

public:

	void generateWorld(const options::GameOptions &options) noexcept;

	void draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options, user_interface::Bios &bios) noexcept;

	void simulate(const options::GameOptions &options) noexcept;

	user_interface::Bios* _bioPtr;

	void processInput(const Input &input, const options::GameOptions & options) noexcept;

	void clearSelected() noexcept;

	std::vector<std::string> getMessages() const noexcept;
	std::vector<std::string> getReadout() const noexcept;

private:

	double _seed;

	std::vector<Tile> _tiles;

	bool _statisticsUpToDate;

	void performStatistics() noexcept;

	StatRequest _statRequest;

	Statistics _statistics;

	Tile *_selectedTile;
};



}//namespace simulation
}//namespace pleistocene


