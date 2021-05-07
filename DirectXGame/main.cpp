// Copyright (c) 2019  PardCode.
// All rights reserved.
//
// This file is part of CPP-3D-Game-Tutorial-Series Project, accessible from https://github.com/PardCode/CPP-3D-Game-Tutorial-Series
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License 
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.


#include "GraphicsEngine.h"
#include "InputSystem.h"
#include "VillageDemo.h"

#include <exception>
#include <iostream>


int main()
{
	try
	{
		GraphicsEngine::create();
		InputSystem::create();
	}
	catch (...) { return -1; }

	{
		try
		{
			VillageDemo app;
			while (app.isRunning())
			{
				app.broadcast();
			}
		}
		catch (const char* msg) {
			std::cerr << msg << std::endl;
			InputSystem::release();
			GraphicsEngine::release();
			return -1;
		};
	}

	InputSystem::release();
	GraphicsEngine::release();
	return 0;
}
