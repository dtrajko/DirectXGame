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

#include "InputListener.h"
#include "Point.h"

#include <unordered_set>


class InputSystem
{

private:
	InputSystem();
	~InputSystem();

public:
	static InputSystem* get();
	static void create();
	static void release();

	void update();
	void addListener(InputListener* listener);
	void removeListener(InputListener* listener);
	void setCursorPosition(const Point& pos);
	void showCursor(bool show);

private:
	std::unordered_set<InputListener*> m_set_listeners;

	short m_keys_state[256] = {};
	short m_old_keys_state[256] = {};

	Point m_old_mouse_pos;
	bool m_first_time = true;

	static InputSystem* m_system;

};
