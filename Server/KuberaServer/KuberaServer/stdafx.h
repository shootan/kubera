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
