#pragma once
#include "globals.h"

namespace pleistocene {
class Input;

namespace option {

enum DrawType {
	ELEVATION,		//Q
	SUNLIT_ELEVATION,	//W

	SURFACE_MATERIAL,
	LAYER_MATERIAL,

	SURFACE_TEMPERATURE,
	LAYER_TEMPERATURE
};



}

class GameOptions {
public:
	GameOptions() noexcept;

	void setMapSize(int sizeOption) noexcept;

	int getRows() const noexcept;
	int getCols() const noexcept;

	bool _dailyDraw = false;
	int _drawHour = 0;

	bool _solarShader = true;

	bool _loopOption = true;
	bool _restrictCameraOption = false;
	bool _maxFramerateOption = false;



	void processInput(Input &input);

private:

	int _Rows;
	int _Cols;

};
}//namespace pleistocene
