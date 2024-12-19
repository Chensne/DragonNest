#include "StdAfx.h"
#include "DnFrameStopBlow.h"

#ifdef _GAMESERVER
// #include "DnGameRoom.h"
// #include "DnGameDataManager.h"
// #include "DnUserSession.h"
#include "DnStateBlow.h"
#else
#include "DnEtcObject.h"
#include "DnLocalPlayerActor.h"
#include "DnInterface.h"
#include "navigationmesh.h"
#include "InputWrapper.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const LOCAL_TIME SHAKE_TIME = 3000;

CDnFrameStopBlow::CDnFrameStopBlow(DnActorHandle hActor, const char *szValue) : CDnBlow(hActor)
#if defined(PRE_FIX_53274)
#ifndef _GAMESERVER
,m_ComboCalc( CDnComboCalculator::CIRCULAR_CHECK )
#endif
#endif // PRE_FIX_53274
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_146;

#if defined(PRE_FIX_53274)
	SetValue(szValue);
	m_fValue = (szValue == NULL ? 0.0f : (float)atof(szValue));

	m_bPlayerCharacter = false;

#ifndef _GAMESERVER
	if (m_fValue != 0)
	{
		std::vector<BYTE> vlKeysToCheck;
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVELEFT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVERIGHT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVELEFT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVERIGHT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVELEFT] );
		vlKeysToCheck.push_back( g_WrappingKeyData[IW_MOVERIGHT] );

		m_ComboCalc.SetKeysToCheck( vlKeysToCheck );

		vlKeysToCheck.clear();
		vlKeysToCheck.push_back( IW_MOVELEFT );		vlKeysToCheck.push_back( IW_MOVELEFT );		vlKeysToCheck.push_back( IW_MOVELEFT );
		vlKeysToCheck.push_back( IW_MOVERIGHT );	vlKeysToCheck.push_back( IW_MOVERIGHT );	vlKeysToCheck.push_back( IW_MOVERIGHT );
		vlKeysToCheck.push_back( IW_MOVEBACK );		vlKeysToCheck.push_back( IW_MOVEBACK );		vlKeysToCheck.push_back( IW_MOVEBACK );
		vlKeysToCheck.push_back( IW_MOVEFRONT );	vlKeysToCheck.push_back( IW_MOVEFRONT );	vlKeysToCheck.push_back( IW_MOVEFRONT );

		m_ComboCalc.SetPadsToCheck( vlKeysToCheck, 3.0f );

		m_bPlayerCharacter = dynamic_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer()) ? true : false;
	}
#endif // _GAMESERVER
#endif // PRE_FIX_53274
}

CDnFrameStopBlow::~CDnFrameStopBlow(void)
{
}

void CDnFrameStopBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	//// 곧바로 플레이 스피드 리셋시키고 정지 시킴. 
	//m_hActor->ResetPlaySpeed();
	//m_hActor->SetPlaySpeed( 1000000, 0.0f );

	m_hActor->AddedFrameStop();

	// #29627 릴리브 사용 가능하게 하기 위해서 hit state 가 아닐 때는 hit state 추가.
	if( m_hActor->IsPlayerActor() )		// 플레이어인 경우만.
	{
		if( false == m_hActor->IsHit() )
		{
			int nState = m_hActor->GetState();
			nState |= CDnActorState::ActorStateEnum::Hit;
			m_hActor->SetState( (CDnActorState::ActorStateEnum)nState );
		}
	}
	//////////////////////////////////////////////////////////////////////////

#if defined(PRE_FIX_53274)
#ifndef _GAMESERVER
	// 플레이어가 걸렸을 경우에 버튼 연타 UI 보여줌
	if( m_fValue != 0.0f && m_bPlayerCharacter )
	{
#if defined(PRE_FIX_57706)
		if (m_hActor->GetStickAniDlgRefCount() == 0)
		{
			GetInterface().ShowStickAniDialog( true );
		}

		m_hActor->AddStickAniDlgRefCount();
#else
		GetInterface().ShowStickAniDialog( true );
#endif // PRE_FIX_57706
	}
#endif // _GAMESERVER
#endif // PRE_FIX_53274

	OutputDebug( "CDnFrameStopBlow::OnBegin, Value:%f \n", m_fValue );
}

#if defined(PRE_FIX_53274)
void CDnFrameStopBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

#ifndef _GAMESERVER
	if (m_fValue != 0.0f)
	{
		if( CDnLocalPlayerActor::s_hLocalActor == m_hActor )
		{
			// TODO: 좌우 입력 받을 때 마다 시간 줄여줌. 결과적으로 서버로도 보내줘야 함...
			int iNumComboCount = 0;
			bool bComboSatisfy = m_ComboCalc.Process( LocalTime, fDelta, &iNumComboCount );

			for( int i = 0; i < iNumComboCount; ++i )
				m_StateBlow.fDurationTime -= 1.0f;

			if( bComboSatisfy )
			{
				OutputDebug( "[위기탈출] %d 회 콤보 성공, 시간 %2.2f 로 줄어듬 \n", iNumComboCount, m_StateBlow.fDurationTime );
			}
		}

		// 자기 자신의 플레이어 캐릭터만 위기 탈출로 시간을 줄일 수 있으므로 체크해서 서버로 상태효과 끝낸다고 보냄.
		if( CDnLocalPlayerActor::s_hLocalActor == m_hActor )
		{
			if( m_StateBlow.fDurationTime < 0.2f )
			{
				// 게임서버로 미리 쏴준다.
#if defined(PRE_FIX_57706)
				//클라이언트에서 상태효과 종료 조건이 확인 됨.. 일단 서버로 패킷 보내고, 서버에서 브로드캐스팅 해줘서 패킷 받아 제거 되도록 수정함..
				//이 클라이언트는 실제 제거는 하지 않고 패킷만 보냄. 시간이 다 되서 자동으로 제거 되거나, 서버에서 패킷 받아서 제거 되도록 한다.
				CDnLocalPlayerActor* pLocalPlayerActor = static_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer());
				if (pLocalPlayerActor)
					pLocalPlayerActor->CmdRemoveStateEffectByServerBlowID(GetServerBlowID());
#else
				m_hActor->CmdRemoveStateEffect( m_StateBlow.emBlowIndex, true );
#endif // PRE_FIX_57706
			}
		}
	}
#endif // _GAMESERVER
}
#endif // PRE_FIX_53274

void CDnFrameStopBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	//m_hActor->ResetPlaySpeed();

	m_hActor->RemovedFrameStop();

#if defined(PRE_FIX_53274)
#ifndef _GAMESERVER
	// 플레이어가 걸렸을 경우에 버튼 연타 UI 보여줌
	if( dynamic_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer()) )
	{
#if defined(PRE_FIX_57706)
		m_hActor->RemoveStickAniDlgRefCount();

		if (m_hActor->GetStickAniDlgRefCount() == 0)
		{
			GetInterface().ShowStickAniDialog( false );
		}
#else
		GetInterface().ShowStickAniDialog( false );
#endif // PRE_FIX_57706
	}
#endif // _GAMESERVER
#endif // PRE_FIX_53274

#if defined(_GAMESERVER)
	if (m_fValue != 0.0f)
	{
		//같은 스킬로 추가된 상태효과들을 찾아서 같이 제거 하도록 한다.
		CDnStateBlow* pStateBlow = m_hActor->GetStateBlow();
		int nStateCount = pStateBlow ? pStateBlow->GetNumStateBlow() : 0;
		for( int iBlow = 0; iBlow < nStateCount; ++iBlow )
		{
			DnBlowHandle hExistingBlow = pStateBlow->GetStateBlow( iBlow );
			CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hExistingBlow->GetParentSkillInfo());

			//스킬이 같고, 같은 스킬 사용자일경우..
			if (pSkillInfo && 
				pSkillInfo->iSkillID == m_ParentSkillInfo.iSkillID && 
				pSkillInfo->hSkillUser &&
				pSkillInfo->hSkillUser == m_ParentSkillInfo.hSkillUser)
			{
				hExistingBlow->SetState(STATE_BLOW::STATE_END);
				m_hActor->CmdRemoveStateEffectFromID(hExistingBlow->GetBlowID());
			}
		}
	}
#endif // _GAMESERVER

	OutputDebug( "CDnFrameStopBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFrameStopBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnFrameStopBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW