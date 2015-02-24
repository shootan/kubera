#include "stdafx.h"
#include "IOCP.h"

IOCPServer Server;
int main()
{
	Server.StartServer(9000);
	while(1)
	{
		Sleep(500);
	}
	return 0;
}
