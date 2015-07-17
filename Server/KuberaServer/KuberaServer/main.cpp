#include "stdafx.h"
#include "IOCP.h"
#include "Timer.h"
#include "ST.h"
#include "ArrangeData.h"


ST* ST::instance = NULL;
IOCPServer Server;
UINT WINAPI Th(LPVOID arg);
CTimer Timer;


int main()
{
	Server.StartServer(9000);
	
	ArrangeData AD;
	Server.ArrangeDataInfo(&AD);

	
	int si = sizeof(int);
	int sss = sizeof(PlayerPacket);
	
	//unsigned int d;
	//HANDLE y  = (HANDLE)_beginthreadex(NULL, 0, Th, NULL, 0, &d);
//	printf("%d", si+sss);
	while(1)
	{
		Timer.Tick(60);

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