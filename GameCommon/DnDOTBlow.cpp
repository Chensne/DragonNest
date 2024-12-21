#include "StdAfx.h"
#include "DnDotBlow.h"
#include "DnSkill.h"
#ifdef _GAMESERVER
#include "DnHighlanderBlow.h"
#endif 

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

const LOCAL_TIME FIRE_DAMAGE_INTERVAL = 2000;


CDnDOTBlow::CDnDOTBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), 
m_IntervalChecker( hActor, GetMySmartPtr() ), 
m_fEffectLength( 0.0f ),
m_bEffecting( false ),
m_IntervalTime(0)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_157;
	SetValue( szValue );
	
	std::string str = szValue;//"damage;IntervalTime";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. ����ȿ�� �ε��� ����Ʈ�� ���� ���� ����
	TokenizeA(str, tokens, delimiters);
	if (2 == tokens.size())
	{
		m_fValue = (float)atof( tokens[0].c_str() );
		m_IntervalTime = atoi(tokens[1].c_str());
	}
	else
	{
		m_fValue = 0.0f;
		m_IntervalTime = 0;
	}

	m_IntervalDamage = 0.0f;
	m_nDamageCount = 0;
	
#ifndef _GAMESERVER
	UseTableDefinedGraphicEffect( false );
#endif
}

CDnDOTBlow::~CDnDOTBlow(void)
{

}

void CDnDOTBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifndef _GAMESERVER
	if( m_pEffectOutputInfo ) {	// ũ���� ���� �߰��մϴ�.. by realgaia
		_ASSERT( EffectOutputInfo::OWNER_HANDLING != m_pEffectOutputInfo->iOutputType );
	}
#else

	// [2011/02/10 semozz]
	// �������� �ش� Actor�� �ִ� HP�� %�� ��´ٴ� �������� ����.
	
	// 1. ����ȿ���� ��ü ���ӽð����� IntervalTime�� �������
	float nCount = m_StateBlow.fDurationTime / (m_IntervalTime * 0.001f);
	
	//������ Ƚ���� ���� �س��´�.
	m_nDamageCount = (int)nCount;

	// 2. ��ü ������(�������� �ִ�/�ּ� ���ݷ��� �߰����� n%)
	
	// ����/���� �������� �ľ�.
	float fTotalDamage = 0.0f;
	CDnDamageBase::SHitParam *pHitparam = m_hActor->GetHitParam();
	switch(pHitparam->cAttackType)
	{
	case 0://���� ����
		{
			if (m_ParentSkillInfo.hSkillUser)
			{
				float fDiff = (float)(m_ParentSkillInfo.hSkillUser->GetAttackPMax() - m_ParentSkillInfo.hSkillUser->GetAttackPMin());
				fTotalDamage = (float)m_ParentSkillInfo.hSkillUser->GetAttackPMin() + (fDiff * 0.5f);
			}
		}
		break;
	case 1://���� ����
		{
			if (m_ParentSkillInfo.hSkillUser)
			{
				float fDiff = (float)(m_ParentSkillInfo.hSkillUser->GetAttackMMax() - m_ParentSkillInfo.hSkillUser->GetAttackMMin());
				fTotalDamage = (float)m_ParentSkillInfo.hSkillUser->GetAttackMMin() + (fDiff * 0.5f);
			}
		}
		break;
	}

	fTotalDamage = fTotalDamage * m_fValue;

	// 3. IntervalTime�� ������ ���
	m_IntervalDamage = fTotalDamage / nCount;

	
	if( m_fValue == 0.0f || m_IntervalTime == 0.0f)
	{
		SetState( STATE_BLOW::STATE_END );
		OutputDebug( "CDnDOTBlow::Damage or IntervalTime was wrong!!\n" );
	}
#endif

	m_IntervalChecker.OnBegin( LocalTime, static_cast<DWORD>(m_IntervalTime) );

#if defined(_GAMESERVER)
	RequestDamage();
#endif // _GAMESERVER

	m_bEffecting = false;
	m_fEffectLength = 0.0f;

#if !defined(_GAMESERVER)
	_AttachGraphicEffect();
#endif // _GAMESERVER

	OutputDebug( "CDnDOTBlow::OnBegin, Value:%2.2f (HP : %d ) \n", m_fValue, m_hActor->GetHP());
}



bool CDnDOTBlow::OnCustomIntervalProcess( void )
{
#ifdef _GAMESERVER
	RequestDamage();
#else
#endif

	return true;
}


void CDnDOTBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
	
	m_IntervalChecker.Process( LocalTime, fDelta );
}


void CDnDOTBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	// LocalTime �� 0���� ȣ��Ǹ� ����, ��øó�� ���� ���� ���� ������.
	if( LocalTime != 0 )
		m_IntervalChecker.OnEnd( LocalTime, fDelta );

#ifndef _GAMESERVER
	_DetachGraphicEffect();
#else
	//������ Ƚ���� ���������� ���� ������ Ƚ����ŭ ���� ��Ű��, ���� ������ Ƚ���� 1���ؼ� ������ ȣ��
	if (m_nDamageCount > 0 && m_hActor->IsDie() == false)
	{
		m_IntervalDamage = m_IntervalDamage * m_nDamageCount;
		m_nDamageCount = 1;

		RequestDamage();

#if defined(PRE_FIX_61382)
		//#48491���� Ÿ�̸ӷ� ������ ����� ActorStatIntervlaManipulator���� Dieȣ���� �ǰ� ������ OnEnd������ Dieó�� �ؾ���.
		DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
		if (hActor && hActor->IsDie())
			hActor->Die(m_ParentSkillInfo.hSkillUser);
#else
		//#48491���� Ÿ�̸ӷ� ������ ����� ActorStatIntervlaManipulator���� Dieȣ���� �ǰ� ������ OnEnd������ Dieó�� �ؾ���.
		if (m_hActor->IsDie())
			m_hActor->Die(m_ParentSkillInfo.hSkillUser);
#endif // PRE_FIX_61382
	}
#endif
 
	OutputDebug( "CDnDOTBlow::OnEnd, (HP : %d ) \n", m_hActor->GetHP());
}


#if defined(_GAMESERVER)
bool CDnDOTBlow::CalcDuplicateValue( const char* szValue )
{
	float fValue = 0.0;
	
	std::string str = szValue;//"damage;IntervalTime";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. ����ȿ�� �ε��� ����Ʈ�� ���� ���� ����
	TokenizeA(str, tokens, delimiters);
	if (2 == tokens.size())
	{
		fValue = (float)atof( tokens[0].c_str() );
	}
	
	//�ð��� ���� �ϰ� �������� ���� ��Ŵ.(�ӽ�...)

	m_fValue += fValue;

	return true;
}

void CDnDOTBlow::RequestDamage()
{
	//�������� �ְ�, ���� Ƚ���� ������..
	if (0.0f < m_IntervalDamage && 
		m_nDamageCount > 0)
	{
#if defined(PRE_FIX_44884)
#if defined(PRE_FIX_61382)
		DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
		if (hActor &&
			hActor->IsDie() == false)
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), static_cast<int>(m_IntervalDamage));

			//#48491 Ÿ�̸ӷ� ������ ����� ActorStatIntervlaManipulator���� Dieȣ���� �ǰ� ����.
			//���⼭�� Dieȣ���� �ʿ� ����.
			//if (m_hActor->IsDie())
			//	m_hActor->Die(m_ParentSkillInfo.hSkillUser);
		}
#else
		if (m_hActor->IsDie() == false)
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), static_cast<int>(m_IntervalDamage));

			//#48491 Ÿ�̸ӷ� ������ ����� ActorStatIntervlaManipulator���� Dieȣ���� �ǰ� ����.
			//���⼭�� Dieȣ���� �ʿ� ����.
			//if (m_hActor->IsDie())
			//	m_hActor->Die(m_ParentSkillInfo.hSkillUser);
		}
#endif // PRE_FIX_61382
#else
		// ���̷��� ����ȿ���� �ִ� ��� ���� �ʾƾ� �Ѵ�.
		float fDamage = m_IntervalDamage;
		if( m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_143 ) )
		{
			DNVector( DnBlowHandle ) vlhHighLanderBlow;
			m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_143, vlhHighLanderBlow );
			_ASSERT( 1 == (int)vlhHighLanderBlow.size() );
			CDnHighlanderBlow* pHighlanderBlow = static_cast<CDnHighlanderBlow*>(vlhHighLanderBlow.front().GetPointer());
			fDamage += pHighlanderBlow->CalcDamage( fDamage );
		}

		DWORD dwHitterUniqueID = m_ParentSkillInfo.hSkillUser ? m_ParentSkillInfo.hSkillUser->GetUniqueID() : -1;
		m_hActor->SetHP( m_hActor->GetHP()-(INT64)fDamage );
		m_hActor->RequestHPMPDelta( m_ParentSkillInfo.eSkillElement, -(INT64)fDamage, dwHitterUniqueID );
#endif // PRE_FIX_44884

		m_nDamageCount--;
	}
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnDOTBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�Ľ̿� �ʿ��� ���� ����
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//�ʿ��� �� ����
	float fValue[2] = {0.0f, };
	float fIntervalTime[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//ù��° ���ڿ� �Ľ�.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) 
	{
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		fIntervalTime[0] = (float)atof( vlTokens[0][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�° ���ڿ� �Ľ�
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) 
	{
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		fIntervalTime[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultValue = fValue[0] + fValue[1];
	float fResultIntervalTime = min(fIntervalTime[0], fIntervalTime[1]);
	
	sprintf_s(szBuff, "%f;%f", fResultValue, fResultIntervalTime);

	szNewValue = szBuff;
}

void CDnDOTBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�Ľ̿� �ʿ��� ���� ����
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//�ʿ��� �� ����
	float fValue[2] = {0.0f, };
	float fIntervalTime[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//ù��° ���ڿ� �Ľ�.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) 
	{
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		fIntervalTime[0] = (float)atof( vlTokens[0][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�° ���ڿ� �Ľ�
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) 
	{
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		fIntervalTime[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultValue = fValue[0] - fValue[1];
	float fResultIntervalTime = fIntervalTime[0] - fIntervalTime[1];

	sprintf_s(szBuff, "%f;%f", fResultValue, fResultIntervalTime);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
