#ifndef _PROBE_H_INCLUDED
#define _PROBE_H_INCLUDED

#include <process.h>
#include <winsock.h>
#include <stdio.h>
#include <map>
#include <vector>

struct _CONADDR {
	unsigned long	addr;
	unsigned short	port;
} ;


struct _SOCKET_DATA {
	SOCKADDR_IN addr;
	int port;
	char buff[16];
	int len;
} ;

//	UDP 서버
SOCKET CreateUDPSocket(int port);
void _udp_service(void* param);
SOCKET CreateTCPListenSocket(int port);
void ConnectTest(unsigned long ip, unsigned short port);
void _tcp_con_service(void* param);
void _udp_con_service(void* param);
void _tcp_service(void* param);
void probemain(int argc, char * argv[]);
#endif