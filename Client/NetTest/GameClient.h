#pragma once
#include "clientsession.h"

class CGameClient :	public CClientSession
{
public:
	CGameClient(void);
	~CGameClient(void);

protected:
	void OnIoRead(void);
	void OnIoConnected(void);
	void OnIoDisconnected(void);

public:
	BOOL Begin( USHORT nRemotePort );
};
