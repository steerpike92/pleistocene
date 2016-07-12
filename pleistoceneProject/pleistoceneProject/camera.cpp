#include "camera.h"
#include "input.h"

Camera::Camera() {}

Camera::Camera(my::Vector2 startingPosition, double startingZoom) {
	_cameraPosition = startingPosition;
	_zoomScale = startingZoom;
}


my::Vector2 Camera::getCameraPosition() const {
	return _cameraPosition;
}

double Camera::getZoomScale() const {
	return _zoomScale;
}

bool Camera::processCommands(const Input &input, int elapsedTime) {

	bool movementflag = false;

	//Zoom in
	//==========================================================================================================================
	if (input.wasKeyReleased(SDL_SCANCODE_EQUALS)) {

		//Move _cameraPosition to center of view
		_cameraPosition = _cameraPosition + (my::Vector2(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2));

		//Change rendering _zoomScale (also scales rendering position) 
		_zoomScale = _zoomScale * 1.25f;

		//_zoomScale camera center position
		_cameraPosition = _cameraPosition * (1.25f);

		//move _cameraPosition back to top left corner
		_cameraPosition = _cameraPosition - (my::Vector2(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2));

		movementflag = true;
	}

	//Zoom out
	//==========================================================================================================================
	if (input.wasKeyReleased(SDL_SCANCODE_MINUS)) {

		if (_zoomScale > 0.01 || (RESTRICT_CAMERA == 0)) {

			if ((globals::GAME_WIDTH_PIXELS * _zoomScale  > globals::SCREEN_WIDTH) &&
				(globals::GAME_HEIGHT_PIXELS * _zoomScale > globals::SCREEN_HEIGHT)
				|| (RESTRICT_CAMERA == 0)) {

				//Move _cameraPosition to center of view
				_cameraPosition = _cameraPosition + (my::Vector2(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2));

				//Change rendering _zoomScale (also scales rendering position) 
				_zoomScale = _zoomScale *  0.8;

				//_zoomScale camera center position
				_cameraPosition = _cameraPosition*(0.8);

				//move _cameraPosition back to top left corner
				_cameraPosition = _cameraPosition - (my::Vector2(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2));


				movementflag = true;
			}
		}

	}


	//Pan Camera
	//==========================================================================================================================
	if (input.wasKeyHeld(SDL_SCANCODE_A)) {
		_cameraPosition.x -= elapsedTime / 2;

		if (_cameraPosition.x < -globals::TILE_WIDTH * _zoomScale*globals::TILE_COLUMNS / 2 && LOOP) {
			_cameraPosition.x = int(globals::TILE_WIDTH * _zoomScale*globals::TILE_COLUMNS / 2);
		}
		movementflag = true;
	}

	if (input.wasKeyHeld(SDL_SCANCODE_D)) {
		_cameraPosition.x += elapsedTime / 2;
		if (_cameraPosition.x >(globals::TILE_WIDTH * _zoomScale*globals::TILE_COLUMNS*1.5 - globals::SCREEN_WIDTH) && LOOP) {
			_cameraPosition.x -= int(globals::TILE_WIDTH * _zoomScale*globals::TILE_COLUMNS);
		}
		movementflag = true;

	}

	if (input.wasKeyHeld(SDL_SCANCODE_W)) {
		if (_cameraPosition.y > -(globals::EFFECTIVE_HEIGHT*3) * _zoomScale) {
			_cameraPosition.y -= elapsedTime / 2;
			movementflag = true;
		}
	}

	if (input.wasKeyHeld(SDL_SCANCODE_S)) {
		if (_cameraPosition.y < ((globals::GAME_HEIGHT_PIXELS) * _zoomScale - globals::SCREEN_HEIGHT)) {
			_cameraPosition.y += elapsedTime / 2;
			movementflag = true;
		}
	}

	return movementflag;
}

my::Vector2 Camera::screenPosToGamePos(my::Vector2 screenPos) const {
	my::Vector2 gamePos;
	gamePos = (screenPos + _cameraPosition) * (1 / _zoomScale);
	return gamePos;
}