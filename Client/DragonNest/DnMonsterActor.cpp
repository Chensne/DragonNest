#include "StdAfx.h"
#include "DnMonsterActor.h"
#include "DnWorld.h"
#include "DnWeapon.h"
#include "EtSoundChannel.h"
#include "MAAiBase.h"
#include "DnTableDB.h"
#include "DnDropItem.h"
#include "DnInterface.h"
#include "DnPartyTask.h"
#include "TaskManager.h"
#include "MAWalkMovement.h"
#include "DnItemTask.h"
#include "DnStateBlow.h"
#include "DnLocalPlayerActor.h"
#include "DnProjectile.h"
#include "EtActionSignal.h"
#include "DnGameTask.h"
#include "DnPvPGameTask.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif
#ifdef PRE_ADD_SHOW_MONACTION
#include "DebugCmdProc.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifndef _FINAL_BUILD
#ifdef _DEBUG
static bool g_bShowActionName = true;
#else
static bool g_bShowActionName = false;
#endif 
#endif

#ifdef PRE_ADD_SHOW_MONACTION
bool g_bShowMonsterAction = false;
int g_nShowMonsterActionId = 0;
DWORD g_dwShowMonsterUniqueId = 0;
#endif

#ifdef PRE_ADD_CHANGE_MONSTER_SKIN
#include "DnActorClassDefine.h"
#endif 

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
#include "GameOption.h"
#endif

CDnMonsterActor::CDnMonsterActor( int nClassID, bool bProcess )
	: CDnActor( nClassID, bProcess )
	, m_nMonsterClassID(0)
	, m_AIDifficult( Easy )
	,m_pszCanBumpActionName(NULL)
	,m_bSignalBallon(false)
{
	CDnActionBase::Initialize( this );
	
	m_bBoss = false;
	m_fScale = 1.f;
	m_fRevisionWeight = 0.f;
	m_nMonsterWeightTableID = -1;
	m_fRotateResistance = 1.f;
	m_bShowHeadName = true;
	m_dwSummonerActorID = UINT_MAX;

#ifdef _SHADOW_TEST
	m_bEnableShadowActor = true;
#endif 

	m_bAlwaysShowExposureInfo = false;

#ifdef PRE_ADD_MONSTER_CATCH
	m_CatchSignalTimeStamp = 0;
	m_ReleaseSignalTimeStamp = 0;
#endif // #ifdef PRE_ADD_MONSTER_CATCH

	m_bFollowSummonerStage = false;
#if defined( PRE_FIX_MOVEBACK )
	m_bNearMoveBack = false;
	m_bPrevMoveBack = false;
#endif

	m_bChangeAxisOnFinishAction = false;

	m_isPuppetSummonMonster = false;

#ifdef PRE_ADD_CHANGE_MONSTER_SKIN
	m_bChangeSkin = false;
#endif 

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
	m_nSwapActorID = -1;
#endif

#ifdef PRE_ADD_MONSTER_CHEAT
	m_nSkillTableID = 0;
#endif 
}

CDnMonsterActor::~CDnMonsterActor()
{
	if( m_hAllowBlowEffect ) {
		m_hAllowBlowEffect->_DetachGraphicEffect();
		SAFE_RELEASE_SPTR( m_hAllowBlowEffect );
	}
}

bool CDnMonsterActor::Initialize()
{
	CalcMonsterWeightIndex();
	CDnMonsterState::Initialize( m_nClassID );
	MAAiReceiver::Initialize( m_nMonsterClassID );
	CDnActor::Initialize();

	if( m_fWeight == 0.f ) m_fRevisionWeight = 0.f;
	else m_fRevisionWeight = m_fWeight + ( ( m_fScale - 1.f ) * CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::ScaleWeightValue ) );

	GenerationDropItem();

#ifdef _SHADOW_TEST
	if( m_bIsShadowActor ) {
		if( m_hObject ) m_hObject->EnableCollision( false );
		return true;
	}
#endif

#ifndef _GAMESERVER
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );
	if( pSox->IsExistItem( m_nMonsterClassID ) ) {
		int nAllowSEDiffuseVariation = pSox->GetFieldFromLablePtr( m_nMonsterClassID, "_AllowSEDiffuseVariation" )->GetInteger();
		m_pStateBlow->AllowDiffuseVariation( (nAllowSEDiffuseVariation == 1) ? true : false );

		int nEffectItemID = pSox->GetFieldFromLablePtr( m_nMonsterClassID, "_AllowEffectIndex" )->GetInteger();
		if( nEffectItemID > 0 ) {
			m_hAllowBlowEffect = (new CDnBlow(GetMySmartPtr()))->GetMySmartPtr();
			m_hAllowBlowEffect->SetEffectOutputInfo( CDnStateBlow::GetEffectOutputInfo( nEffectItemID ) );
			m_hAllowBlowEffect->_AttachGraphicEffect();
		}
	}

#endif

	if( m_hObject ) {
		m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 1 ) );
		m_hObject->SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) | COLLISION_GROUP_DYNAMIC( 2 ) );
	}

	return true;
}

void CDnMonsterActor::ProcessLook( LOCAL_TIME LocalTime, float fDelta )
{
	if( bIsAILook() )
	{
		Look( *m_pAi->GetAILook() );
	}
	else
	{
		bool bNaviMode		= IsNaviMode();

#if defined( PRE_MOD_LOCK_TARGET_LOOK )
		if( !bNaviMode && m_bLockTarget && 0 != m_nLockLookEventArea )
		{
			if( GetLookTarget() ) 
				ResetLook();
		}
		else 
#endif	// #if defined( PRE_MOD_LOCK_TARGET_LOOK )
		if( !bNaviMode && m_bLockTarget && GetAggroTarget() ) 
		{
			if( !GetLookTarget() && GetLookTarget() != GetAggroTarget() )
				LookTarget( GetAggroTarget() );
		}
		else 
		{
			if( GetLookTarget() ) 
				ResetLook();
		}
	}

	if( !IsSignalRange( STE_RotateResistance ) ) m_fRotateResistance = 1.f;
}

extern bool g_bNaviDraw;
void CDnMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{	
	m_pszCanBumpActionName	= NULL;

	ProcessLook( LocalTime, fDelta );

	m_bLockTarget = false;
#if defined( PRE_MOD_LOCK_TARGET_LOOK )
	m_nLockLookEventArea = 0;
#endif	// #if defined( PRE_MOD_LOCK_TARGET_LOOK )

	CDnActor::ProcessActor( LocalTime, fDelta );
	SetMoveVectorX( m_matexWorld.m_vXAxis );
	SetMoveVectorZ( m_matexWorld.m_vZAxis );

	if ( g_bNaviDraw )
	{
		MAWalkMovement* pWalkMovement = dynamic_cast<MAWalkMovement*>(GetMovement());
		if ( pWalkMovement )
		{
			
			WAYPOINT_LIST& fur_waypoint_list = pWalkMovement->GetWayPoints();

			WAYPOINT_LIST::iterator it = fur_waypoint_list.begin();
			for ( ; it != fur_waypoint_list.end() ; it++ )
			{
				WAYPOINT& point = *it;


				EtVector3 vPos1, vPos2;
				vPos1 = point.Position;

				WAYPOINT_LIST::iterator nextIt = it;
				++nextIt;
				if ( nextIt == fur_waypoint_list.end() )
					break;

				vPos2 = (*nextIt).Position;
				EternityEngine::DrawLine3D(vPos1, vPos2, 0xff0000ff);
			}
		}
	}

#ifdef PRE_ADD_MONSTER_CATCH
	ProcessCatchActor( LocalTime, fDelta );
#endif // #ifdef PRE_ADD_MONSTER_CATCH

	ProcessVisual( LocalTime, fDelta );
}


void CDnMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
#ifdef PRE_ADD_LOTUSGOLEM
	if (HitParam.bIgnoreShowDamage)
		return;
#endif

	CDnActor::OnDamage( pHitter, HitParam );

	CDnGameTask* pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	DnActorHandle hHitter = pHitter ? pHitter->GetActorHandle() : CDnActor::Identity();
	if( pGameTask && HitParam.bRecvOnDamage )
	{		
		pGameTask->OnDamage( GetActorHandle(), hHitter, HitParam.nDamage );
	}

	bool bMine = false;
	if( pHitter )
	{
		bMine = (pHitter->GetActorHandle() == CDnActor::s_hLocalActor) || 
				(pHitter->GetActorHandle()->GetSummonerUniqueID() == CDnActor::s_hLocalActor->GetUniqueID());
	}
#ifdef _RDEBUG
	g_Log.Log(LogType::_FILELOG, "[DMGERR] CDnMonsterActor::OnDamage() : pHitter(%x) bIgnoreShowDamage(%d)\n", pHitter, (int)HitParam.bIgnoreShowDamage);
#endif

#ifdef PRE_ADD_CRAZYDUC_UI
	if( pGameTask && pGameTask->IsShowMODDialog() )
	{
		CDnInterface::GetInstance().SetCrazyDucScore( *GetPosition(), m_nMonsterClassID );
	}
	else
	{
		GetInterface().SetDamage( 
			GetBodyPosition(), 
			(int)HitParam.nDamage, 
			HitParam.HitType==CDnWeapon::Critical, 
			HitParam.HitType==CDnWeapon::CriticalRes,
			true,
			bMine,
			HitParam.HasElement,
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
			HitParam.bStageLimit
#else
			false
#endif
			);
	}

#else // PRE_ADD_CRAZYDUC_UI

	GetInterface().SetDamage( 
		GetBodyPosition(), 
		(int)HitParam.nDamage, 
		HitParam.HitType==CDnWeapon::Critical, 
		HitParam.HitType==CDnWeapon::CriticalRes,
		true,
		bMine,
		HitParam.HasElement,
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
		HitParam.bStageLimit
#else
		false
#endif
		);
#endif // PRE_ADD_CRAZYDUC_UI
	
	GetInterface().OnDamage( GetMySmartPtr() );


	ResetCustomAction();
	ResetMixedAnimation();

	// 화살같은경우 무기가 플에이되면서 발사되기 때문에 Hit 시 Idle 를 해준다.
	if( GetWeapon() && GetWeapon()->IsExistAction( "Idle" ) )
		GetWeapon()->SetActionQueue( "Idle" );
}

void CDnMonsterActor::OnDie( DnActorHandle hHitter )
{
	CDnActor::OnDie( hHitter );
	CDnPartyTask *pTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
	if( !pTask ) return;

	RemoveAllBlow();
	float fDeadDurabilityRevision = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MonsterDeadDurabilityRevision );
	int nPartyCount = pTask->GetPartyCount();
	int nDeadDurability = (int)( ( GetDeadDurability() * ( 1.f + ( fDeadDurabilityRevision * ( nPartyCount - 1 ) ) ) ) / nPartyCount );
	int nFinalDeadDurability = nDeadDurability;
	for( int i=0 ; i<nPartyCount ; ++i )
	{
		CDnPartyTask::PartyStruct* pStruct = pTask->GetPartyData(i);
		if( !pStruct || !pStruct->hActor || pStruct->hActor->IsDie() )
			continue;
		CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(pStruct->hActor.GetPointer());

		if (pPlayer)
		{
			//파티원의 길드 정보가 설정이 안 되어 있을 수 있음.(길드 정보는 클라이언트에서 서버에 요청 했을때만 설정됨)
			//자신의 길드 정보는 GuildTask에서 가져 오도록 한다..
			int nGuildRewardItemValue = pPlayer->GetPlayerGuildRewardEffectValue(GUILDREWARDEFFECT_TYPE_REDUCEDURABILITYRATIO);
		
			if( nGuildRewardItemValue > 0 )
				nFinalDeadDurability -= (int)(nFinalDeadDurability * nGuildRewardItemValue * 0.01);
			// 파티원 내구도 감소
			pPlayer->OnDecreaseEquipDurability( nFinalDeadDurability );
		}
	}
	/*
	if( CDnActor::s_hLocalActor && !CDnActor::s_hLocalActor->IsDie() )
	{
		CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		pPlayer->OnDecreaseEquipDurability( GetDeadDurability() );
	}
	*/

	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask ) pGameTask->CheckBattleMode( GetMySmartPtr(), false );

	DropItems();

	if( m_hAllowBlowEffect ) {
		m_hAllowBlowEffect->_DetachGraphicEffect();
	}

	pGameTask->OnDie( GetMySmartPtr(), hHitter );

	if (m_hSummonerPlayerActor)
		m_hSummonerPlayerActor->SummonOnOffSkillInit(GetMonsterClassID());

#if defined(PRE_ADD_65808)
	if (m_hSummonerPlayerActor)
	{
		m_hSummonerPlayerActor->RemoveSummonMonsterGlyphStateEffects(GetMonsterClassID());
	}
#endif // PRE_ADD_65808
}

void CDnMonsterActor::DropItems()
{
	// 특별히 해줄게 없다. 패킷 오면 처리하면 된당.
	return;
}

void CDnMonsterActor::OnDrop( float fCurVelocity )
{
	if( IsAir() ) {
		if( !IsHit() ) {
			char szStr[64];
			sprintf_s( szStr, "%s_Landing", GetCurrentAction() );
			if( IsExistAction( szStr ) )
			{
				SetActionQueue( szStr, 0, 0.f, 0.f, true, false );
			}
			if( !GetLastFloorForceVelocity() ) {
				TileTypeEnum TileType = CDnWorld::GetInstance().GetTileType( m_matexWorld.m_vPosition );

				CreateEnviEffectParticle( ConditionEnum::Landing, TileType, &m_matexWorld );
				CreateEnviEffectSound( ConditionEnum::Landing, TileType, m_matexWorld.m_vPosition );
			}
			SetMovable( false );
		}
		else if( !IsDie() ) {
			std::string szAction;
			float fBlendFrame = 2.f;
			// 떨어지는 속도가 10이상이면 bigBounce로 한번 더 띄어준다.
			if( fCurVelocity < -6.f && m_HitParam.vVelocity.y != 0.f && abs(m_HitParam.vVelocity.y) > 0.1f ) {
				if( m_HitParam.vVelocity.y > 0.f ) {
					m_HitParam.vVelocity.y *= 0.6f;
					SetVelocityY( m_HitParam.vVelocity.y );
				}
				else { // 가속도가 처음부터 바닥으로 향해있는 경우에는 뒤집어줘야한다.
					m_HitParam.vVelocity.y *= -0.6f;
					if( m_HitParam.vResistance.y > 0.f )
						m_HitParam.vResistance.y *= -1.f;
					SetVelocityY( m_HitParam.vVelocity.y );

					if( m_HitParam.vVelocity.y > 0 && m_HitParam.vResistance.y <= 0 )
						SetResistanceY( -15.0f );
					else
						SetResistanceY( m_HitParam.vResistance.y );
				}
				szAction = "Hit_AirBounce";
			}
			else {
				szAction = "Down_SmallBounce";
				fBlendFrame = 0.f;
			}
			SetActionQueue( szAction.c_str(), 0, fBlendFrame, 0.f, true, false );

			TileTypeEnum TileType = CDnWorld::GetInstance().GetTileType( m_matexWorld.m_vPosition );

			CreateEnviEffectParticle( ConditionEnum::Down, TileType, &m_matexWorld );
			CreateEnviEffectSound( ConditionEnum::Down, TileType, m_matexWorld.m_vPosition );
		}
	}
}

void CDnMonsterActor::OnStop( EtVector3 &vPosition )
{
	if( GetAIBase() ) return;
	if( IsDie() ) return;

	if( IsMove() ) CmdStop( "Stand" );	
}

void CDnMonsterActor::OnBeginNaviMode()
{
	if( IsDie() ) return;
	if( IsMove() && !IsNaviMode() )
		CmdStop( "Stand" );
}

void CDnMonsterActor::OnMoveNavi( EtVector3 &vPosition )
{
	EtVector3 vXVec, vZVec;

	vZVec = vPosition - *GetPosition();
	EtVec3Normalize( &vZVec, &vZVec );

	Look( EtVec3toVec2( vZVec ), true );
}

void CDnMonsterActor::CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount, float fBlendFrame )
{
//	if( CGlobalValue::GetInstance().m_bMainClient && !IsMovable() ) return;
	MovePos( vPos, true );
	if( SetActionQueue( szActionName, nLoopCount, fBlendFrame, 0.f, false ) == false ) return;
}

void CDnMonsterActor::CmdMove( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount, float fBlendFrame )
{
//	if( CGlobalValue::GetInstance().m_bMainClient && !IsMovable() ) return;
	MoveTarget( hActor, fMinDistance );
	if( SetActionQueue( szActionName, nLoopCount, fBlendFrame, 0.f, false ) == false ) return;
}

void CDnMonsterActor::CmdMoveNavi( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	if( !hActor )
		return;

	MoveTargetNavi(hActor, fMinDistance, szActionName);
	if( SetActionQueue( szActionName, nLoopCount, fBlendFrame, 0.f, false ) == false ) 
		return;
}

void CDnMonsterActor::CmdMoveNavi( EtVector3& vTargetPos, float fMinDistance, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	MoveTargetNavi( vTargetPos, fMinDistance, szActionName);
	if( SetActionQueue( szActionName, nLoopCount, fBlendFrame, 0.f, false ) == false ) 
		return;
}

void CDnMonsterActor::CmdStop( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bResetStop, bool bForce )
{
	if( SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame, false ) == false ) 
		return;

	ResetMove();
	ResetLook();
	//LogWnd::Log( 1, _T("CmdStop Look 엄따. "));
}

void CDnMonsterActor::CmdLook( EtVector2 &vVec, bool bForce )
{
	Look( vVec, bForce );
}

void CDnMonsterActor::CmdLook( DnActorHandle hActor, bool bLock )
{
	if( bLock ) LookTarget( hActor );
	else {
		if( !hActor ) ResetLook();
		else {
			EtVector2 vVec;
			vVec.x = hActor->GetPosition()->x - GetPosition()->x;
			vVec.y = hActor->GetPosition()->z - GetPosition()->z;
			EtVec2Normalize( &vVec, &vVec );
			Look( vVec );
		}
	}
}

void CDnMonsterActor::CmdSuicide( bool bDropItem, bool bDropExp )
{
	if( IsDie() ) return;

	SetDieDelta( m_fMaxDieDelta );
	SetHP( 0 );
	ResetMove();
	ResetLook();

	if( bDropItem ) DropItems();

	std::string szDieAction;
	if( !bDropItem && !bDropExp ) szDieAction = "Summon_Off";
	else szDieAction = "Die";

	if( IsExistAction( szDieAction.c_str() ) )
	{
		// #36030 Summon_Off 액션 이미 실행중이면 다시 액션 실행시키지 않는다.
		if( szDieAction != GetCurrentAction() )
			SetAction( szDieAction.c_str(), 0, 3.f, false );
	}
	else 
	{
		SetDestroy();
	}

	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask ) pGameTask->CheckBattleMode( GetMySmartPtr(), false );

	if( !CDnPartyTask::GetInstance().IsSyncComplete() ) {
		SetDestroy();
	}

	if (m_hSummonerPlayerActor)
		m_hSummonerPlayerActor->SummonOnOffSkillInit(GetMonsterClassID());
}

void CDnMonsterActor::GenerationDropItem()
{
	m_VecDropItemList.clear();

	if( m_nItemDropGroupTableID < 1 ) return;

	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	int nExtendDropRate = 0;
	if (pGameTask)
		nExtendDropRate = pGameTask->GetEventExtendDropRate();

	float fCalcDropCount = ((float)((float)nExtendDropRate/100) + 1.0f);
	for (int h = 0; 0 < fCalcDropCount; h++)
	{
		CDnDropItem::CalcDropItemList( m_AIDifficult, m_nItemDropGroupTableID, m_VecDropItemList );

		for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
			if( CDnDropItem::PreInitializeItem( m_VecDropItemList[i].nItemID ) == false ) {
				m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
				i--;
			}
		}

		fCalcDropCount -= 1.0f;		
		if (h >= 4 || (fCalcDropCount < 1.0f && ((float)(_rand()%100)/100) > fCalcDropCount))
			break;			//4개이상(이벤트3개)은 불가하게 조절, 1보다 작은 값이 남은경우 확율계산하여 한던 더돌지 판단
	}
}

void CDnMonsterActor::SetGenerationArea( SOBB &Box )
{
	m_GenerationArea = Box;
}

SOBB *CDnMonsterActor::GetGenerationArea()
{
	return &m_GenerationArea;
}

void CDnMonsterActor::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
{
	switch( dwActorProtocol ) {
		case eActor::SC_CMDMOVE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount;
				EtVector3 vPos, vXVec, vTargetPos;
				EtVector2 vZVec, vLook;

				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vTargetPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

				Look( vLook );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				ResetMove();
				// 위치 보간 SetPosition
				MAWalkMovement *pMovement = dynamic_cast<MAWalkMovement *>(GetMovement());
				if( pMovement ) pMovement->SetMagnetPosition( vPos );
				else SetPosition( vPos );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				if( nActionIndex != -1 && GetElement( nActionIndex ) ) {
					CmdMove( vTargetPos, GetElement( nActionIndex )->szName.c_str(), nLoopCount, 3.f );
				}
			}
			break;
		case eActor::SC_CMDSTOP:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos;
				int nActionIndex, nLoopCount;

				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

				// 위치 보간 SetPosition
				MAWalkMovement *pMovement = dynamic_cast<MAWalkMovement *>(GetMovement());
				if( pMovement ) pMovement->SetMagnetPosition( vPos );
				else SetPosition( vPos );

				if( nActionIndex != -1 && GetElement( nActionIndex ) ) {
					CmdStop( GetElement( nActionIndex )->szName.c_str(), nLoopCount );
				}

			}
			break;
		case eActor::SC_CMDACTION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount;
				float fBlendFrame;
				EtVector3	vPos;
				EtVector2	vAILook;
				bool		bAILook = false;
				bool		bFromStateBlow = false;
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1.f );
				Stream.Read( &bAILook, sizeof(bool) );
				Stream.Read( &vAILook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &bFromStateBlow, sizeof(bool) );				
#if defined( PRE_FIX_MOVEBACK )
				Stream.Read( &m_bPrevMoveBack, sizeof(bool) );
#endif

				if( m_pAi )
					m_pAi->SetAILook( bAILook, vAILook );
				Look( vAILook );

				//LogWnd::Log( 1, _T("[%d]AILook=%d"), GetLocalTime(), bAILook );

				if( nActionIndex != -1 && GetElement( nActionIndex ) ) {
					std::string szAction = GetElement( nActionIndex )->szName;
					if( bAILook && m_pAi && strstr( szAction.c_str(), "Attack" ) )
						m_pAi->SetAILookAction( szAction.c_str() );
					CmdAction( szAction.c_str(), nLoopCount, fBlendFrame );

					// 서버나 클라 둘 중에 상태효과쪽에서 발생시킨 액션.
					if( bFromStateBlow )
						m_pStateBlow->OnCmdActionFromPacket( szAction.c_str() );
				}

				// 잠깐 넣어놓습니다. 어색한정도 보기 위함.
				MAWalkMovement *pMovement = dynamic_cast<MAWalkMovement *>(GetMovement());
				if( pMovement ) pMovement->SetMagnetPosition( vPos );
				else SetPosition( vPos );
			}
			break;
		case eActor::SC_CMDMOVETARGET:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount;
				EtVector3 vXVec, vPos;
				EtVector2 vZVec, vLook;
				DWORD dwUniqueID;
				float fMinDistance;

				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &dwUniqueID, sizeof(DWORD) );
				Stream.Read( &fMinDistance, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1.f );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

				Look( vLook );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				// 위치 보간 SetPosition
				MAWalkMovement *pMovement = dynamic_cast<MAWalkMovement *>(GetMovement());
				if( pMovement ) pMovement->SetMagnetPosition( vPos );
				else SetPosition( vPos );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				if( nActionIndex != -1 && GetElement( nActionIndex ) ) {
					CmdMove( FindActorFromUniqueID( dwUniqueID ), fMinDistance, GetElement( nActionIndex )->szName.c_str(), nLoopCount, 8.f );
				}
			}
			break;
		case eActor::SC_CMDMOVETARGET_NAVI:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount;
				EtVector3 vXVec, vPos, vTargetPos;
				EtVector2 vZVec, vLook;
				float fMinDistance;

				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &vTargetPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &fMinDistance, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1.f );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

				Look( vLook );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				// 위치 보간 SetPosition
				SetPosition( vPos );
				/*
				MAWalkMovement *pMovement = dynamic_cast<MAWalkMovement *>(this);
				if( pMovement ) pMovement->SetMagnetPosition( vPos );
				else SetPosition( vPos );
				*/

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				//OutputDebug( "SC_CMDMOVETARGET_NAVI\r\n" );
				if( nActionIndex != -1 && GetElement( nActionIndex ) ) 
				{
					//OutputDebug( "CmdMoveNavi\r\n" );
					CmdMoveNavi( vTargetPos, fMinDistance, GetElement( nActionIndex )->szName.c_str(), nLoopCount, 8.f );
				}
			}
			break;
		case eActor::SC_CMDLOOKTARGET:
			{
				CMemoryStream Stream( pPacket, 128 );

				DWORD dwUniqueID;
				bool bLock;
				Stream.Read( &dwUniqueID, sizeof(DWORD) );
				Stream.Read( &bLock, sizeof(bool) );
				CmdLook( FindActorFromUniqueID( dwUniqueID ), bLock );
			}
			break;
		case eActor::SC_CMDLOOK:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector2 vVec;
				bool bForce;
				Stream.Read( &vVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &bForce, sizeof(bool) );
				CmdLook( vVec, bForce );

			}
			break;
		case eActor::SC_AGGROTARGET:
			{
				CMemoryStream Stream( pPacket, 128 );

				DWORD dwUniqueID;
				Stream.Read( &dwUniqueID, sizeof(DWORD) );

				DnActorHandle hActor = CDnActor::FindActorFromUniqueID( dwUniqueID );
				SetAggroTarget( hActor );

				// LookTarget 과 MoveTarget 이 틀린문제때문에 어그로타겟이 바뀌었을경우 강제로 MoveTarget 을 Reset시킨다.(김밥)
//				if( hActor )
				ResetMove();
				m_bSignalBallon = true;

				// 어그로 타겟값을가지고 전투모드인지를 판단한다.
				////////////////////////////////////////
				CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
				if( !pGameTask ) break;
				pGameTask->CheckBattleMode( GetMySmartPtr(), ( hActor ) ? true : false );
				////////////////////////////////////////
				if( hActor ) {
					if( GetGrade() >= Champion ) {
						GetInterface().ShowEnemyGauge( GetMySmartPtr(), true );	// 머리위 이름만 나오게 하기 위해 이렇게 호출.
					}
				}
			}
			break;
		case eActor::SC_USESKILL:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nSkillTableID;
				BYTE cLevel;
				EtVector3 vPos;
				EtVector2 vLook;


				Stream.Read( &nSkillTableID, sizeof(int) );
				Stream.Read( &cLevel, sizeof(char) );

				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
#if defined( PRE_ADD_ACADEMIC )
				int nSummonerSkillID = 0;
				Stream.Read( &nSummonerSkillID, sizeof(int) );
#endif // #if defined( PRE_ADD_ACADEMIC )

				Look( vLook );

				MAWalkMovement *pMovement = dynamic_cast<MAWalkMovement *>(GetMovement());
				if( pMovement ) pMovement->SetMagnetPosition( vPos );
				else SetPosition( vPos );

				AddSkill( nSkillTableID, cLevel );
				UseSkill( nSkillTableID, false );
				OnSkillUse( nSkillTableID, cLevel );

#if defined( PRE_ADD_ACADEMIC ) // 소환체 몬스터에게 명령하는 스킬의 쿨타임은< ActiveToggle > 이때 갱신해주도록 하자.
				if( GetSummonerPlayerActor() && GetSummonerPlayerActor() == CDnActor::s_hLocalActor && nSummonerSkillID > 0)
				{
					DnSkillHandle hSkill = GetSummonerPlayerActor()->FindSkill(nSummonerSkillID);
					if(hSkill)
					{
						hSkill->OnBeginCoolTime(); // 쿨타임은 존재하지만 사실상 비쥬얼을 위한 쿨타임이다.
						hSkill->EnableActiveToggleCoolTime(true);
					}
				}
#endif

			}
			break;
		case eActor::SC_CMDMIXEDACTION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nMaintenanceBone, nActionBone;
				float fFrame, fBlendFrame;

				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &nActionBone, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &nMaintenanceBone, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &fFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
				Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) break;

				std::string szAction = pStruct->szName;
				std::string szActiionBoneName = GetBoneName(nActionBone);
				std::string szMaintenanceBoneName = GetBoneName(nMaintenanceBone);
				if( IsCustomAction() ) {
					ResetCustomAction();
				}
				CmdMixedAction( szActiionBoneName.c_str(), szMaintenanceBoneName.c_str(), szAction.c_str(), 0, fFrame, fBlendFrame );
				SetCustomAction( szAction.c_str(), fFrame );
			}
			break;
		case eActor::SC_POSREV:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				EtVector3 vPos;
				EtVector2 vZVec;

				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				// 위치 보간 SetPosition
				MAWalkMovement *pMovement = dynamic_cast<MAWalkMovement *>(GetMovement());
				if( pMovement ) pMovement->SetMagnetPosition( vPos );
				else SetPosition( vPos );

				SetMoveVectorZ( EtVec2toVec3( vZVec ) );
			}
			break;
		case eActor::SC_PROJECTILE:
			{
				_CreateProjectile( pPacket );
				return;
			}

		case eActor::SC_MONSTER_PROJECTILE_FROM_PROJECTILE:
			{
				//_CreateProjectileFromProjectile( pPacket );
				CDnProjectile* pProjectile = CDnProjectile::CreateProjectileFromServerPacket( GetMySmartPtr(), pPacket );
				if( pProjectile )
				{
					OnSkillProjectile( pProjectile );
				}
				return;
			}
			break;

		case eActor::SC_SLAVE_OF:
			{
				bool bSummoned = false;
				CPacketCompressStream Stream( pPacket, 32 );
				Stream.Read( &m_dwSummonerActorID, sizeof(DWORD) );
				Stream.Read( &bSummoned, sizeof(bool) );

				if( bSummoned )
				{
					INT64 nHP = 0;
					INT64 nMaxHP = 0;
					bool bFollowSummonerStage = false;
					bool bReCreatedFollowStageMonster = false;
					Stream.Read( &nMaxHP, sizeof(INT64) );
					Stream.Read( &nHP, sizeof(INT64) );
					Stream.Read( &bFollowSummonerStage, sizeof(bool) );
					Stream.Read( &bReCreatedFollowStageMonster, sizeof(bool) );

#if defined(PRE_ADD_SKILLBUF_RENEW)
					m_bCopiedFromSummoner = true;
#endif // PRE_ADD_SKILLBUF_RENEW

#if defined(PRE_FIX_55618)
					INT64 nBaseMaxHP = 0;
					float nBaseMaxHPRate = 0.0f;
					int nMoveSpeed = 0;

					Stream.Read( &nBaseMaxHP, sizeof(INT64) );
					Stream.Read( &nBaseMaxHPRate, sizeof(float) );
					Stream.Read( &nMoveSpeed, sizeof(int) );

					m_BaseState.SetMaxHP(nBaseMaxHP);
					m_BaseState.SetMaxHPRatio(nBaseMaxHPRate);
					m_BaseState.SetMoveSpeed(nMoveSpeed);
					RefreshState();
					SetHP( GetMaxHP() );

#endif // PRE_FIX_55618

#if defined(PRE_FIX_61382)
					bool isPuppetSummonMonster = false;
					Stream.Read(&isPuppetSummonMonster, sizeof(bool));

					SetPuppetSummonMonster(isPuppetSummonMonster);
#endif // PRE_FIX_61382					
					
					DnActorHandle hSummonMaster = CDnActor::FindActorFromUniqueID( m_dwSummonerActorID );
					
					//몬스터인 경우도 소환됨..
					m_hSummonerPlayerActor = hSummonMaster;

					if( hSummonMaster && hSummonMaster->IsPlayerActor() )
					{
						m_bFollowSummonerStage = bFollowSummonerStage;

						// 스테이지 따라가는 몬스터면 Summon_On 액션을 하지 않고 곧바로 Stand..
						// 이 플래그는 true 가 되는 경우에 스테이지 따라가는 몬스터가 스테이지 이동을
						// 해서 새로 생성되고나서 게임서버에서 보내준다.
						if( m_bFollowSummonerStage && bReCreatedFollowStageMonster )
							SetActionQueue( "Stand" );

						if( CDnActor::s_hLocalActor && hSummonMaster == CDnActor::s_hLocalActor)
						{
							CmdShowExposureInfo(true); // 내가 소환한 소환체는 이름과 HP 를 표시해주자
							ShowHeadName(true);
						}
										
						CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

						if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP )
						{
							MASingleBody *pSingleBody = dynamic_cast<MASingleBody *>(GetActorHandle().GetPointer());
							DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TACTOR );

							if(pSox && pSingleBody)
							{
#ifdef PRE_FIX_MEMOPT_EXT
								std::string szSkinName, szAniName;
								CommonUtil::GetFileNameFromFileEXT(szSkinName, pSox, GetClassID(), "_SkinName");
								CommonUtil::GetFileNameFromFileEXT(szAniName, pSox, GetClassID(), "_AniName");
#else
								std::string szSkinName = pSox->GetFieldFromLablePtr( GetClassID() , "_SkinName" )->GetString();
								std::string szAniName = pSox->GetFieldFromLablePtr( GetClassID() , "_AniName" )->GetString();
#endif
								std::vector<std::string> tokens;
								TokenizeA(szSkinName,tokens,".");
								if(!tokens.empty())
									szSkinName = tokens[0];
	
								// 길드전 옵져버 까지는 팀에 맞게 셋팅 , 이것 상황에따라서 틀려서 통합하던지 해야할듯.. 
								// 팀별로 색지정을 명확하게 하면 모르겟는데.. PVP 일반같은경우에는 내가 실질적으로 빨간팀이라도.. 머리는 파란색으로 표시가된다.. 내팀기준은 파란색
								if( (
									((CDnPvPGameTask*)pGameTask)->GetGameMode() == PvPCommon::GameMode::PvP_GuildWar ) || 
									( CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer) )
								{
									if( hSummonMaster->GetTeam() == PvPCommon::Team::A ) // 팀에맞게 셋팅된 경우에는 이렇게 표시
										szSkinName += "_BLUE.skn";
									else if( hSummonMaster->GetTeam() == PvPCommon::Team::B )
										szSkinName += "_RED.skn";
								}
								else
								{
									if( hSummonMaster->GetTeam() == CDnActor::s_hLocalActor->GetTeam() ) // 일반적으로 내팀이면 표시
										szSkinName += "_BLUE.skn";
									else
										szSkinName += "_RED.skn";
								}
								
								bool bExistSkin = false;
								CEtResourceMng::GetInstance().GetFullName( szSkinName.c_str() , &bExistSkin);

								if(bExistSkin)
								{
									pSingleBody->FreeSkin();
									pSingleBody->LoadSkin( CEtResourceMng::GetInstance().GetFullName( szSkinName).c_str() , CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
								}
							}
						}
					}
				}
			}
			break;

		case eActor::SC_SLAVE_RELEASE:
			{
				if( m_dwSummonerActorID )
				{
					m_dwSummonerActorID = 0;

					if( m_hSummonerPlayerActor )
						m_hSummonerPlayerActor.Identity();
				}
			}
			break;

		case eActor::SC_CMDCHATBALLOON :
#if defined(PRE_ADD_MULTILANGUAGE)
		case eActor::SC_CMDCHATBALLOON_AS_INDEX:
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			{
				//시그널에 의해 말풍선이 생성되고 렌더링 중이라면 말풍선을 멈춘다.
				if( m_ChatBalloon.IsRenderChatBalloon() )
					m_ChatBalloon.StopRenderChatBalloon();
			}
			break;

#ifdef PRE_ADD_MONSTER_CATCH
		case eActor::SC_CATCH_ACTOR:
			{
				CatchActorFromServer( pPacket );
			}
			break;

		case eActor::SC_CATCH_ACTOR_FAILED:
			{
				CatchActorFailedFromServer( pPacket );
			}
			break;

		case eActor::SC_RELEASE_ACTOR:
			{
				ReleaseActorFromServer( pPacket );
			}
			break;
#endif // #ifdef PRE_ADD_MONSTER_CATCH

#if defined(PRE_FIX_64312)
		case eActor::SC_APPLY_SUMMON_MONSTER_EX_SKILL:
			{
				ApplySummonMonsterExSkill(pPacket);
			}
			break;
#endif // PRE_FIX_64312
	}
	CDnActor::OnDispatchMessage( dwActorProtocol, pPacket );
}

void CDnMonsterActor::_CreateProjectile( BYTE* pPacket )
{
	CPacketCompressStream Stream( pPacket, 128 );

	DWORD				dwProjectileUniqueID;
	MatrixEx		LocalCross;
	ProjectileStruct	sStruct;
	ProjectileStruct*	pStruct = &sStruct;
	int					iActionIndex, iSignalIndex;

	Stream.Read( &dwProjectileUniqueID,		sizeof(DWORD) );
	Stream.Read( &LocalCross.m_vPosition,	sizeof(EtVector3),	CPacketCompressStream::VECTOR3_BIT		);
	Stream.Read( &LocalCross.m_vXAxis,		sizeof(EtVector3),	CPacketCompressStream::VECTOR3_SHORT	);
	Stream.Read( &LocalCross.m_vYAxis,		sizeof(EtVector3),	CPacketCompressStream::VECTOR3_SHORT	);
	Stream.Read( &LocalCross.m_vZAxis,		sizeof(EtVector3),	CPacketCompressStream::VECTOR3_SHORT	);	
	Stream.Read( &iActionIndex,				sizeof(int) );
	Stream.Read( &iSignalIndex,				sizeof(int) );

	bool bUsedForceTarget = false;
	EtVector3 vForceDir;
	Stream.Read( &bUsedForceTarget, sizeof(bool) );
	if( bUsedForceTarget )
		Stream.Read( &vForceDir, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );

	// ActionIndex, SignalListArrayIndex 로 SignalStruct 얻는다.
	bool bFound = false;
	ActionElementStruct* pElementStruct = GetElement( iActionIndex );
	if( pElementStruct )
	{
		for( UINT i=0 ; i<pElementStruct->pVecSignalList.size() ; ++i )
		{
			if( pElementStruct->pVecSignalList[i]->GetSignalListArrayIndex() == iSignalIndex )
			{
				// 클라에서 시그널을 잘못 찾는 경우가 있어서 방어코드 넣어놓고 지켜봅니다.
				if( pElementStruct->pVecSignalList[i]->GetSignalIndex() != STE_Projectile ) 
					return;

				ProjectileStruct* pProjectileStruct	= static_cast<ProjectileStruct*>( pElementStruct->pVecSignalList[i]->GetData() );
#ifdef PRE_FIX_MEMOPT_SIGNALH
				CopyShallow_ProjectileStruct(*pStruct, pProjectileStruct);
#else
				*pStruct = *pProjectileStruct;
#endif

				//pStruct->nProjectileIndex			= pProjectileStruct->nProjectileIndex;
				//pStruct->nOrbitType					= pProjectileStruct->nOrbitType;
				//pStruct->nDestroyOrbitType			= pProjectileStruct->nDestroyOrbitType;
				//pStruct->VelocityType				= pProjectileStruct->VelocityType;
				//pStruct->nWeaponTableID				= pProjectileStruct->nWeaponTableID;
				//pStruct->fSpeed						= pProjectileStruct->fSpeed;
				//pStruct->fResistance				= pProjectileStruct->fResistance;
				//pStruct->nValidTime					= pProjectileStruct->nValidTime;
				//pStruct->nMaxHitCount				= pProjectileStruct->nMaxHitCount;
				//pStruct->bPierce					= pProjectileStruct->bPierce;
				//pStruct->bDirectionFollowView		= pProjectileStruct->bDirectionFollowView;
				//pStruct->bIncludeMainWeaponLength	= pProjectileStruct->bIncludeMainWeaponLength;
				//pStruct->vOffset					= pProjectileStruct->vOffset;
				//pStruct->vDirection					= pProjectileStruct->vDirection;
				//pStruct->vDestPosition				= pProjectileStruct->vDestPosition;
				//pStruct->nTargetType				= pProjectileStruct->nTargetType;
				//pStruct->fProjectileOrbitRotateZ	= pProjectileStruct->fProjectileOrbitRotateZ;
				
				bFound = true;
				break;
			}
		}
	}

	DN_ASSERT( bFound, "시그널을 찾을 수 없음!!! 서버,클라이언트간 Action파일이 동일확지 확인바람!" );

	// 덤프 의심가는 것 수정해봅니다.
	if( false == bFound )
		return;

	EtVector3		vTargetPos	= EtVector3( 0.f, 0.f, 0.f );
	DnActorHandle	hTarget		= CDnActor::Identity();

	switch( pStruct->nTargetType )
	{
		case CDnProjectile::TargetTypeEnum::TargetPosition:	// TargetPosition
		case CDnProjectile::TargetTypeEnum::DestPosition:
		{
			Stream.Read( &vTargetPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			break;
		}
		case CDnProjectile::TargetTypeEnum::Target:	// Target
		{
			DWORD dwUniqueID;
			Stream.Read( &dwUniqueID, sizeof(DWORD) );

			hTarget = FindActorFromUniqueID( dwUniqueID );
			if( !hTarget )
				hTarget = GetActorHandle();
			break;
		}
	}

	// LocalCross 를 가공할 필요가 있을 때. 현재는 대포 몬스터에서만 사용됨.
	OnCreateProjectileBegin( LocalCross );

	CDnProjectile* pProjectile = CDnProjectile::CreateProjectile( GetMySmartPtr(), LocalCross, pStruct, vTargetPos, hTarget, 
																				   bUsedForceTarget ? &vForceDir : NULL );
	if( pProjectile )
	{
		pProjectile->SetUniqueID( dwProjectileUniqueID );
#ifdef PRE_MOD_PROJECTILE_HACK
		OnProjectile( pProjectile );
#else
		OnProjectile( pProjectile, -1 );
#endif
		OnSkillProjectile( pProjectile );
		pProjectile->ShowWeapon( IsShow() );
	}

	OnCreateProjectileEnd();
}


// CDnPlayerActor의 SC_PROJECTILE 처리 부분과 동일.
void CDnMonsterActor::_CreateProjectileFromProjectile( BYTE* pPacket )
{
	CDnProjectile* pProjectile = CDnProjectile::CreateProjectileFromServerPacket( GetMySmartPtr(), pPacket );
	if( pProjectile )
	{
		OnSkillProjectile( pProjectile );
	}
}


void CDnMonsterActor::SyncClassTime( LOCAL_TIME LocalTime )
{
	MAActorRenderBase::m_LocalTime = LocalTime;

	CDnActor::SyncClassTime( LocalTime );
}

void CDnMonsterActor::RenderCustom( float fElapsedTime )
{
#ifdef PRE_ADD_SHOW_MONACTION
	if (g_bShowMonsterAction)
	{
		if (g_nShowMonsterActionId < 0 || (g_nShowMonsterActionId == m_nMonsterClassID))
		{
			bool bPresent = true;
			if (g_dwShowMonsterUniqueId > 0 && g_dwShowMonsterUniqueId != m_dwUniqueID)
				bPresent = false;

			if (bPresent)
			{
				std::wstring wszCurAction;
				ToWideString(m_szAction, wszCurAction);
				
				if (g_nShowMonsterActionId < 0)
					wszCurAction = FormatW(L"%d(%d) : %s", m_nMonsterClassID, (int)m_dwUniqueID, wszCurAction.c_str());

				AddHeadNameElement_Highest( (WCHAR*)wszCurAction.c_str(), s_nFontIndex, 16, textcolor::MONSTER, textcolor::MONSTER_S );
			}
		}
	}
#endif

	CDnActor::RenderCustom( fElapsedTime );
#ifndef _FINAL_BUILD
	#ifdef PRE_ADD_SHOW_MONACTION
	#else
	if( g_bShowActionName ) 
	{
		std::wstring wszCurAction;
		ToWideString(m_szAction, wszCurAction);
		
		AddHeadNameElement( 1, (WCHAR*)wszCurAction.c_str(), s_nFontIndex, 16, textcolor::MONSTER, textcolor::MONSTER_S );
	}
	#endif
#ifdef RENDER_PRESS_hayannal2009
	RenderPress();
#endif // RENDER_PRESS_hayannal2009
#endif
}

void CDnMonsterActor::SetScale( float fValue )
{
	m_fScale = fValue;
	if( m_hObject ) {
		m_hObject->SetCollisionScale( m_fScale );
	}
	MAActorRenderBase::SetScale( EtVector3( m_fScale, m_fScale, m_fScale ) );
	if( IsApplyEffectScale() ) SetActionSignalScale( EtVector3( m_fScale, m_fScale, m_fScale ) );


	if( m_fWeight == 0.f ) m_fRevisionWeight = 0.f;
	else m_fRevisionWeight = m_fWeight + ( ( m_fScale - 1.f ) * CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::ScaleWeightValue ) );
}

int CDnMonsterActor::GetMonsterClassID() 
{ 
	return m_nMonsterClassID; 
}

float CDnMonsterActor::GetWeight()
{
	return m_fRevisionWeight;
}

void CDnMonsterActor::PreProcess( LOCAL_TIME LocalTime, float fDelta )
{
	CDnActor::PreProcess( LocalTime, fDelta );
	ProcessHeadLook( m_matexWorld, fDelta, IsSignalRange( STE_HeadLook ) );
}

void CDnMonsterActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

	if( pTask && pTask->GetGameTaskType() != GameTaskType::PvP )	
	{ 
		if( ! CGameOption::GetInstance().m_bEnablePlayerEffect )
		{
			DnActorHandle hSummonerActor = this->FindActorFromUniqueID( this->GetSummonerUniqueID() );

			if( hSummonerActor && hSummonerActor->IsPlayerActor() 
				&& CDnActor::s_hLocalActor->GetUniqueID() != hSummonerActor->GetUniqueID() )
			{
				if( ! SetEnableSignal( Type, pPtr ) )		
					return;
			}
		}
	}
#endif 

	switch( Type ) {
		case STE_LockTargetLook:
			{
#if defined( PRE_MOD_LOCK_TARGET_LOOK )
				LockTargetLookStruct * pStruct = static_cast<LockTargetLookStruct*>(pPtr);
				m_nLockLookEventArea = pStruct->LookEeventAreaID;
#endif	// PRE_MOD_LOCK_TARGET_LOOK

#if defined( PRE_FIX_MOVEBACK )
				if( !IsNearMoveBack() )
#endif
				{
					m_bLockTarget = true;
				}
			}
			break;

		case STE_CanBumpWall:
			{
				CanBumpWallStruct* pStruct = static_cast<CanBumpWallStruct*>(pPtr);
				m_pszCanBumpActionName = pStruct->szActionName;
				break;
			}

		case STE_HeadLook:
			{
#ifdef PRE_FIX_MEMOPT_SIGNALH
				HeadLookStruct Struct;
				CopyShallow_HeadLook(Struct, static_cast<HeadLookStruct*>(pPtr));
#else
				HeadLookStruct Struct = *(HeadLookStruct *)pPtr;
#endif

				if( Struct.nResponeLenth == 0 ) 
					Struct.nResponeLenth = ( GetAIBase() ) ? GetAIBase()->GetThreatRange() : 0;

				OnSignalHeadLook( &Struct );	
			}
			return;
		
		case STE_Projectile:
			{
				ProjectileStruct* pStruct = (ProjectileStruct *)pPtr;
				// 슈터프랍의 액터가 아닌 일반 몬스터라면 서버에서 패킷을 쏴주므로 처리하지 않는다.
				if( GetActorType() != CDnActorState::PropActor )
					return;
			}
			break;

		case STE_ProjectileTargetPosition:
			{
				if( GetAIBase() )
					GetAIBase()->SetProjectileTarget();
			}
			break;
		case STE_RotateResistance:
			{
				RotateResistanceStruct *pStruct = (RotateResistanceStruct *)pPtr;
				m_fRotateResistance = pStruct->fResistanceRatio;
			}
			break;
		case STE_Dialogue :
			{
				if( ( MAChatBalloon::m_siTotalBalloon < 3 && HeadEffect_None == IsEffectPlaying() ) )	//2개이하이고 현재 말풍선이 없어야한다.
				{
					DialogueStruct * pStruct = (DialogueStruct *)pPtr;

					// 시그널 처리 가능 몬스터 식별 처리
					if( !IsBalloon( pStruct ) )					
						break;

					//랜덤 처리
					if( _rand()%100 <= pStruct->Rate )
					{
						if( !m_ChatBalloon.IsRenderChatBalloon() && m_bSignalBallon )
						{
							//채팅 타입
							m_ChatBalloon.SetChatBalloonCustom(
								pStruct->UIFileName,
								GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pStruct->Index),
								0xFFFFFFFF,
								GetTickCount(),
								1,
								pStruct->LifeTime);

							m_ChatBalloon.IncreaseBalloon();
						}
					}
				}
			}
			break;
		case STE_SyncPlayerCameraYaw:
			{
				// 기획분이랑 얘기해서 몬스터 특수스킬에만 넣기로 했습니다.
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera ) break;
				if( hCamera->GetCameraType() != CDnCamera::PlayerCamera ) break;
				if( CDnActor::s_hLocalActor && !CDnActor::s_hLocalActor->IsDie() ) {
					CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
					pPlayer->ForceSyncViewTime( (float)(SignalEndTime - LocalTime) * 0.001f );
				}
			}
			break;
		case STE_ChangeAxis:
			{
				m_bChangeAxisOnFinishAction = true;
			}
			break;
#ifdef PRE_ADD_CHANGE_MONSTER_SKIN
		case STE_ChangeSkin:
			{			
				ChangeSkinStruct* pStruct = (ChangeSkinStruct*)pPtr;
				EtAniObjectHandle hHandle = static_cast<EtAniObjectHandle>( CEtObject::GetSmartPtr( pStruct->nObjectSkinIndex ) );
				// 생성을 AniObject로 했기 때문에 AniObject 를 체크 한다. 
				if( this->GetAniObjectHandle() && hHandle ) 
				{
					if( hHandle->GetSkin() )
					{
						SwapSkin( hHandle );
						m_bChangeSkin = true;
					}			
				}	
			}break;
#endif 

#ifdef PRE_ADD_MONSTER_CATCH
			// 슈퍼 아머로 견디는 것 때문에 클라에서 독자적으로 처리할 수 없게됨..
			// 연출상 반응이 느려도 어쩔 수 없음. ㅠㅠ

		//	// 몬스터가 캐릭터 잡는 것은 서버 주도적으로 처리되기는 하지만 연출을 위해 클라는 클라대로 처리를 하고 
		//	// 서버쪽에서 패킷이 오는지 확인만한다. 일정 시간(500ms) 서버에서 패킷이 않오면 놓도록 처리.
		//case STE_CatchActor:
		//	{
		//		CatchActorFromSignal( reinterpret_cast<CatchActorStruct*>(pPtr), nSignalIndex );
		//	}
		//	break;

		//case STE_ReleaseActor:
		//	{
		//		m_ReleaseSignalTimeStamp = LocalTime;
		//		ReleaseActor();
		//	}
		//	break;
#endif // #ifdef PRE_ADD_MONSTER_CATCH

			//case STE_CameraEffect_RadialBlur:
			//	{
			//		//static CameraEffect_RadialBlurStruct pStruct;
			//		//DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
			//		//if( !hCamera ) break;
			//		//if( hCamera->GetCameraType() != CDnCamera::PlayerCamera ) break;
			//		//if( hCamera->FindCameraEffect( pStruct.nCameraEffectRefIndex ) ) break;
			//		//float fLength = (float)( SignalEndTime - SignalStartTime );
			//		//if( IsModifyPlaySpeed() ) {
			//		//	float fTemp = CDnActionBase::m_fFps / 60.f;
			//		//	fLength *= fTemp;
			//		//}

			//		////pStruct.vCenter->x = 0.7f;
			//		////pStruct.vCenter->y = 0.5f;
			//		//pStruct.fBlurSize = 0.1; 
			//		//pStruct.fBeginRatio = 0.2;
			//		//pStruct.fEndRatio = 0.8;

			//		//static EtVector2 vCenter;
			//		//vCenter.x = 0.5;
			//		//vCenter.y = 0.5;

			//		//// 16.6666
			//		//DWORD dwTime = (DWORD)( fLength - 16.6666f );
			//		//// 레디얼 블러에서 크리티컬 섹션이 발생한다. 왜 이럴까 ? 
			//		//pStruct.nCameraEffectRefIndex = hCamera->RadialBlur( dwTime, vCenter, pStruct.fBlurSize, pStruct.fBeginRatio, pStruct.fEndRatio );

			//		CameraEffect_RadialBlurStruct *pStruct = (CameraEffect_RadialBlurStruct *)pPtr;
			//		DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
			//		if( !hCamera ) break;
			//		if( hCamera->GetCameraType() != CDnCamera::PlayerCamera ) break;
			//		if( hCamera->FindCameraEffect( pStruct->nCameraEffectRefIndex ) ) break;

			//		float fLength = (float)( SignalEndTime - SignalStartTime );
			//		if( IsModifyPlaySpeed() ) {
			//			float fTemp = CDnActionBase::m_fFps / 60.f;
			//			fLength *= fTemp;
			//		}
			//		// 16.6666
			//		DWORD dwTime = (DWORD)( fLength - 16.6666f );
			//		pStruct->nCameraEffectRefIndex = hCamera->RadialBlur( dwTime, *pStruct->vCenter, pStruct->fBlurSize, pStruct->fBeginRatio, pStruct->fEndRatio );
			//	}	
			//	break;
	}

	CDnActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

void CDnMonsterActor::OnChangeAction( const char* szPrevAction )
{
#if defined( PRE_FIX_MOVEBACK )
	SetNearMoveBack();
#endif

	if( m_pAi )
		m_pAi->OnChangeAction( szPrevAction );

	CDnActor::OnChangeAction( szPrevAction );
}

void CDnMonsterActor::OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time)
{
	if( m_bChangeAxisOnFinishAction )
	{
		EtVector2 vView = EtVec3toVec2( *GetLookDir() );
		vView *= -1.f;
		Look( vView, true );
		m_bChangeAxisOnFinishAction = false;
	}

	if( m_pAi ) 
		m_pAi->OnFinishAction( szPrevAction, szNextAction, time );
#if defined( PRE_FIX_MOVEBACK )
	SetPrevMoveBack(false);
#endif
}

void CDnMonsterActor::CalcMonsterWeightIndex()
{
	if( m_nMonsterClassID == 0 ) return;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTERWEIGHT );
	if( !pSox ) return;

	std::vector<int> nVecList;
	pSox->GetItemIDListFromField( "_MonsterTableIndex", m_nMonsterClassID, nVecList );

	int nDifficulty;
	for( DWORD i=0; i<nVecList.size(); i++ ) {
		nDifficulty = pSox->GetFieldFromLablePtr( nVecList[i], "_Difficulty" )->GetInteger();
		if( nDifficulty == m_AIDifficult ) {
			m_nMonsterWeightTableID = nVecList[i];
			break;
		}
	}
}

float CDnMonsterActor::GetRotateAngleSpeed()
{
	return CDnMonsterState::GetRotateAngleSpeed() * m_fRotateResistance;
}

void CDnMonsterActor::AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex, bool bDelete )
{
	CDnActor::AttachWeapon( hWeapon, nEquipIndex, bDelete );
	// 무기 내구도 제설정
	if( m_nMonsterWeightTableID == -1 ) return;
	if( hWeapon->IsInfinityDurability() || hWeapon->GetDurability() <= 0 ) return;

	DNTableFileFormat* pWeight = GetDNTable( CDnTableDB::TMONSTERWEIGHT );
	int nDurability = (int)( hWeapon->GetDurability() * pWeight->GetFieldFromLablePtr( m_nMonsterWeightTableID, "_DurabilityWeight" )->GetFloat() );
	hWeapon->SetDurability( nDurability );
	hWeapon->SetMaxDurability( nDurability );
}

void CDnMonsterActor::OnBumpWall()
{
	if( !m_pszCanBumpActionName )
		return;

	CmdAction( m_pszCanBumpActionName, 0, g_fBendFrame );
}


void CDnMonsterActor::OnRestorePlaySpeed( float fPrevFPS )
{
	if( fPrevFPS == 0.f ) {
		if( m_fAlphaLayers[ AL_NORMAL ] == 0.f )
			SetAlphaBlend( 1.f );
	}
}

void CDnMonsterActor::PreThreadRelease()
{
	CDnActor::PreThreadRelease();
	if( CDnInterface::IsActive() )
		GetInterface().HideEnemyGauge( GetMySmartPtr() );
}

bool CDnMonsterActor::SetActionQueue( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bCheck, bool bCheckStateEffect )
{
	if( IsDie() && ( strstr( GetCurrentAction(), "Die" ) || strstr( GetCurrentAction(), "Summon_Off" ) ) )
		return false;
	return CDnActor::SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame, bCheck, bCheckStateEffect );
}

void CDnMonsterActor::CmdShowExposureInfo( bool bShow )
{
	CDnActor::CmdShowExposureInfo( bShow );
	m_bAlwaysShowExposureInfo = bShow;
	if( bShow ) {
		GetInterface().ShowEnemyGauge( GetMySmartPtr(), false );
	}
	else GetInterface().HideEnemyGauge( GetMySmartPtr() );
}

bool CDnMonsterActor::IsBalloon( DialogueStruct * pStruct )
{
	for( int itr = 0; itr < 4; ++itr )
	{
		if( (*pStruct->MonIDFirstGroup)[itr] == GetClassID() )
			return true;
		
		if( (*pStruct->MonIDSecondGroup)[itr] == GetClassID() )
			return true;

		if( (*pStruct->ExeptMonID)[itr] == GetClassID() )
			return false;
	}

	return false;
}

void CDnMonsterActor::SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop )
{
	CDnActor::SetAction( CheckChangeActionBlow( szActionName ), fFrame, fBlendFrame, bLoop );
}

#ifdef PRE_ADD_MONSTER_CATCH

// 서버로부터 온 CatchActor
void CDnMonsterActor::CatchActorFromServer( BYTE* pPacket )
{
#ifdef _SHADOW_TEST
	if( IsShadowActor() )
		return;
#endif // #ifdef _SHADOW_TEST

	// 서버에서 날려준 정보로 캐릭터 잡기.
	CPacketCompressStream Stream( pPacket, 128 );

	DWORD dwCatchedActorID = 0;
	int nCatchActionIndex = 0;
	int nCatchSignalArrayIndex = 0;
	Stream.Read( &dwCatchedActorID, sizeof(DWORD) );
	Stream.Read( &nCatchActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Read( &nCatchSignalArrayIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	m_CatchSignalTimeStamp = 0;

	//if( m_hCatchedActor )
	//{
	//	// 서버에서는 다른 애를 잡고 있다면 곧바로 바꿔준다.
	//	if( m_hCatchedActor->GetUniqueID() != dwCatchedActorID )
	//	{
	//		ReleaseActor();
	//	}
	//}
	
	CatchActor( nCatchActionIndex, nCatchSignalArrayIndex, dwCatchedActorID );
}

void CDnMonsterActor::CatchActorFailedFromServer( BYTE* pPacket )
{
#ifdef _SHADOW_TEST
	if( IsShadowActor() )
		return;
#endif // #ifdef _SHADOW_TEST
	
	// 서버에서 날려준 정보로 캐릭터 잡기.
	CPacketCompressStream Stream( pPacket, 128 );

	int iFailedType = -1;
	int iBlowID = 0;
	DWORD dwCatchFailedActorID = 0;
	int iParringOrBlockActionIndex = -1;
	string strBlockOrParringAction;
	Stream.Read( &iFailedType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Read( &iBlowID, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Read( &dwCatchFailedActorID, sizeof(DWORD) );
	Stream.Read( &iParringOrBlockActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	m_CatchSignalTimeStamp = 0;

	DnActorHandle hCatchFailedActor = FindActorFromUniqueID( dwCatchFailedActorID );
	if( hCatchFailedActor )
	{
		switch( iFailedType )
		{
			// 패링
			case 0:
				{
					ActionElementStruct* pActionElement = hCatchFailedActor->GetElement( iParringOrBlockActionIndex );
					if( pActionElement )
					{
						const char* pParringActionName = pActionElement->szName.c_str();
						hCatchFailedActor->SetActionQueue( pParringActionName );
					}
				}
				break;

			// 블록
			case 1:
				{
					ActionElementStruct* pActionElement = hCatchFailedActor->GetElement( iParringOrBlockActionIndex );
					if( pActionElement )
					{
						const char* pBlockActionName = pActionElement->szName.c_str();
						hCatchFailedActor->SetActionQueue( pBlockActionName );
					}
				}
				break;

			// 슈퍼아머로 견딤.
			case 2:
				{
					// 아직 클라에서 뭔가 해주진 않음.
				}
				break;
		}
	}
}

//// 로컬 시그널에서 곧바로 호출되는 CatchActor
//// 보통 클라이언트에서는 이쪽으로 호출됨.
//void CDnMonsterActor::CatchActorFromSignal( CatchActorStruct* pCatchActor, int nSignalIndex )
//{
//#ifdef _SHADOW_TEST
//	if( IsShadowActor() )
//		return;
//#endif // #ifdef _SHADOW_TEST
//
//	if( m_hCatchedActor )
//		return;
//
//	m_strCatchBoneName = pCatchActor->szCatchBoneName;
//	m_strTargetActorCatchBoneName = pCatchActor->szTargetActorCatchBoneName;
//
//	if( pCatchActor->szCatchedActorAction != NULL &&
//		0 < strlen(pCatchActor->szCatchedActorAction) )
//	{
//		m_strCatchedActorAction.assign( pCatchActor->szCatchedActorAction );
//	}
//	else
//		m_strCatchedActorAction.assign( "Hold" );
//
//	EtVector3 vCatchBonePos( 0.0f, 0.0f, 0.0f );
//	EtMatrix matBoneWorld = GetBoneMatrix( m_strCatchBoneName.c_str() );
//	memcpy_s( &vCatchBonePos, sizeof(EtVector3), &matBoneWorld._41, sizeof(EtVector3) );
//
//	// 사용하는 본 이름으로 본의 위치를 얻어오고 현재 본 위치 근처에서 시그널에서 설정한 범위보다 
//	// 거리가 짧은 애가 있는지 확인.
//	DNVector( DnActorHandle ) vlhActors;
//	ScanActor( vCatchBonePos, pCatchActor->fCatchDistance, vlhActors );
//
//	float fMinDistanceSQ = FLT_MAX;
//	DnActorHandle hResultActor;
//	for( int i = 0; i < (int)vlhActors.size(); ++i )
//	{
//		DnActorHandle hActor = vlhActors.at( i );
//
//		// 플레이어 액터만.
//		if( false == hActor->IsPlayerActor() )
//			continue;
//
//		// 같은 팀 제외.
//		if( GetTeam() == hActor->GetTeam() )
//			continue;
//
//		// 가장 가까운 녀석을 잡기 처리.
//		EtVector3 vTargetActorPos = *hActor->GetPosition();
//		EtVector3 vDist = vCatchBonePos - vTargetActorPos;
//		float fDistanceSQ = EtVec3LengthSq( &vDist );
//		if( fDistanceSQ < fMinDistanceSQ )
//		{
//			// 높이도 맞는지 확인.
//			bool bHeightMax =  vTargetActorPos.y < (pCatchActor->fHeightMax + vCatchBonePos.y);
//			bool bHeightMin = (vCatchBonePos.y + pCatchActor->fHeightMin) < vTargetActorPos.y;		// HeightMin 값은 음수임.
//			if( bHeightMax && bHeightMin )
//			{
//				hResultActor = hActor;
//				fMinDistanceSQ = fDistanceSQ;
//			}
//		}
//	}
//
//	if( hResultActor )
//	{
//		m_hCatchedActor = hResultActor;
//		m_hCatchedActor->SetActionQueue( m_strCatchedActorAction.c_str() );
//		if( m_hCatchedActor->IsPlayerActor() )
//		{
//			static_cast<CDnPlayerActor*>(m_hCatchedActor.GetPointer())->SetCatcherMonsterActor( GetMySmartPtr() );
//		}
//
//		m_CatchSignalTimeStamp = this->CDnActionBase::m_LocalTime;
//
//		CatchActor( GetCurrentActionIndex(), nSignalIndex, m_hCatchedActor->GetUniqueID() );
//	}
//}

void CDnMonsterActor::CatchActor( int nCatchActionIndex, int nCatchSignalArrayIndex, DWORD dwCatchedActorID )
{
#ifdef _SHADOW_TEST
	if( IsShadowActor() )
		return;
#endif // #ifdef _SHADOW_TEST

	S_CATCH_ACTOR_INFO CatchActorInfo;

	_ASSERT( GetCurrentActionIndex() == nCatchActionIndex );

	ActionElementStruct* pActionElement = GetElement( (DWORD)nCatchActionIndex );

	_ASSERT( nCatchSignalArrayIndex < (int)pActionElement->pVecSignalList.size() );

	CatchActorStruct* pCatchActorInfo = NULL;
	for( int i = 0; i < (int)pActionElement->pVecSignalList.size(); ++i )
	{
		CEtActionSignal* pCatchActionSignal = pActionElement->pVecSignalList.at( i );
		if( nCatchSignalArrayIndex == pCatchActionSignal->GetSignalListArrayIndex() )
		{
			pCatchActorInfo = reinterpret_cast<CatchActorStruct*>(pCatchActionSignal->GetData());
			break;
		}
	}

	m_strCatchBoneName = pCatchActorInfo->szCatchBoneName;
	m_strTargetActorCatchBoneName = pCatchActorInfo->szTargetActorCatchBoneName;
	m_strCatchedActorAction.assign( "Hold" );

	if( pCatchActorInfo->szCatchedActorAction &&
		0 < strlen( pCatchActorInfo->szCatchedActorAction ) )
	{
		m_strCatchedActorAction = pCatchActorInfo->szCatchedActorAction;
	}
	else
		m_strCatchedActorAction.assign( "Hold" );

	CatchActorInfo.hCatchedActor = FindActorFromUniqueID( dwCatchedActorID );
	_ASSERT( CatchActorInfo.hCatchedActor );

	CatchActorInfo.hCatchedActor->SetActionQueue( m_strCatchedActorAction.c_str() );
	if( CatchActorInfo.hCatchedActor->IsPlayerActor() )
	{
		static_cast<CDnPlayerActor*>(CatchActorInfo.hCatchedActor.GetPointer())->SetCatcherMonsterActor( GetMySmartPtr() );
	}
	m_vPrevCatchPos = *CatchActorInfo.hCatchedActor->GetPosition();

	m_vlCatchedActors.push_back( CatchActorInfo );
}

void CDnMonsterActor::ReleaseActorFromServer( BYTE* pPacket )
{
#ifdef _SHADOW_TEST
	if( IsShadowActor() )
		return;
#endif // #ifdef _SHADOW_TEST

	// 서버에서 온 경우 패킷이 유효한지 체크.
	if( NULL != pPacket )
	{
		CPacketCompressStream Stream( pPacket, 128 );

		// 몬스터 손에서 놓기.
		DWORD dwCatchedActor = 0;
		Stream.Read( &dwCatchedActor, sizeof(DWORD) );

		DnActorHandle hCatchedActor = FindActorFromUniqueID( dwCatchedActor );
		ReleaseThisActor( hCatchedActor, true );
	}
}

void CDnMonsterActor::ReleaseThisActor( DnActorHandle hCatchedActor, bool bEraseInVector )
{
#ifdef _SHADOW_TEST
	if( IsShadowActor() )
		return;
#endif // #ifdef _SHADOW_TEST

	if( hCatchedActor )
	{
		vector<S_CATCH_ACTOR_INFO>::iterator iter = m_vlCatchedActors.begin();
		for( iter; iter != m_vlCatchedActors.end(); ++iter )
		{
			S_CATCH_ACTOR_INFO& CatchActorInfo = *iter;
			if( CatchActorInfo.hCatchedActor &&
				CatchActorInfo.hCatchedActor == hCatchedActor)
			{
				if( hCatchedActor->IsPlayerActor() )
				{
					CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>( hCatchedActor.GetPointer() );
					pPlayerActor->OnReleaseFromMonster( GetMySmartPtr() );
				}

				if( bEraseInVector )
					m_vlCatchedActors.erase( iter );

				break;
			}
		}
	}
}

void CDnMonsterActor::ProcessCatchActor( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _SHADOW_TEST
	if( IsShadowActor() )
		return;
#endif // #ifdef _SHADOW_TEST

	//if( m_hCatchedActor )

	vector<S_CATCH_ACTOR_INFO>::iterator iter = m_vlCatchedActors.begin();
	for( iter; iter != m_vlCatchedActors.end(); )
	{
		//// 서버에서 패킷이 오는지 기다린다. 일정시간 동안 안오면 놔버림.
		//// 제대로 패킷이 도착했고 셋팅이 끝났다면 m_CatchSignalTimeStamp 가 0 임. 
		//// 따라서 실제 타임 스탬프가 있을 경우에만 체크한다.
		//if( 0 < m_CatchSignalTimeStamp )
		//{
		//	if( 500 < LocalTime - m_CatchSignalTimeStamp )
		//	{
		//		ReleaseThisActor();
		//		return;
		//	}
		//}

		S_CATCH_ACTOR_INFO CatchActorInfo = *iter;

		// 잡고 있는 플레이어가 죽었을 때. 놔준다.
		// TODO: 잡고 있는 이 몬스터가 죽었을 때도 놔줘야 한다.
		if( CatchActorInfo.hCatchedActor && 
			false == CatchActorInfo.hCatchedActor->IsDie() && 
			false == IsDie() )
		{
			EtVector3 vCatchBonePos( 0.0f, 0.0f, 0.0f );
			EtMatrix matBoneWorld = GetBoneMatrix( m_strCatchBoneName.c_str() );
			memcpy_s( &vCatchBonePos, sizeof(EtVector3), &matBoneWorld._41, sizeof(EtVector3) );

			// vCatchBonePos 에 캐릭터 Bip01 을 배치시켜야 한다.
			// 현재 이 몹의 vCatchBonePos 와 잡힐 캐릭터의 Bip01 의 거리를 계산해서 
			// 그만큼 위치에 적용시켜준다.
			EtVector3 vPlayerCatchedPointPos( 0.0f, 0.0f, 0.0f );
			matBoneWorld = CatchActorInfo.hCatchedActor->GetBoneMatrix( m_strTargetActorCatchBoneName.c_str() );
			memcpy_s( &vPlayerCatchedPointPos, sizeof(EtVector3), &matBoneWorld._41, sizeof(EtVector3) );

			EtVector3 vDelta = vCatchBonePos - vPlayerCatchedPointPos;
			EtVector3 vNowCatchedActorPos = m_vPrevCatchPos;//*CatchActorInfo.hCatchedActor->GetPosition();
			EtVector3 vCatchedActorPos = vNowCatchedActorPos + vDelta;
			//EtVector3 vCatchedActorPos = vCatchBonePos;
			CatchActorInfo.hCatchedActor->SetPosition( vCatchedActorPos );
			CatchActorInfo.hCatchedActor->SetPrevPosition( vCatchedActorPos );

			m_vPrevCatchPos = vCatchedActorPos;

			// 다른 액션을 취하고 있으면 잡기 액션으로 고쳐준다.
			int iCurrentActionIndex = CatchActorInfo.hCatchedActor->GetCurrentActionIndex();
			int iCatchedActorActionIndex = CatchActorInfo.hCatchedActor->GetElementIndex( m_strCatchedActorAction.c_str() );
			if( iCurrentActionIndex != iCatchedActorActionIndex )
				CatchActorInfo.hCatchedActor->SetActionQueue( m_strCatchedActorAction.c_str() );

			++iter;
		}
		else
		{
			ReleaseThisActor( CatchActorInfo.hCatchedActor, false );
			iter = m_vlCatchedActors.erase( iter );
		}
	}
}
#endif // #ifdef PRE_ADD_MONSTER_CATCH

#if defined(PRE_FIX_64312)
void CDnMonsterActor::ApplySummonMonsterExSkill(BYTE* pPacket)
{
	// 서버에서 날려준 정보로 캐릭터 잡기.
	CPacketCompressStream Stream( pPacket, 128 );
	
	int nBaseSkillID = -1;
	int nLevel = 0;
	int nSelectedType = 0;
	DWORD dwMasterUniqueID = 0;
	int nMasterBaseSkillID = -1;

	Stream.Read(&nBaseSkillID, sizeof(int));
	Stream.Read(&nLevel, sizeof(int));
	Stream.Write(&nSelectedType, sizeof(int));
	Stream.Read(&dwMasterUniqueID, sizeof(DWORD));
	Stream.Read(&nMasterBaseSkillID, sizeof(int));


	DnSkillHandle hMonsterSkill = FindSkill(nBaseSkillID);
	if (!hMonsterSkill)
	{
		AddSkill(nBaseSkillID, nLevel, nSelectedType);
		hMonsterSkill = FindSkill(nBaseSkillID);
	}

	DnSkillHandle hMasterBaseSkill;
	DnSkillHandle hMonsterExSkill;

	DnActorHandle hMasterActor = GetSummonerPlayerActor();
	if (hMasterActor && hMasterActor->GetUniqueID() == dwMasterUniqueID)
		hMasterBaseSkill = hMasterActor->FindSkill(nMasterBaseSkillID);

	if (hMasterBaseSkill)
		hMonsterExSkill = hMasterBaseSkill->GetSummonMonsterEnchangeSkill();

	if (hMonsterSkill && hMonsterExSkill)
		hMonsterSkill->ApplySummonMonsterEnchantSkill(hMonsterExSkill);
}
#endif // PRE_FIX_64312

#ifdef PRE_ADD_CHANGE_MONSTER_SKIN
void CDnMonsterActor::SwapSkin( EtAniObjectHandle hSwapObject )
{	
	EtAniObjectHandle pAniObject = this->GetAniObjectHandle();
	if( hSwapObject == NULL ) return;
	if( pAniObject && pAniObject->GetSkin().m_nIndex == hSwapObject->GetSkin().m_nIndex ) return;
	
	pAniObject->ReBuildSkin( hSwapObject->GetSkin() );

}
	
#endif 