#pragma once

class GameOptions {
public:
	GameOptions();

	void setMapSize(int sizeOption);
	void setLoopOption(bool option);
	void setRestrictCameraOption(bool option);
	void setMaxFramerateOption(bool option);

	int getRows()const;
	int getCols()const;
	bool getLoopOption()const;
	bool getRestrictCameraOption()const;
	bool getMaxFramerateOption()const;

private:

	int Rows;
	int Cols;
	bool _loopOption = true;
	bool _restrictCameraOption = false;
	bool _maxFramerateOption = false;
};