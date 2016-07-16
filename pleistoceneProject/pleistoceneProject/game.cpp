#include "game.h"
namespace pleistocene {

Game::Game() noexcept {
	initialize();
	gameLoop();
}

void Game::initialize() noexcept {

	_options = options::GameOptions();
	_infoBar = user_interface::InfoBar(_graphics);
	_bios = user_interface::Bios(_graphics);
	_map = Map(_graphics, &_bios, _options);


	_camera =graphics::Camera(my::Vector2(0, 0), pow(.8, 10), &_options);

	_graphics.setCamera(_camera);
	_graphics.setInput(_input);
	_input.setCamera(_camera);

	_map.simulate();//one initial call to simulate for graphical setup
	_map.draw(_graphics, true, _options);//one guaranteed call checking draw positions
	_lastUpdateTime_MS = SDL_GetTicks();

	srand(size_t(time(NULL)));
}


void Game::gameLoop()  noexcept {
	while (!_quitFlag) {
		_input.beginNewFrame();	//Sorts input events into callable information
		determineElapsedTime();
		processInput(_elapsedTime_MS);
		update(_elapsedTime_MS);
		draw();
	}
}


void Game::determineElapsedTime() noexcept {
	_elapsedTime_MS = SDL_GetTicks() - _lastUpdateTime_MS;
	size_t delay;

	if (_elapsedTime_MS < 10 && 0) {//STUB "delay"
		delay = 10 - _elapsedTime_MS;
		SDL_Delay(delay);
		_elapsedTime_MS = 10;
	}

	_lastUpdateTime_MS = SDL_GetTicks();
	_elapsedTime_MS = std::min(_elapsedTime_MS, globals::MAX_FRAME_TIME);
}

void Game::processInput(int elapsedTime) noexcept {


	if (_camera.processCommands(_input, elapsedTime)) {
		_cameraMovementFlag = true;
	}

	 

	//Quit
	if (_input._quitFlag || _input.wasKeyPressed(SDL_SCANCODE_ESCAPE)) {
		_quitFlag = true;
		return;
	}

	//New map (resets all simulation data and generates new tile elevations with a random seed
	if (_input.wasKeyPressed(SDL_SCANCODE_G)) {
		_map.generateMap(rand(), _options);
		_map.simulate();
	}

	//Update options
	_options.processInput(_input);

	//selection
	if (_input.wasButtonPressed(1)) {
		_bios.clear();
		_graphics._selecting = true;
	}
	else { _graphics._selecting = false; }
	if (_input.wasButtonPressed(3)) { _bios.clear(); }

	//simulation
	if (_input.wasKeyPressed(SDL_SCANCODE_RETURN) || _input.wasKeyHeld(SDL_SCANCODE_BACKSLASH) || _options._continuousSimulation)
	{
		updateSimulation();
	}
}

void Game::update(int elapsedTime)  noexcept {
	_map.update(elapsedTime);
	_bios.update(_options);
	_infoBar.update();
}

void Game::updateSimulation() noexcept {
	my::SimulationTime::updateGlobalTime();
	_map.simulate(_options);
}

void Game::draw()  noexcept {

	//Low/High framerate control
	if ((!_options._dailyDraw) ||							//draw each hour if daily draw off
		_graphics._selecting ||							//draw whenever user clicks on a tile
		my::SimulationTime::_globalTime.getHour() == _options._drawHour ||	//draw on specified draw hour
		_cameraMovementFlag)							//draw when camera moves
	{
		_graphics.clear();
		_map.draw(_graphics, _cameraMovementFlag, _options);
		_cameraMovementFlag = false;//i.e. processed
	}

	_infoBar.draw(_graphics, _options);
	_bios.draw(_graphics);
	_graphics.flip();
}

}//namespace pleistocene