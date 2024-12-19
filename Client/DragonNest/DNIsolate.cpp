
#include "stdafx.h"
#include "DNIsolate.h"
#include "IsolateSendPacket.h"
#include "DnBlockDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnIsolateTask::CDnIsolateTask() : CTaskListener(false)
							, m_pBlockDlg(NULL)
							, m_ReqLimitTime(-1.f)
							, m_EnableReqServerList(true)
{
}

CDnIsolateTask::~CDnIsolateTask()
{
}

void CDnIsolateTask::Initialize()
{
}

void CDnIsolateTask::OnDisconnectTcp(bool bValidDisconnect)
{
}

void CDnIsolateTask::OnDisconnectUdp(bool bValidDisconnect)
{
}

void CDnIsolateTask::OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize)
{
	switch (nMainCmd)
	{
		case SC_ISOLATE: OnRecvIsolateMessage(nSubCmd, pData, nSize); break;
	}
}

bool CDnIsolateTask::ReqIsolateList(bool bForceReqToServer)
{
	if (m_EnableReqServerList || bForceReqToServer)
	{
		SendIsolateListReq();
		return true;
	}

	return false;
}

void CDnIsolateTask::ReqIsolateAdd(const WCHAR * pIsolateName)
{
	SendIsolateAdd(pIsolateName);
}

void CDnIsolateTask::ReqIsolateDel(const WCHAR * pIsolateName)
{
	SendIsolateDel(pIsolateName);
}

void CDnIsolateTask::OnRecvIsolateMessage(int nSubCmd, char * pData, int nSize)
{
	switch (nSubCmd)
	{
		case eIsolate::SC_ISOLATE_LIST:		OnRecvIsolateList((SCIsolateList*)pData); break;
		case eIsolate::SC_ISOLATE_ADD:		OnRecvIsolateAdd((SCIsolateAdd*)pData);	break;
		case eIsolate::SC_ISOLATE_DELETE:	OnRecvIsolateDel((SCIsolateDelete*)pData); break;
		case eIsolate::SC_ISOLATE_RESULT:	OnRecvIsolateResult((SCIsolateResult*)pData); break;
		default: _ASSERT(0);
	}
}

void CDnIsolateTask::OnRecvIsolateList(SCIsolateList * pPacket)
{
	if (pPacket == NULL || m_pBlockDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	WCHAR wszName[NAMELENMAX];
	int i = 0, nLen = 0;

	m_BlockUserCacheList.clear();
	for (; i < pPacket->cCount; ++i)
	{
		memset(wszName, 0, sizeof(wszName));
		_wcscpy(wszName, _countof(wszName), pPacket->wszIsolateName + nLen, pPacket->cNameLen[i]);
		nLen += pPacket->cNameLen[i];

		m_BlockUserCacheList.push_back(wszName);
	}

	m_EnableReqServerList = false;
	m_ReqLimitTime = _REQ_SERVER_LIMIT_SEC;

	m_pBlockDlg->RefreshList();
}

void CDnIsolateTask::OnRecvIsolateAdd(SCIsolateAdd * pPacket)
{
	if (pPacket == NULL || m_pBlockDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_BlockUserCacheList.push_back(pPacket->wszIsolateName);
	m_pBlockDlg->RefreshList();
	m_pBlockDlg->ShowAddDlg(false);

	focus::ReleaseControl();

	wchar_t wszTemp[256]={0};
	swprintf_s( wszTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4205 ), pPacket->wszIsolateName );
	GetInterface().MessageBox( wszTemp, MB_OK );
}

void CDnIsolateTask::OnRecvIsolateDel(SCIsolateDelete * pPacket)
{
	if (pPacket == NULL || m_pBlockDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	bool bErase = false;
	BLACK_LIST::iterator iter = m_BlockUserCacheList.begin();
	for (; iter != m_BlockUserCacheList.end(); ++iter)
	{
		std::wstring& user = (*iter);
		if (!__wcsicmp_l(user.c_str(), pPacket->wszIsolateName))
		{
			m_BlockUserCacheList.erase(iter);
			bErase = true;
			break;
		}
	}

	if (bErase == false)
	{
		_ASSERT(0);
		return;
	}

	m_pBlockDlg->RefreshList();
	GetInterface().RefreshPartyInfoList();
}

void CDnIsolateTask::OnRecvIsolateResult(SCIsolateResult * pPacket)
{
	if (pPacket == NULL)
	{
		_ASSERT(0);
		return;
	}

	focus::ReleaseControl();

	if (pPacket->nRetCode == ERROR_NONE) return;
	GetInterface().ServerMessageBox(pPacket->nRetCode, MB_OK, MESSAGEBOX_BLOCK_SERVER_ERROR_MSG, m_pBlockDlg);
}

void CDnIsolateTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if (m_EnableReqServerList == false)
	{
		m_ReqLimitTime -= fDelta;
		if (m_ReqLimitTime <= 0.f)
			m_EnableReqServerList = true;
	}
}

bool CDnIsolateTask::IsBlackList(const WCHAR* name) const
{
	BLACK_LIST::const_iterator iter = m_BlockUserCacheList.begin();
	for (; iter != m_BlockUserCacheList.end(); ++iter)
	{
		const std::wstring& user = (*iter);
		if (!__wcsicmp_l(user.c_str(), name))
			return true;
	}

	return false;
}