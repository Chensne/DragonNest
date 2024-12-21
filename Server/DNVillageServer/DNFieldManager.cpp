#include "StdAfx.h"
#include "DNFieldManager.h"
#include "DNGameDataManager.h"
#include "DNFieldData.h"
#include "DNFieldDataManager.h"
#include "Log.h"
#include "DNPvPField.h"
#include "DNPvPLobbyField.h"
#include "DNDarkLairField.h"
#include "DNDBConnectionManager.h"
#include "DNDBConnection.h"
#include "DNField.h"
#include "DNMasterConnection.h"
#include "DNFarmField.h"
#include "DNLadderSystemManager.h"
#if defined(PRE_ADD_DWC)
#include "DNDWCField.h"
#endif

extern TVillageConfig g_Config;
CDNFieldManager* g_pFieldManager;

CDNFieldManager::CDNFieldManager(void)
{
	m_pFields.clear();
	m_bIsInit = false;
}

CDNFieldManager::~CDNFieldManager(void)
{
	Final();
}

void CDNFieldManager::DoUpdate(DWORD CurTick)
{
	if (m_pFields.empty()) return;

	for (int i = 0; i < (int)m_pFields.size(); i++){
		m_pFields[i]->DoUpdate(CurTick);
	}
}

#include "DNUserSessionManager.h"
bool CDNFieldManager::Init()
{
	if (m_bIsInit) return true;

	Final();
	
	//Init Field
	const TVillageInfo * pInfo = g_pDataManager->GetVillageInfo(g_Config.nWorldSetID, g_Config.nVillageID);
	if (pInfo)
	{
		std::vector <TChannelInfo>::const_iterator ii;
		for (ii = pInfo->vOwnedChannelList.begin(); ii != pInfo->vOwnedChannelList.end(); ii++)
		{
			if (AddField((*ii).nChannelID, (*ii).nAttribute, (*ii).nChannelMaxUser, (*ii).nMapIdx, (*ii).wszMapName) == false)
			{
				g_Log.Log( LogType::_FILELOG, L"AddField Fail [CHID:%d] [MAPIDX:%d] [MAPNAME:%s]\n", (*ii).nChannelID, (*ii).nMapIdx, (*ii).wszMapName);
				return false;
			}
			g_pUserSessionManager->IncreaseChannelUserCount((*ii).nChannelID);
			g_pUserSessionManager->DecreaseChannelUserCount((*ii).nChannelID);
		}
		m_bIsInit = true;
		return true;
	}
	g_Log.Log( LogType::_ERROR, L"Field Init Fail [WORLDSETID:%d] [VILLAGEID:%d] Not Found!!!!\n", g_Config.nWorldSetID, g_Config.nVillageID );
	return false;
}

void CDNFieldManager::Final()
{
	if (m_pFields.empty()) return;

	CDNField *pField = NULL;
	for (int i = 0; i < (int)m_pFields.size(); i++){
		SAFE_DELETE(m_pFields[i]);
	}

	m_pFields.clear();
}

bool CDNFieldManager::AddField(int nChannelID, int nChannelAtt, int nChannelMaxUser, int nMapIdx, const WCHAR * szMapName)
{
	const TMapInfo * pMap = g_pDataManager->GetMapInfo(nMapIdx);
	if (pMap != NULL)
	{
		char buf[128];
		WideCharToMultiByte(CP_ACP, 0, szMapName, -1, buf, 128, NULL, NULL);
		CDNFieldData * pFieldData = g_pFieldDataManager->FindFieldData(buf);
		if (pFieldData != NULL)
		{
			if (GetField(nChannelID) != NULL)
				return false;

			CDNField* pField = NULL;
			
			// PvP 마을
			if( nChannelAtt&GlobalEnum::CHANNEL_ATT_PVP )
				pField = new CDNPvPField;
			// PvP 로비
			else if( nChannelAtt&GlobalEnum::CHANNEL_ATT_PVPLOBBY )
			{
				pField = new CDNPvPLobbyField;
				LadderSystem::CManager::GetInstance().SetChannelID( nChannelID );
			}
			// 다크레어마을
			else if( nChannelAtt&GlobalEnum::CHANNEL_ATT_DARKLAIR )
				pField = new CDNDarkLairField;
			else if (nChannelAtt&GlobalEnum::CHANNEL_ATT_FARMTOWN)
				pField = new CDNFarmField;
#if defined(PRE_ADD_DWC)
			// DWC 마을
			else if( nChannelAtt&GlobalEnum::CHANNEL_ATT_DWC)
				pField = new CDNDWCField;
#endif
			// 기타
			else
				pField = new CDNField;

			if( !pField ) 
				return false;

			int nPermitLevel = g_pDataManager->GetMapSubType(nMapIdx) == GlobalEnum::MAP_UNKNOWN ? g_pDataManager->GetMapPermitLevel(nMapIdx) : 0;
			if (nPermitLevel < 0)
				g_Log.Log(LogType::_FILELOG, L"Check MapGateTable.ext MapIndex[%d] [_PermitPlayerLevel] Not Found!!!!\n", nMapIdx);
			
			if (pField->Init(nChannelID, nChannelAtt, nChannelMaxUser, nMapIdx, pMap, pFieldData, nPermitLevel) == false)
			{
				SAFE_DELETE(pField);
				return false;
			}

			m_pFields.push_back(pField);
			pFieldData->LoadCreateNpc(pField);
			return true;
		}
	}
	return false;
}

bool CDNFieldManager::DelField(int nMapIndex)
{
	if (m_pFields.empty()) return false;
	if (nMapIndex <= 0) return NULL;

	for (int i = 0; i < (int)m_pFields.size(); i++){
		if (m_pFields[i]->GetMapIndex() == nMapIndex){
			SAFE_DELETE(m_pFields[i]);
			m_pFields.erase(m_pFields.begin() + i);
			return true;
		}
	}

	return false;
}

bool CDNFieldManager::bIsIncludePvPLobby()
{
	for( UINT i=0 ; i<m_pFields.size() ; ++i )
	{
		if( m_pFields[i]->bIsPvPLobby() )
			return true;
	}
	return false;
}

void CDNFieldManager::FinalUser( CDNBaseObject* pObj )
{
	if (m_pFields.empty()) return;
	if (!pObj) return;

	for (int i = 0; i < (int)m_pFields.size(); i++){
		m_pFields[i]->FinalUser(pObj);
	}
}

CDNField * CDNFieldManager::GetFieldByIdx(int idx)
{
	return idx >= (int)m_pFields.size() ? NULL : m_pFields[idx];
}

CDNField* CDNFieldManager::GetField(int nChannelID)
{
	if (m_pFields.empty()) return NULL;

	if (nChannelID <= 0) return NULL;

	for (int i = 0; i < (int)m_pFields.size(); i++){
		if ((m_pFields[i]->GetChnnelID() == nChannelID))
			return m_pFields[i];
	}

	return NULL;
}

CDNField * CDNFieldManager::GetFieldByMapIdx(int nMapIdx)
{
	if (m_pFields.empty()) return NULL;

	if (nMapIdx <= 0) return NULL;

	for (int i = 0; i < (int)m_pFields.size(); i++){
		if ((m_pFields[i]->GetMapIndex() == nMapIdx))
			return m_pFields[i];
	}
	return NULL;
}

CDNNpcObject* CDNFieldManager::GetNpcObjectByNpcObjID(int nChannelD, UINT nNpcObjID)
{
	CDNField *pField = GetField(nChannelD);
	if (!pField) return NULL;

	return pField->GetNpcObjectByNpcObjID(nNpcObjID);
}

void CDNFieldManager::SendFieldNotice(int nMapIndex, int cLen, const WCHAR *pwszChatMsg, int nShowSec)
{
	if (m_pFields.empty()) return;
	if (nMapIndex <= 0) return;

	for (int i = 0; i < (int)m_pFields.size(); i++){
		if ((m_pFields[i]->GetMapIndex() == nMapIndex))
			m_pFields[i]->SendFieldNotice(cLen, pwszChatMsg, nShowSec);
	}
}

void CDNFieldManager::SendChannelNotice(int nChannelID, int nLen, const WCHAR * pszMsg, int nShowSec)
{
	CDNField *pField = GetField(nChannelID);
	if (!pField) return;

	pField->SendFieldNotice(nLen, pszMsg, nShowSec);
}

void CDNFieldManager::SendChannelChat(int nChannelID, int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg, int nRet)
{
	CDNField *pField = GetField(nChannelID);
	if (!pField) return;

	pField->SendChannelChat(cLen, pwszCharacterName, pwszChatMsg);
}

void CDNFieldManager::SendDebugChat(int nChannelID, const WCHAR* szMsg)
{
	CDNField *pField = GetField(nChannelID);
	if (!pField) return;

	pField->SendDebugChat(szMsg);
}

void CDNFieldManager::UpdateChannelShowInfo( bool bForce/*=true*/ )
{
	ScopeLock<CSyncLock> lock(m_ChannleShowLock);

	if( bForce == true )
		m_mChannelShow.clear();

	for( UINT i=0 ; i<m_pFields.size() ; ++i )
	{
		BYTE cThreadID;
		CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( m_pFields[i]->GetChnnelID(), cThreadID );

		bool bShow = false;
		if( pDBCon && pDBCon->GetActive() && pDBCon->m_nServerID != -1)
			bShow = true;

		std::map<int,bool>::iterator itor = m_mChannelShow.find( m_pFields[i]->GetChnnelID() );
		if( itor == m_mChannelShow.end() )
		{
			m_mChannelShow.insert( std::make_pair(m_pFields[i]->GetChnnelID(),bShow) );
		}
		else
		{
			bool bPrev = (*itor).second;
			if( bShow == bPrev )
				continue;
			(*itor).second = bShow;
		}

		if( g_pMasterConnection )
			g_pMasterConnection->SendUpdateChannelShowInfo( m_pFields[i]->GetChnnelID(), bShow, pDBCon ? pDBCon->m_nServerID : -1, cThreadID );
	}
}

#if defined(PRE_ADD_DWC)
bool CDNFieldManager::bIsIncludeDWCVillage()
{
	for( UINT i=0 ; i<m_pFields.size() ; ++i )
	{
		if( m_pFields[i]->bIsDWCVillage() )
			return true;
	}
	return false;
}
#endif