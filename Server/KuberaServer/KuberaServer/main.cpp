#include "stdafx.h"
#include "IOCP.h"

IOCPServer Server;
float a;
UINT WINAPI Th(LPVOID arg);
int main()
{
	a = 0.0f;
	Server.StartServer(9000);
	//unsigned int d;
	//HANDLE y  = (HANDLE)_beginthreadex(NULL, 0, Th, NULL, 0, &d);
	while(1)
	{
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