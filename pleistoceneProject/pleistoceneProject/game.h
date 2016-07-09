#pragma once
#include "globals.h"
#include "graphics.h"
#include "input.h"
#include "bios.h"
#include "map.h"
#include "camera.h"


class Game {
public:
	Game();
	~Game();
private:
	void initialize();
	void gameLoop();

	void determineElapsedTime();//also delay
	int _lastUpdateTime_MS;
	int _elapsedTime_MS;

	void processInput(int elapsedTime);
	bool _quitFlag = false;

	//update for screen/animations. called each frame
	void update(int elapsedTime);

	//update for simulation logic. 
	//simulates an hour of the simulation.
	void updateSimulation();

	//Drawing to renderer and flip to window
	void draw();

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
