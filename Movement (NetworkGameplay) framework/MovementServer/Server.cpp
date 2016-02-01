#include "ServerApp.h"
#include <Windows.h>

int main()
{
	ServerApp app;
	bool end = false;
	while (!end)
	{
		Sleep(10);
		end = app.Loop();
	}
	return 0;
}