#pragma once
#include "globals.h"

class Input;

class Camera {
public:
	Camera();
	~Camera();

	Camera(MyVector2 startingPosition, double startingZoom);

	MyVector2 getCameraPosition() const;
	double getZoomScale() const;

	void processCommands(const Input &input, int elapsedTime);

	MyVector2 screenPosToGamePos(MyVector2 screenPosition) const;

private:
	MyVector2 _cameraPosition;
	double _zoomScale;
};
