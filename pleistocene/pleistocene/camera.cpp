#include "camera.h"
#include "input.h"
#include "game-options.h"

namespace pleistocene {
namespace graphics {

Camera::Camera() noexcept {}

Camera::Camera(my::Vector2 startingPosition, double startingZoom, const options::GameOptions &options) noexcept {
	using namespace options;
	_cameraPosition = startingPosition;
	_zoomScale = startingZoom;
	updateCameraOptions(options);
}

void Camera::updateCameraOptions(const options::GameOptions &options) noexcept {
	using namespace options;
	_gameWidth_pixels = options.getCols()*globals::kTileWidth;
	_gameHeight_pixels = options.getRows()*globals::kEffectiveTileHeight;
}

my::Vector2 Camera::getCameraPosition() const noexcept {
	return _cameraPosition;
}

double Camera::getZoomScale() const noexcept {
	return _zoomScale;
}

bool Camera::processCommands(const Input &input, int elapsedTime, const options::GameOptions &options) noexcept {

	bool movementflag = false;

	//Zoom in
	//==========================================================================================================================
	if (input.wasKeyReleased(SDL_SCANCODE_EQUALS)) {//equals because plus

		//Move _cameraPosition to center of view
		_cameraPosition = _cameraPosition + (my::Vector2(globals::kScreenWidth / 2, globals::kScreenHeight / 2));

		//Change rendering _zoomScale (also scales rendering position) 
		_zoomScale = _zoomScale * 1.25f;

		//_zoomScale camera center position
		_cameraPosition = _cameraPosition * (1.25f);

		//move _cameraPosition back to top left corner
		_cameraPosition = _cameraPosition - (my::Vector2(globals::kScreenWidth / 2, globals::kScreenHeight / 2));

		movementflag = true;
	}

	//Zoom out
	//==========================================================================================================================
	if (input.wasKeyReleased(SDL_SCANCODE_MINUS)) {

		if (_zoomScale > 0.01 || (options._restrictCameraOption == 0)) {

			if ((_gameWidth_pixels * _zoomScale > globals::kScreenWidth) &&
				(_gameHeight_pixels * _zoomScale > globals::kScreenHeight)
				|| (options._restrictCameraOption == 0)) {

				//Move _cameraPosition to center of view
				_cameraPosition = _cameraPosition + (my::Vector2(globals::kScreenWidth / 2, globals::kScreenHeight / 2));

				//Change rendering _zoomScale (also scales rendering position) 
				_zoomScale = _zoomScale *  0.8;

				//_zoomScale camera center position
				_cameraPosition = _cameraPosition*(0.8);

				//move _cameraPosition back to top left corner
				_cameraPosition = _cameraPosition - (my::Vector2(globals::kScreenWidth / 2, globals::kScreenHeight / 2));


				movementflag = true;
			}
		}

	}


	//Pan Camera
	//==========================================================================================================================
	if (input.wasKeyHeld(SDL_SCANCODE_A)) {
		_cameraPosition.x -= elapsedTime / 2;
		movementflag = true;


		//World loop
		if ((_cameraPosition.x < (-globals::kTileWidth *options.getCols()+ kEdgeOffset) * _zoomScale)  && options._loopOption) {
			_cameraPosition.x = int(kEdgeOffset*_zoomScale);
		}
	}

	if (input.wasKeyHeld(SDL_SCANCODE_D)) {
		_cameraPosition.x += elapsedTime / 2;
		movementflag = true;

		//World loop
		if ((_cameraPosition.x > kEdgeOffset*_zoomScale) && options._loopOption) {
			_cameraPosition.x = int( (-globals::kTileWidth*options.getCols()+ kEdgeOffset) * _zoomScale);
		}
	}

	if (input.wasKeyHeld(SDL_SCANCODE_W)) {
		if (_cameraPosition.y > -(globals::kEffectiveTileHeight * 3) * _zoomScale) {
			_cameraPosition.y -= elapsedTime / 2;
			movementflag = true;
		}
	}

	if (input.wasKeyHeld(SDL_SCANCODE_S)) {
		if (_cameraPosition.y < ((_gameHeight_pixels)* _zoomScale - globals::kScreenHeight)) {
			_cameraPosition.y += elapsedTime / 2;
			movementflag = true;
		}
	}

	return movementflag;
}

my::Vector2 Camera::screenPosToGamePos(my::Vector2 screenPos) const noexcept {
	my::Vector2 gamePos;
	gamePos = (screenPos + _cameraPosition) * (1 / _zoomScale);
	return gamePos;
}

}//namespace graphics
}//namespace pleistocene