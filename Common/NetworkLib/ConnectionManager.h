#pragma once

#if defined(_SERVER)

#include "Connection.h"

using namespace std;

const BYTE CONNECTIONPROCESSCOUNT = 40;	// 초당 오브젝트 처리개수

typedef vector<CConnection*> TVecConnections;
typedef map<UINT, CConnection*> TMapConnections;

class CConnectionManager
{
private:
protected:
	int m_nProcessCount;
	TVecConnections m_Connections;
	TMapConnections m_KeyConnections;

#ifdef _USE_ACCEPTEX
public:
#endif
	CSyncLock m_ConSync;

public:
	CConnectionManager(void);
#ifdef _USE_ACCEPTEX
	CConnectionManager( UINT uiWorkerThreadCount );
#endif
	virtual ~CConnectionManager(void);

	virtual void Clear();
	virtual void DoUpdate(DWORD CurTick);

	virtual CConnection* AddConnection(const char *pIp, const USHORT nPort) = 0;
	bool DelConnection(CConnection *pCon);
	int PushConnection(CConnection *pCon);

#ifdef _USE_ACCEPTEX
	virtual CConnection* GetConnection(UINT nUID, bool bDoLock = true);
	virtual CConnection* GetConnectionByIPPort(const char *pIp, const USHORT nPort);
#else
	CConnection* GetConnection(UINT nUID, bool bDoLock = true);
	CConnection* GetConnectionByIPPort(const char *pIp, const USHORT nPort);
#endif

	UINT GetCount(bool bDoLock = true);
};

#endif	// #if defined(_SERVER)