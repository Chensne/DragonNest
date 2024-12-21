#include "StdAfx.h"
#include "DnFireBurnBlow.h"
#include "DnSkill.h"
#ifdef _GAMESERVER
#include "DnHighlanderBlow.h"
#endif 

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const LOCAL_TIME FIRE_DAMAGE_INTERVAL = 2000;

#if defined(_GAMESERVER)
extern int GetGaussianRandom( int nMin, int nMax, CMultiRoom *pRoom );
#endif // _GAMESERVER

CDnFireBurnBlow::CDnFireBurnBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), 
																				m_IntervalChecker( hActor, GetMySmartPtr() ), 
																				//m_StartTime( 0 ),
																				//m_iExpectedDamageCount( 0 ),
																				//m_iProcessedDamageCount( 0 ),
																				m_fEffectLength( 0.0f ),
																				m_bEffecting( false ),
																				m_fIntervalDamage( 0.0f ),
																				m_fDamageProb( 0.0f )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_042;
	SetValue( szValue );
	//m_fValue = (float)atof( szValue );

#if defined(PRE_FIX_53266)
	m_fAddDamageValue = 0.0f;

	std::string str = szValue;//"������Ȯ��;����������;�߰�������", �Ǵ� "������";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. ����ȿ�� �ε��� ����Ʈ�� ���� ���� ����
	TokenizeA(str, tokens, delimiters);

	int nTokenSize = (int)tokens.size();
	//���� ���� 1�̸� ������� ���ݷ� �ݿ�?
	if (1 == nTokenSize)
	{
		m_fValue = (float)atof( tokens[0].c_str() );
		m_fDamageProb = -1.0f;
	}
	else if (nTokenSize >= 2)
	{
		m_fValue = (float)atof(tokens[0].c_str());
		m_fDamageProb = (float)atof(tokens[1].c_str());

		if (nTokenSize >= 3)
			m_fAddDamageValue = (float)atof(tokens[2].c_str());
	}
	else
	{
		OutputDebug("%s Invalid Value %s\n", __FUNCTION__, szValue);
	}
#else
	// ���ڰ� 1���� ������� ���ݷ� �ݿ�. 2���� ���� �߰��� ������� ó��.
	string strValue(szValue);
	int iSemiColonIndex = (int)strValue.find_first_of( ';', 0 );
	bool bDamageProbMode = (string::npos != iSemiColonIndex);
	//_ASSERT( bValidArgument && "FireBurn ����ȿ�� ȿ�� ��ġ�� �߸� �Ǿ����ϴ�. �����ݷ��� ã�� ���� ����" );
	if( bDamageProbMode )
	{
		string strProb = strValue.substr( 0, iSemiColonIndex );
		string strDamageProb = strValue.substr( iSemiColonIndex+1, strValue.length() );

		// Ȯ��
		m_fValue = (float)atof( strProb.c_str() );
		m_fDamageProb = (float)atof( strDamageProb.c_str() );

#ifdef _GAMESERVER
		_ASSERT( 0.0f <= m_fDamageProb /*&& m_fDamageProb <= 1.0f*/ );
#if defined(PRE_ADD_SKILLBUF_RENEW)
		//������ ����� ��ų ���� �����Ǵ� �������� �ϵ��� �Ѵ�.(OnSetParentSkillInfo)
		//m_fIntervalDamage = CalcIntervalDamage(CDnState::Dark);
#else
		m_fIntervalDamage = m_hActor->GetLastDamage() * m_fDamageProb;
#endif // PRE_ADD_SKILLBUF_RENEW
		
		//_ASSERT( m_fIntervalDamage < 50.0f && "������ �̻�!" );
#endif
	}
	else
	{
		m_fValue = (float)atof( szValue );
		m_fDamageProb = -1.0f;
	}
#endif // PRE_FIX_53266

#ifndef _GAMESERVER
	UseTableDefinedGraphicEffect( false );
#endif
}

CDnFireBurnBlow::~CDnFireBurnBlow(void)
{

}

#ifdef _GAMESERVER
bool CDnFireBurnBlow::CanBegin( void )
{
	bool bResult = true;

	// ���� 1���� �ִ� ������� ó���ϴ� ��Ŀ����� ���� üũ�� ����.
	if( -1.0f == m_fDamageProb )
		return true;

	// fire ���� üũ.
	DNTableFileFormat* pSox = NULL;
	int iClassID = m_hActor->GetClassID();
	int nWeightItemID = 0;
	float fWeight = 0.0f;

	// �÷��̾��� ��쿡�� �Ӽ� ����ġ ����.
	bool bPlayer = false;
	if( m_hActor->GetActorType() <= CDnActor::Reserved6 )
		bPlayer = true;

	if( bPlayer )
	{
		fWeight = CPlayerWeightTable::GetInstance().GetValue( iClassID, CPlayerWeightTable::ElementDefense );
	}

	// <�ǰ����� fireȸ��Ȯ��> = <fire�Ӽ� ���� x PlayerWeightTable�� �Ӽ���>
	// �ǰݴ���� ���������� �ɸ� Ȯ�� = <��ų�� fireȮ��> - <�ǰ����� fireȸ��Ȯ��>
	float fAvoidProb = 1.0f - (m_fValue - (m_hActor->GetElementDefense( CDnState::Fire ) * fWeight));
	if( _rand(GetRoom()) % 10000 <= int(fAvoidProb * 10000.0f) )
	{
		// ȣ���� �ʿ��� CanBegin ȣ���ϰ� ���н� ��� ����ȿ�� ������� ����.
		//SetState( STATE_BLOW::STATE_END );
		OutputDebug( "CDnFireBurnBlow::CanAdd - FireBurn Fail\n" );
		bResult = false;
	}

	return bResult;
}
#endif


void CDnFireBurnBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifndef _GAMESERVER
	if( m_pEffectOutputInfo ) {	// ũ���� ���� �߰��մϴ�.. by realgaia
		_ASSERT( EffectOutputInfo::OWNER_HANDLING != m_pEffectOutputInfo->iOutputType );
	}
#else
	if( -1.0f == m_fDamageProb )
	{
		// ȭ�� ������ ó���� �� �� ����� �ΰ� ��� ��� ��.
		if( m_ParentSkillInfo.hSkillUser )
		{
			CDnDamageBase::SHitParam HitParam;
			HitParam.cAttackType = 1;					// ��ų �������� ���� ������
			HitParam.HasElement = CDnState::Fire;
			m_fIntervalDamage = m_hActor->PreCalcDamage( m_ParentSkillInfo.hSkillUser.GetPointer(), HitParam, 1.0f, m_fValue );
		}
	}
	
	if( m_fIntervalDamage < 1.0f )
	{
		SetState( STATE_BLOW::STATE_END );
		//Ŭ���̾�Ʈ�� ���� ��Ŷ ��������..
		m_hActor->CmdRemoveStateEffectFromID(GetBlowID());
		OutputDebug( "CDnFireBurnBlow::CanAdd - Damage is too small. ( < 1) FireBurn Fail\n" );
	}
#endif
	m_IntervalChecker.OnBegin( LocalTime, FIRE_DAMAGE_INTERVAL );

	m_bEffecting = false;
	m_fEffectLength = 0.0f;

	OutputDebug( "CDnFireBurnBlow::OnBegin, Value:%2.2f (HP : %d ) \n", m_fValue, m_hActor->GetHP());
}



bool CDnFireBurnBlow::OnCustomIntervalProcess( void )
{
#ifdef _GAMESERVER
	if( 0.0f < m_fIntervalDamage )
	{
#if defined(PRE_FIX_44884)
#if defined(PRE_FIX_61382)
		DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
		if (hActor &&
			hActor->IsDie() == false)
		{	
			// ȭ���� ������ ���̽� ��ų�� ������� �ҼӼ����� ǥ���ش޶�� ��û #51908
			CDnState::ElementEnum eBackUpElement = m_ParentSkillInfo.eSkillElement;
			m_ParentSkillInfo.eSkillElement = CDnState::ElementEnum::Fire;
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)m_fIntervalDamage);
			m_ParentSkillInfo.eSkillElement = eBackUpElement; 

			//#48491 Ÿ�̸ӷ� ������ ����� ActorStatIntervlaManipulator���� Dieȣ���� �ǰ� ����.
			//���⼭�� Dieȣ���� �ʿ� ����.
			//if (m_hActor->IsDie())
			//	m_hActor->Die(m_ParentSkillInfo.hSkillUser);
		}

#else
		if (m_hActor->IsDie() == false)
		{	
			// ȭ���� ������ ���̽� ��ų�� ������� �ҼӼ����� ǥ���ش޶�� ��û #51908
			CDnState::ElementEnum eBackUpElement = m_ParentSkillInfo.eSkillElement;
			m_ParentSkillInfo.eSkillElement = CDnState::ElementEnum::Fire;
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)m_fIntervalDamage);
			m_ParentSkillInfo.eSkillElement = eBackUpElement; 

			//#48491 Ÿ�̸ӷ� ������ ����� ActorStatIntervlaManipulator���� Dieȣ���� �ǰ� ����.
			//���⼭�� Dieȣ���� �ʿ� ����.
			//if (m_hActor->IsDie())
			//	m_hActor->Die(m_ParentSkillInfo.hSkillUser);
		}
#endif // PRE_FIX_61382
#else
		// ���̷��� ����ȿ���� �ִ� ��� ���� �ʾƾ� �Ѵ�.
		float fDamage = m_fIntervalDamage;
		if( m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_143 ) )
		{
			// IsAppliedThisStateBlow() �Լ����� true �� ���ϵ����� ���ӽð��� �� ��
			// ����ȿ���� GatherAppliedStateBlowByBlowIndex() ���� �������� �����Ƿ� ����Ʈ�� ������� �� �� �ִ�.
			DNVector( DnBlowHandle ) vlhHighLanderBlow;
			m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_143, vlhHighLanderBlow );
			if( 0 < (int)vlhHighLanderBlow.size() )
			{
				CDnHighlanderBlow* pHighlanderBlow = static_cast<CDnHighlanderBlow*>(vlhHighLanderBlow.front().GetPointer());
				fDamage += pHighlanderBlow->CalcDamage( fDamage );
			}
		}

		DWORD dwHitterUniqueID = m_ParentSkillInfo.hSkillUser ? m_ParentSkillInfo.hSkillUser->GetUniqueID() : -1;
		m_hActor->SetHP( m_hActor->GetHP()-(INT64)fDamage );
		m_hActor->RequestHPMPDelta( m_ParentSkillInfo.eSkillElement, -(INT64)fDamage, dwHitterUniqueID );
#endif // PRE_FIX_44884
	}
#else
	_AttachGraphicEffect();
	if( m_hEtcObjectEffect )
	{
		CEtActionBase::ActionElementStruct* pStruct = m_hEtcObjectEffect->GetElement( "Idle" );
		if( pStruct )
		{
			m_fEffectLength = (float)pStruct->dwLength / 1000.0f;
		}
		_SetDiffuse( 1.0f, 0.0f, 0.0f, 1.0f );
	}
#endif

	return true;
}


void CDnFireBurnBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
	
	m_IntervalChecker.Process( LocalTime, fDelta );
	
#ifndef _GAMESERVER
	if( m_bEffecting )
	{
		m_fEffectLength -= fDelta;
		if( m_fEffectLength < 0.0f )
		{
			m_fEffectLength = 0.0f;
			_SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );

			m_bEffecting = false;
		}
	}
#endif

	//OutputDebug( "CDnFireBurnBlow::Process, Value:%2.2f (HP : %d ) \n", m_fValue, m_hActor->GetHP());
}


void CDnFireBurnBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	// LocalTime �� 0���� ȣ��Ǹ� ����, ��øó�� ���� ���� ���� ������.
	if( LocalTime != 0 )
		m_IntervalChecker.OnEnd( LocalTime, fDelta );

	m_hActor->DelBlowState( &m_State );

#ifndef _GAMESERVER
	_SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
	_DetachGraphicEffect();
	//m_hActor->DetachSEEffect( m_ParentSkillInfo, m_StateBlow.emBlowIndex, GetBlowID() );
#endif
 
	OutputDebug( "CDnFireBurnBlow::OnEnd, (HP : %d ) \n", m_hActor->GetHP());
}

#if defined(_GAMESERVER)
void CDnFireBurnBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	if( -1.0f == m_fDamageProb )
	{
		if( m_ParentSkillInfo.hSkillUser )
		{
			CDnDamageBase::SHitParam HitParam;
			HitParam.cAttackType = 1;					// ��ų �������� ���� ������
			HitParam.HasElement = CDnState::Fire;
			m_fIntervalDamage = m_hActor->PreCalcDamage( m_ParentSkillInfo.hSkillUser.GetPointer(), HitParam, 1.0f, m_fValue );
		}
	}
}

bool CDnFireBurnBlow::CalcDuplicateValue( const char* szValue )
{
	float fValue = 0.0;
	float fDamageProb = 0.0f;

#if defined(PRE_FIX_53266)
	float fAddDamageValue = 0.0f;

	std::string str = szValue;//"������Ȯ��;����������;�߰�������", �Ǵ� "������";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. ����ȿ�� �ε��� ����Ʈ�� ���� ���� ����
	TokenizeA(str, tokens, delimiters);

	int nTokenSize = (int)tokens.size();
	//���� ���� 1�̸� ������� ���ݷ� �ݿ�?
	if (1 == nTokenSize)
	{
		fValue = (float)atof( tokens[0].c_str() );
		fDamageProb = -1.0f;

		m_fValue += fValue;
	}
	else if (nTokenSize >= 2)
	{
		fValue = (float)atof(tokens[0].c_str());
		fDamageProb = (float)atof(tokens[1].c_str());

		if (nTokenSize >= 3)
			fAddDamageValue = (float)atof(tokens[2].c_str());

		m_fValue += fValue;
		m_fDamageProb += fDamageProb;
		m_fDamageProb = min(1.0f, m_fDamageProb);

		m_fIntervalDamage = CalcIntervalDamage(CDnState::Fire);
	}
	else
	{
		OutputDebug("%s Invalid Value %s\n", __FUNCTION__, szValue);
	}
#else
	// ���ڰ� 1���� ������� ���ݷ� �ݿ�. 2���� ���� �߰��� ������� ó��.
	string strValue(szValue);
	int iSemiColonIndex = (int)strValue.find_first_of( ';', 0 );
	bool bDamageProbMode = (string::npos != iSemiColonIndex);
	if( bDamageProbMode )
	{
		string strProb = strValue.substr( 0, iSemiColonIndex );
		string strDamageProb = strValue.substr( iSemiColonIndex+1, strValue.length() );

		// Ȯ��
		fValue = (float)atof( strProb.c_str() );
		fDamageProb = (float)atof( strDamageProb.c_str() );

		m_fValue += fValue;
		m_fDamageProb += fDamageProb;
		if( 1.0f < m_fDamageProb )
			m_fDamageProb = 1.0f;

		_ASSERT( 0.0f <= m_fDamageProb );
#if defined(PRE_ADD_SKILLBUF_RENEW)
		m_fIntervalDamage = CalcIntervalDamage(CDnState::Fire);
#else
		m_fIntervalDamage = m_hActor->GetLastDamage() * m_fDamageProb;
#endif // PRE_ADD_SKILLBUF_RENEW
	}
	else
	{
		fValue = (float)atof( szValue );
		fDamageProb = -1.0f;

		m_fValue += fValue;
	}
#endif // PRE_FIX_53266

	return true;
}

#if defined(PRE_ADD_SKILLBUF_RENEW)
float CDnFireBurnBlow::CalcIntervalDamage(CDnState::ElementEnum haElement)
{
	float fCalcDamage = 1.0f;

	//���� ���ݷ�..
	int HitterAttackValue[2] = {0, };
	CDnState *pState = NULL;
	if (m_ParentSkillInfo.hSkillUser)
		pState = static_cast<CDnActorState *>(m_ParentSkillInfo.hSkillUser.GetPointer());

	if (pState)
	{
		//��ų ������� ���� ���ݷ��� ���� �´�.(��ų�� ���� ������?�� ����, ������ ����?)
		if (m_ParentSkillInfo.hSkillUser)
		{
			HitterAttackValue[0] = m_ParentSkillInfo.hSkillUser->GetAttackMMinWithoutSkill();//pState->GetAttackMMin();
			HitterAttackValue[1] = m_ParentSkillInfo.hSkillUser->GetAttackMMaxWithoutSkill();//pState->GetAttackMMax();
		}

		//////////////////////////////////////////////////////////////////////////
		// �Ӽ� ����ġ
		float fElementWeight = 1.f;
		fElementWeight = ( ( 1.f + pState->GetElementAttack( haElement ) ) * ( 1.f - m_hActor->GetElementDefense( haElement ) ) );
#ifndef PRE_ADD_BUFF_STATE_LIMIT
		fElementWeight = max( fElementWeight, CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::ElementMinRevision ) );
#endif
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//����..
		float fDefensePower = 1.f;
		float fDefenseWeight = 0.0f;
		int nDefense = 0;

		nDefense = m_hActor->GetDefenseM();
		fDefenseWeight = m_ParentSkillInfo.hSkillUser->GetDefenseConstant();

		fDefensePower = nDefense / fDefenseWeight;
		if( fDefensePower < 0.f ) fDefensePower = 0.f;
		else if( fDefensePower > CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DefenseMax ) ) 
			fDefensePower = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DefenseMax );

		fDefensePower = 1.f - fDefensePower;
		//////////////////////////////////////////////////////////////////////////

		fCalcDamage = (float)GetGaussianRandom( HitterAttackValue[0], HitterAttackValue[1], m_hActor->GetRoom() ) * fElementWeight;
		
		
		fCalcDamage = fCalcDamage * m_fDamageProb * fDefensePower;
#if defined(PRE_FIX_53266)
		fCalcDamage += m_fAddDamageValue;
#endif // PRE_FIX_53266
	}
	else
		fCalcDamage = 1.0f;

	return fCalcDamage;
}

void CDnFireBurnBlow::OnSetParentSkillInfo()
{
	m_fIntervalDamage = CalcIntervalDamage(CDnState::Dark);
}
#endif // PRE_ADD_SKILLBUF_RENEW

#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFireBurnBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�Ľ̿� �ʿ��� ���� ����
	std::vector<string> vlTokens[2];
	string strArgument[2];

	int nValueCount = 0;

	//�ʿ��� �� ����
	float fValue[2];
	float fDamageProb[2];


	//////////////////////////////////////////////////////////////////////////
	//ù��° ���ڿ� �Ľ�.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() > 1 ) {
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		fDamageProb[0] = (float)atof( vlTokens[0][1].c_str() );

		nValueCount = 2;
	}
	else 
	{
		fValue[0] = (float)atof( szOrigValue );
		fDamageProb[0] = -1.0f;

		nValueCount = 1;
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�° ���ڿ� �Ľ�
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() > 1 ) {
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		fDamageProb[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	else 
	{
		fValue[1] = (float)atof( szOrigValue );
		fDamageProb[1] = -1.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultValue = fValue[0] + fValue[1];
	float fResultDamageProb = fDamageProb[0] + fDamageProb[1];

	if (nValueCount == 2)
	{
		sprintf_s(szBuff, "%f;%f", fResultValue, fResultDamageProb);
	}
	else
	{
		sprintf_s(szBuff, "%f", fResultValue);
	}

	szNewValue = szBuff;
}

void CDnFireBurnBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�Ľ̿� �ʿ��� ���� ����
	std::vector<string> vlTokens[2];
	string strArgument[2];

	int nValueCount = 0;

	//�ʿ��� �� ����
	float fValue[2];
	float fDamageProb[2];


	//////////////////////////////////////////////////////////////////////////
	//ù��° ���ڿ� �Ľ�.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() > 1 ) {
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		fDamageProb[0] = (float)atof( vlTokens[0][1].c_str() );

		nValueCount = 2;
	}
	else 
	{
		fValue[0] = (float)atof( szOrigValue );
		fDamageProb[0] = -1.0f;

		nValueCount = 1;
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�° ���ڿ� �Ľ�
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() > 1 ) {
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		fDamageProb[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	else 
	{
		fValue[1] = (float)atof( szOrigValue );
		fDamageProb[1] = -1.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultValue = fValue[0] - fValue[1];
	float fResultDamageProb = fDamageProb[0] - fDamageProb[1];

	if (nValueCount == 2)
	{
		sprintf_s(szBuff, "%f;%f", fResultValue, fResultDamageProb);
	}
	else
	{
		sprintf_s(szBuff, "%f", fResultValue);
	}

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW