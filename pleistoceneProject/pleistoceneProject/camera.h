#pragma once
#include "globals.h"

namespace pleistocene {

namespace options{class GameOptions;}

class Input;

namespace graphics {

class Camera {
public:
	Camera() noexcept;

	Camera(my::Vector2 startingPosition, double startingZoom, const options::GameOptions &options) noexcept;

	void updateCameraOptions(const options::GameOptions &options) noexcept;

	my::Vector2 getCameraPosition() const noexcept;
	double getZoomScale() const noexcept;

	//returns true iff camera moved
	bool processCommands(const Input &input, int elapsedTime, const options::GameOptions &options) noexcept;

	my::Vector2 screenPosToGamePos(my::Vector2 screenPosition) const noexcept;

private:
	my::Vector2 _cameraPosition;
	double _zoomScale;

	int _gameWidth_pixels;
	int _gameHeight_pixels;


};

}//namespace graphics
}//namespace pleistocene