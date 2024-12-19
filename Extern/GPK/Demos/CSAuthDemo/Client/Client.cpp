#include <WinSock2.h>
#include "stdio.h"
#include "GPKitClt.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"GPKitClt.lib")
int port=27015;
char szIp[255]="127.0.0.1";

//SGPK::IGPKCltDynCode *pGpk;

void GPK_API SendToServer(void *socket,unsigned char *lpData,unsigned long nLen)
{
	send(*(SOCKET*)socket,(char*)lpData,nLen,0);
}
int main()
{
	SOCKET client;
	WSADATA wsa;
	sockaddr_in addr;
	int ret;
	char         szBuff[128];  

	SGPK::IGPKCltDynCode *pCltDynCode = SGPK::GPKStart("http://autopatch.gpk.sdo.com/gpk", "gpkdemo");
	if(pCltDynCode == NULL)
	{
		printf("Create IGPKCltDynCode failed!\n");
		return 0;
	};

	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
		printf("WSAStartup error!\n");
	client=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=inet_addr(szIp);
	if(connect(client,(struct sockaddr *)&addr,sizeof(addr))!=0)
		printf("connect error\n");
	Sleep(100);
	while(1)
	{
		ret   =   recv(client,szBuff,128,0); 
		ret = pCltDynCode->ProcessAuth(&client,SendToServer,(unsigned char *)szBuff,ret);

	}
	

	WSACleanup();

	getchar();

	return 0;
}