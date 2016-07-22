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
		_rows = 3;
		_cols = 3;
		break;
	case(2) :
		_rows = 71;
		_cols = 60;
		break;
	case(3) :
		_rows = 121;
		_cols = 90;
		break;
	case(4) :
		_rows = 181;
		_cols = 180;
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

	
}

}//namespace options
}//namespace pleistocene