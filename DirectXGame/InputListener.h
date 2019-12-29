#pragma once


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
};
