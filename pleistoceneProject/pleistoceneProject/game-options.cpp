#include "game-options.h"
#include "globals.h"
#include "input.h"

namespace pleistocene {
namespace options {

GameOptions::GameOptions() noexcept {
	setWorldSize(2);
	my::Address::getOptions(*this);
}

int GameOptions::getRows() const noexcept { return _rows; }

int GameOptions::getCols() const noexcept { return _cols; }


void GameOptions::setWorldSize(int sizeOption) noexcept {
	switch (sizeOption) {
	case(0) :
		_rows = 1;
		_cols = 1;
		break;
	case(1) :
		_rows = 31;
		_cols = 30;
		break;
	case(2) :
		_rows = 121;
		_cols = 90;
		break;
	}

	my::Address::getOptions(*this);
}

void GameOptions::processInput(Input &input) {

	//toggle low frequency redraw
	if (input.wasKeyPressed(SDL_SCANCODE_TAB)) {
		_dailyDraw = !_dailyDraw;
		_drawHour = my::SimulationTime::_globalTime.getHour();
	}

	//toggle solar shading
	if (input.wasKeyPressed(SDL_SCANCODE_GRAVE)) _sunlit = !_sunlit;

	//play/pause toggle
	if (input.wasKeyPressed(SDL_SCANCODE_SPACE)) _continuousSimulation = !_continuousSimulation;

	_newStatistic = false;

	//draw type selection
	if (input.wasKeyPressed(SDL_SCANCODE_1)) { _statistic = ELEVATION; _newStatistic = true; }
	if (input.wasKeyPressed(SDL_SCANCODE_2)) { _statistic = TEMPERATURE;_newStatistic = true; }
	if (input.wasKeyPressed(SDL_SCANCODE_3)) { _statistic = MATERIAL_PROPERTIES; _newStatistic = true; }
	if (input.wasKeyPressed(SDL_SCANCODE_4)) { _statistic = FLOW; _newStatistic = true; }
	if (input.wasKeyPressed(SDL_SCANCODE_5)) { _statistic = MOISTURE; _newStatistic = true; }
	//draw section selector
	if (input.wasKeyPressed(SDL_SCANCODE_6)) { _drawSection = SURFACE; _drawLayer = 0; _newStatistic = true; }
	if (input.wasKeyPressed(SDL_SCANCODE_7)) { _drawSection = HORIZON; _drawLayer = 0; _newStatistic = true; }
	if (input.wasKeyPressed(SDL_SCANCODE_8)) { _drawSection = EARTH; _drawLayer = 0; _newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_9)) { _drawSection = SEA; _drawLayer = 0; _newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_0)) { _drawSection = AIR; _drawLayer = 0;  _newStatistic = true; }

	//layer selection
	if (input.wasKeyPressed(SDL_SCANCODE_LEFTBRACKET)) { _drawLayer--; _newStatistic = true;}
	if (input.wasKeyPressed(SDL_SCANCODE_RIGHTBRACKET)) { _drawLayer++; _newStatistic = true;}
	_drawLayer %= 8;
}

}//namespace options
}//namespace pleistocene