#pragma once

#include "InputListener.h"

#include <map>


class InputSystem
{

public:
	InputSystem();
	static InputSystem* get();

	void update();
	void addListener(InputListener* listener);
	void removeListener(InputListener* listener);
	~InputSystem();

private:
	std::map<InputListener*, InputListener*> m_map_listeners;
	unsigned char m_keys_state[256] = {};
	unsigned char m_old_keys_state[256] = {};

};
