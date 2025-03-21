
#include "probe.h"

#pragma warning(push)
#pragma warning(disable:4311 4312)

//	UDP 서버
SOCKET CreateUDPSocket(int port)
{
	SOCKET s;
	SOCKADDR_IN addr;

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (s == INVALID_SOCKET)
		return false;

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(s);
		return INVALID_SOCKET;
	}

	return s;
}

void _udp_service(void* param)
{
	int udpport = reinterpret_cast<int>(param);
	SOCKET s = CreateUDPSocket(udpport);

	printf("start UDP port probe server (%d)...\n", udpport);

	while(1)
	{
		char buf[256];
		sockaddr_in addr;
		int len, addrlen = sizeof(addr);

		struct _RETURN_UDP_ADDR
		{
			unsigned short	port[2];
			unsigned long	ip[2];
		}	packet;

		len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*) &addr, &addrlen);

#ifdef _DEBUG
		printf("read %d bytes %d.%d.%d.%d (%d)\n", len,
			addr.sin_addr.S_un.S_un_b.s_b1,
			addr.sin_addr.S_un.S_un_b.s_b2,
			addr.sin_addr.S_un.S_un_b.s_b3,
			addr.sin_addr.S_un.S_un_b.s_b4,
			ntohs(addr.sin_port));
#endif

		packet.port[0] = packet.port[1] = ntohs(addr.sin_port);
		packet.ip[0] = packet.ip[1] = addr.sin_addr.S_un.S_addr;

		sendto(s, (char*)&packet, sizeof(packet), 0, (struct sockaddr*) &addr, addrlen);
	}

	closesocket(s);
}

//	TCP 서버
SOCKET CreateTCPListenSocket(int port)
{
	SOCKADDR_IN addr;
	SOCKET s;

	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET)
		return INVALID_SOCKET;

	addr.sin_family				= AF_INET;
	addr.sin_port				= htons(port);
	addr.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(s);

		return INVALID_SOCKET;
	}

	if (listen(s, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(s);
		return INVALID_SOCKET;
	}

	return s;
}

#include <map>
#include <vector>

void ConnectTest(unsigned long ip, unsigned short port)
{
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addr;

	if (s == INVALID_SOCKET)
		return ;

	addr.sin_family				= AF_INET;
	addr.sin_port				= htons(0);
	addr.sin_addr.S_un.S_addr	= htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(s);

		return;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = ip;

	if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
#ifdef _DEBUG
		printf("연결실패 (%d.%d.%d.%d:%d)\n",
			addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2,
			addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4, port);
#endif
		closesocket(s);
		return ;
	}

#ifdef _DEBUG
	printf("연결성공 (%d.%d.%d.%d:%d)\n",
		addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2,
		addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4, port);
#endif
	closesocket(s);
}

void _tcp_con_service(void* param)
{
	_CONADDR * list = (_CONADDR*) param;
	for(int i=0; list[i].addr != 0; i++)
		ConnectTest(list[i].addr, list[i].port);
	delete [] list;
}

void _udp_con_service(void* param)
{
	_CONADDR * list = (_CONADDR*) param;
	SOCKET hUDP = CreateUDPSocket(0);
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;

	for(int i=0; list[i].addr != 0; i++)
	{
		addr.sin_port = htons(list[i].port);
		addr.sin_addr.s_addr = list[i].addr;
		sendto(hUDP, "OTL", 3, 0, (struct sockaddr*) &addr, sizeof(addr));
	}

	closesocket(hUDP);
}

void _tcp_service(void* param)
{
	int tcpport = (int) param;
	SOCKET hListen = CreateTCPListenSocket(tcpport);
	fd_set fds;
	std::map <SOCKET, _SOCKET_DATA *> list;
	std::vector <_CONADDR> conlist;
	std::vector <_CONADDR> udplist;
	std::vector <_SOCKET_DATA *> empty;
	_SOCKET_DATA * b;
	unsigned long lastcon = 0;
	unsigned int i;

	if (hListen == INVALID_SOCKET)
	{
		printf("TCP 소켓 (port:%d)를 열 수 없습니다.\n", tcpport);
		return;
	}

	printf("start TCP probe server (%d)...\n", tcpport);

	FD_ZERO(&fds);
	FD_SET(hListen, &fds);

	while(1)
	{
		fd_set testfds = fds;
		timeval tm;

		tm.tv_sec	= 0;
		tm.tv_usec	= 500*1000;

		int r = select(FD_SETSIZE, (fd_set*)&testfds, (fd_set*)0, (fd_set*)0,
					   udplist.size() == 0 && conlist.size() == 0 ? NULL : &tm);

		if (r != -1)
		{
			for(i=0; i<testfds.fd_count; i++)
			{
				SOCKET socket = testfds.fd_array[i];

				if (socket == hListen)
				{
					SOCKADDR_IN addr;

					int addrlen = sizeof(addr);
					SOCKET client = accept(hListen, (struct sockaddr *)&addr, &addrlen);

					if (client == INVALID_SOCKET)
					{
						printf("Accpet Error(%d)\n", GetLastError());
					}	else
					if (fds.fd_count >= sizeof(fds.fd_array)/sizeof(*fds.fd_array))
					{
						//	MAX치
						closesocket(client);
					}	else
					{
						if (empty.size() > 0)
						{
							b = empty[empty.size()-1];
							empty.pop_back();
						}	else
							b = new _SOCKET_DATA;

						memcpy(&b->addr, &addr, sizeof(addr));
						b->len = 0;

						list[client] = b;
						FD_SET(client, &fds);
					}
				}	else
				{
					std::map <SOCKET, _SOCKET_DATA *> ::iterator ii;
					unsigned long len;

					ii = list.find(socket);
//					_ASSERT(ii != list.end());
					b = (*ii).second;

					ioctlsocket(socket, FIONREAD, &len);

					if (len > 0)
					{
						if (ii != list.end())
						{
							len = recv(socket, b->buff, sizeof(b->buff) - b->len, 0);
							b->len += len;

							if (b->len >= 4)
							{
								unsigned short port[2];
								memcpy(port, b->buff, 4);

								if (port[1] == (unsigned short)(port[0]-7071) ||
									port[1] == (unsigned short)(port[0]-3212)) // verify
								{
									_CONADDR c;
									c.addr = b->addr.sin_addr.S_un.S_addr;
									c.port = port[0];
									conlist.push_back(c);
								}	else
								if (port[1] == (unsigned short)(port[0]-9321) ||
									port[1] == (unsigned short)(port[0]-1110)) // verify
								{
									_CONADDR c;
									c.addr = b->addr.sin_addr.S_un.S_addr;
									c.port = port[0];
									udplist.push_back(c);
								}
							}
						}
					}

					if (len != SOCKET_ERROR || len == 0 || b->len >= 4)
					{
						closesocket(socket);
						list.erase(ii);
						empty.push_back(b);
						FD_CLR(socket, &fds);
					}
				}
			}

			if (timeGetTime() - lastcon > 500) // 최소 간격은 0.5초
			{
				lastcon = timeGetTime();

				if (conlist.size() > 0)
				{
					_CONADDR * conset = new _CONADDR [conlist.size()+1];
					for(i=0; i<conlist.size(); i++)
						conset[i] = conlist[i];
					conset[i].addr = 0;
					conlist.clear();

					_beginthread(_tcp_con_service, 0, (void*) conset);
				}	else
				if (udplist.size() > 0)
				{
					_CONADDR * udpset = new _CONADDR [udplist.size()+1];
					for(i=0; i<udplist.size(); i++)
						udpset[i] = udplist[i];
					udpset[i].addr = 0;
					udplist.clear();

					_beginthread(_udp_con_service, 0, (void*) udpset);
				}
			}
		}	else
		{
			printf("ERROR(%d)\n", GetLastError());
		}
	}

	closesocket(hListen);
}

#ifdef _CONSOLEMODE
void main(int argc, char * argv[])
#else
void probemain(int argc, char * argv[])
#endif
{
	WSADATA	WSAData;
	int udpport, tcpport;

	if (argc <= 2)
		return;

	if (WSAStartup(MAKEWORD(1, 1), &WSAData))
		return;

	udpport = atoi(argv[1]);
	tcpport = atoi(argv[2]);

	_beginthread(_udp_service, 0, (void*) udpport);
	_beginthread(_tcp_service, 0, (void*) tcpport);

	Sleep(INFINITE);
}

#pragma warning(pop)