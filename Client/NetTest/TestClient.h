#pragma once
#include "clientsession.h"

class CTestClient :	public CClientSession
{
public:
	CTestClient(void);
	~CTestClient(void);

protected:
	void OnIoRead(void);
	void OnIoConnected(void);
	void OnIoDisconnected(void);

public:

};
