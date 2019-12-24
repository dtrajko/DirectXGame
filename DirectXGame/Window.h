#pragma once
#include <Windows.h>

class Window
{
public:
	Window();
	// Initialize the Window
	bool init();
	// Release the Window
	bool release();
	~Window();

protected:
	HWND m_hwnd;
};
