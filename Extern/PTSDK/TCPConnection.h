#pragma once
#include "sdkconfig.h"
#include "byteio.h"
#include "ThreadMaster.h"
#include <WinSock2.h>
#include "InfoCast.h"
#include "Lock\Lock.h"
#include <string>
using namespace std;
using namespace CleverLock;

#define SUCCEESS	0
class PTSDK_STUFF CTCPConnection :
	public CByteIO,public CInfoNode
{
public:
	CTCPConnection(void);
	~CTCPConnection(void);

#ifdef GAME_CHUANQI
	enum TCPLinkType{DispatchLink=0, StaticLink};
#endif

	string	m_strServer;
	int		m_ServerPort;	

	int		Connect( const char * ip = NULL, int port = -1, bool b_forceReconnect = false, const char *firstData = NULL, int firstDatalen = 0);	
	void	DisConnect();
	bool	IsConnect();
	void	AutoConnect(bool b_auto = true);
	
#ifdef GAME_CHUANQI
	void	SetLinkType(int n_LinkType);
	int		GetLinkType();
#endif

	virtual bool IsValuable();
	virtual bool CanSend(int time);
	virtual bool CanRecv(int time);
	virtual int  Send(const char * buf, int bufLen);
	virtual int  Recv(char * buf, int bufLen);
	
protected:
    
	CWithLock m_SocketLock;
	SOCKET	m_Socket;
	bool	m_bAutoConnect;
#ifdef GAME_CHUANQI
	int		m_nLinkType;
#endif

	static void AutoConnect(void * parameter);
	CThreadMaster m_AutoConnectThread;	
};
