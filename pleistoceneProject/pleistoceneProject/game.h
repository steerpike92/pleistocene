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

	GameOptions _options;

	//Holds input information (key maps, mouse info)
	Input _input;

	//Displays debug info
	Bios _bios;

	//displays date
	InfoBar _infoBar;

	//holds textures, rederer, window. Performs rendering
	Graphics _graphics;

	//holds and updates and draws simulation
	Map _map;

	//holds camera position, 
	Camera _camera;


};

}//namespace pleistocene
