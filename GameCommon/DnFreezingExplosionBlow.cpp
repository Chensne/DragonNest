#include "StdAfx.h"
#include "DnFreezingExplosionBlow.h"
#include "DnMonsterActor.h"
#include "EtActionSignal.h"

#ifdef _GAMESERVER
#include "DnGameRoom.h"
#include "DnGameDataManager.h"
#include "DnUserSession.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define FREEZING_EXPLOSION_ACTION_NAME "Hit_Freezing"
#ifdef PRE_FIX_70011
#define FREEZING_EXPLOSION_HIT_ACTION_NAME "Hit_Pushed"
#endif // PRE_FIX_70011

CDnFreezingExplosionBlow::CDnFreezingExplosionBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																								  m_pActionElement( NULL ),
																								  m_fFrame( 0.0f )
#ifdef _GAMESERVER
																								 ,m_iOriAttackMMin( 0 ),
																								  m_iOriAttackMMax( 0 ),
																								  m_bFreezingHitAction( false )
#endif
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_117;
	AddCallBackType(SB_ONTARGETHIT);

	SetValue( szValue );
	m_fValue = (float)atof(szValue);

	m_pActionElement = m_hActor->GetElement( FREEZING_EXPLOSION_ACTION_NAME );
	_ASSERT( m_pActionElement && "화상 폭발 액션 정보를 찾을 수 없음." );

	m_ActionDurationTime = 0;

	if (m_pActionElement)
	{
		m_ActionDurationTime = m_pActionElement->dwLength;
		m_vlpSignalList = m_pActionElement->pVecSignalList;
	}
}

CDnFreezingExplosionBlow::~CDnFreezingExplosionBlow(void)
{

}

bool CDnFreezingExplosionBlow::CanBegin( void )
{
	// 결빙 상태효과가 걸려있는 상태가 아니라면 시작될 수 없다.
	bool bCanBegin = m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_041 ) ||
					 m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_144 );	// 동상 상태효과도 제거. (#27838);
	
	if( false == bCanBegin )
		OutputDebug( "CDnFreezingExplosionBlow::CanBegin - 결빙 상태효과가 걸려있지 않으므로 결빙 폭발 상태효과 발동 실패.\n" );

	// 폭발 액션 없을 땐 발동시키지 않는다.
	if( NULL == m_pActionElement )
	{
		bCanBegin = false;
		OutputDebug( "CDnFreezingExplosionBlow::CanBegin - 결빙 폭발 액션이 없으므로 화상 폭발 상태효과 발동 실패.\n" );
	}

	return bCanBegin;
}

void CDnFreezingExplosionBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// 결빙 상태효과 해제
#ifdef _GAMESERVER

	// #28747 결빙, 동상 상태효과 둘 중에 하나만 제거한다.
	if( m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_041 ) )
		m_hActor->CmdRemoveStateEffect( STATE_BLOW::BLOW_041 );

	if( m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_144 ) )
		m_hActor->CmdRemoveStateEffect( STATE_BLOW::BLOW_144 );	// 동상 상태효과도 제거. (#27838)

	_ApplyOtherSE( m_hActor );		// 상태효과 걸린 주체에게 느려짐 상태효과 추가.

#endif

#ifdef PRE_FIX_70011
	if( m_hActor->GetHitParam()->bSuccessNormalDamage == true && m_hActor->GetHitParam()->szActionName.empty() == true )
		m_hActor->CmdAction( FREEZING_EXPLOSION_HIT_ACTION_NAME );
#endif // PRE_FIX_70011

	// 이 상태효과가 끼어있는 스킬의 시전자의 마법 공격력으로 상태효과가 적용된 액터의 마법공격력을 지속시간 동안 대체시킴.
#ifdef _GAMESERVER
	_ASSERT( m_ParentSkillInfo.hSkillUser );
	if( !m_ParentSkillInfo.hSkillUser )
	{
		g_Log.Log( LogType::_ERROR, L"CDnFreezingExplosionBlow::OnBegin - 결빙 폭발 상태효과 액터가 유효하지 않아서 발동 실패..\n" );
		SetState( STATE_BLOW::STATE_END );
		return;
	}

	m_iOriAttackMMin = m_hActor->GetAttackMMin();
	m_iOriAttackMMax = m_hActor->GetAttackMMax();

	m_hActor->SetAttackMMin( m_ParentSkillInfo.hSkillUser->GetAttackMMin() );
	m_hActor->SetAttackMMax( m_ParentSkillInfo.hSkillUser->GetAttackMMax() );

	// 몬스터라면 클라에서 데미지 표시를 위해 셋팅.
	if( m_hActor && CDnActorState::Reserved6 < m_hActor->GetActorType() )
		static_cast<CDnMonsterActor*>(m_hActor.GetPointer())->SlaveOf( m_ParentSkillInfo.hSkillUser->GetUniqueID() );
#endif

	// 지속시간은 폭발 액션의 지속시간으로 처리한다.
	float fFPS = 60.0f;
	//m_StateBlow.fDurationTime = (float)m_pActionElement->dwLength / fFPS;
	m_StateBlow.fDurationTime = m_ActionDurationTime != 0 ? (float)m_ActionDurationTime / fFPS : 0.0f;

	// 바로 종료가 될 수 있으므로 정상 상태일때만 추가
	if (STATE_BLOW::STATE_BEGIN == GetBlowState())
		m_hActor->AddStateBlowSignal(this->GetMySmartPtr());

	OutputDebug( "CDnFreezingExplosionBlow::OnBegin\n" );
}


void CDnFreezingExplosionBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

}


void CDnFreezingExplosionBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	m_hActor->SetAttackMMin( m_iOriAttackMMin );
	m_hActor->SetAttackMMax( m_iOriAttackMMax );

	// 몬스터라면 클라에서 데미지 표시를 위해 셋팅한 것 원상 복구.
	//if( CDnActorState::Reserved6 < m_hActor->GetActorType() )
	if( m_hActor && m_hActor->IsMonsterActor() )
	{
		DWORD dwInvalidMasterActorID = UINT_MAX;
		static_cast<CDnMonsterActor*>(m_hActor.GetPointer())->SlaveOf( dwInvalidMasterActorID );
	}
#endif

	m_hActor->RemoveStateBlowSignal(this->GetMySmartPtr());

	OutputDebug( "CDnFreezingExplosionBlow::OnEnd\n" );
}

void CDnFreezingExplosionBlow::OnTargetHit( DnActorHandle hTargetActor )
{
#ifdef _GAMESERVER
	if( m_bFreezingHitAction )
		_ApplyOtherSE( hTargetActor );
#endif
}

#ifdef _GAMESERVER
void CDnFreezingExplosionBlow::_ApplyOtherSE( DnActorHandle hActor )
{
	// 일단 시험적인 부분으로, 코드에 박아놓는다. (#7842)
	//- 프레임 증가 = 0.5
	//- 이속 감소 = -0.5
	//- 상태이상 효과(동상) -> 이펙트를 말하는 것임.

	// 네스트의 보스라면 부가 상태효과들이 추가되지 않는다.
	CDNGameRoom* pGameRoom = static_cast<CDNGameRoom*>(m_hActor->GetRoom());
	bool bNestMap = false;
	if( false == pGameRoom->bIsPvPRoom() )
	{
		UINT uiSessionID = 0;
		pGameRoom->GetLeaderSessionID( uiSessionID );
		CDNUserSession *pUserSession = pGameRoom ? pGameRoom->GetUserSession(uiSessionID) : NULL;
		if( pUserSession )
		{
			const TMapInfo* pMapData = g_pDataManager->GetMapInfo( pUserSession->GetMapIndex() );
			if( pMapData )
#if defined(PRE_ADD_DRAGON_FELLOWSHIP)
				bNestMap = CDnBlow::CheckEffectIgnoreMapType(pMapData->MapType, pMapData->MapSubType);
#else	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
				bNestMap = (GlobalEnum::MAP_DUNGEON == pMapData->MapType) && (GlobalEnum::MAPSUB_NEST == pMapData->MapSubType);
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
		}
	}

	bool bBossMonster = false;
	if( hActor->IsMonsterActor() )
	{
		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
		if( (CDnMonsterState::Boss == pMonsterActor->GetGrade() ||
			CDnMonsterState::BossHP4 == pMonsterActor->GetGrade() ||
			CDnMonsterState::NestBoss == pMonsterActor->GetGrade() ||
			CDnMonsterState::NestBoss8 == pMonsterActor->GetGrade()) )
		{
			bBossMonster = true;
		}
	}

	//#51048 235번 상태효과가 적용 되어 있는 경우 행동감소 효과가 적용되지 않도록한다.
	if( m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_235) )
	{
		bNestMap = true;
		bBossMonster = true;
	}

	if( false == bNestMap ||
		false == bBossMonster )
	{
		// 동일한 스킬의 이 상태효과로 들어가 있는 프레임, 이동속도 감소 상태효과가 있다면 추가하지 않는다.
		bool bCanAdd = true;
		if( hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_025) &&
			hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_076) )
		{
			DNVector(DnBlowHandle) vlBlows;
			hActor->GetAllAppliedStateBlowBySkillID( m_ParentSkillInfo.iSkillID, vlBlows );
			bCanAdd = vlBlows.empty();
			//for( int i = 0; i < (int)vlBlows.size(); ++i )
			//{
			//	DnBlowHandle hBlow = vlBlows.at( i );
			//}
		}

		if( bCanAdd )
		{
			CDnSkill::SkillInfo SkillInfo = m_ParentSkillInfo;
			char szBuff[128] = {0, };
			_snprintf_s(szBuff, _countof(szBuff), _TRUNCATE, "%d", 22 );
			SkillInfo.szEffectOutputIDToClient = szBuff;
			//SkillInfo.hSkillUser = m_hActor;/*GetMySmartPtr();*/
			hActor->CmdAddStateEffect( &SkillInfo, STATE_BLOW::BLOW_025, 7000, "-0.5" );
			hActor->CmdAddStateEffect( &SkillInfo, STATE_BLOW::BLOW_076, 7000, "-0.5" );
		}
	}
}
#endif
//#ifdef _GAMESERVER
//
//#endif


void CDnFreezingExplosionBlow::SignalProcess( LOCAL_TIME LocalTime, float fDelta )
{
	// 실제 액션을 실행하지 않고 시그널들만 실행시키도록 한다.
	float fFPS = 60.0f;
	float fPrevFrame = m_fFrame;
	m_fFrame += (fDelta * fFPS);

	//if(m_pActionElement->dwLength < (DWORD)m_fFrame )
	if (m_ActionDurationTime < (DWORD)m_fFrame)
	{
		m_fFrame = 1.0f;
		fPrevFrame = -0.1f;

		// 액션을 다 실행했다면 더 이상 시그널 처리를 하지 않고 상태효과가 끝난다. 
		// 액션 길이 만큼 지속시간을 설정했으므로 곧 상태효과 종료됨.
		return;
	}

	if( 0.0f == fPrevFrame )
		fPrevFrame = -(m_fFrame * 0.1f);

	CEtActionSignal *pSignal = NULL;

#if defined(PRE_FIX_55378)
	m_hActor->SetChargerAction(FREEZING_EXPLOSION_ACTION_NAME);
#endif // PRE_FIX_55378

#if defined(PRE_FIX_62309)
#if defined(_GAMESERVER)
	//#62309
	//시그널 프로세서 돌면서 OnSignal로 Hit시 속성이 설정되지 않을 수 있다.
	//시그널 프로세서 호출전에 강제속성을 설정 해 놓고, Hit시그널 처리시에 설정된 속성값으로
	//강제설정 하도록 한다..
	m_hActor->PushForceHitElement(m_ParentSkillInfo.eSkillElement);
#endif // _GAMESERVER
#endif // PRE_FIX_62309

	//for( DWORD i=0; i < m_pActionElement->pVecSignalList.size(); i++ )
	for( DWORD i=0; i < m_vlpSignalList.size(); i++ )
	{
		//pSignal = m_pActionElement->pVecSignalList[i];
		pSignal = m_vlpSignalList[i];
		if( pSignal->CheckSignal( fPrevFrame, m_fFrame ) == true ) 
		{
#ifdef _GAMESERVER
			if( STE_Hit == pSignal->GetSignalIndex() )
				m_bFreezingHitAction = true;
#endif

			LOCAL_TIME StartTime = LocalTime - (LOCAL_TIME)( 1000.f / fFPS * ( m_fFrame - pSignal->GetStartFrame() ) );
			LOCAL_TIME EndTime = LocalTime + (LOCAL_TIME)( 1000.f / fFPS * ( pSignal->GetEndFrame() - m_fFrame ) );

			m_hActor->OnSignal( (SignalTypeEnum)pSignal->GetSignalIndex(), pSignal->GetData(), LocalTime, 
				StartTime, EndTime, pSignal->GetSignalListArrayIndex() );

#ifdef _GAMESERVER
			if( m_bFreezingHitAction )
				m_bFreezingHitAction = false;
#endif
		}
	}

#if defined(PRE_FIX_62309)
#if defined(_GAMESERVER)
	m_hActor->PopForceHitElement();
#endif // _GAMESERVER
#endif // PRE_FIX_62309

#if defined(PRE_FIX_55378)
	m_hActor->SetChargerAction(NULL);
#endif // PRE_FIX_55378
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFreezingExplosionBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnFreezingExplosionBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//값 계산
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW