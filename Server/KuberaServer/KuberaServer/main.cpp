#include "stdafx.h"
#include "IOCP.h"

static IOCPServer Server;
int main()
{
	Server.StartServer(9000);
	while(1)
	{
		Sleep(1000);
	}
	return 0;
}