#include "stdafx.h"
#include "IOCP.h"
#include "Timer.h"
#include "ST.h"

ST* ST::instance = NULL;
IOCPServer Server;
UINT WINAPI Th(LPVOID arg);
CTimer Timer;
Vector3 Root1[13];
Vector3 Root2[13];
Vector3 Root3[13];
Vector3 Root4[13];

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

void SetRoot()
{
	for(int i=0; i<14; i++)
	{
		Root1[i].y = 0;
	}
	Root1[0].x = 546;
	Root1[1].x = 446;
	Root1[2].x = 446;
	Root1[3].x = 436;
	Root1[4].x = 326;
	Root1[5].x = 245;
	Root1[6].x = 158;
	Root1[7].x = 0;
	Root1[8].x = -31;
	Root1[9].x = -230;
	Root1[10].x = -350;
	Root1[11].x = -437;
	Root1[12].x = -446;

	Root1[0].z = -14;
	Root1[1].z = 7;
	Root1[2].z = 88;
	Root1[3].z = 196;
	Root1[4].z = 193;
	Root1[5].z = 188;
	Root1[6].z = 188;
	Root1[7].z = 188;
	Root1[8].z = 188;
	Root1[9].z = 188;
	Root1[10].z = 188;
	Root1[11].z = 188;
	Root1[12].z = 188;

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