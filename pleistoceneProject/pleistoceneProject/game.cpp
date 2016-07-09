#include "game.h"

Game::Game() {
	//try
	//{
	initialize();
	gameLoop();
	/*}
	catch (int exception_num)
	{
		LOG("Handled Error");
		switch (exception_num) {
		case(0) : LOG("Startup error"); break;
		case(1) : LOG("Image loading error"); break;
		case(2) : LOG("Game logic error"); break;
		default: LOG("Unkown exception number"); break;
		}
		system("pause");
		return;
	}
	catch (...) {
		LOG("Unhandled Error");
		system("pause");
		return;
	}*/
}


void Game::initialize() {
	_infoBar = InfoBar(_graphics);
	_bios = Bios(_graphics);
	_map = Map(_graphics, &_bios);
	_camera = Camera(MyVector2(0, 0), pow(.8, 10));

	_graphics.setCamera(_camera);
	_graphics.setInput(_input);
	_input.setCamera(_camera);

	_map.simulate();//one initial call to simulate for graphical setup
	_lastUpdateTime_MS = SDL_GetTicks();
}

void Game::gameLoop() {
	while (!_quitFlag) {
		_input.beginNewFrame();	//Sorts input events into callable information
		determineElapsedTime();
		processInput(_elapsedTime_MS);
		update(_elapsedTime_MS);
		draw();
	}
}

void Game::determineElapsedTime() {
	_elapsedTime_MS = SDL_GetTicks() - _lastUpdateTime_MS;
	size_t delay;

	if (_elapsedTime_MS < 10 && DELAY) {
		delay = 10 - _elapsedTime_MS;
		SDL_Delay(delay);
		_elapsedTime_MS = 10;
	}

	_lastUpdateTime_MS = SDL_GetTicks();
	_elapsedTime_MS = std::min(_elapsedTime_MS, globals::MAX_FRAME_TIME);
}

void Game::processInput(int elapsedTime) {
	_camera.processCommands(_input, elapsedTime);

	//Quit
	if (_input._quitFlag || _input.wasKeyPressed(SDL_SCANCODE_ESCAPE)) {
		_quitFlag = true;
		return;
	}

	//New map (resets all simulation data and generates new tile elevations with a random seed
	if (_input.wasKeyPressed(SDL_SCANCODE_G)) {
		_map.generateMap(rand());
		_map.simulate();
	}

	//Map Draw Type
	if (_input.wasKeyPressed(SDL_SCANCODE_0)) {_map.setDrawType(0);}
	if (_input.wasKeyPressed(SDL_SCANCODE_1)) {_map.setDrawType(1);}
	if (_input.wasKeyPressed(SDL_SCANCODE_2)) {_map.setDrawType(2);}
	if (_input.wasKeyPressed(SDL_SCANCODE_3)) {_map.setDrawType(3);}
	if (_input.wasKeyPressed(SDL_SCANCODE_4)) {_map.setDrawType(4);}
	if (_input.wasKeyPressed(SDL_SCANCODE_5)) {_map.setDrawType(5);}


	/*if (_input.wasKeyHeld(SDL_SCANCODE_R)) _map.alterElevation(10);
	if (_input.wasKeyHeld(SDL_SCANCODE_L)) _map.alterElevation(-10);*/

	//selection
	if (_input.wasButtonPressed(1)) {
		_bios.clear();
		_graphics._selecting = true;
	}
	else {_graphics._selecting = false;}
	if (_input.wasButtonPressed(3)) {_bios.clear();}

	//simulation
	if (_input.wasKeyPressed(SDL_SCANCODE_RETURN) || _input.wasKeyHeld(SDL_SCANCODE_BACKSLASH)) 
	{
		updateSimulation();
	}
}

void Game::update(int elapsedTime) {
	_map.update(elapsedTime);
	_bios.update();
	_infoBar.update();
}

void Game::updateSimulation() {
	SimulationTime::updateGlobalTime();
	_map.simulate();
}

void Game::draw() {
	if (!DAILY_DRAW) {
		_graphics.clear();
		_map.draw(_graphics);
	}
	else if (SimulationTime::_globalTime.getHour() == 0) {
		_graphics.clear();
		_map.draw(_graphics);
	}

	_infoBar.draw(_graphics);
	_bios.draw(_graphics);
	_graphics.flip();
}

Game::~Game() { std::cout << "Exiting\n"; }
