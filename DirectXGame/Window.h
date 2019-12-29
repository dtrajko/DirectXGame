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

	RECT getClientWindowRect();

	void setHWND(HWND hwnd);

	// Events
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();

	~Window();

protected:
	HWND m_hwnd;
	bool m_is_run;
};
