#pragma once
#include "globals.h"

class GameOptions;
class Input;

class Camera {
public:
	Camera();

	Camera(my::Vector2 startingPosition, double startingZoom, GameOptions *options);

	void updateCameraOptions();

	my::Vector2 getCameraPosition() const;
	double getZoomScale() const;

	//returns true iff camera moved
	bool processCommands(const Input &input, int elapsedTime);

	my::Vector2 screenPosToGamePos(my::Vector2 screenPosition) const;

private:
	my::Vector2 _cameraPosition;
	double _zoomScale;
	GameOptions *_optionsPtr;

	int _gameWidth_pixels;
	int _gameHeight_pixels;

	bool LOOP;
	bool RESTRICT_CAMERA;





};
