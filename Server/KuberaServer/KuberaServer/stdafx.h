#pragma once
#pragma comment(lib, "ws2_32")
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>


// BUFSIZE
#define BUFSIZE 1024*2

// STATE
struct UserState{
#define CONNECT		0
#define WAIT			1
#define PLAYGAME		2
};

// �����Լ�������������� 
void err_quit(char *msg) 
{ 
	LPVOID lpMsgBuf; 
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER| 
		FORMAT_MESSAGE_FROM_SYSTEM, 
		NULL, WSAGetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR)&lpMsgBuf, 0, NULL); 
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR); 
	LocalFree(lpMsgBuf); 
	exit(-1); 
}
// �����Լ�������� 
void err_display(char *msg) 
{ 
	LPVOID lpMsgBuf; 
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER| 
		FORMAT_MESSAGE_FROM_SYSTEM, 
		NULL, WSAGetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR)&lpMsgBuf, 0, NULL); 
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf); 
	LocalFree(lpMsgBuf); 
}