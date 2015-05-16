#include "stdafx.h"
#include "IOCP.h"
#include "Timer.h"
#include "ST.h"


ST* ST::instance = NULL;
IOCPServer Server;
UINT WINAPI Th(LPVOID arg);
CTimer Timer;


int main()
{
	Server.StartServer(9000);
	Server.Arrange.SetRoot();

	float time = 0.0f;
	
	//unsigned int d;
	//HANDLE y  = (HANDLE)_beginthreadex(NULL, 0, Th, NULL, 0, &d);
	while(1)
	{
		Timer.Tick(60);
		//system("cls");
		//Timer.GetFrameRate();

		//printf("8: %d, 3024 : %d   0 : %d .\n", Server.Check8, Server.Check3000, Server.Check0);
		
		Server.ArrangeDataInfo(Timer.GetTimeElapsed());

		//Server.SendData(Timer.GetTimeElapsed());
	}
	return 0;
}


//UINT WINAPI Th(LPVOID arg)
//{
//	int b = 0;
//	while(1)
//	{
//		b += 1;
//		Sleep(500);
//	}
//	return 0;
//}