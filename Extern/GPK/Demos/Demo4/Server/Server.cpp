// Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdio.h>
#include <winsock2.h>
#include <conio.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

//
// 包含 GPK Server 端头文件
//
#include <GPKitSvr.h>

//
// GPK Server 端相应的Lib
//
#ifdef _DEBUG
#pragma comment(lib, "GPKitSvrD.lib")
#else
#pragma comment(lib, "GPKitSvr.lib")
#endif

//
// GPK命令空间引用
//
using namespace std;
using namespace SGPK;

//
// 函数申明
//
BOOLEAN
InitGPKServer(
	VOID
	);

BOOLEAN
SendDynCodeToClient(
	IN SOCKET ClientSocket
	);

const char * GetRootPath(void);

//
// 全局变量申明
//
IGPKSvrDynCode * pSvrDynCode = NULL;
int				nCodeIdx = 0;

void main()
{
	string str;
	string dir[2];
	char szSvrDir[MAX_PATH];  
	char szCltDir[MAX_PATH];  
	int nBinCount;

	//
	// 初始化 GPK 服务器端组件
	//
	if ( !InitGPKServer() )
		return;						// 初始化失败


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

	/*绑定这个socket*/
	sockaddr_in service;

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	service.sin_port = htons( 27015 );

	if ( bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) {
		printf( "bind() failed.\n" );
		closesocket(m_socket);
		return;
	}

	/*侦听这个socket*/
	if ( listen( m_socket, 1 ) == SOCKET_ERROR )
		printf( "Error listening on socket.\n");

	/*等待客户端连接*/
	SOCKET AcceptSocket;

	while(1)
	{
		printf( "Waiting for a client to connect...\n" );
		while (1) {
			AcceptSocket = SOCKET_ERROR;
			while ( AcceptSocket == SOCKET_ERROR ) {
				AcceptSocket = accept( m_socket, NULL, NULL );
			}
			printf( "Client Connected.\n");
			m_socket = AcceptSocket; 

			cin >> str;

			break;
		}

		if(str == "exit")
			goto __end;
		else if(str == "load")                          //example: load Server1 Client1
		{
			cin >> dir[0] >> dir[1];
#ifdef BIT64                                                      
			sprintf(szSvrDir, "%s\\DynCodeBin\\%s", GetRootPath(),dir[0].c_str()); 
#else                                                             
			sprintf(szSvrDir, "%s\\DynCodeBin\\%s", GetRootPath(),dir[0].c_str());  
#endif                                                            

			sprintf(szCltDir, "%s\\DynCodeBin\\%s", GetRootPath(),dir[1].c_str());  

			printf("%s\n%s\n",szSvrDir,szCltDir);

			nBinCount = pSvrDynCode->LoadBinary(szSvrDir, szCltDir); //import dynamic code
			printf("Load Binary: %d binary are loaded\n", nBinCount);

			if(0 == nBinCount)
			{
				printf("Load bin file failed\n");  
			}
		}

		//
		// 客户端连接上了，发送动态代码给客户端
		//
		if ( !SendDynCodeToClient(m_socket) )
		{
			// 发送动态代码失败，结束
			goto __end;
		}

		/*接收客户端发来的数据并返回一个数据给客户端*/
		int bytesSent;
		int bytesRecv = SOCKET_ERROR;
		char sendbuf[32] = "Server DATA!!!";
		int  sendDataLen = strlen(sendbuf) + 1;
		char recvbuf[32] = "";

		bytesRecv = recv( m_socket, recvbuf, 32, 0 );
		printf( "Bytes Recv: %ld\n", bytesRecv );

		//
		// 解密客户端发来的数据
		//
		if(false == pSvrDynCode->Decode((unsigned char*)recvbuf, bytesRecv, nCodeIdx))
		{
			printf("Decode fail\n");
			goto __end;
		}
		printf("Decode Succeed\n");
		printf("Data after decode: %s\n", recvbuf);


		//
		// 加密将要发送的数据
		//
		if(false == pSvrDynCode->Encode((unsigned char*)sendbuf, sendDataLen, nCodeIdx))
		{
			printf("Encode fail\n");
			goto __end;
		}
		printf("Encode Succeed\n");

		bytesSent = send( m_socket, sendbuf, sendDataLen, 0 );
		printf( "Bytes Sent: %ld\n", bytesSent );
	}

__end:
	if(pSvrDynCode != NULL)
		pSvrDynCode->Release();

	WSACleanup();

#if _MSC_VER >= 1400
	_getch();
#else
	getch();
#endif

	return;
}

const char * GetRootPath(void)
{
	static char szPath[MAX_PATH];
	static bool bFirstTime = true;

	if(bFirstTime)
	{
		bFirstTime = false;
		GetModuleFileName(NULL, szPath, sizeof(szPath));
		char *p = strrchr(szPath, '\\');
		*p = '\0';
	}

	return szPath;
}

BOOLEAN
InitGPKServer(
	VOID
	)
/*++

函数说明:
	初始化GPKServer端组件

参数说明:
	无

返回值:
	TRUE - 成功
	FALSE - 失败

--*/
{
	char szSvrDir[MAX_PATH];
	char szCltDir[MAX_PATH];

	pSvrDynCode = GPKCreateSvrDynCode();

	if(NULL == pSvrDynCode)
	{
		printf("Create SvrDynCode component failed\n");
		return FALSE;
	}

#ifdef BIT64
	sprintf(szSvrDir, "%s\\DynCodeBin\\Server64", GetRootPath());
#else
	sprintf(szSvrDir, "%s\\DynCodeBin\\Server0", GetRootPath());
#endif

	sprintf(szCltDir, "%s\\DynCodeBin\\Client0", GetRootPath());

	int nBinCount = pSvrDynCode->LoadBinary(szSvrDir, szCltDir);
	printf("Load Binary: %d binary are loaded\n", nBinCount);

	if(0 == nBinCount)
	{
		printf("Load DynCode failed.\n");
		return FALSE;
	}

	return TRUE;
} 

BOOLEAN
SendDynCodeToClient(
	IN SOCKET ClientSocket
	)
/*++

函数说明:
	发送动态代码给客户端

参数说明:
	ClientSocket - 客户端Socket

返回值:
	TRUE - 成功
	FALSE - 失败

--*/
{
	int bytesSent;

	const unsigned char *pCode = NULL;
	
	nCodeIdx = pSvrDynCode->GetRandIdx();

	int nCodeLen = pSvrDynCode->GetCltDynCode(nCodeIdx, &pCode);
	
	if(nCodeLen < 0)
		return FALSE;

	bytesSent = send( ClientSocket, (const char*)pCode, nCodeLen, 0 );
	while (bytesSent != SOCKET_ERROR && bytesSent < nCodeLen)
	{
		Sleep(20);
		nCodeLen -= bytesSent;
		bytesSent = send( ClientSocket, (const char*)pCode + bytesSent, nCodeLen, 0);
	}

	if (bytesSent == SOCKET_ERROR)
	{
		printf("Send DynCode to Client Failed.\n");
		return FALSE;
	}

	return TRUE;
}
