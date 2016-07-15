#include "gameOptions.h"
#include "globals.h"
#include "input.h"

namespace pleistocene {
namespace options {

GameOptions::GameOptions() noexcept {
	setMapSize(2);
	my::Address::getOptions(*this);
}

int GameOptions::getRows() const noexcept { return _rows; }

int GameOptions::getCols() const noexcept { return _cols; }


void GameOptions::setMapSize(int sizeOption) noexcept {
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

	//draw type selection
	if (input.wasKeyPressed(SDL_SCANCODE_1)) _drawType = ELEVATION;
	if (input.wasKeyPressed(SDL_SCANCODE_2)) _drawType = TEMPERATURE;
	if (input.wasKeyPressed(SDL_SCANCODE_3)) _drawType = MATERIAL_PROPERTIES;
	if (input.wasKeyPressed(SDL_SCANCODE_4)) _drawType = FLOW;
	if (input.wasKeyPressed(SDL_SCANCODE_5)) _drawType = MOISTURE;
	//if (input.wasKeyPressed(SDL_SCANCODE_6)) _drawType = LAYER_TEMPERATURE;
	if (input.wasKeyPressed(SDL_SCANCODE_7)) _drawSection = SURFACE;
	if (input.wasKeyPressed(SDL_SCANCODE_8)) _drawSection = EARTH;
	if (input.wasKeyPressed(SDL_SCANCODE_9)) _drawSection = SEA;
	if (input.wasKeyPressed(SDL_SCANCODE_0)) _drawSection = AIR;


}

}//namespace options
}//namespace pleistocene