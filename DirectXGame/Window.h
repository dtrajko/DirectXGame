#pragma once
#include <Windows.h>

class Window
{
public:

	Window();

	// Initialize the Window
	bool init();

	bool broadcast();

	// Release the Window
	bool release();

	bool isRunning();

	// Events
	virtual void onCreate() = 0;
	virtual void onUpdate() = 0;
	virtual void onDestroy();

	~Window();

protected:
	HWND m_hwnd;
	bool m_is_run;
};
