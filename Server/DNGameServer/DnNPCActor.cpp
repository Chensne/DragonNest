#include "stdafx.h"
#include "DnNPCActor.h"

#include "DnTableDB.h"
#include "DnActorClassDefine.h"
#include "DNQuest.h"
#include "DNQuestManager.h"
#include "DnActor.h"
#include "DNRUDPGameServer.h"
#include "DNUserSession.h"
#include "DNGameServerManager.h"
#include "DnWorld.h"
#include "DnWorldActProp.h"



DnActorHandle CreateNpcActor(CMultiRoom *pRoom, UINT nNpcID, TNpcData* pNpcData, EtVector3 Position, float fRotate)
{
	std::string szName;
	int  nActorTableID = -1;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNPC );

	DNTableCell* pSoxField = pSox->GetFieldFromLablePtr( nNpcID, "_ActorIndex" );
	if ( !pSoxField )
		return CDnActor::Identity();

	nActorTableID  = pSoxField->GetInteger();

	// npc 생성
	DnActorHandle hNPC;
	hNPC = CreateActor( pRoom, nActorTableID );
	if( !hNPC )
		return CDnActor::Identity();

	CDnNPCActor* pNpc = dynamic_cast<CDnNPCActor*>(hNPC.GetPointer());
	if ( pNpc )
	{
		pNpc->SetUniqueID( STATIC_INSTANCE_(CDnActor::s_dwUniqueCount)++ );
		pNpc->SetPosition( Position );
		pNpc->Initialize(pNpcData);

		EtVector2 vDir( sin( EtToRadian( fRotate ) ), cos( EtToRadian( fRotate ) ) );
		pNpc->Look( vDir );

		hNPC->SetActionQueue( "Stand" );
	}
	return hNPC;
}


CDnNPCActor::CDnNPCActor( CMultiRoom *pRoom, int nClassID )
: CDnActor( pRoom, nClassID )
{
	CDnActionBase::Initialize( this );
	CDnActorState::Initialize(nClassID);
	SetActionQueue("Stand");

	SetHP(INT_MAX);	

	m_bIsTriggerNpc = false;
}

CDnNPCActor::~CDnNPCActor()
{

}

MAMovementBase* CDnNPCActor::CreateMovement()
{
	MAMovementBase* pMovement = new IBoostPoolMAWalkMovement();
	return pMovement;
}

void CDnNPCActor::Initialize(TNpcData* pNpcData)
{
#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	CDnNPCActor::Initialize();
#else
	CDnActor::Initialize();
#endif

	memset(&m_NpcData, 0, sizeof(TNpcData));
	if( pNpcData )
		memcpy(&m_NpcData, pNpcData, sizeof(TNpcData));
	
	CDnNpc::Create(pNpcData);
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

void CDnNPCActor::Process(LOCAL_TIME LocalTime, float fDelta )
{
	CDnActor::Process( LocalTime, fDelta );

	MASingleBody::PreProcess( LocalTime, fDelta );
	m_pMovement->Process( LocalTime, fDelta );
	MASingleBody::Process( m_Cross, LocalTime, fDelta );
}

bool CDnNPCActor::Talk(CDNUserBase* pUserBase, UINT nNpcUniqueID, IN std::wstring& wszIndex, IN std::wstring& wszTarget)
{
	CDNUserSession* pUserSession = static_cast<CDNUserSession*>(pUserBase);
	UINT userObjectID = pUserSession->GetSessionID();

	CDNRUDPGameServer * pServer = g_pGameServerManager->GetGameServerByUID(userObjectID);
	if (pServer == NULL)	
	{
		_DANGER_POINT();
		return false;
	}

	DN_ASSERT(NULL != pUserBase,	"Invalid!");

	std::transform(wszTarget.begin(), wszTarget.end(), wszTarget.begin(), towlower); 
	TALK_MAP_IT iter = m_TalkMap.find(wszTarget);
	
	// npc 토크가 아니라면 
	if ( iter == m_TalkMap.end() )
	{
		// 퀘스트에서 한번 뒤져 본다.
		CDNQuest* pQuest = g_pQuestManager->GetQuest(wszTarget);

		if ( !pQuest )
		{
			g_Log.Log(LogType::_ERROR, L"QuestFind : Cannot found TalkTarget : %s\n",  wszTarget.c_str());
			_DANGER_POINT();
			return false;
		}

		pUserSession->SetCalledNpcResponse(false, false);

		// 대사 파일 대상이 퀘스트라면 퀘스트 '..._OnTalk' 실행
		return pQuest->OnTalk(pUserSession->GetGameRoom(), userObjectID, GetUniqueID(),  wszIndex, wszTarget);
	}

	pUserSession->SetCalledNpcResponse(false, false);

	return m_TalkMap[wszTarget]->OnTalk(userObjectID, GetUniqueID(),  wszIndex, wszTarget);
	
}

void CDnNPCActor::CmdAction( const char *szActionName, int nLoopCount, float fBlendFrame, bool bCheckOverlapAction, 
							 bool bFromStateBlow/* = false*/, bool bSkillChain/* = false*/ )
{
	if( SetActionQueue( szActionName, nLoopCount, fBlendFrame, 0.f, bCheckOverlapAction ) == false ) 
		return;

	int nActionIndex = GetElementIndex( szActionName );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1.f );

	Send( eActor::SC_CMDACTION, &Stream );
}

void CDnNPCActor::CmdEnableOperator( DnActorHandle hActor, bool bEnable )
{
	if( bEnable ) {
		DNVector(DnActorHandle)::iterator it = std::find( m_hVecDisableActor.begin(), m_hVecDisableActor.end(), hActor );
		if( it == m_hVecDisableActor.end() ) return;
		m_hVecDisableActor.erase( it );
	}
	else {
		if( std::find( m_hVecDisableActor.begin(), m_hVecDisableActor.end(), hActor ) != m_hVecDisableActor.end() ) return;
		m_hVecDisableActor.push_back( hActor );
	}

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &bEnable, sizeof(bool) );

	Send( eActor::SC_CMDENABLEOPERATOR, hActor, &Stream );
}

void CDnNPCActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_Projectile:
			return;
	}
	CDnActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

void CDnNPCActor::OnChangeAction( const char *szPrevAction )
{
	if( m_hPropHandle ) ((CDnWorldActProp*)m_hPropHandle.GetPointer())->SetAction( m_szAction.c_str(), CDnActionBase::m_fFrame, 0.f, false );
}

void CDnNPCActor::OnChangeActionQueue( const char *szPrevAction )
{
	if( m_hPropHandle ) ((CDnWorldActProp*)m_hPropHandle.GetPointer())->SetActionQueue( m_szActionQueue.c_str(), m_nLoopCount, m_fQueueBlendFrame, m_fQueueStartFrame );
}
