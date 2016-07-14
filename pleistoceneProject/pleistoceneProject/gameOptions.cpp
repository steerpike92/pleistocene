#include "gameOptions.h"
#include "globals.h"

GameOptions::GameOptions() noexcept {
	setMapSize(2);
	my::Address::getOptions(*this);
}

int GameOptions::getRows() const noexcept {return Rows;}

int GameOptions::getCols() const noexcept {return Cols; }

void GameOptions::setMapSize(int sizeOption) noexcept {
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