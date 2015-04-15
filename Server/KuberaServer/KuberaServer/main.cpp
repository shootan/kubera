#include "stdafx.h"
#include "IOCP.h"
#include "Timer.h"

IOCPServer Server;
UINT WINAPI Th(LPVOID arg);
CTimer Timer;
int main()
{
	Server.StartServer(9000);
	//unsigned int d;
	//HANDLE y  = (HANDLE)_beginthreadex(NULL, 0, Th, NULL, 0, &d);
	while(1)
	{
		Timer.Tick(60);
		//system("cls");
		//Timer.GetFrameRate();
		Server.SendData();
	}
	return 0;
}

UINT WINAPI Th(LPVOID arg)
{
	int b = 0;
	while(1)
	{
		b += 1;
		Sleep(500);
	}
	return 0;
}