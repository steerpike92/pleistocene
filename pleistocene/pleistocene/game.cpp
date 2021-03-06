#include "game.h"
namespace pleistocene {

Game::Game() noexcept :
_options(options::GameOptions()),
_graphics(graphics::Graphics()),
_input(Input()),
_infoBar(user_interface::InfoBar(_graphics)),
_bios(user_interface::Bios(_graphics)),
_camera(graphics::Camera(my::Vector2(0, 0), pow(.8, 10), _options)),
_world(simulation::World(_graphics, _options))
{
	initialize();
	gameLoop();
}

void Game::initialize() noexcept 
{
	//sets initial camera position/zoomout level
	_graphics.setCamera(_camera);
	_graphics.setInput(_input);
	_input.setCamera(_camera);

	_world.simulate(_options);//one initial call to simulate for graphical setup
	_world.draw(_graphics, true, _options, _bios);//one guaranteed call checking draw positions
	_lastUpdateTime_MS = SDL_GetTicks();

	srand(size_t(time(NULL)));
}


//LOOP
//====================================================
void Game::gameLoop()  noexcept 
{
	while (!_quitFlag) {
		_input.beginNewFrame();//Sorts input events into callable information
		determineElapsedTime();		
		processInput();	
		update();
		draw();
	}
}



void Game::determineElapsedTime() noexcept 
{
	_elapsedTime_MS = SDL_GetTicks() - _lastUpdateTime_MS;
	size_t delay;

	if (_elapsedTime_MS < 10) {//STUB "delay"
		delay = 10 - _elapsedTime_MS;
		SDL_Delay(delay);
		_elapsedTime_MS = 10;
	}

	_lastUpdateTime_MS = SDL_GetTicks();
	_elapsedTime_MS = std::min(_elapsedTime_MS, globals::kMaxFrameTime);
}


//The logical guts of Game
void Game::processInput() noexcept 
{

	//Quit
	if (_input._quitFlag || _input.wasKeyPressed(SDL_SCANCODE_ESCAPE)) {
		_quitFlag = true;
		return;
	}

	//Process commands returns true if there is any camera movement
	if (_camera.processCommands(_input, _elapsedTime_MS, _options)) {
		_cameraMovementFlag = true;
	}
	 
	// Update options
	_options.processInput(_input);

	_world.processInput(_input, _options);


	//selection (left click)
	if (_input.wasButtonPressed(1)) { _graphics._selecting = true; }
	else { _graphics._selecting = false; }

	//de-selection (right click)
	if (_input.wasButtonPressed(3)) { _world.clearSelected(); }
}


void Game::update()  noexcept 
{
	_bios.update(_world.getMessages());
	_infoBar.update(_world.getReadout());
}


void Game::draw()  noexcept 
{

	//Low/High framerate control
	if ((!_options._dailyDraw) ||							//draw each hour if daily draw off
		_graphics._selecting ||							//draw whenever user clicks on a tile
		my::SimulationTime::_globalTime.getHour() == _options._drawHour ||	//draw on specified draw hour
		_cameraMovementFlag)							//draw when camera moves
	{
		_graphics.clear();
		_world.draw(_graphics, _cameraMovementFlag, _options, _bios);
		_cameraMovementFlag = false;//i.e. processed
	}

	_infoBar.draw(_graphics);
	_bios.draw(_graphics);
	_graphics.flip();
}

}//namespace pleistocene