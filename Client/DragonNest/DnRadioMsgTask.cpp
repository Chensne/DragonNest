#include "stdafx.h"
#include "DnRadioMsgTask.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "RadioSendPacket.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "VillageSendPacket.h"
#include "DnGameMacroMsgOptDlg.h"
#include "DnHeadIcon.h"
#include "DnActor.h"
#include "DnMinimap.h"
#include "DnInterfaceString.h"
#include "DnGameTask.h"
#include "DnPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnRadioMsgTask::CDnRadioMsgTask() : CTaskListener(false)
{
	m_bEnableTransformRadioMsg = false;
	m_nTrasnformMsgIndex = 0;
	m_RadioMsgSendTime = 0;
	m_nCurSessionID = 0;
}

CDnRadioMsgTask::~CDnRadioMsgTask()
{
	Finalize();
}

void CDnRadioMsgTask::OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize)
{
	switch (nMainCmd)
	{
	case SC_RADIO : OnRecvRadioMsgMessage(nSubCmd, pData, nSize); break;
	}
}

void CDnRadioMsgTask::Initialize()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TRADIOMSG );

	int nNumItem = pSox->GetItemCount();
	for( int nItem = 0; nItem < nNumItem; ++nItem )
	{
		int nItemID = pSox->GetItemID( nItem );
		if( pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger() == 0 )
			continue;

		SRadioMsgInfo* pNewRadioMsgInfo = new SRadioMsgInfo;
		pNewRadioMsgInfo->nID = nItemID;
		pNewRadioMsgInfo->szName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger() );
		pNewRadioMsgInfo->nIconID = pSox->GetFieldFromLablePtr( nItemID, "_IconID" )->GetInteger();
		pNewRadioMsgInfo->nMacroNum = pSox->GetFieldFromLablePtr( nItemID, "_MacroNum" )->GetInteger();
		if( pNewRadioMsgInfo->nMacroNum == 0 )
		{
			for( int i = 0; i < NUM_JOB; ++i )
			{
				char szLabel[32];
				sprintf_s(szLabel, 32, "%s%d", "_MsgID", i+1);
				pNewRadioMsgInfo->szMsg[i] = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger() );

				sprintf_s(szLabel, 32, "%s%d", "_MsgSnd", i+1);
				std::string szMsgSndStr = pSox->GetFieldFromLablePtr( nItemID, szLabel )->GetString();
				for( int j = 0; ; ++j )
				{
					//char *pPtr = _GetSubStrByCount( i, (char*)szStr.c_str(), ';' );
					std::string strValue = _GetSubStrByCountSafe( j, (char*)szMsgSndStr.c_str(), ';' );
					if( strValue.size() == 0 ) break;
					std::string szTemp = strValue;
					if( szTemp.empty() ) break;

					int nSndIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szTemp.c_str() ).c_str(), true, false );
					if( nSndIndex != -1 )
						pNewRadioMsgInfo->vecMsgSndIndex[i].push_back( nSndIndex );
				}
			}

			pNewRadioMsgInfo->hHeadIconTex = LoadResource( CEtResourceMng::GetInstance().GetFullName( pSox->GetFieldFromLablePtr( nItemID, "_IconFile" )->GetString() ).c_str(), RT_TEXTURE );
		}

		m_pVecRadioMsgInfo.push_back( pNewRadioMsgInfo );
	}

	m_RadioMsgSendTime = 0;
	m_nCurSessionID = 0;

}

void CDnRadioMsgTask::Finalize()
{
	// 사운드 먼저 해제 후
	for( DWORD i=0; i<m_pVecRadioMsgInfo.size(); i++ )
	{
		for( int j = 0; j < NUM_JOB; ++j )
		{
			for( int k = 0; k < (int)m_pVecRadioMsgInfo[i]->vecMsgSndIndex[j].size(); ++k )
			{
				CEtSoundEngine::GetInstance().RemoveSound( m_pVecRadioMsgInfo[i]->vecMsgSndIndex[j][k] );
			}
		}
		SAFE_RELEASE_SPTR( m_pVecRadioMsgInfo[i]->hHeadIconTex );
	}
	SAFE_DELETE_PVEC( m_pVecRadioMsgInfo );

	if(!m_mapTransformRadioMsgInfo.empty())
	{
		for(std::map< int , STransformMsgInfo >::iterator it = m_mapTransformRadioMsgInfo.begin(); it != m_mapTransformRadioMsgInfo.end(); ++it )
		{
			for(DWORD i=0; i< it->second.vecMsgSndIndex.size(); i++)
				CEtSoundEngine::GetInstance().RemoveSound( it->second.vecMsgSndIndex[i] ); // 로드된 사운드 싹다지웁니다.
		}
	}

	SAFE_DELETE_MAP( m_mapTransformRadioMsgInfo ); // 해제.
}

CDnRadioMsgTask::SRadioMsgInfo *CDnRadioMsgTask::GetRadioMsgInfoFromIndex( DWORD dwIndex )
{
	if( dwIndex >= m_pVecRadioMsgInfo.size() ) return NULL;
	return m_pVecRadioMsgInfo[dwIndex];
}

CDnRadioMsgTask::SRadioMsgInfo *CDnRadioMsgTask::GetRadioMsgInfoFromID( int nID )
{
	for( DWORD i=0; i<m_pVecRadioMsgInfo.size(); i++ )
	{
		if( m_pVecRadioMsgInfo[i]->nID == nID )
		{
			return m_pVecRadioMsgInfo[i];
		}
	}
	return NULL;
}

void CDnRadioMsgTask::UseRadioMsg( USHORT nRadioID )
{
	// 먼저 파티에 가입되어있는지 확인.
	CDnPartyTask* pPartyTask = static_cast<CDnPartyTask*>(CTaskManager::GetInstance().GetTask( "PartyTask" ));
	if( !pPartyTask ) return;

	if ( pPartyTask->GetPartyRole() == CDnPartyTask::SINGLE )
		return;

	CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm )
		return;

	// 전에 보낸 것부터 3초가 지났는지 확인
	if( GetLocalTime() < m_RadioMsgSendTime + RADIOMSG_DELAY )
		return;

	m_RadioMsgSendTime = GetLocalTime();

	// 라디오 메세지 정보 확인.
	SRadioMsgInfo *pInfo = GetRadioMsgInfoFromID( nRadioID );
	if( !pInfo ) return;

	if( pInfo->nMacroNum == 0 )
	{
		SendUseRadio( nRadioID );
	}
	else
	{
		// 여기서부턴 매크로다.
		// 해당 매크로를 얻어와서, 파티말 하듯이 전송.
		int nMacroIndex = pInfo->nMacroNum-1;
		LPCWSTR wszMacro = NULL;
		if( CDnInterface::IsActive() && GetInterface().GetGameMacroMsgOptDlg() )
			wszMacro = GetInterface().GetGameMacroMsgOptDlg()->GetMacroString( nMacroIndex );
		if( wszMacro ) {
			std::wstring chatMsg = wszMacro;
			DN_INTERFACE::UTIL::CheckChat( chatMsg, '*' );
			if( pPartyTask->IsPvpGame() )
				SendChatMsg( CHATTYPE_TEAMCHAT, chatMsg.c_str() );
			else
				SendChatMsg( CHATTYPE_PARTY, chatMsg.c_str() );
		}
	}
}

void CDnRadioMsgTask::OnRecvRadioMsgMessage(int nSubCmd, char * pData, int nSize)
{
	switch (nSubCmd)
	{
	case eRadio::SC_USERADIO: OnRecvUseRadio((SCUseRadio*)pData);	break;
	}
}

void CDnRadioMsgTask::OnRecvUseRadio(SCUseRadio * pPacket)
{
	// 먼저 파티에 가입되어있는지 확인.
	CDnPartyTask* pPartyTask = static_cast<CDnPartyTask*>(CTaskManager::GetInstance().GetTask( "PartyTask" ));
	if( !pPartyTask ) return;

	if ( pPartyTask->GetPartyRole() == CDnPartyTask::SINGLE )
		return;

	CDnPartyTask::PartyStruct *pPartyInfo = pPartyTask->GetPartyDataFromSessionID( pPacket->nSessionID );
	if( !pPartyInfo ) return;

	SRadioMsgInfo *pRadioMsgInfo = GetRadioMsgInfoFromID( pPacket->nID );
	if( !pRadioMsgInfo ) return;

	if( !CDnActor::s_hLocalActor ) return;

	DnActorHandle hCurActor;
	if( CDnActor::s_hLocalActor->GetUniqueID() == pPacket->nSessionID )
		hCurActor = CDnActor::s_hLocalActor;
	else
		hCurActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );


	int nClassID = pPartyInfo->cClassID;

	// 직업에 맞는 메세지 처리.
	// 세션아이디로 액터핸들 구해서 계정레벨이 gm이라면 CHATTYPE_PARTY_GM을 사용해 음영을 표현할 수 있다.(우선 생략)
	if( CDnInterface::IsActive() )
	{
		if(m_bEnableTransformRadioMsg)
		{
			std::map< int , STransformMsgInfo >::iterator it = m_mapTransformRadioMsgInfo.find(m_nTrasnformMsgIndex);
			if(it != m_mapTransformRadioMsgInfo.end())
			{
				int nMsgIndex = pPacket->nID-1;

				if( (nMsgIndex >= 0) && ( nMsgIndex < (int)it->second.vecMsgSndIndex.size()) )
				{
					const WCHAR *ChatMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, it->second.vecMsgIndex[nMsgIndex] );
					GetInterface().AddChatMessage( CHATTYPE_PARTY, pPartyInfo->wszCharacterName, ChatMsg );
				}
			}
		}
		else
			GetInterface().AddChatMessage( CHATTYPE_PARTY, pPartyInfo->wszCharacterName, pRadioMsgInfo->szMsg[nClassID-1].c_str() );

		// 직업에 맞는 보이스 재생
		bool bPlaying = false;
		if( m_hCurVoiceChannel && m_hCurVoiceChannel->IsPlay() )
			bPlaying = true;

		if( bPlaying == false && pRadioMsgInfo->vecMsgSndIndex[nClassID-1].size() > 0 )
		{
			int nToPlaySndIndex = pRadioMsgInfo->vecMsgSndIndex[nClassID-1][_rand()%pRadioMsgInfo->vecMsgSndIndex[nClassID-1].size()];
			if(m_bEnableTransformRadioMsg)
			{
				std::map< int , STransformMsgInfo >::iterator it = m_mapTransformRadioMsgInfo.find(m_nTrasnformMsgIndex);
				if(it != m_mapTransformRadioMsgInfo.end())
				{
					int nMsgIndex = pPacket->nID-1;

					if( (nMsgIndex >= 0) && (nMsgIndex < (int)it->second.vecMsgSndIndex.size()) )
						nToPlaySndIndex = it->second.vecMsgSndIndex[nMsgIndex];
				}	
			}
			EtSoundChannelHandle hChannel;
			hChannel = CEtSoundEngine::GetInstance().PlaySound( "VOICE", nToPlaySndIndex, false, true );
			if( hChannel && hCurActor )
			{
				static float fSoundRange = 1500.0f;
				static float fRollOff = 0.375f;

				hChannel->SetVolume( 1.f );
				hChannel->SetPosition( *hCurActor->GetPosition() );
				hChannel->SetRollOff( 3, 0.f, 1.f, fSoundRange * fRollOff, 1.f, fSoundRange, 0.f );
				//hChannel->GetChannel()->setSpeakerMix( 0.f, 0.0f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f );
				hChannel->Resume();

				m_hCurVoiceChannel = hChannel;
			}
		}
	}

	// 캐릭터 머리 위에 아이콘.
	// 만약 기존의 말풍선과 겹친다면, 말풍선 삭제.
	if( hCurActor )
	{
		hCurActor->SetHeadIcon( pRadioMsgInfo->hHeadIconTex, GetTickCount(), 5000 );

		// 미니맵에 5초동안 표시
		// 준영씨. 여기에서 미니맵 표시하는 루틴 작성하시면 될거에요~
		if( CDnMinimap::IsActive() ) {
			int nIconIndex = pRadioMsgInfo->nID - 1;
			AssertRange( nIconIndex , 0, 5 );
			if( nIconIndex >= 0 && nIconIndex <= 5 ) {
				CDnMinimap::GetInstance().SetRadioMark( hCurActor, nIconIndex );
			}
		}
	}

	// Process위해 별도로 저장.
	m_nCurSessionID = pPacket->nSessionID;
}

void CDnRadioMsgTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !CDnActor::s_hLocalActor ) return;

	if( m_hCurVoiceChannel && m_hCurVoiceChannel->IsPlay() )
	{
		// 혹시 중간에 캐릭이 나가거나 할수도 있어서 이렇게 매번 구하는게 안전하긴 하다.
		DnActorHandle hActor;
		if( CDnActor::s_hLocalActor->GetUniqueID() == m_nCurSessionID )
			hActor = CDnActor::s_hLocalActor;
		else
			hActor = CDnActor::FindActorFromUniqueID( m_nCurSessionID );
		if( hActor )
		{
			m_hCurVoiceChannel->SetPosition( *hActor->GetPosition() );
		}
	}
}

void CDnRadioMsgTask::EnableTransformSound(int nTransformID,bool bEnable)
{
	if(bEnable)
	{
		DNTableFileFormat* pMonsterRadioBaseSox = GetDNTable( CDnTableDB::TMONSTER_TRANS_RADIOBASE );
		if(!pMonsterRadioBaseSox || !pMonsterRadioBaseSox->IsExistItem(nTransformID))
			return;

		int nRadioIndex = pMonsterRadioBaseSox->GetFieldFromLablePtr( nTransformID , "_MonsterRadio" )->GetInteger();
		m_nTrasnformMsgIndex = nRadioIndex;
	}
	else
	{
		m_nTrasnformMsgIndex = 0;
	}

	m_bEnableTransformRadioMsg = bEnable;
}

void CDnRadioMsgTask::LoadTransformSound()
{
	STransformMsgInfo TempInfo;
	memset(&TempInfo,0,sizeof(TempInfo));

	DNTableFileFormat* pMonsterRadioSox = GetDNTable( CDnTableDB::TMONSTER_TRANS_RADIOMSG );
	if(!pMonsterRadioSox )
		return;

	for(int i=0;i<pMonsterRadioSox->GetItemCount();i++)
	{
		int nItemIndex = pMonsterRadioSox->GetItemID(i);

		if(pMonsterRadioSox->IsExistItem(nItemIndex))
		{
			for(int i=0 ; i<NUM_SOUNDMSG ; i++)
			{
				std::string wstSoundName =  pMonsterRadioSox->GetFieldFromLablePtr( nItemIndex, FormatA("_MsgSnd%d", i+1).c_str())->GetString();
				int nMsgIndex = pMonsterRadioSox->GetFieldFromLablePtr( nItemIndex, FormatA("_MsgID%d", i+1).c_str() )->GetInteger();
				int nSndIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( wstSoundName.c_str() ).c_str(), true, false );

				TempInfo.vecMsgIndex.push_back(nMsgIndex);
				TempInfo.vecMsgSndIndex.push_back(nSndIndex);
			}

			m_mapTransformRadioMsgInfo.insert( std::make_pair( nItemIndex , TempInfo ) );
		}
	}
}

void CDnRadioMsgTask::ClearTransformSound()
{
	if(m_mapTransformRadioMsgInfo.empty())
		return;

	for(std::map< int , STransformMsgInfo >::iterator it = m_mapTransformRadioMsgInfo.begin(); it != m_mapTransformRadioMsgInfo.end(); ++it )
	{
		for(DWORD i=0; i< it->second.vecMsgSndIndex.size(); i++)
			CEtSoundEngine::GetInstance().RemoveSound( it->second.vecMsgSndIndex[i] ); // 로드된 사운드 싹다지웁니다.
	}

	m_mapTransformRadioMsgInfo.clear(); // 비워놓고 나중에 파이널라이즈할때 싹제거.
}
