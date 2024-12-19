#include "StdAfx.h"
#include "DNDLGameRoom.h"
#include "DnGameTask.h"
#include "DnItemTask.h"
#include "DnDropItem.h"
#include "DnDLGameTask.h"
#include "DNUserSession.h"
#include "DNDBConnectionManager.h"
#include "DNDBConnection.h"
#include "DNGameDataManager.h"
#include "TaskManager.h"

CDNDLGameRoom::CDNDLGameRoom( CDNRUDPGameServer* pServer, unsigned int iRoomID, MAGAReqRoomID* pPacket )
: CDNGameRoom( pServer, iRoomID, pPacket )
{
	m_iProtectionKey = 0;
	m_bRecvRankQueryData = false;
	m_pRankQueryResultInfo = NULL;
#if defined(PRE_FIX_69108)
	m_bSendRankQueryData = false;
#endif
}

CDNDLGameRoom::~CDNDLGameRoom()
{
	SAFE_DELETE( m_pRankQueryResultInfo );
}

void CDNDLGameRoom::MakeRankQueryData()
{
	m_iProtectionKey = _roomrand(this)%INT_MAX;
	memset( &m_RankQueryData, 0, sizeof(m_RankQueryData) );

	m_RankQueryData.cWorldSetID = GetWorldSetID();
	m_RankQueryData.iProtectionKey = m_iProtectionKey;
	m_RankQueryData.iMapIndex = m_pGameTask->GetMapTableID();
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	if( ((CDnDLGameTask*)m_pGameTask)->IsChallengeDarkLair() )
		m_RankQueryData.iMapIndex += DarkLair::TopFloorAbstractMapIndex;
#endif
	m_RankQueryData.unPlayRound = ((CDnDLGameTask*)m_pGameTask)->GetRound();
	m_RankQueryData.uiPlaySec = GetDungeonPlayTime() / 1000;
	m_RankQueryData.iRoomID = GetRoomID();
	m_RankQueryData.cPartyUserCount = (BYTE)GetUserCount();

	if( GetStartMemberCount() == 1 && wcslen( GetPartyName() ) < 1 && GetUserData(0) ) 
	{
		// ��ũ���� �����ؼ� %s���� ��Ƽ �Ҷ� �������� �߻�
		// DB SP���� �ٲ��� �ؼ� �ڿ� ���� ©���� ó��
		WCHAR wszTemp[MAX_PATH];
#if defined(PRE_ADD_MULTILANGUAGE)
		//��Ƽ�̸��� ����Ʈ ���̼����� ó��
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3416, MultiLanguage::eDefaultLanguage ), GetUserData(0)->GetStatusData()->wszCharacterName );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3416 ), GetUserData(0)->GetStatusData()->wszCharacterName );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		_wcscpy( m_RankQueryData.wszPartyName, _countof(m_RankQueryData.wszPartyName), wszTemp, static_cast<int>(wcslen(wszTemp)) );
	}
	else 
	{
		// ��Ƽ�̸��� %s %d �� ���Ե� ��� invalid param error �߻��Ͽ� wcscpy �� ��ü.
		_wcscpy( m_RankQueryData.wszPartyName, _countof(m_RankQueryData.wszPartyName), GetPartyName(), (int)wcslen(GetPartyName()) );
		//swprintf_s( m_RankQueryData.wszPartyName, GetPartyName() );
	}

	for( DWORD i=0; i<GetUserCount(); i++ ) {
		CDNUserSession *pSession = GetUserData(i);
		if( !pSession ) continue;
		m_RankQueryData.sUserData[i].bUpdate = false;
		m_RankQueryData.sUserData[i].i64CharacterDBID = pSession->GetCharacterDBID();
		m_RankQueryData.sUserData[i].iJobIndex = pSession->GetUserJob();
		m_RankQueryData.sUserData[i].unLevel = pSession->GetLevel();
	}
	m_bRecvRankQueryData = false;
#if defined(PRE_FIX_69108)
	m_bSendRankQueryData = false;
#endif
}

void CDNDLGameRoom::UpdateResultRankQueryData()
{
	m_RankQueryData.unPlayRound = ((CDnDLGameTask*)m_pGameTask)->GetRound();
	m_RankQueryData.uiPlaySec = GetDungeonPlayTime() / 1000;
}

void CDNDLGameRoom::UpdateResultRankMapIndex()
{
	if( m_RankQueryData.iMapIndex >= DarkLair::TopFloorAbstractMapIndex )
		return;

	const TDLMapData* pMapData = g_pDataManager->GetDLMapData( m_RankQueryData.iMapIndex );
	if( pMapData == NULL )
		return;

	if( pMapData->nFloor != 1 )
		return;

	m_RankQueryData.iMapIndex += DarkLair::TopFloorAbstractMapIndex;
}

void CDNDLGameRoom::SetUpdateRankData( CDNUserSession *pSession )
{
	// ��������� ����Ʈ�� ��ԵǹǷ� Query������ �ٽ� ���Ƽ� üũ�غ���.
	for( int j=0; j<m_RankQueryData.cPartyUserCount; j++ ) {
		if( m_RankQueryData.sUserData[j].i64CharacterDBID == pSession->GetCharacterDBID() ) {
			m_RankQueryData.sUserData[j].bUpdate = true;
			break;
		}
	}
}

void CDNDLGameRoom::RequestRankQueryData()
{
#if defined(PRE_FIX_69108)
	if(m_bSendRankQueryData) return;
	m_bSendRankQueryData = true;	
#endif
	UpdateResultRankQueryData();

	BYTE cThreadID;
	CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
	pDBCon->QueryUpdateDarkLairResult( cThreadID, &m_RankQueryData, sizeof(m_RankQueryData) - sizeof(m_RankQueryData.sUserData) + ( sizeof(SDarkLairUpdateUserData) * m_RankQueryData.cPartyUserCount ) );
	SAFE_DELETE( m_pRankQueryResultInfo );
}

void CDNDLGameRoom::OnDBMessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen)
{
	switch( iMainCmd )
	{
		case MAINCMD_DARKLAIR: OnDBRecvDarkLair(iSubCmd, pData, iLen); break;
	}

	CDNGameRoom::OnDBMessageProcess( iMainCmd, iSubCmd, pData, iLen );
}

void CDNDLGameRoom::OnDBRecvDarkLair(int nSubCmd, char *pData, int iLen)
{
	switch (nSubCmd)
	{
	case QUERY_UPDATE_DARKLAIR_RESULT:
		{
			TAUpdateDarkLairResult* pPacket = reinterpret_cast<TAUpdateDarkLairResult*>(pData);
			if( pPacket->iProtectionKey != m_iProtectionKey )
			{
				_DANGER_POINT();
				return;
			}

			m_pRankQueryResultInfo = new TAUpdateDarkLairResult;
			memcpy( m_pRankQueryResultInfo, pPacket, iLen );
			m_bRecvRankQueryData = true;
			return;
		}
	}

	CDNGameRoom::OnDBRecvDarkLair(nSubCmd, pData);
}

void CDNDLGameRoom::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	//rlkt
	wprintf(L" !!! CDNDLGameRoom::OnDie : %ws killed %ws \n",hActor->GetName(),hHitter->GetName());
	__super::OnDie( hActor, hHitter );
}

void CDNDLGameRoom::OnDelPartyMember( UINT iDelMemberSessionID, char cKickKind )
{
	CDNGameRoom::OnDelPartyMember( iDelMemberSessionID, cKickKind );

	if( m_pGameTask ) {
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( this, iDelMemberSessionID );
		((CDnDLGameTask*)m_pGameTask)->CheckAndRequestDungeonClear( hActor );
	}
}

#ifdef PRE_MOD_DARKLAIR_RECONNECT
void CDNDLGameRoom::OnSuccessBreakInto( CDNUserSession* pGameSession )
{
	CDNGameRoom::OnSuccessBreakInto( pGameSession );
	// ���Լ����ߴٴ� �̺�Ʈ Ʈ���� �߻�
	GetWorld()->OnTriggerEventCallback( "CPvPGameMode::OnSuccessBreakInto", 0, 0 );
}
#endif // PRE_MOD_DARKLAIR_RECONNECT