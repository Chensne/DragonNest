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

	// �ð��� 2���� ����Ѵ�. ���ڷ� hp 1�� ������ �ð��� �޴´�.
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

				// �� Ÿ���� �޾Ƽ� hp �� 0 ���ϰ� �Ǿ� �װ� �ȴٸ� �������� 1�� ���� �������ش�.
				// �׸��� ���̷��� ���·� �ٲ���.
				if( (iNowHP - (int)fOriginalDamage) <= 0 )
				{
					//fOriginalDamage = float(iNowHP - 1);
					fResult = float(iNowHP - 1)-fOriginalDamage;

					// ���¸� ���̷����� �ٲ��ְ�, ����ȿ�� �ð��� 
					// ���̷��� �����϶� �ð����� �������ش�.
					m_iState = HIGHLANDER;
					m_StateBlow.fDurationTime = m_fValue;

					// Ŭ���̾�Ʈ���Ե� �� ����ȿ���� ���ӽð��� ��� ������Ʈ �� ������ �ٲ��ش�.
					m_hActor->CmdModifyStateEffect( GetBlowID(), m_StateBlow );
				}

			}
			break;

		case HIGHLANDER:
			{
				// ���̷��� ���¿����� ������ �ð��� �� �Ǿ� ����ȿ���� ���ӵ� ������ ���� �ʴ´�.
				// ���̳� ���ర�� �ɷ�  HP �� 1 �̻��� �Ǿ��ٸ� �ٽ� 1 ������ HP ������ �������� ���.
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
	// #25781 CanHit ���� ��Ʈ �ñ׳��� ��� ���̷����� ���õȴ�.
	if( HitParam.bIgnoreCanHit )
		return 0.0f;

	return CalcDamage( fOriginalDamage );
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnHighlanderBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnHighlanderBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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