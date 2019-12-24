#include "AppWindow.h"


int main()
{
	AppWindow app;
	if (app.init())
	{
		while (app.isRunning())
		{
			app.broadcast();
		}
	}

	return 0;
}
