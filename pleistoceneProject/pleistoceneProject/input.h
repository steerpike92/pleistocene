#pragma once
#include "globals.h"

class Camera;

class Input {
public:



	//Call at beggining of every frame, handles all events,
	//creates key maps
	void beginNewFrame() noexcept;
	
	//report key states
	bool wasKeyPressed(SDL_Scancode) const noexcept;
	//report key states
	bool wasKeyHeld(SDL_Scancode) const noexcept;
	//report key states
	bool wasKeyReleased(SDL_Scancode) const noexcept;


	//report mouse states
	bool wasButtonPressed(int buttonIndex) const noexcept;
	bool wasButtonHeld(int buttonIndex) const noexcept;
	bool wasButtonReleased(int buttonIndex) const noexcept;

	//report mouse location in game position
	my::Vector2 getMouseLocation() const noexcept;

	//Report mouse location in screen position
	SDL_Point getMousePoint() const noexcept;

	SDL_Rect* getSelectionRect() noexcept;

	//quit flag
	bool _quitFlag=false;

	void setCamera(Camera &camera) noexcept;

private:

	Camera *_cameraPtr;

	//updates key maps
	void keyUpEvent(const SDL_Event &event) noexcept;

	//updates key maps
	void keyDownEvent(const SDL_Event &event) noexcept;

	//...but for mouse
	void buttonDownEvent(const SDL_Event& event) noexcept;
	void buttonUpEvent(const SDL_Event& event) noexcept;
	void mouseMovementEvent(const SDL_Event& event) noexcept;
	void storeMousePositionData(const SDL_Event& event) noexcept;

	//map of keys pressed this frame
	std::map<SDL_Scancode, bool> _pressedKeys;

	//map of held keys
	std::map<SDL_Scancode, bool> _heldKeys;

	//mop of keys released this frame
	std::map<SDL_Scancode, bool> _releasedKeys;

	template<typename keyType>
	bool checkBoolMap(keyType key, std::map<keyType, bool> boolMap) const noexcept;

	//MOUSE BUTTON DATA
	std::map<int, bool> _heldButtons;
	std::map<int, bool> _pressedButtons;
	std::map<int, bool> _releasedButtons;

	//MOUSE LOCATION DATA
	my::Vector2 _mouseLocation;
	SDL_Point _mousePoint;

	//Drag rect start and end points
	SDL_Point _points[2];
	SDL_Rect _selectionRect;
};
