#pragma once
#include "globals.h"

namespace pleistocene {

class Input;

namespace options {



enum DrawType {
	ELEVATION,		//1
	SUNLIT_ELEVATION,	//2

	SURFACE_MATERIAL,	//3
	LAYER_MATERIAL,		//4

	SURFACE_TEMPERATURE,	//5
	LAYER_TEMPERATURE	//6
};

enum DrawSection {
	SURFACE,		//7
	EARTH,			//8
	SEA,			//9
	AIR			//0

};



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

}//namespace options
}//namespace pleistocene
