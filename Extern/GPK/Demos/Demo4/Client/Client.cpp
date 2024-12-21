// Client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <stdio.h>
#include <winsock2.h>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")

//
// ���� GPK Client ��ͷ�ļ�
//
#include <GPKitClt.h>
#pragma comment(lib, "GPKitClt.lib")

//
// GPK����ռ�����
//
using namespace SGPK;

BOOLEAN
RecvDynCodeFromServer(
	IN SOCKET ServerSocket
	);

//
// ȫ�ֱ�������
//
IGPKCltDynCode * pCltDynCode = NULL;

PUCHAR			DynCode = NULL;


void main() 
{

	//
	// ����GPK��������ʼ
	// NOTE: GPKStart����Ҫ����Ϣѭ������֮ǰִ��
	//
	pCltDynCode = GPKStart("http://127.0.0.1/Updates/TestGame", "GPKDEMO");
	if ( !pCltDynCode )
	{
		printf("GPKit Start failed.\n");
		return;
	}

	/*��ʼ��Winsock*/
	WSADATA wsaData;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
		printf("Error at WSAStartup()\n");

	/*����һ��Socket*/
	SOCKET m_socket;
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( m_socket == INVALID_SOCKET ) {
		printf( "Error at socket(): %ld\n", WSAGetLastError() );
		WSACleanup();
		return;
	}

	/*���ӵ�������*/
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
	// ���ӳɹ������ն�̬����
	//
	DynCode = (PUCHAR)VirtualAlloc(NULL, 32*1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!RecvDynCodeFromServer(m_socket))
		goto __end;

	/*�����������һ�����ݹؽ���һ�����Է������ķ�������*/
	int bytesSent;
	int bytesRecv = SOCKET_ERROR;
	char sendbuf[32] = "Client DATA...";
	int  sendDataLen = strlen(sendbuf) + 1;
	char recvbuf[32] = "";

	//
	// ���ܽ�Ҫ���͵�����
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
	// ���ܿͻ��˷���������
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
