#include "gameOptions.h"
#include "globals.h"

GameOptions::GameOptions() {
	setMapSize(2);
	my::Address::getOptions(*this);
}

void GameOptions::setLoopOption(bool loop) { _loopOption = loop; }

void GameOptions::setRestrictCameraOption(bool option) { _restrictCameraOption = option; }

void GameOptions::setMaxFramerateOption(bool option) { _maxFramerateOption = option; }

int GameOptions::getRows()const{return Rows;}

int GameOptions::getCols()const{return Cols; }


bool GameOptions::getLoopOption()const{return _loopOption;}

bool GameOptions::getRestrictCameraOption()const{return _restrictCameraOption;}

bool GameOptions::getMaxFramerateOption()const{return _maxFramerateOption;}

void GameOptions::setMapSize(int sizeOption) {
	switch (sizeOption) {
	case(0) :
		Rows = 1;
		Cols = 1;
		break;
	case(1) :
		Rows = 31;
		Cols = 30;
		break;
	case(2) :
		Rows = 121;
		Cols = 90;
		break;
	}

	my::Address::getOptions(*this);
}