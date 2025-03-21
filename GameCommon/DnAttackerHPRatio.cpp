#include "StdAfx.h"
#include "DnAttackerHPRatio.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnAttackerHPRatio::CDnAttackerHPRatio( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_092;
	SetValue( szValue );
	m_fValue = (float)atof( szValue );

#ifdef _GAMESERVER
	AddCallBackType( SB_ONDEFENSEATTACK );
	AddCallBackType( SB_ONCALCDAMAGE );
#endif

}

CDnAttackerHPRatio::~CDnAttackerHPRatio(void)
{

}

void CDnAttackerHPRatio::OnSetParentSkillInfo()
{

#if defined( _GAMESERVER )
	CalcHealValueLimit( m_StateBlow.emBlowIndex, m_fValue );
#endif

}

void CDnAttackerHPRatio::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnAttackerHPRatio::OnBegin \n");
}


void CDnAttackerHPRatio::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnAttackerHPRatio::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnAttackerHPRatio::OnEnd \n");
}

#ifdef _GAMESERVER
// hp 를 Attacker 에게 채워주면서 자신은 슈퍼아머 처리등 데미지 처리를 해야하므로 false 를 리턴해서
// 데미지 처리 루틴 돌도록 해주고, OnCalcDamage 함수를 오버로딩해서 받았던 데미지를 없앤다. 
bool CDnAttackerHPRatio::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	// 때릴 때 마다 hp 를 비율로 적용시켜 줌
	// HP 회복 패킷 보냄. (음수일수도 있음)
	INT64 iMaxHP = hHitter->GetMaxHP();
	INT64 iNowHP = hHitter->GetHP();
	INT64 iDelta = INT64((float)iMaxHP*m_fValue);
	INT64 iResultHP = iNowHP+iDelta;

	if( iResultHP > iMaxHP )
	{
		iDelta = iMaxHP - iNowHP;
		iResultHP = iMaxHP;
	}
	else
	if( iResultHP < 0 )
	{
		iDelta = iDelta - iResultHP;
		iResultHP = 0;
	}

	if( 0 != iDelta )
	{
		hHitter->SetHP( iResultHP );
		hHitter->RequestHPMPDelta( CDnState::ElementEnum_Amount, iDelta, hHitter->GetUniqueID() );
	}

	// OnCalcDamage 에서 데미지를 0으로 만들기 때문에 슈퍼아머처리가 되면 자연스레 피격 액션이 나오지 않으므로
	// 여기선 그냥 흘려보내면 된다.
	//HitParam.szActionName.clear();
	HitParam.vResistance = EtVector3( 0.0f, 0.0f, 0.0f );
	HitParam.vVelocity = EtVector3( 0.0f, 0.0f, 0.0f );
	HitParam.vViewVec = *(m_hActor->GetLookDir());

	return false;
}

float CDnAttackerHPRatio::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	return -fOriginalDamage;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAttackerHPRatio::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnAttackerHPRatio::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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
