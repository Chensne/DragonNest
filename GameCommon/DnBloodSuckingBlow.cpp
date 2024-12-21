#include "StdAfx.h"
#include "DnBloodSuckingBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnBloodSuckingBlow::CDnBloodSuckingBlow(DnActorHandle hActor, const char *szValue)
	: CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_227;
	SetValue(szValue);
	
	m_fValue = (float)atof(szValue);

#ifdef _GAMESERVER
	AddCallBackType( SB_ONCALCDAMAGE );
#endif

}

CDnBloodSuckingBlow::~CDnBloodSuckingBlow(void)
{
}

void CDnBloodSuckingBlow::OnSetParentSkillInfo()
{

#if defined( _GAMESERVER )
	CalcHealValueLimit( m_StateBlow.emBlowIndex, m_fValue );
#endif

}

void CDnBloodSuckingBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}

void CDnBloodSuckingBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_hActor->IsDie() )
	{
		SetState( STATE_BLOW::STATE_END );
		return;
	}
}

void CDnBloodSuckingBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug( "%s\n", __FUNCTION__ );
}

#ifdef _GAMESERVER
float CDnBloodSuckingBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	//���� ��/�ٸ� �� ��� �����Ѵ�.
	if (m_ParentSkillInfo.hSkillUser)
	{
		//�ٸ� ���� ���
		if (m_ParentSkillInfo.hSkillUser->GetTeam() != m_hActor->GetTeam() &&
			fOriginalDamage > 0.0f )
		{
			//�������� %��ŭ..
			INT64 iDelta = INT64(fOriginalDamage * m_fValue);

			//���� �������� %��ŭ�� �ڽ�(��ų �����)�� HP�� ȸ���մϴ�.
			INT64 iSkillUserHP = m_ParentSkillInfo.hSkillUser->GetHP();
			INT64 iSkillUserMaxHP = m_ParentSkillInfo.hSkillUser->GetMaxHP();

			//��ų ����� maxHP�� ���� �ʵ��� ���� �Ѵ�.
			INT64 iResult = iSkillUserHP + iDelta;
			if( iSkillUserMaxHP < iResult )
			{
				iResult = iSkillUserMaxHP;
				iDelta = iSkillUserMaxHP - iSkillUserHP;
			}
			
			m_ParentSkillInfo.hSkillUser->SetHP(iResult);
			m_ParentSkillInfo.hSkillUser->RequestHPMPDelta( CDnState::ElementEnum_Amount, iDelta, m_ParentSkillInfo.hSkillUser->GetUniqueID() );

			OutputDebug( "%s ��ų ����� HP���� ActorID: %d : %d + %d = %d\n", __FUNCTION__, m_ParentSkillInfo.hSkillUser->GetUniqueID(), iSkillUserHP, iDelta, iResult );
		}
	}

	return 0.0f;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnBloodSuckingBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnBloodSuckingBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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