#pragma once


class CDNGameSession;
//class CDNUserConnection;
class CGameSession
{
public:
	CGameSession();
	virtual ~CGameSession();

protected:

public:
	virtual void OnDispatchMessageUdp(CDNGameSession * pSession, unsigned int iHeader, char * pData, int iLen) {}
	virtual int OnDispatchMessageTcp(CDNGameSession * pSession, char *pData) { return -1; }

	//virtual void OnDispatchMessageUdp(CDNUserConnection *pConnection, unsigned int header, char * data, int size) {}
	//virtual void OnDispatchMessageTcp( CDNUserConnection *pConnection, char *pData ) {}
	//virtual void OnDispatchMessageUdp(CDNUserConnection *pConnection, unsigned int header, char * data, int size) {}
	//virtual int OnDispatchMessageTcp( CDNUserConnection *pConnection, char *pData ) { return -1; }

	virtual DWORD GetProcessID();
	virtual int GetSessionID();
};