#pragma once
#include "globals.h"

namespace pleistocene {

class Input;

namespace options {

class GameOptions {
public:
	GameOptions() noexcept;

	void setWorldSize(int sizeOption) noexcept;

	int getRows() const noexcept;
	int getCols() const noexcept;

	bool _dailyDraw = false;
	int _drawHour = 0;

	bool _loopOption = true;
	bool _restrictCameraOption = false;
	bool _maxFramerateOption = false;

	bool _sunlit = true;

	void processInput(Input &input);

	bool _continuousSimulation=false;
private:

	int _rows;
	int _cols;

};

}//namespace options
}//namespace pleistocene
