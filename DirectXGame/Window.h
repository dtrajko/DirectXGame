// Copyright (c) 2019  PardCode.
// All rights reserved.
//
// This file is part of CPP-3D-Game-Tutorial-Series Project, accessible from https://github.com/PardCode/CPP-3D-Game-Tutorial-Series
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License 
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.


#pragma once
#include <Windows.h>

class Window
{
protected:
	// Initialize the window
	Window();
	// Release the window
	~Window();

public:
	bool broadcast();
	bool release();
	bool isRunning();
	RECT getClientWindowRect();

	// Events
	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();


protected:
	HWND m_hwnd;
	bool m_is_run;
	bool m_is_init;

};
