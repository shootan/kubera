#include "IOCP.h"

BOOL IOCPServer::StartServer(int port)
{
	if(!this->InitServer(port))
		return FALSE;
	
	if(!this->InitThread())
		return FALSE;

	return TRUE;

}

BOOL IOCPServer::InitThread()
{

	return TRUE;
}