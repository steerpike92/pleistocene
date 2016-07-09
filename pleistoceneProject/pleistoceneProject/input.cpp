#include "input.h"
#include "globals.h"
#include "game.h"
#include "camera.h"

void Input::beginNewFrame() {
	_pressedKeys.clear();
	_releasedKeys.clear();

	_pressedButtons.clear();
	_releasedButtons.clear();

	SDL_Event event;

	while (SDL_PollEvent(&event)) {

		switch (event.type) {

		case SDL_MOUSEMOTION:
			mouseMovementEvent(event);
			break;

		case SDL_MOUSEBUTTONDOWN:
			buttonDownEvent(event);
			break;

		case SDL_MOUSEBUTTONUP:
			buttonUpEvent(event);
			break;

		case(SDL_QUIT): 
			_quitFlag = true; 
			return;

		case(SDL_KEYDOWN) :
			if (event.key.repeat == 0) {
				keyDownEvent(event);
			}
			break;
		case(SDL_KEYUP) : 
			keyUpEvent(event); 
			break;
		}

	}
}


void Input::keyDownEvent(const SDL_Event &event) {
	_pressedKeys[event.key.keysym.scancode] = true;
	_heldKeys[event.key.keysym.scancode] = true;
}

void Input::keyUpEvent(const SDL_Event &event) {
	_heldKeys[event.key.keysym.scancode] = false;
	_releasedKeys[event.key.keysym.scancode] = true;
}


void Input::storeMousePositionData(const SDL_Event &event) {

	_mousePoint.x = event.button.x;
	_mousePoint.y = event.button.y;

	_mouseLocation = _cameraPtr->screenPosToGamePos(MyVector2(_mousePoint));
}

//Called when button pressed
void Input::buttonDownEvent(const SDL_Event& event) {

	_pressedButtons[event.button.button] = true;

	_heldButtons[event.button.button] = true;

	//store position data


	storeMousePositionData(event);
	

	if (event.button.button == 1) _points[0] = _mousePoint;

}

void Input::buttonUpEvent(const SDL_Event& event) {
	_releasedButtons[event.button.button] = true;
	_heldButtons[event.button.button] = false;

	storeMousePositionData(event);

	if (event.button.button == 1) _points[1] = _mousePoint;
}

void Input::mouseMovementEvent(const SDL_Event& event) {
	storeMousePositionData(event);
	_points[1] = _mousePoint;
}

template<typename keyType>
bool Input::checkBoolMap(keyType key, std::map<keyType, bool> boolMap) const{
	//Check uninitialized
	if (boolMap.count(key) == 0) {
		return false;
	}
	return boolMap.at(key);
}

bool Input::wasKeyPressed(SDL_Scancode key) const{	
	return checkBoolMap(key, _pressedKeys);
}
bool Input::wasKeyHeld(SDL_Scancode key)const {
	return checkBoolMap(key, _heldKeys);
}
bool Input::wasKeyReleased(SDL_Scancode key)const {
	return checkBoolMap(key, _releasedKeys);

}

bool Input::wasButtonPressed(int buttonIndex)const {
	return checkBoolMap(buttonIndex, _pressedButtons);

}
bool Input::wasButtonHeld(int buttonIndex)const {
	return checkBoolMap(buttonIndex, _heldButtons);
}
bool Input::wasButtonReleased(int buttonIndex) const {
	return checkBoolMap(buttonIndex, _releasedButtons);
}

MyVector2 Input::getMouseLocation() const {
	return _mouseLocation;
}
SDL_Point Input::getMousePoint() const {
	return _mousePoint;
}

SDL_Rect* Input::getSelectionRect() {
	SDL_EnclosePoints(_points, 2, NULL, &_selectionRect);
	return &_selectionRect;
}

void Input::setCamera(Camera &camera) {
		_cameraPtr = &camera;
}