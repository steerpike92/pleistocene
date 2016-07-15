#pragma once
#include "globals.h"
#include "gameOptions.h"
#include "graphics.h"
#include "input.h"
#include "bios.h"
#include "map.h"
#include "camera.h"

namespace pleistocene {

class Game {
public:
	Game() noexcept;
private:
	void initialize() noexcept;
	void gameLoop() noexcept;

	void determineElapsedTime() noexcept;//also delay
	int _lastUpdateTime_MS;
	int _elapsedTime_MS;

	void processInput(int elapsedTime) noexcept;
	bool _quitFlag = false;
	bool _cameraMovementFlag = true;

	//update for screen/animations. called each frame
	void update(int elapsedTime) noexcept;

	//update for simulation logic. 
	//simulates an hour of the simulation.
	void updateSimulation() noexcept;

	//Drawing to renderer and flip to window
	void draw() noexcept;

	options::GameOptions _options;

	//Holds input information (key maps, mouse info)
	Input _input;

	//Displays debug info
	user_interface::Bios _bios;

	//displays date
	user_interface::InfoBar _infoBar;

	//holds textures, rederer, window. Performs rendering
	graphics::Graphics _graphics;

	//holds camera position, 
	graphics::Camera _camera;

	//holds and updates and draws simulation
	Map _map;

	


};

}//namespace pleistocene
