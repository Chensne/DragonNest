#pragma once

#include "NxThread.h"
#include "NxSocket.h"

class NxConnectionManager;

class NxAcceptor : public NxThread
{
public:
	NxAcceptor(NxConnectionManager* pConnectionManager, const char* szThreadNick = "Acceptor" );
	virtual	~NxAcceptor();

	bool			Open(const TCHAR* szIPAddress, u_short nPortNo );
	void			Close();
	virtual	void	Run();

private:
	NxConnectionManager*	m_pConnectionManager;
	NxSocket			m_ListenSocket;
	bool				m_bFlagKill;
	unsigned short		m_nPortNo;

private:
	NxAcceptor();
};
