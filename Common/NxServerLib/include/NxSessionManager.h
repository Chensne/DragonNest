#pragma once

#include "NxDataStructure.h"

class NxServerApp;
class NxSession;

class NxSessionManager
{
public:
	NxSessionManager();
	virtual	~NxSessionManager();

	bool		Create(DWORD nSessionCnt, NxServerApp* pServerApp);
	void		Destroy();

	NxSession*	FindActiveSession(DWORD nUID);
	NxSession*	Find(DWORD nUID);

	NxSession*	ActiveSession(DWORD nUID);
	bool		InactiveSession(NxSession* pSession);
	bool		InactiveSession(DWORD nUID);

	DWORD		GetActiveSessionCnt()		{ return ( DWORD )m_ActiveSessionMap.size();	}

protected:

	virtual	void	_CreateSessionPool(DWORD nSessionCnt);
	virtual	void	_DeleteSessionPool();

protected:

	typedef	NxMap<DWORD, NxSession*>	SessionMap;
	typedef	SessionMap::iterator		SessionMapIt;

	typedef std::vector<NxSession*>		SessionArray;

	SessionArray		m_SessionArray;
	SessionMap			m_ActiveSessionMap;

	NxServerApp*			m_pServerApp;

};

