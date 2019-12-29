#pragma once

#include "Point.h"


class InputListener
{

public:
	InputListener()
	{

	}
	
	~InputListener()
	{

	}

	// Keyboard pure virtual callback functions
	virtual void onKeyDown(int key) = 0;
	virtual void onKeyUp(int key) = 0;

	// Mouse pure virtual callback functions
	virtual void onMouseMove(const Point& delta_mouse_pos) = 0;
};
