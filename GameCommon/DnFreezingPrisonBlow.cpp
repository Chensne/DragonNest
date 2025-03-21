#include "StdAfx.h"
#include "DnFreezingPrisonBlow.h"
#include "DnHighLanderBlow.h"
#include "DnCantMoveBlow.h"
#include "DnCantActionBlow.h"
#include "DnFrameStopBlow.h"

#include "EtActionBase.h"
#include "EtActionSignal.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

const int FREEZING_PRISON_INTERVAL_TIME = 1000;


CDnFreezingPrisonBlow::CDnFreezingPrisonBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																							m_pCantMoveBlow( new CDnCantMoveBlow(hActor, NULL ) ),
																							m_pCantActionBlow( new CDnCantActionBlow(hActor, NULL) ),
																							m_pFrameStopBlow( new CDnFrameStopBlow(hActor, NULL) ),
																							m_IntervalChecker( hActor, GetMySmartPtr() ),
																							m_fIntervalDamage( 0.0f ),
																							m_iDurability( 0 ),
																							m_bDurabilityDestroyed( false )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_149;

	SetValue( szValue );
	
	m_iFinalDamage = 0;
	m_iOriginalTeam = -1;

	// 인자가 총 3개임.
	// 내구도, 지속시간 다 되어 폭발시 데미지, 초당 데미지
	string strValue( szValue );
	
	// 스트링을 공백단위로 분할한 후
	std::vector<string> vlTokens;
	TokenizeA( strValue, vlTokens, ";" );

	bool bValidArgument = (3 == (int)vlTokens.size());
	_ASSERT( bValidArgument && "얼음 감옥 상태효과 인자 셋팅이 잘못되었습니다." );

	if( bValidArgument )
	{
		string strDurability = vlTokens.at( 0 );
		string strFinalDamage = vlTokens.at( 1 );
		string strIntervalDamage = vlTokens.at( 2 );

		m_iDurability = atoi( strDurability.c_str() );
		m_iFinalDamage = atoi( strFinalDamage.c_str() );
		m_fIntervalDamage = (float)atoi( strIntervalDamage.c_str() );

#ifdef _GAMESERVER
		AddCallBackType( SB_ONDEFENSEATTACK );
		AddCallBackType( SB_ONCALCDAMAGE );
#endif

		m_pCantActionBlow->SetActionWhenCancelAttack( "Freezing" );
	}

	m_fOrigWight = 0.0f;
	m_iOrigPressLevel = 0;
	m_iMaxDurability = m_iDurability;

	// [2010/12/13 semozz]
	// 상태효과가 상태를 가지고 있는 경우 계속 유지를 위해서
	if (m_pCantMoveBlow)
		m_pCantMoveBlow->SetPermanent(true);
	if (m_pCantActionBlow)
		m_pCantActionBlow->SetPermanent(true);
	if (m_pFrameStopBlow)
		m_pFrameStopBlow->SetPermanent(true);
}

CDnFreezingPrisonBlow::~CDnFreezingPrisonBlow(void)
{
	SAFE_DELETE( m_pCantMoveBlow );
	SAFE_DELETE( m_pCantActionBlow );
	SAFE_DELETE( m_pFrameStopBlow );
}

void CDnFreezingPrisonBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	m_IntervalChecker.OnBegin( LocalTime, FREEZING_PRISON_INTERVAL_TIME );

	// 행동불가, 이동불가
	if( m_pCantMoveBlow ) 
		m_pCantMoveBlow->OnBegin( LocalTime, fDelta );

	if( m_pCantActionBlow ) 
		m_pCantActionBlow->OnBegin( LocalTime, fDelta );

	if( m_pFrameStopBlow )
		m_pFrameStopBlow->OnBegin( LocalTime, fDelta );


#if defined(PRE_FIX_50482)
	if (m_hActor && m_hActor->GetChangeTeamRefCount() == 0)
	{
		// 팀을 이 스킬을 쓴 액터의(현재는 몬스터) 팀으로 바꿔준다.
		m_iOriginalTeam = m_hActor->GetTeam();
		//원래 팀이 어디 였는지 설정 해 놓는다.
		m_hActor->SetOriginalTeam(m_iOriginalTeam);
	}
	m_hActor->AddChangeTeamRefCount();

	m_hActor->SetTeam( m_ParentSkillInfo.iSkillUserTeam );
#else
	// 팀을 이 스킬을 쓴 액터의(현재는 몬스터) 팀으로 바꿔준다.
	m_iOriginalTeam = m_hActor->GetTeam();
	m_hActor->SetTeam( m_ParentSkillInfo.iSkillUserTeam );

	//원래 팀이 어디 였는지 설정 해 놓는다.
	m_hActor->SetOriginalTeam(m_iOriginalTeam);
#endif // PRE_FIX_50482
	
#ifndef _GAMESERVER
	_AttachGraphicEffect();

	// 처음엔 idle 액션을 실행해준다.
	if( m_hEtcObjectEffect )
		m_hEtcObjectEffect->SetActionQueue( "Idle" );
#endif

	//시작될때 Weight/PressLevel을 저장해 놓는다.
	m_fOrigWight = m_hActor->GetWeight();
	m_iOrigPressLevel = m_hActor->GetPressLevel();

	//캐릭터 밀림을 방지 하기 위해 설정.
	m_hActor->SetWeight(0.0f);
	m_hActor->SetPressLevel(-1);

#if defined(_GAMESERVER)
	//StateBlow가 시작될때 UI 표시
	m_hActor->CmdFreezingPrisonDurablity(GetBlowID(), 100.0f, true);
#endif

	OutputDebug( "CDnFreezingPrisonBlow::OnBegin\n" );
}

#ifdef _GAMESERVER
void CDnFreezingPrisonBlow::_RequestDamage( float fDamage )
{
#if defined(PRE_FIX_44884)
#if defined(PRE_FIX_61382)
	DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
	if (hActor &&
		hActor->IsDie() == false)
	{
		m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)fDamage);

		//#48491 타이머로 데미지 적용시 ActorStatIntervlaManipulator에서 Die호출이 되고 있음.
		//여기서는 Die호출이 필요 없다.
		//if (m_hActor->IsDie())
		//	m_hActor->Die(m_ParentSkillInfo.hSkillUser);
	}
#else
	if (m_hActor->IsDie() == false)
	{
		m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)fDamage);

		//#48491 타이머로 데미지 적용시 ActorStatIntervlaManipulator에서 Die호출이 되고 있음.
		//여기서는 Die호출이 필요 없다.
		//if (m_hActor->IsDie())
		//	m_hActor->Die(m_ParentSkillInfo.hSkillUser);
	}
#endif // PRE_FIX_61382
#else
	// 하이랜더 상태효과가 있는 경우 죽지 않아야 한다.
	float fResultDamage = fDamage;
	if( m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_143 ) )
	{
		// IsAppliedThisStateBlow() 함수에선 true 가 리턴되지만 지속시간이 다 된
		// 상태효과는 GatherAppliedStateBlowByBlowIndex() 에서 얻어와지지 않으므로 리스트가 비어있을 수 도 있다.
		DNVector( DnBlowHandle ) vlhHighLanderBlow;
		m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_143, vlhHighLanderBlow );
		if( 0 < (int)vlhHighLanderBlow.size() )
		{
			CDnHighlanderBlow* pHighlanderBlow = static_cast<CDnHighlanderBlow*>(vlhHighLanderBlow.front().GetPointer());
			fResultDamage += pHighlanderBlow->CalcDamage( fResultDamage );
		}
	}

	DWORD dwHitterUniqueID = m_ParentSkillInfo.hSkillUser ? m_ParentSkillInfo.hSkillUser->GetUniqueID() : -1;
	m_hActor->SetHP( m_hActor->GetHP()-(INT64)fResultDamage );
	m_hActor->RequestHPMPDelta( m_ParentSkillInfo.eSkillElement, -(INT64)fResultDamage, dwHitterUniqueID );
#endif // PRE_FIX_44884
}
#endif

bool CDnFreezingPrisonBlow::OnCustomIntervalProcess( void )
{
#ifdef _GAMESERVER
	if( 0.0f < m_fIntervalDamage )
	{
		_RequestDamage( m_fIntervalDamage );
	}
#else
	//if( m_hEtcObjectEffect )
	//{	
	//	CEtActionBase::ActionElementStruct* pStruct = m_hEtcObjectEffect->GetElement( "Idle" );
	//	if( pStruct )
	//	{
	//		m_fEffectLength = (float)pStruct->dwLength / 1000.0f;
	//	}

	//	_SetDiffuse( 0.0f, 1.0f, 0.0f, 1.0f );
	//	m_bEffecting = true;
	//}
#endif

	return true;
}



void CDnFreezingPrisonBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

	m_IntervalChecker.Process( LocalTime, fDelta );

	if( m_pCantMoveBlow ) 
		m_pCantMoveBlow->Process( LocalTime, fDelta );

	if( m_pCantActionBlow ) 
		m_pCantActionBlow->Process( LocalTime, fDelta );

	if( m_pFrameStopBlow )
		m_pFrameStopBlow->Process( LocalTime, fDelta );
}


void CDnFreezingPrisonBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifndef _GAMESERVER
	//_SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );

	// 이펙트 삭제. 이미 사라졌다해도 괜찮다.
	//_DetachGraphicEffect();
#endif

	if( m_pCantMoveBlow ) 
		m_pCantMoveBlow->OnEnd( LocalTime, fDelta );

	if( m_pCantActionBlow ) 
		m_pCantActionBlow->OnEnd( LocalTime, fDelta );

	if( m_pFrameStopBlow )
		m_pFrameStopBlow->OnEnd( LocalTime, fDelta );

	// 이렇게 해야 곧바로 경직이 풀린다.
	m_hActor->SetStiffDelta( 0.001f );

#ifdef _GAMESERVER
	// 지속시간 다 되어 종료되는 것이라면 지정된 데미지를 준다.
	if( false == m_bDurabilityDestroyed )
	{
		_RequestDamage( (float)m_iFinalDamage );

		//#48491에서 타이머로 데미지 적용시 ActorStatIntervlaManipulator에서 Die호출이 되고 있지만 OnEnd에서는 Die처리 해야함.
#if defined(PRE_FIX_61382)
		DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
		if (hActor && hActor->IsDie())
			hActor->Die(m_ParentSkillInfo.hSkillUser);

#else
		if (m_hActor->IsDie())
			m_hActor->Die(m_ParentSkillInfo.hSkillUser);
#endif // PRE_FIX_61382
	}
#else

	if (m_hActor->IsDie())
	{
		// 캐릭터가 죽었다면 자연스럽게 Effect 사라지도록..
		if( m_hEtcObjectEffect )
			m_hEtcObjectEffect->SetActionQueue( "Summon_Off" );

		OutputDebug("SetActionQueue ---> Summon_Off\n");
	}
	else
	{
		// 클라이언트에서는 지속시간이 남아있는데 서버에서 삭제하라고 지속시간이 끝나기 전에 패킷이 온 경우
		// 내구도가 다 닳아 없어진 거고 지속 시간 다 되어 끝나는 것은 폭발하는 것이므로 액션을 다르게 보여준다.
		if( IsEnd() )
		{
			// 이펙트 객체에 폭발 액션 실행.
			if( m_hEtcObjectEffect )
				m_hEtcObjectEffect->SetActionQueue( "Attack" );

			OutputDebug("SetActionQueue ---> Attack\n");
		}
		else
		{
			// 이펙트 객체의 내구도가 다 되어 사라지는 액션 실행.
			if( m_hEtcObjectEffect )
				m_hEtcObjectEffect->SetActionQueue( "Break" );

			OutputDebug("SetActionQueue ---> Break\n");
		}
	}

	// 깨지는 액션 실행될 때는 링크 끊어줌
	TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::SmartPtrSignalImpStruct *pStruct = 
		(TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::SmartPtrSignalImpStruct *)m_hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetStruct( STATEBLOWEFFECT_ETCOFFSET + m_StateBlow.emBlowIndex, GetBlowID() );
	if( pStruct ) 
		pStruct->bLinkObject = false;

	// 효과 삭제... 실제로 이펙트를 없애는 건 아니고, 이펙트 자체는 destroy 시그널로 알아서 죽는다.
	// 하지만 상태효과는 이 시점에서 끝이므로 이펙트를 추가할 때 판단 근거가 되는 스킬 정보를 여기서 지워줘야 한다.
	// 그렇지 않으면 스킬 정보 계속 남아있어서 이펙트가 추가가 안됨.
	m_hActor->DetachSEEffectSkillInfo( m_ParentSkillInfo );
#endif

	m_IntervalChecker.OnEnd( LocalTime, fDelta );


	//상태가 끝날때 기존에 저장해놓은 값으로 복원
	m_hActor->SetWeight(m_fOrigWight);
	m_hActor->SetPressLevel(m_iOrigPressLevel);

#if defined(_GAMESERVER)
	//StateBlow가 끝날때 UI를 없애야함.
	m_hActor->CmdFreezingPrisonDurablity(GetBlowID(), 0.0f, false);
#endif

#if defined(PRE_FIX_50482)
	if (m_hActor && m_hActor->GetChangeTeamRefCount() == 1)
	{
		m_hActor->SetTeam(m_iOriginalTeam);
	}
	m_hActor->RemoveChangeTeamRefCount();
#else
	// 팀을 이 스킬을 쓴 액터의(현재는 몬스터) 팀으로 바꿔준다.
	m_hActor->SetTeam( m_iOriginalTeam );
#endif // PRE_FIX_50482

	OutputDebug( "CDnFreezingPrisonBlow::OnEnd\n" );
}

#if !defined(_GAMESERVER)
void CDnFreezingPrisonBlow::OnHit()
{
	if (m_hEtcObjectEffect)
		m_hEtcObjectEffect->SetActionQueue( "Hit" );
}

//Guage액션에서 Action시그널 정보를 얻어 Offset정보를 확인한다.
EtVector3 CDnFreezingPrisonBlow::GetGaugePos()
{
	// m_hEtcObjectEffect가 유효하지 않을 수 있다. [2010/12/15 semozz]
	EtVector3 vPos = EtVector3(0.0f, 0.0f, 0.0f);

	if (m_hEtcObjectEffect)
	{
		vPos = *m_hEtcObjectEffect->GetPosition();

		//여기에는 Hit Signal만 있어야 한다..
		CEtActionBase::ActionElementStruct* pActionElement = m_hEtcObjectEffect->GetElement("Gauge");
		if (pActionElement)
		{
			if (pActionElement->pVecSignalList.size() != 1)
			{
				OutputDebug("Gauge 정보에 Hit Signal이 존재 하지 않음.. !!!\n");
			}
			else
			{
				CEtActionSignal *pSignal = pActionElement->pVecSignalList[0];
				HitStruct *pStruct = pSignal ? (HitStruct *)pSignal->GetData() : NULL;

				if (pStruct)
					vPos += *pStruct->vOffset;
			}
		}

	}

	return vPos;
}
#endif


#ifdef _GAMESERVER

bool CDnFreezingPrisonBlow::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	// 피격시 공중에 뜨지 못하는 하는 설정?을 위해
	HitParam.vResistance = EtVector3( 0.0f, 0.0f, 0.0f );
	HitParam.vVelocity = EtVector3( 0.0f, 0.0f, 0.0f );
	HitParam.vViewVec = *(m_hActor->GetLookDir());

	return false;
}

float CDnFreezingPrisonBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	// 히트 시그널에 기록된 내구도 깍는 정보를 그대로 사용.
	m_iDurability -= (int)(HitParam.fDurability * 100.0f);
	
	//내구도가 떨어질때 마다 피격 이펙트 표시 & 내구도 게이지 갱신
	float fCurrentValue = min((float)m_iMaxDurability, max(0.0f, (float)m_iDurability));
	float fRate = m_iMaxDurability != 0 ? (fCurrentValue / (float)m_iMaxDurability) : 0.0f;
	
	// 여기서 false로 보내 주면 UI가 먼저 사라지고 이펙트는 OnEnd에서 사라져
	// 동기화에 문제가 있음. 그래서 여기에서는 bShowGauge값을 항상 true로만 보낸다.
	// OnEnd에서 UI 사라지게 하는 함수 호출..
	bool bShowGauge = true;//fRate > 0.0f;
	m_hActor->CmdFreezingPrisonDurablity(GetBlowID(), fRate, bShowGauge);

	//내구도 0 이하이면
	if( m_iDurability <= 0 )
	{
		// 내구도 다 되어 부서졌으므로 아무 일 없이 상태효과 종료 시킨다.
		m_bDurabilityDestroyed = true;
		
		// CDnStateBlow::Process() 함수가 종료되면 곧바로 여기서 예약 걸어놨던 상태효과들이 제거된다.
		// 클라쪽에도 패킷으로 보내줌.
		m_hActor->AddReserveRemoveBlow( GetMySmartPtr() );
		OutputDebug( "CDnFreezingBlow::OnCalcDamage - Freezing Broken by Attack!\n" );
	}

	return -fOriginalDamage;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFreezingPrisonBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	int iDurability[2] = {0, };
	int iFinalDamage[2] = {0, };
	float fIntervalDamage[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) 
	{
		iDurability[0] = atoi( vlTokens[0][0].c_str() );
		iFinalDamage[0] = atoi( vlTokens[0][1].c_str() );
		fIntervalDamage[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) 
	{
		iDurability[1] = atoi( vlTokens[1][0].c_str() );
		iFinalDamage[1] = atoi( vlTokens[1][1].c_str() );
		fIntervalDamage[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	int iResultDurability = iDurability[0] + iDurability[1];
	int iResultFinalDamage = iFinalDamage[0] + iFinalDamage[1];
	float fResultIntervalDamage = fIntervalDamage[0] + fIntervalDamage[1];

	sprintf_s(szBuff, "%d;%d;%f", iResultDurability, iResultFinalDamage, fResultIntervalDamage);

	szNewValue = szBuff;
}

void CDnFreezingPrisonBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	int iDurability[2] = {0, };
	int iFinalDamage[2] = {0, };
	float fIntervalDamage[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) 
	{
		iDurability[0] = atoi( vlTokens[0][0].c_str() );
		iFinalDamage[0] = atoi( vlTokens[0][1].c_str() );
		fIntervalDamage[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) 
	{
		iDurability[1] = atoi( vlTokens[1][0].c_str() );
		iFinalDamage[1] = atoi( vlTokens[1][1].c_str() );
		fIntervalDamage[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	int iResultDurability = iDurability[0] - iDurability[1];
	int iResultFinalDamage = iFinalDamage[0] - iFinalDamage[1];
	float fResultIntervalDamage = fIntervalDamage[0] - fIntervalDamage[1];

	sprintf_s(szBuff, "%d;%d;%f", iResultDurability, iResultFinalDamage, fResultIntervalDamage);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW