// Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdio.h>
#include <winsock2.h>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")

//
// 包含 GPK Client 端头文件
//
#include <GPKitClt.h>
#pragma comment(lib, "GPKitClt.lib")

//
// GPK命令空间引用
//
using namespace SGPK;

BOOLEAN
RecvDynCodeFromServer(
	IN SOCKET ServerSocket
	);

//
// 全局变量申明
//
IGPKCltDynCode * pCltDynCode = NULL;

PUCHAR			DynCode = NULL;


void main() 
{

	//
	// 启动GPK，保护开始
	// NOTE: GPKStart必须要在消息循环建立之前执行
	//
	pCltDynCode = GPKStart("http://127.0.0.1/Updates/TestGame", "GPKDEMO");
	if ( !pCltDynCode )
	{
		printf("GPKit Start failed.\n");
		return;
	}

	/*初始化Winsock*/
	WSADATA wsaData;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
		printf("Error at WSAStartup()\n");

	/*创建一个Socket*/
	SOCKET m_socket;
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( m_socket == INVALID_SOCKET ) {
		printf( "Error at socket(): %ld\n", WSAGetLastError() );
		WSACleanup();
		return;
	}

	/*连接到服务器*/
	sockaddr_in clientService;

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	clientService.sin_port = htons( 27015 );

	if ( connect( m_socket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) {
		printf( "Failed to connect.\n" );
		WSACleanup();
		return;
	}

	//
	// 连接成功，接收动态代码
	//
	DynCode = (PUCHAR)VirtualAlloc(NULL, 32*1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!RecvDynCodeFromServer(m_socket))
		goto __end;

	/*向服务器发送一个数据关接收一个来自服务器的返回数据*/
	int bytesSent;
	int bytesRecv = SOCKET_ERROR;
	char sendbuf[32] = "Client DATA...";
	int  sendDataLen = strlen(sendbuf) + 1;
	char recvbuf[32] = "";

	//
	// 加密将要发送的数据
	//
	if(false == pCltDynCode->Encode((unsigned char*)sendbuf, sendDataLen))
	{
		printf("Encode fail\n");
		goto __end;
	}
	printf("Encode Succeed\n");

	bytesSent = send( m_socket, sendbuf, sendDataLen, 0 );
	printf( "Bytes Sent: %ld\n", bytesSent );


	bytesRecv = recv( m_socket, recvbuf, 32, 0 );
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ) {
		printf( "Connection Closed.\n");
		goto __end;
	}

	if (bytesRecv < 0)
		goto __end;

	printf( "Bytes Recv: %ld\n", bytesRecv );

	//
	// 解密客户端发来的数据
	//
	if(false == pCltDynCode->Decode((unsigned char*)recvbuf, bytesRecv))
	{
		printf("Decode fail\n");
		goto __end;
	}
	printf("Decode Succeed\n");
	printf("Data after decode: %s\n", recvbuf);

__end:

	if (pCltDynCode != NULL)
		pCltDynCode->Release();

	WSACleanup();

#if _MSC_VER >= 1400
	_getch();
#else
	getch();
#endif

	return;
}

BOOLEAN
RecvDynCodeFromServer(
	IN SOCKET ServerSocket
	)
{
	int bytesRecv;

	bytesRecv = recv(ServerSocket, (char *)DynCode, 32*1024, 0);
	if (bytesRecv > 0)
	{
		pCltDynCode->SetDynCode(DynCode, bytesRecv);

		return TRUE;
	}

	return FALSE;
}
