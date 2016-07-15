#include "gameOptions.h"
#include "globals.h"
#include "input.h"

namespace pleistocene {
namespace options {

GameOptions::GameOptions() noexcept {
	setMapSize(2);
	my::Address::getOptions(*this);
}

int GameOptions::getRows() const noexcept { return _Rows; }

int GameOptions::getCols() const noexcept { return _Cols; }


void GameOptions::setMapSize(int sizeOption) noexcept {
	switch (sizeOption) {
	case(0) :
		_Rows = 1;
		_Cols = 1;
		break;
	case(1) :
		_Rows = 31;
		_Cols = 30;
		break;
	case(2) :
		_Rows = 121;
		_Cols = 90;
		break;
	}

	my::Address::getOptions(*this);
}

void GameOptions::processInput(Input &input) {

	if (input.wasKeyPressed(SDL_SCANCODE_TAB)) {
		if (_dailyDraw) _dailyDraw = false;
		else {
			_dailyDraw = true;
			_drawHour = my::SimulationTime::_globalTime.getHour();
		}
	}

}

}//namespace options
}//namespace pleistocene