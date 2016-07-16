#pragma once
#include "globals.h"
#include "gameOptions.h"
#include "graphics.h"
#include "input.h"
#include "bios.h"
#include "map.h"
#include "camera.h"

namespace pleistocene {
/*
Game

======================================
Initializes major components:
======================================

	namespace graphics
	{
		Graphics
		Camera
	}

	Input

	(TODO: Include Input into a single module/class "UserInterface")
	
	namespace user_interface
	{
		Bios (debug info display) (needs name change probably as Bios means something different to computer people)
		Info Bar
	(TODO: Menu)
	}

	namespace simulation
	{
		Map
	}


======================================
Interfaces between major modules
======================================

======================================
Operates game loop
======================================
	

*/

class Game {
public:
	Game() noexcept;
private:
	//CLASS MEMBERS 
	//============================

	//Game Options
	//Holds currently selected user options in a rather public format
	//However it gets passed around as const to guard against changes
	options::GameOptions _options;

	//Input
	//Processes direct user input
	//Holds input information (key press maps, mouse button maps, mouse location)
	Input _input;

	//Displays debug info
	user_interface::Bios _bios;

	//displays date and current map draw state
	user_interface::InfoBar _infoBar;

	//holds textures, rederer, window. Performs rendering
	graphics::Graphics _graphics;

	//holds camera position, 
	graphics::Camera _camera;

	//holds and updates and draws simulation
	simulation::Map _map;


	//METHODS
	//===================================


	//First call. Builds game
	void initialize() noexcept;

	//Loops for duration of game
	void gameLoop() noexcept;


	//calculates real world time since last update
	void determineElapsedTime() noexcept;
	int _lastUpdateTime_MS;
	int _elapsedTime_MS;

	//Process stored user input into changes
	void processStoredInput() noexcept;
	bool _quitFlag = false;
	bool _cameraMovementFlag = true;

	//update for screen/animations. called each frame
	void update() noexcept;

	//update for simulation logic. 
	//simulates an hour of the simulation.
	void updateSimulation() noexcept;

	//Drawing to renderer and flip to window
	void draw() noexcept;


};//class Game

}//namespace pleistocene
