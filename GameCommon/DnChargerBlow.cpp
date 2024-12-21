#include "StdAfx.h"
#include "DnChargerBlow.h"
#include "DnActionBase.h"
#include "EtActionSignal.h"
#include "DnProjectile.h"
#include "DnActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnChargerBlow::CDnChargerBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
//#ifdef _GAMESERVER
																			  ,m_pActionElement( NULL ),
																			  m_fFrame( 0.0f ),
																			  m_iAdditionalWeaponLength( 0 )
//#endif
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_062;

	// 인자값은 시전자의 차져로 공격할 액션 이름
	_ASSERT( strlen(szValue) < STATE_BLOW_ARGUMENT_MAX_SIZE );
	SetValue( szValue );

//#ifdef _GAMESERVER
	// 추후에 인자가 복수개가 될 경우 수정해야함.
	m_strChargerActionName = szValue;
//#endif

#if defined(PRE_FIX_65287)
	m_fShooterFinalDamageRate = 0.0f;
#endif // PRE_FIX_65287
}

CDnChargerBlow::~CDnChargerBlow(void)
{

}

void CDnChargerBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// 액션 받아옴.
//#ifdef _GAMESERVER
	if( !m_ParentSkillInfo.hSkillUser )
	{		
		SetState( STATE_BLOW::STATE_END );
		OutputDebug( "CDnChargerBlow::OnBegin fail!! SkillUser is Invalid!!\n" );
		return;
	}
		
	m_pActionElement = NULL;
	m_pActionElement = m_ParentSkillInfo.hSkillUser->GetElement( m_strChargerActionName.c_str() );
	OutputDebug("%s %s", __FUNCTION__, m_strChargerActionName.c_str());

	_ASSERT( m_pActionElement && "차져 액션 정보를 찾을 수 없음." );

	// 스킬 사용할 때의 추가 사거리를 받아온다.
	DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->GetProcessSkill();
	if( hParentSkill )
		m_iAdditionalWeaponLength = hParentSkill->GetIncreaseRange();

	if( m_pActionElement )
	{
		m_vlpSignalList = m_pActionElement->pVecSignalList;

#ifdef _GAMESERVER
		if( m_ParentSkillInfo.hSkillUser )
		{
			m_pShooterStateSnapshot.reset( new CDnState );
			*m_pShooterStateSnapshot= *static_cast<CDnState*>( m_ParentSkillInfo.hSkillUser.GetPointer() );

#if defined(PRE_FIX_65287)

			m_fShooterFinalDamageRate = 0.0f;
			if (m_ParentSkillInfo.hSkillUser && m_ParentSkillInfo.hSkillUser->IsAppliedThisStateBlow(STATE_BLOW::BLOW_050))
			{
				DNVector(DnBlowHandle) vlhBlows;
				m_ParentSkillInfo.hSkillUser->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_050, vlhBlows );
				int iNumBlow = (int)vlhBlows.size();
				for( int i = 0; i < iNumBlow; ++i )
				{
					m_fShooterFinalDamageRate += vlhBlows[i]->GetFloatValue();
				}
			}
#endif // PRE_FIX_65287
		}
#endif

		// 바로 종료가 될 수 있으므로 정상 상태일때만 추가
		if (STATE_BLOW::STATE_BEGIN == GetBlowState())
			m_hActor->AddStateBlowSignal(this->GetMySmartPtr());

		OutputDebug( "CDnChargerBlow::OnBegin\n" );
	}
	else
	{
		SetState( STATE_BLOW::STATE_END );
		OutputDebug( "CDnChargerBlow::OnBegin fail!! Can't find Charger action element!!\n" );
	}
//#else
//	OutputDebug( "CDnChargerBlow::OnBegin\n" );
//#endif
}


void CDnChargerBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
 
}


void CDnChargerBlow::OnProjectileSignal( CDnProjectile* pProjectile )
{
	// 스킬 사거리 추가.
	pProjectile->SetWeaponLength( pProjectile->GetWeaponLength() + m_iAdditionalWeaponLength );

	// 발사체에 차져 상태효과로 인해 나갔다는 것을 표시.
	// 이 녀석으로 공격이 되더라도 플레이어의 콤보 카운트에서는 제외시킨다. (#11329)
#ifdef _GAMESERVER
	pProjectile->FromCharger();
	pProjectile->FromSkill( true );
	pProjectile->SetShooterStateSnapshot( m_pShooterStateSnapshot );
	pProjectile->SetForceHitElement( m_ParentSkillInfo.eSkillElement );

#if defined(PRE_FIX_65287)
	pProjectile->SetShooterFinalDamageRate(m_fShooterFinalDamageRate);
#endif // PRE_FIX_65287
#endif
}


void CDnChargerBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	m_hActor->RemoveStateBlowSignal(this->GetMySmartPtr());

	OutputDebug( "CDnChargerBlow::OnEnd\n" );
}

void CDnChargerBlow::SignalProcess( LOCAL_TIME LocalTime, float fDelta )
{
//#ifdef _GAMESERVER
	if( m_ParentSkillInfo.hSkillUser &&
		m_hActor )
	{
		// 이 상태효과가 발동중인 액터의 위치로 위치만 바꿔서 시그널 처리
		if( m_pActionElement )
		{
			//float fFPS = m_ParentSkillInfo.hSkillUser->CDnActionBase::GetFPS();
			float fFPS = 60.0f;			// 프레임 변경 없이 디폴트 프레임으로 차져 액션 시그널이 처리 되도록 한다. #26439

			// [2011/03/07 semozz]
			// fDelta가 1/60(0.01666)보다 작을때
			// m_fFrame값이 1보다 작아짐..
			// CheckSignal에서 첫번째 if문에서 걸리고, fCur가 0.0가 아니어서
			// 첫프레임에 있는 시그널들이 처리가 안될 수 있음.
			// 그래서 frame증가는 마지막으로 미루고, fPrevFrame값은 m_fFrame에서 조금 뺀값을 설정한다..
			// cur가 Frame [S, E]사이에 있을때, cur/pre비교를 int로 캐스팅해서 크기 비교를 해서 같고 cur가 0.0이 아니면 실패..
			// cur/pre int캐스팅 비교시 같지 않도록 1.0 차이가 나도록 수정한다..
			float fPrevFrame = m_fFrame - 1.0f;

			//float fPrevFrame = m_fFrame;
			//m_fFrame += (fDelta * fFPS);

			if( m_pActionElement->dwLength < (DWORD)m_fFrame )
			{
				m_fFrame = 1.0f;
				fPrevFrame = -0.1f;
			}

			//if( 0.0f == fPrevFrame )
			//	fPrevFrame = -(m_fFrame * 0.1f);

			// 위치만 현재 액터로 바꿔서 시그널돌리고 다시 복구시킴.
			MatrixEx OriginalCross = *(m_ParentSkillInfo.hSkillUser->GetMatEx());
			MatrixEx ChargerActorCross = *(m_hActor->GetMatEx());
			*(m_ParentSkillInfo.hSkillUser->GetMatEx()) = ChargerActorCross;
			m_ParentSkillInfo.hSkillUser->SetOnSignalFromChargerSE( true );
			m_ParentSkillInfo.hSkillUser->SetChargerDestActor( m_hActor );
#ifdef _GAMESERVER
			m_ParentSkillInfo.hSkillUser->SetChargerBlow( GetMySmartPtr() );
			m_ParentSkillInfo.hSkillUser->PushForceHitElement( m_ParentSkillInfo.eSkillElement );
#endif

			CEtActionSignal *pSignal = NULL;
			
#if defined(PRE_FIX_55378)
			m_ParentSkillInfo.hSkillUser->SetChargerAction(m_strChargerActionName.c_str());
#endif // PRE_FIX_55378

			for( DWORD i=0; i < m_vlpSignalList.size(); i++ )
			{
				pSignal = m_vlpSignalList[i];
				if( pSignal->CheckSignal( fPrevFrame, m_fFrame ) == true ) 
				{
					LOCAL_TIME StartTime = LocalTime - (LOCAL_TIME)( 1000.f / fFPS * ( m_fFrame - pSignal->GetStartFrame() ) );
					LOCAL_TIME EndTime = LocalTime + (LOCAL_TIME)( 1000.f / fFPS * ( pSignal->GetEndFrame() - m_fFrame ) );

					m_ParentSkillInfo.hSkillUser->OnSignal( (SignalTypeEnum)pSignal->GetSignalIndex(), pSignal->GetData(), LocalTime, 
						StartTime, EndTime, pSignal->GetSignalListArrayIndex() );
				}

				//////////////////////////////////////////////////////////////////////////
				//OutputDebug("%s, Signal start=%d, preFrame=%f, curFrame=%f, end=%d ====> %s\n", __FUNCTION__, pSignal->GetStartFrame(), fPrevFrame, m_fFrame, pSignal->GetEndFrame(), (isCheckSignal ? "true" : "false"));
				//////////////////////////////////////////////////////////////////////////
			}

#if defined(PRE_FIX_55378)
			m_ParentSkillInfo.hSkillUser->SetChargerAction(NULL);
#endif // PRE_FIX_55378

			*(m_ParentSkillInfo.hSkillUser->GetMatEx()) = OriginalCross;

			m_ParentSkillInfo.hSkillUser->SetOnSignalFromChargerSE( false );
			DnActorHandle hNullActor;
			m_ParentSkillInfo.hSkillUser->SetChargerDestActor( hNullActor );

#ifdef _GAMESERVER
			DnBlowHandle hNullBlow;
			m_ParentSkillInfo.hSkillUser->SetChargerBlow( hNullBlow );
			m_ParentSkillInfo.hSkillUser->PopForceHitElement();
#endif


			//프레임 증가는 나중으로..
			m_fFrame += (fDelta * fFPS);
		}
	}
	//#endif
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChargerBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnChargerBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
