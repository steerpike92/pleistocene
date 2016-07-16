#pragma once
#include "globals.h"

namespace pleistocene {
 
namespace graphics { class Graphics; }
namespace user_interface { class Bios; }
class Input;
namespace options { class GameOptions; }

namespace simulation {

class Tile;

enum StatType {
	ELEVATION,		//1
	TEMPERATURE,		//2
	MATERIAL_PROPERTIES,	//3. Component names, Albedo, Heat Capacity, Porousness, Permeability, Salinity, Pressure
	FLOW,			//4. Surface water flow, groundwater flow, currents, airflow
	MOISTURE		//5. soil moisture, groundwater, ---, humidity.
};

enum Section {
	SURFACE,		//6
	HORIZON,		//7
	EARTH,			//8
	SEA,			//9
	AIR			//0
};



struct StatRequest {
	StatType _statType;
	Section _section;
	int _layer;

	StatRequest():
		_statType(ELEVATION),
		_section(SURFACE),
		_layer(0)
	{}

	StatRequest(StatType statType, Section section, int layer) :
		_statType(statType),
		_section(section),
		_layer(layer)
	{}
};


//TODO
//The World class needs a rename and some shit to do.

//Extract all of the ridiculous Static members in tile and put them here



class World {
public:
	World() noexcept;
	World(graphics::Graphics &graphics, user_interface::Bios *bios, const options::GameOptions &options) noexcept;

	void generateWorld(int seed, const options::GameOptions &options) noexcept;

	void draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options) noexcept;
	void update(int elapsedTime) noexcept;

	void simulate(const options::GameOptions &options) noexcept;

	user_interface::Bios* _bioPtr;

	bool _exists = false;


	void processInput(const Input &input, const options::GameOptions & options) noexcept;

private:

	StatRequest _statRequest;


};



}//namespace simulation
}//namespace pleistocene


