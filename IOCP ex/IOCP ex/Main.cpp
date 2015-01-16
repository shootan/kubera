#pragma comment(lib, "ws2_32")
#include <winsock2.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <list>
using namespace std;
#define BUFSIZE 512
// 소켓정보를저장하기위한구조체 
struct SOCKETINFO 
{ 
	OVERLAPPED overlapped; 
	SOCKET sock; 
	char buf[BUFSIZE+1]; 
	int recvbytes; 
	int sendbytes; 
	WSABUF wsabuf; 
};

list<SOCKET> list1;
// 소켓입출력함수 
DWORD WINAPI WorkerThread(LPVOID arg);
// 오류출력함수 
void err_quit(char *msg); 
void err_display(char *msg); 

int accept_proc(SOCKET *listen_sock, HANDLE *hcp) 
{ 
	
	int retval; 
	// accept() 
	SOCKADDR_IN clientaddr; 
	int addrlen = sizeof(clientaddr); 
	SOCKET client_sock = accept(*listen_sock, (SOCKADDR *)&clientaddr, &addrlen); 
	list1.push_back(client_sock);
	if(client_sock == INVALID_SOCKET) 
	{ 
		err_display("accept()"); 
		return -1; 
	} 
	printf("[TCP 서버] 클라이언트접속: IP 주소= %s, 포트번호= %d \n", 
		inet_ntoa(clientaddr.sin_addr), 
		ntohs(clientaddr.sin_port)); 

	// 소켓과입출력완료포트연결 
	HANDLE hResult = CreateIoCompletionPort((HANDLE)client_sock, 
		*hcp, (DWORD)client_sock, 0); 
	if(hResult == NULL) 
	{ 
		err_quit("create iocp"); 
	} 

	// 소켓정보구조체할당 
	SOCKETINFO *ptr = new SOCKETINFO; 
	if(ptr == NULL) 
	{ 
		err_quit("[오류] 메모리부족!!\n"); 
	} 
	ZeroMemory(&(ptr->overlapped), sizeof(ptr->overlapped)); 
	ptr->sock = client_sock; 
	ptr->recvbytes = 0; 
	ptr->sendbytes = 0; 
	ptr->wsabuf.buf = ptr->buf; 
	ptr->wsabuf.len = BUFSIZE; 

	// 비동기입출력시작 
	DWORD recvbytes; 
	DWORD flags = 0; 
	retval = WSARecv(client_sock, &(ptr->wsabuf), 1, &recvbytes, &flags, &(ptr->overlapped), NULL);
	if(retval == SOCKET_ERROR) 
	{ 
		if(WSAGetLastError() != ERROR_IO_PENDING) 
		{ 
			err_display("wsarecv()"); 
		} 
		return -1; 
	}
	

	return 0; 
} 

int is_log_out(DWORD *cbTransferred, SOCKET *client_sock, SOCKETINFO *ptr, SOCKADDR_IN *clientaddr, int retval) 
{         
	// 비동기입출력결과확인 
	if(retval == 0 || *cbTransferred == 0) 
	{ 
		if(retval == 0) 
		{ 
			DWORD temp1, temp2; 
			WSAGetOverlappedResult(ptr->sock, &(ptr->overlapped), 
				&temp1, FALSE, &temp2); 
			err_display("WSAGetOverlappedResult()"); 
		} 
		closesocket(ptr->sock); 
		printf("[TCP 서버] 클라이언트종료: IP 주소= %s, 포트번호= %d \n", 
			inet_ntoa(clientaddr->sin_addr), 
			ntohs(clientaddr->sin_port)); 
		/*list< SOCKET >::iterator iterEnd = list1.end();
		for(list< int >::iterator iterPos = list1.begin(); iterPos != iterEnd; 	++iterPos )
		{
			
		}*/
		delete ptr;

		return 1; 
	} 
	return 0; 
} 

int send_proc(SOCKETINFO *ptr) 
{ 
	int retval; 

	// 데이터보내기 
	ZeroMemory(&(ptr->overlapped), sizeof(ptr->overlapped)); 
	ptr->wsabuf.buf = ptr->buf + ptr->sendbytes; 
	ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes; 

	DWORD sendbytes; 
	retval = WSASend(ptr->sock, &(ptr->wsabuf), 1, &sendbytes, 0, &(ptr->overlapped),
		NULL); 
	if(retval == SOCKET_ERROR) 
	{ 
		if(WSAGetLastError() != WSA_IO_PENDING) 
		{ 
			err_display("WSASend()"); 
		} 
		return -1; 
	} 
	return 0; 
} 

int recv_proc(SOCKETINFO *ptr) 
{ 
	int retval; 

	ptr->recvbytes = 0; 

	// 데이터받기 
	ZeroMemory(&(ptr->overlapped), sizeof(ptr->overlapped)); 
	ptr->wsabuf.buf = ptr->buf; 
	ptr->wsabuf.len = BUFSIZE; 

	DWORD recvbytes; 
	DWORD flags = 0; 
	retval = WSARecv(ptr->sock, &(ptr->wsabuf), 1, 
		&recvbytes, &flags, &(ptr->overlapped), NULL); 
	if(retval == SOCKET_ERROR) 
	{ 
		if(WSAGetLastError() != WSA_IO_PENDING) 
		{ 
			err_display("WSARecv()"); 
		} 
		return -1; 
	} 
	return 0;
} 

int main(int argc, char *argv[]) 
{ 
	int retval; 
	struct Stack *pstack;
	pstack = new struct Stack;
	retval = pstack->Count();

	// 윈속초기화 
	WSADATA wsa; 
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) 
		return -1;

	retval = WSAGetLastError();

	// 입출력완료포트생성 
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); 
	if(hcp == NULL) 
		return -1; 

	// CPU 갯수확인 
	SYSTEM_INFO si; 
	GetSystemInfo(&si); 

	// (CPU 개수* 2)개의작업자스레드생성 
	HANDLE hThread; 
	DWORD ThreadId; 
	for(int i=0; i < (int)si.dwNumberOfProcessors*2; ++i) 
	{ 
		hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, &ThreadId); 
		if(hThread == NULL) 
			return -1; 
		CloseHandle(hThread); 
	} 

	// socket() 
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0); 
	if(listen_sock == INVALID_SOCKET) 
		err_quit("socket()"); 

	// bind() 
	SOCKADDR_IN serveraddr; 
	ZeroMemory(&serveraddr, sizeof(serveraddr)); 
	serveraddr.sin_family = AF_INET; 
	serveraddr.sin_port = htons(9000); 
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr)); 
	if(retval == SOCKET_ERROR) 
		err_quit("bind()"); 

	// listen() 
	retval = listen(listen_sock, SOMAXCONN); 
	if(retval == SOCKET_ERROR) 
		err_quit("listen()"); 

	while(1) 
	{ 
		if(accept_proc(&listen_sock, &hcp) == -1) 
			continue; 
	} 

	// 윈속종료 
	WSACleanup(); 
	return 0; 
} 

DWORD WINAPI WorkerThread(LPVOID arg) 
{ 
	HANDLE hcp = (HANDLE)arg; 
	int retval; 

	while(1) 
	{ 
		// 비동기입출력완료기다리기 
		DWORD cbTransferred; 
		SOCKET client_sock; 
		SOCKETINFO *ptr;
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred, 
			(LPDWORD)&client_sock, (LPOVERLAPPED*)&ptr, INFINITE); 

		// 클라이언트정보얻기 
		SOCKADDR_IN clientaddr; 
		int addrlen = sizeof(clientaddr); 
		getpeername(ptr->sock, (SOCKADDR*)&clientaddr, &addrlen); 

		if(is_log_out(&cbTransferred, &client_sock, ptr, &clientaddr, retval) == 1)
			continue;
			

		// 데이터전송량갱신 
		if(ptr->recvbytes == 0) 
		{ 
			ptr->recvbytes = cbTransferred; 
			ptr->sendbytes = 0; 

			// 받은데이터출력 
			ptr->buf[ptr->recvbytes] = '\0'; 
			printf("[TCP/%s:%d]%s\n", 
				inet_ntoa(clientaddr.sin_addr), 
				ntohs(clientaddr.sin_port), ptr->buf); 
		} 
		else 
		{ 
			ptr->sendbytes += cbTransferred; 
		} 

		if(ptr->recvbytes > ptr->sendbytes) 
		{ 
			if(send_proc(ptr) == -1) 
				continue; 
		} 
		else 
		{ 
			if(recv_proc(ptr) == -1) 
				continue; 
		} 
	} 

	return 0; 
} 

// 소켓함수오류출력후종료 
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
// 소켓함수오류출력 
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