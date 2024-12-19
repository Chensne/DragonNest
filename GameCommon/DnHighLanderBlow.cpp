#include "StdAfx.h"
#include "DnHighlanderBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

// dnactor.cpp
#ifdef _GAMESERVER
extern int GetGaussianRandom( int nMin, int nMax, CMultiRoom *pRoom );
#endif

CDnHighlanderBlow::CDnHighlanderBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_143;

	// 시간만 2개로 사용한다. 인자로 hp 1인 상태의 시간을 받는다.
	SetValue( szValue );
	m_fValue = (float)atof( szValue );
	
	m_iState = NORMAL;

	AddCallBackType( SB_ONCALCDAMAGE );
}

CDnHighlanderBlow::~CDnHighlanderBlow(void)
{

}

void CDnHighlanderBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	m_iState = NORMAL;

	OutputDebug( "CDnHighlanderBlow::OnBegin Value:%2.2f\n", m_fValue );
}


void CDnHighlanderBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnHighlanderBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

	OutputDebug( "CDnHighlanderBlow::OnEnd Value:%2.2f\n", m_fValue );
}

#ifdef _GAMESERVER
float CDnHighlanderBlow::CalcDamage( float fOriginalDamage )
{
	float fResult = 0.0f;

	switch( m_iState )
	{
		case NORMAL:
			{
				int iNowHP = (int)m_hActor->GetHP();

				// 이 타격을 받아서 hp 가 0 이하가 되어 죽게 된다면 데미지를 1만 남게 수정해준다.
				// 그리고 하이랜더 상태로 바꿔줌.
				if( (iNowHP - (int)fOriginalDamage) <= 0 )
				{
					//fOriginalDamage = float(iNowHP - 1);
					fResult = float(iNowHP - 1)-fOriginalDamage;

					// 상태를 하이랜더로 바꿔주고, 상태효과 시간을 
					// 하이랜더 상태일때 시간으로 셋팅해준다.
					m_iState = HIGHLANDER;
					m_StateBlow.fDurationTime = m_fValue;

					// 클라이언트에게도 이 상태효과의 지속시간을 방금 업데이트 된 것으로 바꿔준다.
					m_hActor->CmdModifyStateEffect( GetBlowID(), m_StateBlow );
				}

			}
			break;

		case HIGHLANDER:
			{
				// 하이랜더 상태에서는 정해진 시간이 다 되어 상태효과가 지속될 때까지 죽지 않는다.
				// 힐이나 물약같은 걸로  HP 가 1 이상이 되었다면 다시 1 까지만 HP 남도록 데미지를 허용.
				int iNowHP = (int)m_hActor->GetHP();
				if( 1 < iNowHP )
				{
					if( (iNowHP - (int)fOriginalDamage) <= 0 )
					{
						//fOriginalDamage = float(iNowHP - 1);
						fResult = float(iNowHP - 1)-fOriginalDamage;
					}
				}
				else
				{
					fResult = -fOriginalDamage;
				}
			}
			break;
	}

	return fResult;
}

float CDnHighlanderBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	// #25781 CanHit 무시 히트 시그널인 경우 하이랜더도 무시된다.
	if( HitParam.bIgnoreCanHit )
		return 0.0f;

	return CalcDamage( fOriginalDamage );
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnHighlanderBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnHighlanderBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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