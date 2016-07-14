#pragma once

namespace options {

	enum drawType{
		STANDARD


	};

	enum drawLayer {



	};


}

class GameOptions {
public:
	GameOptions() noexcept;

	void setMapSize(int sizeOption) noexcept;

	int getRows() const noexcept;
	int getCols() const noexcept;

	bool _dailyDraw = true;

	bool _solarShader = true;

	bool _loopOption = true;
	bool _restrictCameraOption = false;
	bool _maxFramerateOption = false;

private:

	int Rows;
	int Cols;

};
