#pragma once
#include "globals.h"

namespace pleistocene {

namespace options{class GameOptions;}

class Input;

namespace graphics {

class Camera {
public:
	Camera() noexcept;

	Camera(my::Vector2 startingPosition, double startingZoom, options::GameOptions *options) noexcept;

	void updateCameraOptions() noexcept;

	my::Vector2 getCameraPosition() const noexcept;
	double getZoomScale() const noexcept;

	//returns true iff camera moved
	bool processCommands(const Input &input, int elapsedTime) noexcept;

	my::Vector2 screenPosToGamePos(my::Vector2 screenPosition) const noexcept;

private:
	my::Vector2 _cameraPosition;
	double _zoomScale;
	options::GameOptions *_optionsPtr;

	int _gameWidth_pixels;
	int _gameHeight_pixels;

	bool _loop;
	bool _restrictCamera;





};

}//namespace graphics
}//namespace pleistocene