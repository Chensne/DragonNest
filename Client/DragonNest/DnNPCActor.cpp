#include "stdafx.h"
#include "EtSoundEngine.h"
#include "DnNPCActor.h"
#include "DnTableDB.h"
#include "DnActorClassDefine.h"
#include "DnWorld.h"
#include "EtWorldEventArea.h"
#include "TaskManager.h"
#include "DnQuestTask.h"
#include "DnDataManager.h"
#include "DnWorldActProp.h"
#include "../../Common/EternityEngine/D3DDevice9/EtStateManager.h"
#include <mmsystem.h>
#include "DnCommonTask.h"

#include "DnTradeTask.h"
#include "DnTradeMail.h"
#include "DnInterface.h"

#include "DnPartyTask.h"
#include "GameOption.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

int STATE_MARK_SCALE_TIME  = 250;
float STATE_MARK_SCALE_RANGE  = 0.1f;

DnActorHandle CreateNpcActor(UINT uUniqueID, UINT nNpcID, EtVector3 Position, float fRotate)
{
	int  nActorTableID = -1;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNPC );
	if( !pSox ) return CDnActor::Identity();

#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat*  pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	if (!pFileNameSox)
	{
		_ASSERT(0);
		return CDnActor::Identity();
	}
#endif

	DNTableCell* pSoxField = pSox->GetFieldFromLablePtr( nNpcID, "_ActorIndex" );
	if ( !pSoxField ) nActorTableID  = -1;
	else nActorTableID  = pSoxField->GetInteger();

	// npc 생성
	DnActorHandle hNPC;
	hNPC = CreateActor( nActorTableID, false, false, false );

	CDnNPCActor* pNpc = dynamic_cast<CDnNPCActor*>(hNPC.GetPointer());
	if ( pNpc )
	{
		CDnNPCActor::NpcData data;
		data.nNpcID = nNpcID;
		char szTalkFileName[256] = {0,};
		WCHAR wszTalkFileName[256] = {0,};
//		strcpy_s(szName, pSox->GetFieldFromLablePtr(nNpcID, "_Name")->GetString());
		data.wszNpcName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nNpcID, "_NameID" )->GetInteger() );
		_strcpy(szTalkFileName, _countof(szTalkFileName), pSox->GetFieldFromLablePtr(nNpcID, "_TalkFile")->GetString(), (int)strlen(pSox->GetFieldFromLablePtr(nNpcID, "_TalkFile")->GetString()));

		MultiByteToWideChar(CP_ACP, 0, szTalkFileName, -1, wszTalkFileName, 256 );
		data.wszTalkFileName = wszTalkFileName;
		data.nParam[0] = pSox->GetFieldFromLablePtr( nNpcID, "_Param1" )->GetInteger();
		data.nParam[1] = pSox->GetFieldFromLablePtr( nNpcID, "_Param2" )->GetInteger();

		float fScale = 1.f;
		int nSizeMin = (int)( pSox->GetFieldFromLablePtr( nNpcID, "_SizeMin" )->GetFloat() * 100 );
		int nSizeMax = (int)( pSox->GetFieldFromLablePtr( nNpcID, "_SizeMax" )->GetFloat() * 100 );
		if( nSizeMin != nSizeMax ) {
			fScale = ( nSizeMin + ( rand()%( nSizeMax - nSizeMin + 1 ) ) ) * 0.01f;
		}
		else fScale = nSizeMin * 0.01f;

		pNpc->SetNPCJobType( CDnNPCActor::emJobType(pSox->GetFieldFromLablePtr( nNpcID, "_NpcJobIndex" )->GetInteger()) );
		pNpc->SetUniqueID( ( uUniqueID != -1 ) ? (DWORD)uUniqueID : CDnActor::s_dwUniqueCount++ );
		pNpc->SetPosition( Position );
		pNpc->SetScale( EtVector3( fScale, fScale, fScale ) );

		char szWeaponLable[32];
		for( int j=0; j<2; j++ ) {
			sprintf_s( szWeaponLable, "_Weapon%dIndex", j + 1 );
			int nWeapon = pSox->GetFieldFromLablePtr( nNpcID, szWeaponLable )->GetInteger();
			if( nWeapon < 1 ) continue;
			DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( nWeapon, _rand() );
			hNPC->AttachWeapon( hWeapon, j, true );
		}


		for( int j=1; j<=4; j++ ) 
		{
			char szLabel[128] = { 0,} ;
#ifdef PRE_FIX_MEMOPT_EXT
			sprintf_s( szLabel, "_BeginTalk%d", j );
			std::string szSoundFile;
			CommonUtil::GetFileNameFromFileEXT(szSoundFile, pSox, nNpcID, szLabel, pFileNameSox);
#else
			sprintf_s( szLabel, "_BeginTalk%d", j );
			if ( pSox->GetFieldFromLablePtr( nNpcID, szLabel ) == NULL )
				continue;

			std::string szSoundFile = pSox->GetFieldFromLablePtr( nNpcID, szLabel )->GetString();
#endif
			if ( !szSoundFile.empty() )
			{
				int nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( szSoundFile.c_str(), true, false );
				if( nSoundIdx == -1 ) {
#ifndef _FINAL_BUILD
					WCHAR wszStr[256];
					swprintf_s( wszStr, L"사운드 파일을 찾을 수 없습니다. : NpcID : %d, 이름 :%s, 파일 : %S\n", nNpcID, data.wszNpcName.c_str(), szSoundFile.c_str() );
					{
						ScopeLock< CSyncLock > Lock( g_pEtRenderLock );
						GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"Debug", wszStr, false);
					}
#endif
					continue;
				}
				data.BeginTalkSound.push_back(nSoundIdx);
			}
		}

		for( int j=1; j<=4; j++ ) 
		{
			char szLabel[128] = { 0,} ;
#ifdef PRE_FIX_MEMOPT_EXT
			sprintf_s( szLabel, "_FinishTalk%d", j );
			DNTableCell* pFinishTalkSoxField = pSox->GetFieldFromLablePtr(nNpcID, szLabel);
			if ( pFinishTalkSoxField == NULL )
				continue;

			std::string szSoundFile;
			CommonUtil::GetFileNameFromFileEXT(szSoundFile, pSox, nNpcID, szLabel, pFileNameSox);
#else
			sprintf_s( szLabel, "_FinishTalk%d", j );
			if ( pSox->GetFieldFromLablePtr( nNpcID, szLabel ) == NULL )
				continue;

			std::string szSoundFile = pSox->GetFieldFromLablePtr( nNpcID, szLabel )->GetString();
#endif
			if ( !szSoundFile.empty() )
			{
				int nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( szSoundFile.c_str(), true, false );
				if( nSoundIdx == -1 ) {
#ifndef _FINAL_BUILD
					WCHAR wszStr[256];
					swprintf_s( wszStr, L"사운드 파일을 찾을 수 없습니다. : NpcID : %d, 이름 :%s, 파일 : %S\n", nNpcID, data.wszNpcName.c_str(), szSoundFile.c_str() );
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"Debug", wszStr, false);
#endif
					continue;
				}
				data.FinishTalkSound.push_back(nSoundIdx);
			}
		}

		for( int j=1; j<=4; j++ ) 
		{
			char szLabel[128] = { 0,} ;
			sprintf_s( szLabel, "_OpenShop%d", j );
			if ( pSox->GetFieldFromLablePtr( nNpcID, szLabel ) == NULL )
				continue;

			std::string szSoundFile = pSox->GetFieldFromLablePtr( nNpcID, szLabel )->GetString();
			
			if ( !szSoundFile.empty() )
			{
				int nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( szSoundFile.c_str(), true, false );
				if( nSoundIdx == -1 ) {
#ifndef _FINAL_BUILD
					WCHAR wszStr[256];
					swprintf_s( wszStr, L"사운드 파일을 찾을 수 없습니다. : NpcID : %d, 이름 :%s, 파일 : %S\n", nNpcID, data.wszNpcName.c_str(), szSoundFile.c_str() );
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"Debug", wszStr, false);
#endif
					continue;
				}
				data.OpenShopSound.push_back(nSoundIdx);
			}
		}

		for( int j=1; j<=4; j++ ) 
		{
			char szLabel[128] = { 0,} ;
			sprintf_s( szLabel, "_CloseShop%d", j );
			if ( pSox->GetFieldFromLablePtr( nNpcID, szLabel ) == NULL )
				continue;

			std::string szSoundFile = pSox->GetFieldFromLablePtr( nNpcID, szLabel )->GetString();
			
			if ( !szSoundFile.empty() )
			{
				int nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( szSoundFile.c_str(), true, false );
				if( nSoundIdx == -1 ) {
#ifndef _FINAL_BUILD
					WCHAR wszStr[256];
					swprintf_s( wszStr, L"사운드 파일을 찾을 수 없습니다. : NpcID : %d, 이름 :%s, 파일 : %S\n", nNpcID, data.wszNpcName.c_str(), szSoundFile.c_str() );
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"Debug", wszStr, false);
#endif
					continue;
				}
				data.CloseShopSound.push_back(nSoundIdx);
			}
		}


		pNpc->Initialize(data);


		EtVector2 vDir( sin( EtToRadian( fRotate ) ), cos( EtToRadian( fRotate ) ) );
		pNpc->Look( vDir );

		std::string strAction = hNPC->GetElement( "Summon_On" ) ? "Summon_On" : "Stand";
		
		CEtActionBase::ActionElementStruct *pStruct = hNPC->GetElement( strAction.c_str() );
		if( pStruct ) {
			hNPC->SetActionQueue( strAction.c_str(), 0, 0.f, (float)( timeGetTime()%pStruct->dwLength ), false );
		}

		hNPC->SetProcess( true );

		bool bSoftAppear = true;
		if( CDnPartyTask::IsActive() && CDnWorld::IsActive() && !CDnPartyTask::GetInstance().IsSyncComplete() && CDnWorld::GetInstance().GetMapType() >= CDnWorld::MapTypeWorldMap )
			bSoftAppear = false;
		hNPC->SetSoftAppear( bSoftAppear );
	}

	return hNPC;
}
 
CDnNPCActor::CDnNPCActor(int nClassID, bool bProcess )
	: CDnActor( nClassID, bProcess )
{
	CDnActionBase::Initialize( this );
	CDnActorState::Initialize(nClassID);

	m_bShowHeadName = true;
	
	m_nLastUpdateTime = 0;
	m_nStateMark = MarkTypeNone;

	m_hStateMarkTex[QuestAvailable_Main]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestGrant.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[QuestPlaying_Main]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestNow.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[QuestRecompense_Main] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestComplete.dds" ).c_str(), RT_TEXTURE );

	m_hStateMarkTex[QuestAvailable_Sub]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestGrantSub.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[QuestPlaying_Sub]		= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestNowSub.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[QuestRecompense_Sub]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestCompleteSub.dds" ).c_str(), RT_TEXTURE );

	m_hStateMarkTex[CrossReQuestGrant]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossReQuestGrant.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[CrossReQuestNow]		= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossReQuestNow.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[CrossReQuestComplete]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossReQuestComplete.dds" ).c_str(), RT_TEXTURE );

	m_hStateMarkTex[MailReceiveNew]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossMail.dds" ).c_str(), RT_TEXTURE );

	m_hStateMarkTex[CrossFavorGrant]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossFavorGrant.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[CrossFavorNow]		= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossFavorNow.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[CrossFavorComplete]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossFavorComplete.dds" ).c_str(), RT_TEXTURE );

	m_hStateMarkTex[CrossQuestGlobalGrant]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestGlobalGrant.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[CrossQuestGlobalNow]		= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestGlobalNow.dds" ).c_str(), RT_TEXTURE );
	m_hStateMarkTex[CrossQuestGlobalComplete]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossQuestGlobalComplete.dds" ).c_str(), RT_TEXTURE );

	m_hStateMarkTex[CrossNoticeWarpNpc]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossWarp.dds" ).c_str(), RT_TEXTURE );

#ifdef PRE_ADD_58449
	m_hStateMarkTex[CrossNoticePcBang]	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossPCNpc.dds" ).c_str(), RT_TEXTURE );
#endif

#ifdef PRE_ADD_REPAIR_NPC
	m_hStateMarkTex[CrossNoticeRepair] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossRepair.dds" ).c_str(), RT_TEXTURE );
#endif
	//rlkt_test added new
	m_hStateMarkTex[CrossNoticePresent1] = LoadResource(CEtResourceMng::GetInstance().GetFullName("CrossPresentNpc.dds").c_str(), RT_TEXTURE);
	m_hStateMarkTex[CrossNoticePresent2] = LoadResource(CEtResourceMng::GetInstance().GetFullName("CrossPresentNpc.dds").c_str(), RT_TEXTURE);
	m_hStateMarkTex[CrossNoticeEventNPC1] = LoadResource(CEtResourceMng::GetInstance().GetFullName("CrossEventNpc.dds").c_str(), RT_TEXTURE);
	m_hStateMarkTex[CrossNoticeEventNPC2] = LoadResource(CEtResourceMng::GetInstance().GetFullName("CrossEventNpc.dds").c_str(), RT_TEXTURE);
	m_hStateMarkTex[CrossNoticeLife] = LoadResource(CEtResourceMng::GetInstance().GetFullName("LifeCrossHair09.dds").c_str(), RT_TEXTURE);



	SetHP(1000);

	m_bExistSafeZone = false;
	m_bEnableOperator = true;

	UpdateStateMark();

}

CDnNPCActor::~CDnNPCActor()
{

	for ( int i = 1 ; i < MaxCount ; i++ )
	{
		SAFE_RELEASE_SPTR( m_hStateMarkTex[i] );
	}

	for ( size_t i = 0 ; i < m_NpcData.BeginTalkSound.size() ; i++ )
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_NpcData.BeginTalkSound[i] );
	}
	for ( size_t i = 0 ; i < m_NpcData.FinishTalkSound.size() ; i++ )
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_NpcData.FinishTalkSound[i] );
	}
	for ( size_t i = 0 ; i < m_NpcData.OpenShopSound.size() ; i++ )
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_NpcData.OpenShopSound[i] );
	}
	for ( size_t i = 0 ; i < m_NpcData.CloseShopSound.size() ; i++ )
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_NpcData.CloseShopSound[i] );
	}
}

void CDnNPCActor::Initialize(NpcData& data)
{
#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	CDnNPCActor::Initialize();
#else
	CDnActor::Initialize();
#endif
	
	SetName(data.wszNpcName.c_str());

	m_NpcData = data;

	RefreshSafeZone();

	if( m_hObject )
	{
		m_hObject->SetCalcPositionFlag( CALC_POSITION_X | CALC_POSITION_Y | CALC_POSITION_Z );
	}

	m_ForceNextActionName = "";
}

#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
bool CDnNPCActor::Initialize()
{
	bool bResult = CDnActor::Initialize();

	if( m_hObject ) {
		m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 3 ) );
		m_hObject->SetTargetCollisionGroup( 0 );
		m_nPressLevel = 0;
	}

	return bResult;
}
#endif

void CDnNPCActor::RefreshSafeZone()
{
	// NPC SafeZone 셋팅
	if( CDnWorld::IsActive() ) {
		char szTemp[32];
		std::vector<CEtWorldEventArea *> VecArea;

		sprintf_s( szTemp, "NpcSafeZone %d", m_NpcData.nNpcID );
		CDnWorld::GetInstance().FindEventAreaFromSubStr( ETE_EventArea, szTemp, &VecArea );
		if( !VecArea.empty() ) {
			m_SafeZone = *VecArea[0]->GetOBB();
			m_SafeZone.Center.y = CDnWorld::GetInstance().GetHeight( m_SafeZone.Center );
			m_bExistSafeZone = true;
		}
		else {
			CDnWorld::GetInstance().FindEventAreaFromName( ETE_EventArea, "NpcSafeZone All", &VecArea );
			if( !VecArea.empty() ) {
				SOBB OBB;
				for( DWORD i=0; i<VecArea.size(); i++ ) {
					OBB = *VecArea[i]->GetOBB();
					OBB.Center.y = CDnWorld::GetInstance().GetHeight( OBB.Center );
					if( OBB.IsInside( *GetPosition() ) ) {
						m_SafeZone = OBB;
						m_bExistSafeZone = true;
						break;
					}
				}
			}
		}
	}
}

void CDnNPCActor::Process(LOCAL_TIME LocalTime, float fDelta )
{
	CDnActor::ProcessActor( LocalTime, fDelta );

	MASingleBody::PreProcess( LocalTime, fDelta );
	m_pMovement->Process( LocalTime, fDelta );
	MASingleBody::Process( m_matexWorld, LocalTime, fDelta );

	if ( LocalTime - m_nLastUpdateTime > 1000 )
	{
		UpdateStateMark();
		m_nLastUpdateTime = LocalTime;
	}
	ProcessHeadLook( m_matexWorld, fDelta, IsSignalRange( STE_HeadLook ) );
	ProcessVisual( LocalTime, fDelta );
}

void CDnNPCActor::OnSoundPlay(eSoundType type)
{
	std::vector<int>* pTargetSoundSet(NULL);

	switch(type)
	{
	case BeginTalk:
		pTargetSoundSet = &(m_NpcData.BeginTalkSound);
		break;
	case FinishTalk:
		pTargetSoundSet = &(m_NpcData.FinishTalkSound);
		break;
	case OpenShop:
		pTargetSoundSet = &(m_NpcData.OpenShopSound);
	    break;
	case CloseShop:
		pTargetSoundSet = &(m_NpcData.CloseShopSound);
	    break;
	}

	if ( pTargetSoundSet == NULL )
		return;
	
	if ( pTargetSoundSet->empty() )
		return;

	int nSize = (int)pTargetSoundSet->size();
	int nIdx = _rand()%nSize;

	int nSoundIdx = -1;
	nSoundIdx = pTargetSoundSet->at(nIdx);
	
	
	if ( nSoundIdx > -1 )
	{
		if ( m_NpcData.m_hCurPlaySound )
		{
			if ( m_NpcData.m_hCurPlaySound->IsPlay() )
			{
				return;
			}
			else
				m_NpcData.m_hCurPlaySound.Identity();
		}

		static float fVolume = 1.f;
		static float fSoundRange = 1200.0f;
		static float fRollOff = 0.375f;

		// 너무 멀면 일단 걍 플레이 안한다.
		EtVector3 vListen = CEtSoundEngine::GetInstance().GetListenerPos();

		float fLength = EtVec3Length( &(EtVector3)( GetObjectCross()->m_vPosition - vListen ) );
		if( fLength > fSoundRange * 1.5f ) 	return;

		m_NpcData.m_hCurPlaySound = CEtSoundEngine::GetInstance().PlaySound__("VOICE", nSoundIdx, false, true );

		if( m_NpcData.m_hCurPlaySound )
		{
			m_NpcData.m_hCurPlaySound->SetVolume( fVolume );
			m_NpcData.m_hCurPlaySound->SetPosition( *GetPosition() );
			m_NpcData.m_hCurPlaySound->SetRollOff( 3, 0.f, 1.f, fSoundRange * fRollOff, 1.f, fSoundRange, 0.f );
			m_NpcData.m_hCurPlaySound->GetChannel()->setSpeakerMix( 0.f, 0.0f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f );
			m_NpcData.m_hCurPlaySound->Resume();
		}
	}
}


void CDnNPCActor::RenderCustom( float fElapsedTime )
{
	if( m_bShowHeadName && IsShow() ) 
	{
		CalcCustomRenderDepth();
		
		int nAlpha = (int)(GetAlphaLayer( AL_APPEAR ) * 255);
		nAlpha = EtClamp(nAlpha, 0, 255);

		// 이름
		AddHeadNameElement( 0, GetName(), s_nFontIndex, 16, (textcolor::NPCNAME & 0x00FFFFFF) | ( nAlpha << 24 ) , textcolor::NPCNAME_S );

		if ( m_nStateMark >= QuestAvailable_Main && m_nStateMark < MaxCount && m_hStateMarkTex[m_nStateMark] )
		{
			EtVector2 vSize;
			vSize.x = m_hStateMarkTex[m_nStateMark]->Width() * 0.5f;
			vSize.y = m_hStateMarkTex[m_nStateMark]->Height() * 0.5f;

			int nTemp = (DWORD)CDnActionBase::m_LocalTime%(STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME);
			if( nTemp < STATE_MARK_SCALE_TIME ) {
				vSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * nTemp;
			}
			else {
				vSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * ( (STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME) - nTemp );
			}

			AddHeadNameElement( 1, m_hStateMarkTex[m_nStateMark], vSize, D3DCOLOR_ARGB(nAlpha,255,255,255), 17.f );
		}
	}
	CDnActor::RenderCustom( fElapsedTime );
}

CDnNPCActor* CDnNPCActor::FindNpcActorFromID( int nNpcID )
{
	for( DWORD i=0; i<CDnActor::s_pVecProcessList.size(); i++ )
	{
		if( CDnActor::s_pVecProcessList[i]->GetActorType() == CDnActorState::Npc )
		{
			CDnNPCActor *pNpcActor = dynamic_cast<CDnNPCActor*>(CDnActor::s_pVecProcessList[i]);
			if( pNpcActor->GetNpcData().nNpcID == nNpcID )
				return pNpcActor;
		}
	}
	return NULL;
}

void CDnNPCActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_ChangeAction:
			{
				ChangeActionStruct *pStruct = (ChangeActionStruct *)pPtr;
				if( _rand()%100 < pStruct->nRandom ) {
					CmdAction( pStruct->szAction );
				}
			}
			return;
		case STE_Projectile: // Npc 는 Projectile 을 날릴 수 없게 상위에서 막아놓차.
			return;
	}
	CDnActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

void CDnNPCActor::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
{
	switch( dwActorProtocol ) {
		case eActor::SC_CMDACTION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount;
				float fBlendFrame;
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );

				if( nActionIndex != -1 && GetElement( nActionIndex ) ) {
					std::string szAction = GetElement( nActionIndex )->szName;
					CmdAction( szAction.c_str(), nLoopCount, fBlendFrame );
				}
			}
			break;
		case eActor::SC_CMDENABLEOPERATOR:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				Stream.Read( &m_bEnableOperator, sizeof(bool) );
			}
			break;
	}
	CDnActor::OnDispatchMessage( dwActorProtocol, pPacket );
}

void CDnNPCActor::OnChangeAction( const char *szPrevAction )
{
	if( m_hPropHandle ) {
		CDnWorldActProp *pProp = (CDnWorldActProp*)m_hPropHandle.GetPointer();
		if( pProp->IsExistAction( m_szAction.c_str() ) )
			pProp->SetAction( m_szAction.c_str(), CDnActionBase::m_fFrame, 0.f, false );
	}
}

void CDnNPCActor::OnChangeActionQueue( const char *szPrevAction )
{
	if( m_hPropHandle ) ((CDnWorldActProp*)m_hPropHandle.GetPointer())->SetActionQueue( m_szActionQueue.c_str(), m_nLoopCount, m_fQueueBlendFrame, m_fQueueStartFrame );
}

bool CDnNPCActor::LoadSkin( const char *szSkinName, const char *szAniName, bool bTwoPassAlpha)
{
	bool bResult = MASingleBody::LoadSkin( szSkinName, szAniName, bTwoPassAlpha );
	if( !bResult ) return false;

	m_hObject->SetCalcPositionFlag( CALC_POSITION_X|CALC_POSITION_Y|CALC_POSITION_Z );
	return true;
}

//// NPC Actor  Quest Func  ////


void CDnNPCActor::UpdateStateMark()
{
	m_nStateMark = CalcStateMark( m_NpcData.nNpcID );
	// 보상 상태 검사
}

bool CDnNPCActor::CanTalk()
{
	if( !m_bEnableOperator ) return false;
	if( !m_bExistSafeZone ) return true;
	SSphere Sphere;
	float fRadius = max( m_SafeZone.Extent[0], m_SafeZone.Extent[2] );

	DNVector(DnActorHandle) hVecList;
	CDnActor::ScanActor( m_SafeZone.Center, fRadius, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( hVecList[i] == GetMySmartPtr() ) continue;
		if( hVecList[i]->GetActorType() <= CDnActorState::Reserved6 ) continue;
		if( hVecList[i]->GetActorType() == CDnActorState::Npc ) continue;
		if( hVecList[i]->GetActorType() == CDnActorState::Pet ) continue;
		if( hVecList[i]->IsDie() ) continue;
		if( CDnActor::s_hLocalActor->GetTeam() == hVecList[i]->GetTeam() ) continue;

		hVecList[i]->GetBoundingSphere( Sphere );
		if( TestOBBToSphere( m_SafeZone, Sphere ) == true ) return false;
	}

	return true;
}

int CDnNPCActor::GetQuestStateFromMark(int nStateMark)
{
	switch(nStateMark)
	{
	case QuestAvailable_Main:
	case QuestAvailable_Sub:
	case CrossFavorGrant:
	case CrossReQuestGrant:
	case CrossQuestGlobalGrant:
		return QuestState_None;       // 0 = 수락가능

	case QuestPlaying_Main:
	case QuestPlaying_Sub:
	case CrossFavorNow:
	case CrossReQuestNow:
	case CrossQuestGlobalNow:
		return QuestState_Playing;    // 1 = 진행중

	case QuestRecompense_Main:
	case QuestRecompense_Sub:
	case CrossFavorComplete:
	case CrossReQuestComplete:
	case CrossQuestGlobalComplete:
		return QuestState_Recompense; // 2 = 완료
	}

	return -1;
}

int CDnNPCActor::CalcStateMarkRank ( int nStateMark ) // [보상 / 진행중 / 습득] +  [메인 / 글로벌 / 호감 / 일반] 순서로 중요도가 결정 됩니다.
{
	int nRank = -1;
	switch(nStateMark)
	{
	case CrossNoticeRepair:        nRank = 1; break;
	case CrossNoticePcBang:        nRank = 1; break;
	case CrossNoticeWarpNpc:       nRank = 1; break;
	case QuestRecompense_Main:     nRank = 1; break;
	case CrossQuestGlobalComplete: nRank = 2; break;
	case CrossFavorComplete:       nRank = 3; break;
	case QuestRecompense_Sub:      nRank = 4; break;
	case CrossReQuestComplete:     nRank = 5; break;
	case QuestPlaying_Main:        nRank = 6; break;
	case CrossQuestGlobalNow:      nRank = 7; break;
	case CrossFavorNow:            nRank = 8; break;
	case QuestPlaying_Sub:         nRank = 9; break;
	case CrossReQuestNow:          nRank = 10; break;
	case QuestAvailable_Main:      nRank = 11; break;
	case CrossQuestGlobalGrant:    nRank = 12; break;
	case CrossFavorGrant:          nRank = 13; break;
	case QuestAvailable_Sub:       nRank = 14; break;
	case CrossReQuestGrant:        nRank = 15; break;
	}
	return nRank;
}

int CDnNPCActor::CalcStateMark( int nNpcID )
{
	CDnQuestTask *pTask = (CDnQuestTask*)CTaskManager::GetInstance().GetTask( "QuestTask" );

	if( !pTask ) 
		return MarkTypeNone;

	std::vector<int> vStateMark;
	vStateMark.reserve(MAX_PLAY_QUEST);
	vStateMark.clear();

	//현재 내가 가지고 있는 퀘스트 목록 얻어서  완료인지 진행인지 알수 잇고
	const TQuestGroup* pQuestGroup = pTask->GetQuestGroup();

	if ( !pQuestGroup )
		return MarkTypeNone;
	// 여기선 느낌표와 보상 마크 찍어줄지 결정한다.
	// 퀘스트 진행중인지 보상 받는지 현재 저널에서 정보를 가져와서 마크 세팅을 해준다. 
	for ( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		int nQuestIdx = pQuestGroup->Quest[i].nQuestID;	
		int nJournalIdx = (int)pQuestGroup->Quest[i].cQuestJournal;

		if ( pQuestGroup->Quest[i].nQuestID > 0 )
		{
			Journal* pJournal = g_DataManager.GetJournalData(pQuestGroup->Quest[i].nQuestID);
			if ( pJournal )
			{
				for ( int j = 0 ; j < (int)pJournal->JounalPages.size() ; j++ )
				{
					if ( pJournal->JounalPages[j].nJournalPageIndex == nJournalIdx &&
						pJournal->JounalPages[j].nStateMarkNpcID == nNpcID)
					{
						vStateMark.push_back( pJournal->JounalPages[j].nStateMarkType );
					}
				}
			}
		}
		
	}

	if(!vStateMark.empty())
	{
		int nRank = MaxCount;
		int nStateMark = 0;

		for(DWORD i =0; i<vStateMark.size() ; i++)
		{
			if((CalcStateMarkRank(vStateMark[i]) < nRank) && (CalcStateMarkRank(vStateMark[i]) > 0))
			{
				nRank = CalcStateMarkRank(vStateMark[i]);        // 가장 우선순위가 높은 마크가 누구인가?
				nStateMark = vStateMark[i];   // 랭킹이 갱신될때마다 넣어줌
			}
		}

		if((nStateMark < MaxCount) && nStateMark > 0)
		{
			return nStateMark;               // 계산한 마크가 존재한다면 리턴
		}
	}


	// 위에서 안걸린 경우는 퀘스트 수락 가능한지 확인해서 물음표 마크를 찍어준다.
	TNpcData* pNPCData = g_DataManager.GetNpcData( nNpcID );
	if ( !pNPCData ) 
		return MarkTypeNone;

	for ( int i = 0 ; i < QUEST_MAX_CNT ; i++ )
	{
		int nQuestIdx = pNPCData->QuestIndexArr[i];

		if ( nQuestIdx > 0 )
		{
			QuestCondition* pQuestCondition = g_DataManager.GetQuestCondition(nQuestIdx);
			Journal* pJournal = g_DataManager.GetJournalData(nQuestIdx);


			if ( pQuestCondition && pJournal )
			{
				if ( pQuestCondition->Check() && pTask->FindPlayingQuest(nQuestIdx) == -1 && pTask->IsClearQuest(nQuestIdx) == false )
				{

					if(CGameOption::GetInstance().m_bHideQuestMarkByLevel && CDnActor::s_hLocalActor && (pJournal->nQuestLevel+10 <= CDnActor::s_hLocalActor->GetLevel()) 
						&& ( pJournal->nQuestMarkType == QuestAvailable_Sub || pJournal->nQuestMarkType == CrossReQuestGrant ) 
						&& g_DataManager.GetQuestHideType(nQuestIdx) ) // 새로받는경우에만 표시해준다 , 진행단계인데 표시하지 않는다면 불편한 상황이 생기기때문.
					{
						continue;
					}

					vStateMark.push_back( pJournal->nQuestMarkType );
				}
			}
		}
	}

	if(!vStateMark.empty())
	{
		int nRank = MaxCount;
		int nStateMark = 0;

		for(DWORD i =0; i<vStateMark.size() ; i++)
		{
			if((CalcStateMarkRank(vStateMark[i]) < nRank) && (CalcStateMarkRank(vStateMark[i]) > 0))
			{
				nRank = CalcStateMarkRank(vStateMark[i]);        // 가장 우선순위가 높은 마크가 누구인가?
				nStateMark = vStateMark[i];   // 랭킹이 갱신될때마다 넣어줌
			}
		}

		if((nStateMark < MaxCount) && nStateMark > 0)
		{
#ifdef PRE_FIX_63975
			if( nStateMark == CrossNoticePcBang )
			{
				CDnCommonTask *pCommonTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask("CommonTask"));
				if( pCommonTask && pCommonTask->IsHavePcBangRentalItem() )
				{
					return MarkTypeNone;
				}
			}
#endif
			return nStateMark;               // 계산한 마크가 존재한다면 리턴
		}
	}


	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNPC);
	if (pSox != NULL)
	{
		CDnNPCActor::emJobType type = CDnNPCActor::emJobType(pSox->GetFieldFromLablePtr( nNpcID, "_NpcJobIndex" )->GetInteger());
		if (type == typeMail)
		{
			if( GetTradeTask().GetTradeMail().GetUnreadMailCount() > 0 )
				return MailReceiveNew;
		}
		// Rotha 길드의뢰 통합 완료 게시판같은경우는 스크립트와 무관하게 커먼테스크에 저장되어있는 퀘스트정보값을 이용한다.
		if(type == typeCompleteQuest)
		{
			CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
			if( pTask && pTask->IsHaveCompletedQuest())
				return CrossReQuestComplete;
		}
	}

	return MarkTypeNone;
}

void CDnNPCActor::GetAvailableQuestList(std::vector<int> &vecAvailableQuestList , int nNpcID ) 
{
	vecAvailableQuestList.clear();

	CDnQuestTask *pTask = (CDnQuestTask*)CTaskManager::GetInstance().GetTask( "QuestTask" );
	if( !pTask ) 
		return;

	const TQuestGroup* pQuestGroup = pTask->GetQuestGroup();
	if ( !pQuestGroup )
		return;

	TNpcData* pNPCData = g_DataManager.GetNpcData( nNpcID );
	if ( !pNPCData ) 
		return;

	
	for ( int i = 0 ; i < QUEST_MAX_CNT ; i++ )
	{
		int nQuestIdx = pNPCData->QuestIndexArr[i];

		if ( nQuestIdx > 0 )
		{
			QuestCondition* pQuestCondition = g_DataManager.GetQuestCondition(nQuestIdx);
			Journal* pJournal = g_DataManager.GetJournalData(nQuestIdx);

			if ( pQuestCondition && pJournal )
			{
				if ( pQuestCondition->Check() && pTask->FindPlayingQuest(nQuestIdx) == -1 && pTask->IsClearQuest(nQuestIdx) == false )
					vecAvailableQuestList.push_back( pJournal->nQuestIndex );
			}
		}
	}

	for ( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		int nQuestIdx = pQuestGroup->Quest[i].nQuestID;
		int nJournalIdx = (int)pQuestGroup->Quest[i].cQuestJournal;

		if ( pQuestGroup->Quest[i].nQuestID > 0 )
		{
			Journal* pJournal = g_DataManager.GetJournalData(pQuestGroup->Quest[i].nQuestID);
			if ( pJournal )
			{
				for ( int j = 0 ; j < (int)pJournal->JounalPages.size() ; j++ )
				{
					if ( pJournal->JounalPages[j].nJournalPageIndex == nJournalIdx &&
						pJournal->JounalPages[j].nStateMarkNpcID == nNpcID)
					{
						vecAvailableQuestList.push_back( pQuestGroup->Quest[i].nQuestID );
					}
				}
			}
		}
	}
}


void CDnNPCActor::GetAvailableQuestName(std::vector<std::wstring> &vecAvailableQuestName , int nNpcID )
{
	vecAvailableQuestName.clear();

	CDnQuestTask *pTask = (CDnQuestTask*)CTaskManager::GetInstance().GetTask( "QuestTask" );
	if( !pTask ) 
		return;

	const TQuestGroup* pQuestGroup = pTask->GetQuestGroup();
	if ( !pQuestGroup )
		return;

	TNpcData* pNPCData = g_DataManager.GetNpcData( nNpcID );
	if ( !pNPCData ) 
		return;

	for ( int i = 0 ; i < MAX_PLAY_QUEST ; i++ ) // 요때는 진행중 태그가 붙는다.
	{
		int nQuestIdx = pQuestGroup->Quest[i].nQuestID;
		int nJournalIdx = (int)pQuestGroup->Quest[i].cQuestJournal;

		if ( pQuestGroup->Quest[i].nQuestID > 0 )
		{
			Journal* pJournal = g_DataManager.GetJournalData(pQuestGroup->Quest[i].nQuestID);
			if ( pJournal )
			{
				for ( int j = 0 ; j < (int)pJournal->JounalPages.size() ; j++ )
				{
					if ( pJournal->JounalPages[j].nJournalPageIndex == nJournalIdx &&
						pJournal->JounalPages[j].nStateMarkNpcID == nNpcID)
					{
						std::wstring wszTitleWithTag;
#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
						if( pJournal->nQuestLevel > 0 )
							wszTitleWithTag = FormatW( L"[%s%d] %s ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7888 ), pJournal->nQuestLevel , pJournal->wszQuestTitle.c_str());
						else
							wszTitleWithTag = pJournal->wszQuestTitle;
#else 
						if( pJournal->nQuestLevel > 0 )
							wszTitleWithTag = FormatW( L"[Lv.%d] %s ", pJournal->nQuestLevel , pJournal->wszQuestTitle.c_str());
						else
							wszTitleWithTag = pJournal->wszQuestTitle;
#endif 

						switch(pJournal->JounalPages[j].nStateMarkType)
						{

						case CDnNPCActor::QuestRecompense_Main:
						case CDnNPCActor::QuestRecompense_Sub:
						case CDnNPCActor::CrossFavorComplete:
						case CDnNPCActor::CrossReQuestComplete:
							{
								wszTitleWithTag += FormatW(L"#j%s#d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 435 ));
							}
							break;

						case CDnNPCActor::QuestPlaying_Main:
						case CDnNPCActor::QuestPlaying_Sub:
						case CDnNPCActor::CrossFavorNow:
						case CDnNPCActor::CrossReQuestNow:
							{
								wszTitleWithTag += FormatW(L"#b%s#d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 406 ));
							}
							break;

						default:
							break;
						}

						vecAvailableQuestName.push_back( wszTitleWithTag );
					}
				}
			}
		}
	}
	
	for ( int i = 0 ; i < QUEST_MAX_CNT ; i++ )
	{
		int nQuestIdx = pNPCData->QuestIndexArr[i];

		if ( nQuestIdx > 0 )
		{
			QuestCondition* pQuestCondition = g_DataManager.GetQuestCondition(nQuestIdx);
			Journal* pJournal = g_DataManager.GetJournalData(nQuestIdx);

			if ( pQuestCondition && pJournal )
			{
				if ( pQuestCondition->Check() && pTask->FindPlayingQuest(nQuestIdx) == -1 && pTask->IsClearQuest(nQuestIdx) == false )
				{
					std::wstring wszTitleWithTag;
					if( pJournal->nQuestLevel > 0 )
#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
						wszTitleWithTag = FormatW( L"[%s%d] %s ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7888 ), pJournal->nQuestLevel , pJournal->wszQuestTitle.c_str());
#else
						wszTitleWithTag = FormatW( L"[Lv.%d] %s ", pJournal->nQuestLevel , pJournal->wszQuestTitle.c_str());
#endif 
					else
						wszTitleWithTag = pJournal->wszQuestTitle;

					vecAvailableQuestName.push_back( wszTitleWithTag );
				}
			}
		}
	}
}

void CDnNPCActor::OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time)
{
	if (m_ForceNextActionName.empty() == false)
	{
		SetActionQueue(m_ForceNextActionName.c_str());
		m_ForceNextActionName = "";
	}

	CDnActor::OnFinishAction( szPrevAction, szNextAction, time );
}

void CDnNPCActor::SetForceActionOnFinishCurrentAction(const std::string& actionName)
{
	m_ForceNextActionName = actionName;
}