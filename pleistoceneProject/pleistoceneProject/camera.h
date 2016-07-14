#pragma once
#include "globals.h"

class GameOptions;
class Input;

class Camera {
public:
	Camera() noexcept;

	Camera(my::Vector2 startingPosition, double startingZoom, GameOptions *options) noexcept;

	void updateCameraOptions() noexcept;

	my::Vector2 getCameraPosition() const noexcept;
	double getZoomScale() const noexcept;

	//returns true iff camera moved
	bool processCommands(const Input &input, int elapsedTime) noexcept;

	my::Vector2 screenPosToGamePos(my::Vector2 screenPosition) const noexcept;

private:
	my::Vector2 _cameraPosition;
	double _zoomScale;
	GameOptions *_optionsPtr;

	int _gameWidth_pixels;
	int _gameHeight_pixels;

	bool LOOP;
	bool RESTRICT_CAMERA;





};
