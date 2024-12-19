#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0501	
#endif	

#include"winsock2.h"  

#include<stdio.h>  
#include<stdlib.h>  


#include "GPKitSvr.h"

#pragma comment(lib,"GPKitSvr.lib")
#pragma comment(lib,"ws2_32.lib")

SGPK::IGPKSvrDynCode *pGpk = NULL;

#define   DEFAULT_PORT     27015 
#define   DEFAULT_COUNT   25  
#define   DEFAULT_BUFFER   128   



int   iPort   =   DEFAULT_PORT   ;    
BOOL   bInterface   =   FALSE,   
bRecvOnly   =   FALSE;      
char   szAddress[128];              


struct myListenSocket
{
	char clientIp[255];
	int port;
	SOCKET conSocket;
};

 
DWORD   WINAPI   ClientThread(LPVOID   lpParam)  
{  
	myListenSocket     *sock   =   (myListenSocket*)lpParam;  
	char         szBuff[DEFAULT_BUFFER];  
	int           ret;  
	SGPK::IGPKCSAuth *p_gpkcmd;
	p_gpkcmd = pGpk->AllocAuthObject();

	

	while(1)  
	{  
		const unsigned char *p = NULL;
		int len = p_gpkcmd->GetAuthData(&p,NULL,NULL);

		send(sock->conSocket,(const char *)p,len,0);
		Sleep(500);
		ret   =   recv(sock->conSocket,szBuff,DEFAULT_BUFFER,0);  
		if(ret   ==   0)                                 //Graceful   close  
			break;  

		
		ret = p_gpkcmd->CheckAuthReply(&p,(unsigned char*)szBuff,ret);


		printf("result: %d\n",ret);
		if( ret != 0 )
		{
			printf("result info: %s\n",&p[8]);
		}

		Sleep(5000);   //根据需要设置检查间隔，一般设置为3-5分钟

	}  

	p_gpkcmd->Release();
	return   0;  
}  
 
int   main(int   argc,char   **argv)  
{  
	WSADATA   wsd;  
	SOCKET     sListen,  
		sClient;  
	int   iAddrSize;  
	HANDLE     hThread;  
	DWORD       dwThreadId;  
	struct   sockaddr_in   local,  client;   


	pGpk = SGPK::GPKCreateSvrDynCode();
	if(pGpk == NULL)
	{
		printf("Create gpk_server failed!\n");
		return 0;
	}



	struct myListenSocket* mls=new myListenSocket();
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0)  
	{  
		printf("Failed   to   load   Winsock!\n");  
		return   1;  
	}  
	
	sListen     =   socket(AF_INET,SOCK_STREAM,IPPROTO_IP);  
	if(sListen   ==   SOCKET_ERROR)  
	{  
		printf("socket()   failed:%d\n",WSAGetLastError());  
		return   1;  
	}  
	
	local.sin_addr.s_addr   =   htonl(INADDR_ANY);  
	local.sin_family   =   AF_INET;  
	local.sin_port   =   htons(iPort);  

	if(bind(sListen,(struct   sockaddr   *)&local,sizeof(local))   ==   SOCKET_ERROR)  
	{  
		printf("bind()   failed:%d\n",WSAGetLastError());      
		return   1;  
	}  
	listen(sListen,8);  

	while(1)  
	{  
		iAddrSize   =   sizeof(client);  
		sClient   =   accept(sListen,(struct   sockaddr   *)&client,&iAddrSize);  
		if(sClient   ==   INVALID_SOCKET)  
		{  
			break;  
		}  

		strcpy(mls->clientIp,inet_ntoa(client.sin_addr));
		mls->port=ntohs(client.sin_port);
		mls->conSocket=sClient;
		hThread   =   CreateThread(NULL,0,ClientThread,(LPVOID)mls,0,&dwThreadId);  
		if(hThread   ==   NULL)  
		{  
			printf("CreateThread()   failed:%d\n",WSAGetLastError());  
			break;  
		}  
		CloseHandle(hThread);  
	}  
	closesocket(sListen);  
	WSACleanup();  
	return   0;  
}   
