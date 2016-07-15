#pragma once
#include "globals.h"

namespace pleistocene {

class Input;

namespace options {



enum DrawType {
	ELEVATION,		//1

	TEMPERATURE,		//2

	MATERIAL_PROPERTIES,	//3. Component names, Albedo, Heat Capacity, Porousness, Permeability, Salinity, Pressure

	FLOW,			//4. Surface water flow, groundwater flow, currents, airflow
	
	MOISTURE		//5. soil moisture, groundwater, ---, humidity.

	
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

	bool _loopOption = true;
	bool _restrictCameraOption = false;
	bool _maxFramerateOption = false;

	DrawType _drawType = ELEVATION;
	bool _sunlit = true;
	DrawSection _drawSection = SURFACE;
	int _drawLayer = 0;

	void processInput(Input &input);

	bool _continuousSimulation=false;
private:

	int _rows;
	int _cols;

};

}//namespace options
}//namespace pleistocene
