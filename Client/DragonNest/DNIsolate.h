
#pragma once

#include "DNPacket.h"
#include "MessageListener.h"
#include "Task.h"
#include "Singleton.h"

#define _REQ_SERVER_LIMIT_SEC 30.f

class CDnBlockDlg;
class CDnIsolateTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDnIsolateTask>
{
public:
	typedef std::list<std::wstring> BLACK_LIST;

protected:
	CDnBlockDlg*	m_pBlockDlg;
	BLACK_LIST		m_BlockUserCacheList;
	float			m_ReqLimitTime;
	bool			m_EnableReqServerList;

public:
	CDnIsolateTask();
	~CDnIsolateTask();

	void Initialize();
	void SetDlg(CDnBlockDlg* dlg)	{ _ASSERT(m_pBlockDlg == NULL); m_pBlockDlg = dlg; }

	void OnDisconnectTcp( bool bValidDisconnect );
	void OnDisconnectUdp( bool bValidDisconnect );
	void OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize);

	//Send
	bool ReqIsolateList(bool bForceReqToServer = false);
	void ReqIsolateAdd(const WCHAR * pIsolateName);
	void ReqIsolateDel(const WCHAR * pIsolateName);

	const BLACK_LIST&	GetBlackList() const			{ return m_BlockUserCacheList; }
	bool				IsBlackList(const WCHAR* name) const;

	int					GetBlackListCount() const		{ return (int)m_BlockUserCacheList.size(); }

protected:
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnRecvIsolateMessage(int nSubCmd, char * pData, int nSize);

	//Recv
	void OnRecvIsolateList(SCIsolateList * pPacket);
	void OnRecvIsolateAdd(SCIsolateAdd * pPacket);
	void OnRecvIsolateDel(SCIsolateDelete * pPacket);
	void OnRecvIsolateResult(SCIsolateResult * pPacket);
};

#define GetIsolateTask()	CDnIsolateTask::GetInstance()