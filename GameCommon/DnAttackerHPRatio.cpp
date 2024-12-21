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
// hp �� Attacker ���� ä���ָ鼭 �ڽ��� ���۾Ƹ� ó���� ������ ó���� �ؾ��ϹǷ� false �� �����ؼ�
// ������ ó�� ��ƾ ������ ���ְ�, OnCalcDamage �Լ��� �����ε��ؼ� �޾Ҵ� �������� ���ش�. 
bool CDnAttackerHPRatio::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	// ���� �� ���� hp �� ������ ������� ��
	// HP ȸ�� ��Ŷ ����. (�����ϼ��� ����)
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

	// OnCalcDamage ���� �������� 0���� ����� ������ ���۾Ƹ�ó���� �Ǹ� �ڿ����� �ǰ� �׼��� ������ �����Ƿ�
	// ���⼱ �׳� ��������� �ȴ�.
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

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnAttackerHPRatio::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
