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

	// ȭ�찰����� ���Ⱑ �ÿ��̵Ǹ鼭 �߻�Ǳ� ������ Hit �� Idle �� ���ش�.
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
			//��Ƽ���� ��� ������ ������ �� �Ǿ� ���� �� ����.(��� ������ Ŭ���̾�Ʈ���� ������ ��û �������� ������)
			//�ڽ��� ��� ������ GuildTask���� ���� ������ �Ѵ�..
			int nGuildRewardItemValue = pPlayer->GetPlayerGuildRewardEffectValue(GUILDREWARDEFFECT_TYPE_REDUCEDURABILITYRATIO);
		
			if( nGuildRewardItemValue > 0 )
				nFinalDeadDurability -= (int)(nFinalDeadDurability * nGuildRewardItemValue * 0.01);
			// ��Ƽ�� ������ ����
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
	// Ư���� ���ٰ� ����. ��Ŷ ���� ó���ϸ� �ȴ�.
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
			// �������� �ӵ��� 10�̻��̸� bigBounce�� �ѹ� �� ����ش�.
			if( fCurVelocity < -6.f && m_HitParam.vVelocity.y != 0.f && abs(m_HitParam.vVelocity.y) > 0.1f ) {
				if( m_HitParam.vVelocity.y > 0.f ) {
					m_HitParam.vVelocity.y *= 0.6f;
					SetVelocityY( m_HitParam.vVelocity.y );
				}
				else { // ���ӵ��� ó������ �ٴ����� �����ִ� ��쿡�� ����������Ѵ�.
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
	//LogWnd::Log( 1, _T("CmdStop Look ����. "));
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
		// #36030 Summon_Off �׼� �̹� �������̸� �ٽ� �׼� �����Ű�� �ʴ´�.
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
			break;			//4���̻�(�̺�Ʈ3��)�� �Ұ��ϰ� ����, 1���� ���� ���� ������� Ȯ������Ͽ� �Ѵ� ������ �Ǵ�
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
				// ��ġ ���� SetPosition
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

				// ��ġ ���� SetPosition
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

					// ������ Ŭ�� �� �߿� ����ȿ���ʿ��� �߻���Ų �׼�.
					if( bFromStateBlow )
						m_pStateBlow->OnCmdActionFromPacket( szAction.c_str() );
				}

				// ��� �־�����ϴ�. ��������� ���� ����.
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

				// ��ġ ���� SetPosition
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

				// ��ġ ���� SetPosition
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

				// LookTarget �� MoveTarget �� Ʋ������������ ��׷�Ÿ���� �ٲ������� ������ MoveTarget �� Reset��Ų��.(���)
//				if( hActor )
				ResetMove();
				m_bSignalBallon = true;

				// ��׷� Ÿ�ٰ��������� ������������� �Ǵ��Ѵ�.
				////////////////////////////////////////
				CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
				if( !pGameTask ) break;
				pGameTask->CheckBattleMode( GetMySmartPtr(), ( hActor ) ? true : false );
				////////////////////////////////////////
				if( hActor ) {
					if( GetGrade() >= Champion ) {
						GetInterface().ShowEnemyGauge( GetMySmartPtr(), true );	// �Ӹ��� �̸��� ������ �ϱ� ���� �̷��� ȣ��.
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

#if defined( PRE_ADD_ACADEMIC ) // ��ȯü ���Ϳ��� ����ϴ� ��ų�� ��Ÿ����< ActiveToggle > �̶� �������ֵ��� ����.
				if( GetSummonerPlayerActor() && GetSummonerPlayerActor() == CDnActor::s_hLocalActor && nSummonerSkillID > 0)
				{
					DnSkillHandle hSkill = GetSummonerPlayerActor()->FindSkill(nSummonerSkillID);
					if(hSkill)
					{
						hSkill->OnBeginCoolTime(); // ��Ÿ���� ���������� ��ǻ� ������� ���� ��Ÿ���̴�.
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

				// ��ġ ���� SetPosition
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
					
					//������ ��쵵 ��ȯ��..
					m_hSummonerPlayerActor = hSummonMaster;

					if( hSummonMaster && hSummonMaster->IsPlayerActor() )
					{
						m_bFollowSummonerStage = bFollowSummonerStage;

						// �������� ���󰡴� ���͸� Summon_On �׼��� ���� �ʰ� ��ٷ� Stand..
						// �� �÷��״� true �� �Ǵ� ��쿡 �������� ���󰡴� ���Ͱ� �������� �̵���
						// �ؼ� ���� �����ǰ��� ���Ӽ������� �����ش�.
						if( m_bFollowSummonerStage && bReCreatedFollowStageMonster )
							SetActionQueue( "Stand" );

						if( CDnActor::s_hLocalActor && hSummonMaster == CDnActor::s_hLocalActor)
						{
							CmdShowExposureInfo(true); // ���� ��ȯ�� ��ȯü�� �̸��� HP �� ǥ��������
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
	
								// ����� ������ ������ ���� �°� ���� , �̰� ��Ȳ������ Ʋ���� �����ϴ��� �ؾ��ҵ�.. 
								// ������ �������� ��Ȯ�ϰ� �ϸ� �𸣰ٴµ�.. PVP �Ϲݰ�����쿡�� ���� ���������� �������̶�.. �Ӹ��� �Ķ������� ǥ�ð��ȴ�.. ���������� �Ķ���
								if( (
									((CDnPvPGameTask*)pGameTask)->GetGameMode() == PvPCommon::GameMode::PvP_GuildWar ) || 
									( CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer) )
								{
									if( hSummonMaster->GetTeam() == PvPCommon::Team::A ) // �����°� ���õ� ��쿡�� �̷��� ǥ��
										szSkinName += "_BLUE.skn";
									else if( hSummonMaster->GetTeam() == PvPCommon::Team::B )
										szSkinName += "_RED.skn";
								}
								else
								{
									if( hSummonMaster->GetTeam() == CDnActor::s_hLocalActor->GetTeam() ) // �Ϲ������� �����̸� ǥ��
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
				//�ñ׳ο� ���� ��ǳ���� �����ǰ� ������ ���̶�� ��ǳ���� �����.
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

	// ActionIndex, SignalListArrayIndex �� SignalStruct ��´�.
	bool bFound = false;
	ActionElementStruct* pElementStruct = GetElement( iActionIndex );
	if( pElementStruct )
	{
		for( UINT i=0 ; i<pElementStruct->pVecSignalList.size() ; ++i )
		{
			if( pElementStruct->pVecSignalList[i]->GetSignalListArrayIndex() == iSignalIndex )
			{
				// Ŭ�󿡼� �ñ׳��� �߸� ã�� ��찡 �־ ����ڵ� �־���� ���Ѻ��ϴ�.
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

	DN_ASSERT( bFound, "�ñ׳��� ã�� �� ����!!! ����,Ŭ���̾�Ʈ�� Action������ ����Ȯ�� Ȯ�ιٶ�!" );

	// ���� �ǽɰ��� �� �����غ��ϴ�.
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

	// LocalCross �� ������ �ʿ䰡 ���� ��. ����� ���� ���Ϳ����� ����.
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


// CDnPlayerActor�� SC_PROJECTILE ó�� �κа� ����.
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
				// ���������� ���Ͱ� �ƴ� �Ϲ� ���Ͷ�� �������� ��Ŷ�� ���ֹǷ� ó������ �ʴ´�.
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
				if( ( MAChatBalloon::m_siTotalBalloon < 3 && HeadEffect_None == IsEffectPlaying() ) )	//2�������̰� ���� ��ǳ���� ������Ѵ�.
				{
					DialogueStruct * pStruct = (DialogueStruct *)pPtr;

					// �ñ׳� ó�� ���� ���� �ĺ� ó��
					if( !IsBalloon( pStruct ) )					
						break;

					//���� ó��
					if( _rand()%100 <= pStruct->Rate )
					{
						if( !m_ChatBalloon.IsRenderChatBalloon() && m_bSignalBallon )
						{
							//ä�� Ÿ��
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
				// ��ȹ���̶� ����ؼ� ���� Ư����ų���� �ֱ�� �߽��ϴ�.
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
				// ������ AniObject�� �߱� ������ AniObject �� üũ �Ѵ�. 
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
			// ���� �Ƹӷ� �ߵ�� �� ������ Ŭ�󿡼� ���������� ó���� �� ���Ե�..
			// ����� ������ ������ ��¿ �� ����. �Ф�

		//	// ���Ͱ� ĳ���� ��� ���� ���� �ֵ������� ó���Ǳ�� ������ ������ ���� Ŭ��� Ŭ���� ó���� �ϰ� 
		//	// �����ʿ��� ��Ŷ�� ������ Ȯ�θ��Ѵ�. ���� �ð�(500ms) �������� ��Ŷ�� �ʿ��� ������ ó��.
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
			//		//// ����� ������ ũ��Ƽ�� ������ �߻��Ѵ�. �� �̷��� ? 
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
	// ���� ������ ������
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

// �����κ��� �� CatchActor
void CDnMonsterActor::CatchActorFromServer( BYTE* pPacket )
{
#ifdef _SHADOW_TEST
	if( IsShadowActor() )
		return;
#endif // #ifdef _SHADOW_TEST

	// �������� ������ ������ ĳ���� ���.
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
	//	// ���������� �ٸ� �ָ� ��� �ִٸ� ��ٷ� �ٲ��ش�.
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
	
	// �������� ������ ������ ĳ���� ���.
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
			// �и�
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

			// ���
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

			// ���۾Ƹӷ� �ߵ�.
			case 2:
				{
					// ���� Ŭ�󿡼� ���� ������ ����.
				}
				break;
		}
	}
}

//// ���� �ñ׳ο��� ��ٷ� ȣ��Ǵ� CatchActor
//// ���� Ŭ���̾�Ʈ������ �������� ȣ���.
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
//	// ����ϴ� �� �̸����� ���� ��ġ�� ������ ���� �� ��ġ ��ó���� �ñ׳ο��� ������ �������� 
//	// �Ÿ��� ª�� �ְ� �ִ��� Ȯ��.
//	DNVector( DnActorHandle ) vlhActors;
//	ScanActor( vCatchBonePos, pCatchActor->fCatchDistance, vlhActors );
//
//	float fMinDistanceSQ = FLT_MAX;
//	DnActorHandle hResultActor;
//	for( int i = 0; i < (int)vlhActors.size(); ++i )
//	{
//		DnActorHandle hActor = vlhActors.at( i );
//
//		// �÷��̾� ���͸�.
//		if( false == hActor->IsPlayerActor() )
//			continue;
//
//		// ���� �� ����.
//		if( GetTeam() == hActor->GetTeam() )
//			continue;
//
//		// ���� ����� �༮�� ��� ó��.
//		EtVector3 vTargetActorPos = *hActor->GetPosition();
//		EtVector3 vDist = vCatchBonePos - vTargetActorPos;
//		float fDistanceSQ = EtVec3LengthSq( &vDist );
//		if( fDistanceSQ < fMinDistanceSQ )
//		{
//			// ���̵� �´��� Ȯ��.
//			bool bHeightMax =  vTargetActorPos.y < (pCatchActor->fHeightMax + vCatchBonePos.y);
//			bool bHeightMin = (vCatchBonePos.y + pCatchActor->fHeightMin) < vTargetActorPos.y;		// HeightMin ���� ������.
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

	// �������� �� ��� ��Ŷ�� ��ȿ���� üũ.
	if( NULL != pPacket )
	{
		CPacketCompressStream Stream( pPacket, 128 );

		// ���� �տ��� ����.
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
		//// �������� ��Ŷ�� ������ ��ٸ���. �����ð� ���� �ȿ��� ������.
		//// ����� ��Ŷ�� �����߰� ������ �����ٸ� m_CatchSignalTimeStamp �� 0 ��. 
		//// ���� ���� Ÿ�� �������� ���� ��쿡�� üũ�Ѵ�.
		//if( 0 < m_CatchSignalTimeStamp )
		//{
		//	if( 500 < LocalTime - m_CatchSignalTimeStamp )
		//	{
		//		ReleaseThisActor();
		//		return;
		//	}
		//}

		S_CATCH_ACTOR_INFO CatchActorInfo = *iter;

		// ��� �ִ� �÷��̾ �׾��� ��. ���ش�.
		// TODO: ��� �ִ� �� ���Ͱ� �׾��� ���� ����� �Ѵ�.
		if( CatchActorInfo.hCatchedActor && 
			false == CatchActorInfo.hCatchedActor->IsDie() && 
			false == IsDie() )
		{
			EtVector3 vCatchBonePos( 0.0f, 0.0f, 0.0f );
			EtMatrix matBoneWorld = GetBoneMatrix( m_strCatchBoneName.c_str() );
			memcpy_s( &vCatchBonePos, sizeof(EtVector3), &matBoneWorld._41, sizeof(EtVector3) );

			// vCatchBonePos �� ĳ���� Bip01 �� ��ġ���Ѿ� �Ѵ�.
			// ���� �� ���� vCatchBonePos �� ���� ĳ������ Bip01 �� �Ÿ��� ����ؼ� 
			// �׸�ŭ ��ġ�� ��������ش�.
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

			// �ٸ� �׼��� ���ϰ� ������ ��� �׼����� �����ش�.
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
	// �������� ������ ������ ĳ���� ���.
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