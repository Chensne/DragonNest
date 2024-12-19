#include "StdAfx.h"
#include "DNUserSession.h"
#include "DNUserSessionManager.h"
#include "DNMasterConnection.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNServiceConnection.h"
#include "DNIocpManager.h"
#include "Log.h"
#include "DNPeriodQuestSystem.h"
#include "DNGuildWarManager.h"

CDNUserSessionManager* g_pUserSessionManager;

CDNUserSessionManager::CDNUserSessionManager(void)
{
	m_dwChangeSaveTick = m_dwLastSaveTick = 0;

	m_pChangeSaveBuffer = new CBuffer(1024 * 1024 * 10);
	m_pLastSaveBuffer = new CBuffer(1024 * 1024 * 10);

	m_bServiceClose = false;
	m_nProcessCount = 0;

	Clear();
}

CDNUserSessionManager::~CDNUserSessionManager(void)
{
	SAFE_DELETE(m_pChangeSaveBuffer);
	SAFE_DELETE(m_pLastSaveBuffer);
}

void CDNUserSessionManager::Clear()
{
	{
		ScopeLock<CSyncLock> lock(m_AccountLock);
		m_pMapAccountDBID.clear();
		m_pMapSessionID.clear();
	}
	{
		ScopeLock<CSyncLock> lock(m_NameLock);
		m_pMapName.clear();
	}
	{
		ScopeLock<CSyncLock> lock(m_VecLock);
		m_pVecSession.clear();
	}
}

void CDNUserSessionManager::ExternalDoUpdate(DWORD CurTick)
{
	TQUpdateCharacter Update;
	CDNDBConnection *pDBCon = NULL;

	// 서버 바꿀때 요기로
	if (m_pChangeSaveBuffer->GetCount() > 0){
		if (CurTick >= m_dwChangeSaveTick + 50){
			if (m_pChangeSaveBuffer->View((char*)&Update, sizeof(TQUpdateCharacter)) == 0){
				{
					ScopeLock <CSyncLock> Lock(m_AccountLock);
					CDNUserSession* pSession = FindUserSessionByAccountDBIDAsync( Update.nAccountDBID );
					if( pSession )
					{
						pDBCon = pSession->GetDBConnection();
					}
					else
					{
						pDBCon = g_pDBConnectionManager->GetDBConnection( Update.uiDBRandomSeed, Update.cThreadID );
					}
				}

				if (pDBCon)
				{
					//기간 지난 아이템 삭제먼저 진행
					pDBCon->QueryDelExpireitem(Update.cThreadID, Update.cWorldSetID, Update.nAccountDBID, Update.biCharacterDBID);
					pDBCon->AddSendData(MAINCMD_STATUS, QUERY_CHANGESERVERUSERDATA, (char*)&Update, sizeof(TQUpdateCharacter));
					m_pChangeSaveBuffer->Skip(sizeof(TQUpdateCharacter));
				}
#ifdef PRE_ADD_CHAGNESERVER_LOGGING
				else
				{
					g_Log.Log(LogType::_ERROR, L"ExternalDoUpdate pDBCon == NULL [ADBID:%d]\n", Update.nAccountDBID);
				}
#endif		//#ifdef PRE_ADD_CHAGNESERVER_LOGGING
			}
			m_dwChangeSaveTick = CurTick;
		}
	}

	// 걍 유저 끊을때
	if (m_pLastSaveBuffer->GetCount() > 0){
		if (CurTick >= m_dwLastSaveTick + 50){
			if (m_pLastSaveBuffer->View((char*)&Update, sizeof(TQUpdateCharacter)) == 0){
				{
					ScopeLock <CSyncLock> Lock(m_AccountLock);
					CDNUserSession* pSession = FindUserSessionByAccountDBIDAsync( Update.nAccountDBID );
					if( pSession )
					{
						pDBCon = pSession->GetDBConnection();
					}
					else
					{
						pDBCon = g_pDBConnectionManager->GetDBConnection( Update.uiDBRandomSeed, Update.cThreadID );
					}
				}

				if (pDBCon){
					pDBCon->AddSendData(MAINCMD_STATUS, QUERY_LASTUPDATEUSERDATA, (char*)&Update, sizeof(TQUpdateCharacter));
					m_pLastSaveBuffer->Skip(sizeof(TQUpdateCharacter));
				}
#ifdef PRE_ADD_CHAGNESERVER_LOGGING
				else
				{
					g_Log.Log(LogType::_ERROR, L"ExternalDoUpdate pDBCon == NULL [ADBID:%d]\n", Update.nAccountDBID);
				}
#endif		//#ifdef PRE_ADD_CHAGNESERVER_LOGGING
			}
			m_dwLastSaveTick = CurTick;
		}
	}

	//ServiceClose Flow
	if (m_bServiceClose)
	{
		if (m_pLastSaveBuffer->GetCount() <= 0 && m_pChangeSaveBuffer->GetCount() <= 0)
		{
			if (g_pServiceConnection)		//서비스매니저에게 처리할꺼 다 끝났다고 알린다.
				g_pServiceConnection->SendServiceClosed();
			m_bServiceClose = false;
		}
	}
}

void CDNUserSessionManager::InternalDoUpdate(DWORD CurTick)
{
#ifdef PRE_FIX_VILLAGEZOMBIE
	DWORD dwCurTick = timeGetTime();
	std::vector <CDNUserSession*> vDeletedSession;
#endif		//#ifdef PRE_FIX_VILLAGEZOMBIE
	if (!m_pVecSession.empty()){
		ScopeLock<CSyncLock> Lock(m_VecLock);

		int Count = (int)m_pVecSession.size();
		if (Count > CONNECTIONPROCESSCOUNT) Count = CONNECTIONPROCESSCOUNT;

		for (int i = 0; i < Count; i++){
			if (m_nProcessCount >= (int)m_pVecSession.size())
				m_nProcessCount = 0;

			CSocketContext* pSocketContext = m_pVecSession[m_nProcessCount];
			if( pSocketContext == NULL )
				continue;
			CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
			if( Scope.bIsDelete() )
				continue;

			CDNUserSession* pSession = reinterpret_cast<CDNUserSession*>(pSocketContext->GetParam());
			if( pSession == NULL )
				continue;

#ifdef PRE_FIX_VILLAGEZOMBIE
			if (pSession->GetInsideDisconnectTick() > 0 && pSession->GetInsideDisconnectTick() + (60*1000*5) < dwCurTick)
			{
				vDeletedSession.push_back(pSession);
				continue;
			}
#endif		//#ifdef PRE_FIX_VILLAGEZOMBIE
			pSession->DoUpdate(CurTick);
			m_nProcessCount++;
		}
	}

#ifdef PRE_FIX_VILLAGEZOMBIE
	std::vector <CDNUserSession*>::iterator ii;
	for (ii = vDeletedSession.begin(); ii != vDeletedSession.end(); ii++)
	{
		(*ii)->SetZombieChecked();
		(*ii)->DetachConnection(L"Zombie Check!");
	}
#endif		//#ifdef PRE_FIX_VILLAGEZOMBIE
}

CDNUserSession *CDNUserSessionManager::AddSession(const char *pIp, const USHORT nPort)
{
	CDNUserSession *pUserSession = new CDNUserSession;
	if (!pUserSession) return NULL;

	pUserSession->SetIp(pIp);
	pUserSession->SetPort(nPort);
	return pUserSession;
}

bool CDNUserSessionManager::DelSession(CDNUserSession *pUserSession, CSocketContext* pDelSocketContext )
{
	if (!pUserSession) return false;

	{
		ScopeLock<CSyncLock> Lock(m_AccountLock);

		if (!m_pMapSessionID.empty()){
			TMapSession::iterator iterS = m_pMapSessionID.find(pUserSession->GetSessionID());
			if (iterS != m_pMapSessionID.end()){
				m_pMapSessionID.erase(iterS);
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pUserSession, L"[DelSession] m_pMapSessionID fail!\r\n");
				return false; // 세션이 없는 넘은 밑에 애들도 지워주지 않는다.
			}				
		}

		if (!m_pMapAccountDBID.empty()){
			TMapSession::iterator iterA = m_pMapAccountDBID.find(pUserSession->GetAccountDBID());
			if (iterA != m_pMapAccountDBID.end()){
				m_pMapAccountDBID.erase(iterA);
			}
			else
				g_Log.Log(LogType::_ERROR, pUserSession, L"[DelSession] m_pMapAccountDBID fail!\r\n");
		}
	}

	{
		ScopeLock<CSyncLock> Lock(m_NameLock);

		if (!m_pMapName.empty()){
			TMapNameSession::iterator itername = m_pMapName.find(pUserSession->wszName());
			if (itername != m_pMapName.end()){
				if (itername->second != pUserSession){
					g_Log.Log(LogType::_ERROR, L"[ObjID:%u] DelSession(m_pMapName) invalid (%s) %x %x\r\n", pUserSession->GetObjectID(), pUserSession->wszName(), itername->second, pUserSession);
				}
				m_pMapName.erase(itername);
			}
			/*
			else
			{
				for (itername = m_pMapName.begin(); itername != m_pMapName.end(); itername++)
				{
					if ((*itername).second == (pUserSession) || (*itername).second->GetAccountDBID() == pUserSession->GetAccountDBID())
					{
						//이럴경우 파인드가 안된거다. 컨테이너 사이즈가 차이가 나는 경우가 생기는 걸로 봐서 지워질때 제대로 안지워지는 경우가 있는듯
						g_Log.Log(LogType::_DELOBJECT_NAMEKEYFINDERR, L"[ObjID:%u] DelSession(m_pMapName) FindErr (%s) %x %x\r\n", pUserSession->GetObjectID(), pUserSession->wszName(), itername->second, pUserSession);
						m_pMapName.erase(itername);
						break;
					}
				}
			}
			*/
		}
	}

	{
		ScopeLock<CSyncLock> lock(m_VecLock);
		
		if (!m_pVecSession.empty()){
			for (int i = 0; i < (int)m_pVecSession.size(); ++i)
			{
				CSocketContext* pSocketContext = m_pVecSession[i];
				if( pSocketContext == NULL )
					continue;

				if( pSocketContext != pDelSocketContext )
					continue;

				CDNUserSession* pSession = reinterpret_cast<CDNUserSession*>(pSocketContext->GetParam());

				if (pSession->GetAccountDBID() == pUserSession->GetAccountDBID())
				{
					m_pVecSession.erase(m_pVecSession.begin() + i);
					break;
				}
				else
				{
					m_pVecSession.erase(m_pVecSession.begin() + i);
					g_Log.Log(LogType::_ERROR, pSession, L"m_pVecSession erase Failed!! AccountDBID different %d!=%d\r\n", pSession->GetAccountDBID(), pUserSession->GetAccountDBID() );
					break;
				}
			}
		}
	}
	
	pUserSession->FinalizeEvent();
	pUserSession->LeaveWorld();
	pUserSession->FinalObject();

	return true;
}

bool CDNUserSessionManager::InsertSession(CDNUserSession *pUserSession)
{
	if (!pUserSession) return false;

	CDNUserSession *pExistUser = FindUserSessionByAccountDBID(pUserSession->GetAccountDBID());
	if (pExistUser){
		pExistUser->DetachConnection(L"Connect|InsertSession fail");	// 원래 있던애를 끊어버린다
		g_Log.Log(LogType::_ERROR, pExistUser, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] AccountDBID Duplicate!!\r\n", pExistUser->GetAccountDBID(), pExistUser->GetCharacterDBID(), pExistUser->GetSessionID(), pExistUser->GetObjectID());
		return false;
	}

	pExistUser = FindUserSession(pUserSession->GetSessionID());
	if( pExistUser)
	{
		pExistUser->DetachConnection(L"Connect|InsertSession fail");	// 원래 있던애를 끊어버린다
		g_Log.Log(LogType::_ERROR, pExistUser, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] SessionID Duplicate!!\r\n", pExistUser->GetAccountDBID(), pExistUser->GetCharacterDBID(), pExistUser->GetSessionID(), pExistUser->GetObjectID());
		return false;
	}

	{
		ScopeLock<CSyncLock> Lock(m_AccountLock);
		std::pair<TMapSession::iterator, bool> Ret = m_pMapAccountDBID.insert(make_pair(pUserSession->GetAccountDBID(), pUserSession));
		if (Ret.second == false){
			pUserSession->DetachConnection(L"ModifySession(m_pMapAccountDBID) fail");
			g_Log.Log(LogType::_ERROR, pUserSession, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] m_pMapAccountDBID fail\r\n", pUserSession->GetAccountDBID(), pUserSession->GetCharacterDBID(), pUserSession->GetSessionID(), pUserSession->GetObjectID());
			return false;
		}
		Ret = m_pMapSessionID.insert(make_pair(pUserSession->GetSessionID(), pUserSession));
		if (Ret.second == false){
			pUserSession->DetachConnection(L"ModifySession(m_pMapSessionID) fail");
			g_Log.Log(LogType::_ERROR, pUserSession, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] m_pMapSessionID fail\r\n", pUserSession->GetAccountDBID(), pUserSession->GetCharacterDBID(), pUserSession->GetSessionID(), pUserSession->GetObjectID());
			return false;
		}
	}

	{
		ScopeLock<CSyncLock> lock(m_VecLock);
		m_pVecSession.push_back( pUserSession->GetSocketContext() );
	}

	return true;
}

bool CDNUserSessionManager::DelTempSession(CDNUserSession *pUserSession, CSocketContext* pDelSocketContext )
{
	ScopeLock <CSyncLock> lock(m_VecTempLock);
	if (m_pVecTempSession.empty() == false)
	{
		for (int i = 0; i < (int)m_pVecTempSession.size(); ++i)
		{
			if (m_pVecTempSession[i] == pUserSession)
			{
				m_pVecTempSession.erase(m_pVecTempSession.begin() + i);
				return true;
			}
		}
	}
	return false;
}

bool CDNUserSessionManager::InsertTempSession(CDNUserSession * pUserSession)
{
	ScopeLock<CSyncLock> lock(m_VecTempLock);
	for (TVecTempSession::iterator ii = m_pVecTempSession.begin(); ii != m_pVecTempSession.end(); ii++)
	{
		if ((*ii)->GetSessionID() == pUserSession->GetSessionID() || (*ii) == pUserSession)
		{
			g_Log.Log(LogType::_ERROR, pUserSession, L"InsertTempSession Failed!!\n");
			return false;
		}
	}

	m_pVecTempSession.push_back(pUserSession);
	return true;
}

bool CDNUserSessionManager::InsertSessionByName(CDNUserSession *pUserSession)
{
	if (!pUserSession) return false;

	if (FindUserSessionByName(pUserSession->wszName())){
		pUserSession->DetachConnection(L"Connect|InsertSessionByName Failed");
		g_Log.Log(LogType::_ERROR, pUserSession, L"Connect|[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] InsertSessionByName failed\r\n", pUserSession->GetAccountDBID(), pUserSession->GetCharacterDBID(), pUserSession->GetSessionID(), pUserSession->GetObjectID());
		return false;
	}

	ScopeLock<CSyncLock> Lock(m_NameLock);
	m_pMapName[pUserSession->wszName()] = pUserSession;
	return true;
}

bool CDNUserSessionManager::SwapUserKeyName(const WCHAR *pwszOriginName, const WCHAR* pwszCharacterName)
{
	if (m_pMapName.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_NameLock);
	TMapNameSession::iterator iter = m_pMapName.find(pwszOriginName);
	if (iter != m_pMapName.end())
	{		
		CDNUserSession* pUserSession = iter->second;
		if (pUserSession == NULL)
		{
			g_Log.Log(LogType::_ERROR, L"SwapUserKeyName pUserSession == NULL\n");
			return false;
		}

		m_pMapName.erase (pwszOriginName);

		if (pUserSession && pwszCharacterName) 
			pUserSession->SetCharacterName(pwszCharacterName);

		m_pMapName[pUserSession->wszName()] = pUserSession;

		return true;
	}

	return false;
}

int CDNUserSessionManager::GetUserCount()
{
	m_AccountLock.Lock();
	int nCount = (int)m_pMapAccountDBID.size();
	m_AccountLock.UnLock();
	return nCount;
}

UINT CDNUserSessionManager::GetUserCount(std::map <int, int> & pMap)
{
	m_AccountLock.Lock();
	pMap = m_MapChannelUserCount;
	UINT nCount = (UINT)m_pMapAccountDBID.size();
	m_AccountLock.UnLock();
	return nCount;
}

void CDNUserSessionManager::IncreaseChannelUserCount(int nChannelID)
{
	ScopeLock<CSyncLock> Lock(m_AccountLock);

	std::map <int, int>::iterator ii = m_MapChannelUserCount.find(nChannelID);
	if (ii == m_MapChannelUserCount.end())
		m_MapChannelUserCount[nChannelID] = 1;
	else 
		(*ii).second++;
}

void CDNUserSessionManager::DecreaseChannelUserCount(int nChannelID)
{
	ScopeLock<CSyncLock> Lock(m_AccountLock);

	std::map <int, int>::iterator ii = m_MapChannelUserCount.find(nChannelID);
	if (ii != m_MapChannelUserCount.end())
	{
		if ((*ii).second > 0)
			(*ii).second--;
	}
	else
		_DANGER_POINT();
}

void CDNUserSessionManager::SendAddUserList()
{
	if (m_pMapSessionID.empty()){
		g_pMasterConnection->SendEndofVillageInfo();
		return;
	}

	ScopeLock<CSyncLock> lock(m_AccountLock);

	CDNUserSession *pSession = NULL;
	for (TMapSession::iterator iter = m_pMapSessionID.begin(); iter != m_pMapSessionID.end(); ++iter){
		pSession = iter->second;
		if (pSession){
			if (pSession->GetChannelID() <= 0) continue;
			g_pMasterConnection->SendAddUserList(pSession);
			g_Log.Log(LogType::_ERROR, pSession, L"[ADBID:%u, CDBID:%I64d, SID:%u, ObjID:%u] [SendAllAddUser]\r\n", pSession->GetAccountDBID(), pSession->GetCharacterDBID(), pSession->GetSessionID(), pSession->GetObjectID());
		}
	}
	g_pMasterConnection->SendEndofVillageInfo();
}

void CDNUserSessionManager::SendNotice(const WCHAR * pMsg, const int nLen, int nShowSec)
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second)
			iter->second->SendNotice(pMsg, nLen, nShowSec);
	}
}

void CDNUserSessionManager::SendChat(eChatType eType, int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg, char cRet)
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second)
			iter->second->SendChat(eType, cLen, pwszCharacterName, pwszChatMsg);
	}
}



void CDNUserSessionManager::SendZoneChat(int nMapIdx, const WCHAR * pFromName, const WCHAR * pMsg, int nLen)
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second && iter->second->GetMapIndex() == nMapIdx)
			iter->second->SendChat(CHATTYPE_CHANNEL, nLen, pFromName, pMsg);
	}
}

void CDNUserSessionManager::SendWorldSystemMsg(const WCHAR *pFromName, char cType, int nID, int nValue, const WCHAR *pToName)
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second)
			iter->second->SendWorldSystemMsg(cType, nID, pFromName, nValue, pToName);
	}
}

void CDNUserSessionManager::SendNoticeCancel()
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second)
			iter->second->SendNoticeCancel();
	}
}

void CDNUserSessionManager::SendCloseService(__time64_t _tNow, __time64_t _tOderedTime, __time64_t _tCloseTime)
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second) {
			iter->second->SendCloseService(_tNow, _tOderedTime, _tCloseTime);
		}
	}
}

void CDNUserSessionManager::SendSaleAbortList(BYTE cCount, int *nAbortList)
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second)
			iter->second->SendSaleAbortList(cCount, nAbortList);
	}
}

void CDNUserSessionManager::SendAssginPeriodQuest (UINT nQuestID, bool bFlag)
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second)
		{
			iter->second->GetQuest()->OnPeriodEvent(nQuestID, bFlag);
			iter->second->SendAssginPeriodQuest(nQuestID, bFlag);
		}
	}
}

void CDNUserSessionManager::SendPeriodQuestNotice(int nItemID, int nNoticeCount)
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second)
			iter->second->SendPeriodQuestNotice(nItemID, nNoticeCount);
	}
}

void CDNUserSessionManager::SendGuildWarEvent(short wScheduleID, char cEventStep, char cEventType)
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter)
	{
		if (iter->second)
		{
			if( iter->second->GetGuildWarScheduleID() != wScheduleID )
			{
				iter->second->GuildWarReset();
				iter->second->SetGuildWarScheduleID(wScheduleID);
			}
			iter->second->SendGuildWarEvent(wScheduleID, cEventStep, cEventType);
		}
	}
}

void CDNUserSessionManager::SendGuildWarEventTime()
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter)
	{
		if (iter->second)
		{			
			iter->second->SendGuildWarEventTime(g_pGuildWarManager->GetGuildWarSchedule(), g_pGuildWarManager->GetGuildWarFinalSchedule(), g_pGuildWarManager->GetFinalProgress());
		}
	}
}

void CDNUserSessionManager::SendGuildWarTournamentWin(SCGuildWarTournamentWin* pPacket)
{
	if (m_pMapAccountDBID.empty()) return;
	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter)
	{
		if (iter->second)
		{			
			iter->second->SendGuildWarTournamentWin(pPacket);
		}
	}
}

void CDNUserSessionManager::ResetGuildWarBuyedItems()
{
	if (m_pMapAccountDBID.empty())
		return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for each (TMapSession::value_type v in m_pMapAccountDBID)
	{
		if (v.second)
			v.second->ResetGuildWarBuyedItems();
	}
}

#ifdef PRE_ADD_CHAGNESERVER_LOGGING
bool CDNUserSessionManager::AddSaveSendData(int nMainCmd, char *pData, int iLen)
{
	int nRet = ERROR_DB;
	switch (nMainCmd)
	{
	case QUERY_CHANGESERVERUSERDATA:
		m_pChangeSaveBuffer->Push(pData, iLen);
		break;

	case QUERY_LASTUPDATEUSERDATA:
		m_pLastSaveBuffer->Push(pData, iLen);
		break;

	default:
		g_Log.Log(LogType::_ERROR, L"AddSaveSendData [MCMD:%d][LEN:%d]\n", nMainCmd, iLen);
		break;
	}

	if (nRet != ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, L"AddSaveSendData [MCMD:%d][LEN:%d]\n", nMainCmd, iLen);
	}
	return nRet == ERROR_NONE ? true : false;
}
#else		//#ifdef PRE_ADD_CHAGNESERVER_LOGGING
void CDNUserSessionManager::AddSaveSendData(int nMainCmd, char *pData, int nSize)
{
	switch (nMainCmd)
	{
	case QUERY_CHANGESERVERUSERDATA:
		m_pChangeSaveBuffer->Push(pData, nSize);
		//g_Log.Log(LogType::_NORMAL, L"[AddChangeSaveSendData] Size:%d\r\n", nSize);
		break;

	case QUERY_LASTUPDATEUSERDATA:
		m_pLastSaveBuffer->Push(pData, nSize);
		//g_Log.Log(LogType::_NORMAL, L"[AddLastSaveSendData] Size:%d\r\n", nSize);
		break;
	}
}
#endif

void CDNUserSessionManager::DetachAllConnection()
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter){
		if (iter->second)
			iter->second->DetachConnection(L"Server Close");
	}

	m_bServiceClose = true;
}

#ifdef _USE_VOICECHAT
void CDNUserSessionManager::TalkingUpdate(UINT * pMemberArr)
{
	if (m_pMapAccountDBID.empty()) return;

	CDNUserSession * pSession = NULL;
	TTalkingInfo Info[PARTYMAX] = { 0, };
	int i, nCount = 0, nArrCount = 0;

	ScopeLock <CSyncLock> lock(m_AccountLock);

	for (i = 0; i < PARTYMAX; i++)
	{
		if (pMemberArr[i] <= 0) continue;
		TMapSession::iterator ii = m_pMapAccountDBID.find(pMemberArr[i]);
		if (ii != m_pMapAccountDBID.end())
		{
			pSession = (CDNUserSession*)ii->second;
			if (pSession->IsTalking(&Info[nCount].cTalking))
			{
				Info[nCount].nSessionID = pSession->GetSessionID();
				nCount++;
			}
		}
	}
	if (nCount <= 0) return;

	for (i = 0; i < PARTYMAX; i++)
	{
		if (pMemberArr[i] <= 0) continue;	
		TMapSession::iterator ii = m_pMapAccountDBID.find(pMemberArr[i]);
		if (ii != m_pMapAccountDBID.end())
		{
			pSession = (CDNUserSession*)ii->second;
			pSession->SendTalkingUpdate(nCount, Info);
		}
	}
}
#endif

CDNUserSession * CDNUserSessionManager::FindUserSessionByAccountDBIDAsync(UINT nAccountDBID)
{
	//상위에서 동기걸고 들어와야 합니다. m_AccountLock
	if (m_pMapAccountDBID.empty()) return NULL;

	CDNUserSession *pUserObj = NULL;
	TMapSession::iterator iter = m_pMapAccountDBID.find(nAccountDBID);
	if (iter != m_pMapAccountDBID.end()){
		pUserObj = (CDNUserSession*)iter->second;
		return pUserObj;
	}

	return NULL;
}

CDNUserSession *CDNUserSessionManager::FindUserSession(UINT nSessionID)
{
	if (m_pMapSessionID.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	TMapSession::iterator iter = m_pMapSessionID.find(nSessionID);
	if (iter != m_pMapSessionID.end())
		return iter->second;

	return NULL;
}

CDNUserSession *CDNUserSessionManager::FindUserSessionByName(const WCHAR *pwszName)
{
	if (m_pMapName.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_NameLock);
	TMapNameSession::iterator iter = m_pMapName.find(pwszName);
	if (iter != m_pMapName.end())
		return iter->second;

	return NULL;
}

CDNUserSession *CDNUserSessionManager::FindUserSessionByObjectID(UINT nObjID)
{
	if (m_pMapSessionID.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	for (TMapSession::iterator iter = m_pMapSessionID.begin(); iter != m_pMapSessionID.end(); ++iter){
		if (iter->second->GetObjectID() == nObjID)
			return iter->second;
	}

	return NULL;
}

CDNUserSession* CDNUserSessionManager::FindUserSessionByAccountDBID(UINT nAccountDBID)
{
	if (m_pMapAccountDBID.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_AccountLock);
	TMapSession::iterator iter = m_pMapAccountDBID.find(nAccountDBID);
	if (iter != m_pMapAccountDBID.end())
		return iter->second;

	return NULL;
}

CDNUserSession * CDNUserSessionManager::FindTempUserSession(UINT nSessionID)
{
	ScopeLock<CSyncLock> Lock(m_VecTempLock);
	CDNUserSession * pSession = NULL;
	for (TVecTempSession::iterator ii = m_pVecTempSession.begin(); ii != m_pVecTempSession.end(); ii++)
	{
		if ((*ii)->GetSessionID() == nSessionID)
		{
			pSession = (*ii);
			m_pVecTempSession.erase(ii);
			break;
		}
	}
	return pSession;
}

#if defined( PRE_DRAGONBUFF )
void CDNUserSessionManager::ApplyWorldBuff( WCHAR *wszCharacterName, int nItemID, int nMapIdx )
{
	if (m_pMapAccountDBID.empty()) return;

	ScopeLock<CSyncLock> Lock(m_AccountLock);	
	for (TMapSession::iterator iter = m_pMapAccountDBID.begin(); iter != m_pMapAccountDBID.end(); ++iter)
	{
		if (iter->second && iter->second->GetMapIndex() == nMapIdx)
		{
			iter->second->ApplyWorldBuff( wszCharacterName, nItemID );			
		}			
	}
}
#endif