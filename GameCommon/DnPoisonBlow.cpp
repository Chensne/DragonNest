#include "StdAfx.h"
#include "DnPoisonBlow.h"
#include "DnDamageBase.h"
#include "DnActionBase.h"
#ifndef _GAMESERVER
#include "DnPlayerActor.h"
#else
#include "DnHighLanderBlow.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


const LOCAL_TIME POISON_DAMAGE_INTERVAL = 2000;
const int DEFAULT_POISON_DAMAGE_PROB = 1000;			// 백분율 * 10 이 단위임

#if defined(_GAMESERVER)
extern int GetGaussianRandom( int nMin, int nMax, CMultiRoom *pRoom );
#endif // _GAMESERVER

CDnPoisonBlow::CDnPoisonBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), 
																			m_IntervalChecker( hActor, GetMySmartPtr() ),
																			m_fEffectLength( 0.0f )
																			,m_bEffecting(false)
#ifdef _GAMESERVER
																			, m_fIntervalDamage( 0.0f )
#endif
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_044;
	SetValue( szValue );

	m_fDamageProb = 0.0f;

#if defined(PRE_FIX_53266)
	m_fAddDamageValue = 0.0f;

	std::string str = szValue;//"데미지확률;데미지비율;추가데미지", 또는 "데미지";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);

	int nTokenSize = (int)tokens.size();
	//설정 값이 1이면 기존대로 공격력 반영?
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
	string strValue(szValue);
	int iSemiColonIndex = (int)strValue.find_first_of( ';', 0 );
	bool bDamageProbMode = (string::npos != iSemiColonIndex);
	if( bDamageProbMode )
	{
		string strProb = strValue.substr( 0, iSemiColonIndex );
		string strDamageProb = strValue.substr( iSemiColonIndex+1, strValue.length() );

		// 확률
		m_fValue = (float)atof( strProb.c_str() );
		m_fDamageProb = (float)atof( strDamageProb.c_str() );

#ifdef _GAMESERVER
		_ASSERT( 0.0f <= m_fDamageProb/* && m_fDamageProb <= 1.0f*/ );
#if defined(PRE_ADD_SKILLBUF_RENEW)
		//데미지 계산은 스킬 정보 설정되는 시점에서 하도록 한다.(OnSetParentSkillInfo)
		//m_fIntervalDamage = CalcIntervalDamage(CDnState::Dark);
#else
		m_fIntervalDamage = m_hActor->GetLastDamage() * m_fDamageProb;
#endif // PRE_ADD_SKILLBUF_RENEW
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

CDnPoisonBlow::~CDnPoisonBlow(void)
{

}

#ifdef _GAMESERVER
bool CDnPoisonBlow::CanBegin( void )
{
	bool bResult = true;

	// 인자 1개만 있는 기존대로 처리하는 방식에서는 저항 체크를 안함.
	if( -1.0f == m_fDamageProb )
		return true;

	// Poison 내성 체크.
	DNTableFileFormat* pSox = NULL;
	int iClassID = m_hActor->GetClassID();
	int nWeightItemID = 0;
	float fWeight = 0.0f;

	// 플레이어인 경우에만 속성 가중치 적용.
	bool bPlayer = false;
	if( m_hActor->GetActorType() <= CDnActor::Reserved6 )
		bPlayer = true;

	if( bPlayer )
	{
		fWeight = CPlayerWeightTable::GetInstance().GetValue( iClassID, CPlayerWeightTable::ElementDefense );
	}

	// <피격자의 poison 회피확률> = <dark 속성 내성 x PlayerWeightTable의 속성방어값>
	// 피격대상의 실제감전에 걸릴 확률 = <스킬의 poison 확률> - <피격자의 poison 회피확률>
	float fAvoidProb = 1.0f - (m_fValue - (m_hActor->GetElementDefense( CDnState::Dark ) * fWeight));
	if( _rand(GetRoom()) % 10000 <= int(fAvoidProb * 10000.0f) )
	{
		// 호출한 쪽에서 CanBegin 호출하고 실패시 즉시 상태효과 삭제토록 변경.
		//SetState( STATE_BLOW::STATE_END );
		OutputDebug( "CDnPoisonBlow::CanAdd - Poison Fail\n" );
		bResult = false;
	}

	return bResult;
}
#endif


void CDnPoisonBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	// 독 데미지 처음에 한 번 계산해 두고 계속 깍아 줌.
	if( -1.0f == m_fDamageProb )
	{
		if( m_ParentSkillInfo.hSkillUser )
		{
			CDnDamageBase::SHitParam HitParam;
			HitParam.cAttackType = 1;					// 스킬 데미지는 마법 공격임
			HitParam.HasElement = CDnState::Dark;
			m_fIntervalDamage = m_hActor->PreCalcDamage( m_ParentSkillInfo.hSkillUser.GetPointer(), HitParam, 1.0f, m_fValue );
		}
		else
		{
			// 프랍에서 쓰는 독 상태효과 등에서는 SkillUser 가 셋팅이 안되어있으므로 
			// 상태효과의 인자값을 그대로 반영시켜준다.
			m_fIntervalDamage = m_fValue;
		}
	}
	
	// 최소한 dot 데미지는 1을 보장한다.
	if( m_fIntervalDamage < 1.0f )
	{
		//SetState( STATE_BLOW::STATE_END );
		//OutputDebug( "CDnPoisonBlow::CanAdd - Damage is too small. ( < 1) Poison Fail\n" );
		m_fIntervalDamage = 1.0f;
	}
#endif

	m_IntervalChecker.OnBegin( LocalTime, POISON_DAMAGE_INTERVAL );

	m_fEffectLength = 0.0f;
	m_bEffecting = false;
	
	OutputDebug( "CDnPoisonBlow::OnBegin, Value:%2.2f (HP : %d ) \n", m_fValue, m_hActor->GetHP());
}



//bool CDnPoisonBlow::OnCheckIntervalProb( void )
//{
//	bool bSuccess = false;
//
//	int iDamageProbability = DEFAULT_POISON_DAMAGE_PROB - m_hActor->GetElementDefense( CDnState::Dark ) * 3;
//
//	if( iDamageProbability < 0 )
//		iDamageProbability = 0;
//	else
//	if( iDamageProbability > 1000 )
//		iDamageProbability = 1000;
//
//	bSuccess = (_rand(GetRoom()) % 1000 < iDamageProbability);
//
//	return bSuccess;
//}


bool CDnPoisonBlow::OnCustomIntervalProcess( void )
{
	//OutputDebug( "포이즌 블로우 : OnCustomInvterval Called id: %d\n", m_hActor->GetUniqueID() );

#ifdef _GAMESERVER
	if( 0.0f < m_fIntervalDamage )
	{
#if defined(PRE_FIX_44884)
#if defined(PRE_FIX_61382)
		DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
		if (hActor &&
			hActor->IsDie() == false)
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)m_fIntervalDamage);

			//#48491 타이머로 데미지 적용시 ActorStatIntervlaManipulator에서 Die호출이 되고 있음.
			//여기서는 Die호출이 필요 없다.
			//if (m_hActor->IsDie())
			//	m_hActor->Die(m_ParentSkillInfo.hSkillUser);
		}
#else
		if (m_hActor->IsDie() == false)
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), (int)m_fIntervalDamage);

			//#48491 타이머로 데미지 적용시 ActorStatIntervlaManipulator에서 Die호출이 되고 있음.
			//여기서는 Die호출이 필요 없다.
			//if (m_hActor->IsDie())
			//	m_hActor->Die(m_ParentSkillInfo.hSkillUser);
		}
#endif // PRE_FIX_61382
#else
		// 하이랜더 상태효과가 있는 경우 죽지 않아야 한다.
		float fDamage = m_fIntervalDamage;
		if( m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_143 ) )
		{
			// IsAppliedThisStateBlow() 함수에선 true 가 리턴되지만 지속시간이 다 된
			// 상태효과는 GatherAppliedStateBlowByBlowIndex() 에서 얻어와지지 않으므로 리스트가 비어있을 수 도 있다.
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
	//DnEtcHandle hEffectHandle = m_hActor->AttachSEEffect( m_ParentSkillInfo, m_StateBlow.emBlowIndex, GetBlowID(), NULL, NULL, "Poisoning.act" );
	_AttachGraphicEffect();
	if( m_hEtcObjectEffect )
	{	
		//OutputDebug( "포이즌 이펙트 생성됨. id: %d\n", m_hActor->GetUniqueID() );
		CEtActionBase::ActionElementStruct* pStruct = m_hEtcObjectEffect->GetElement( "Idle" );
		if( pStruct )
		{
			m_fEffectLength = (float)pStruct->dwLength / 1000.0f;
		}

		_SetDiffuse( 0.0f, 1.0f, 0.0f, 1.0f );
		m_bEffecting = true;
	}
#endif

	return true;	
}


//#ifndef _GAMESERVER
//void CDnPoisonBlow::_SetDiffuse( fl00000000oat fRed, float fGreen, float fBlue, float fAlpha )
//{
//	EtAniObjectHandle hAniObject = m_hActor->GetObjectHandle();
//	int iCustomParamIndex = hAniObject->AddCustomParam( "g_MaterialDiffuse", EPT_VECTOR );
//	EtColor Color( fRed, fGreen, fBlue, fAlpha );
//
//	if( -1 != iCustomParamIndex )
//	{
//		hAniObject->SetCustomParam( iCustomParamIndex, EPT_VECTOR, &Color );
//	}
//	else
//	{
//		// TODO: 다중 파츠 액터인 경우 각 파츠별로 색깔을 지정해줘야 할 듯..
//		// 플레이어인 경우
//		CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer());
//		if( pPlayerActor )
//		{
//			for( int iPart = (int)CDnParts::Face; iPart < CDnParts::PartsTypeEnum_Amount; ++iPart )
//			{
//				DnPartsHandle hBodyParts = pPlayerActor->GetParts( (CDnParts::PartsTypeEnum)iPart );
//				if( hBodyParts )
//				{
//					EtAniObjectHandle hBodyObject = hBodyParts->GetObjectHandle();
//					int iCustomParam = hBodyObject->AddCustomParam( "g_MaterialDiffuse", EPT_VECTOR );
//
//					hBodyObject->SetCustomParam( iCustomParam, EPT_VECTOR, &Color );
//				}
//			}
//		}
//	}
//
//}
//#endif


void CDnPoisonBlow::Process( LOCAL_TIME LocalTime, float fDelta )
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

	//LOCAL_TIME ElapsedTime = LocalTime - m_StartTime;
	//int iDamageCount = int(ElapsedTime / POISON_DAMAGE_INTERVAL);
	//while( m_iDamageCount < iDamageCount )
	//{
	//	if(  )
	//	{
	//		CDnDamageBase::SHitParam HitParam;
	//		HitParam.vPosition = *m_hActor->GetPosition();
	//		HitParam.vViewVec = *m_hActor->GetLookDir();
	//		HitParam.fDamage = m_fValue;
	//		m_hActor->OnDamage( m_hActor.GetPointer(), HitParam );

	//		//m_hActor->SetHP( m_hActor->GetHP() - (INT64)m_fValue );
	//	}

	//	m_LastDamageCheckTime = LocalTime;

	//	++m_iDamageCount;
	//}

	// TODO: 매 2초마다 다크 속성 데미지 먹음
	// 다크 속성 저항력이 있으면 데미지를 회피할 수도 있음. 디폴트는 100% 독 데미지 먹음 (다크내성 1당 0.3% 회피확률 증가)
	// 회피 되었을 경우 시각적으로 표시해준다.
	//if( LocalTime - m_LastDamageCheckTime > POISON_DAMAGE_INTERVAL )
	//{
		//int iDamageProbability = DEFAULT_POISON_DAMAGE_PROB - m_hActor->GetElementDefense( CDnState::Dark ) * 3;
		//
		//if( iDamageProbability < 0 )
		//	iDamageProbability = 0;
		//else
		//if( iDamageProbability > 1000 )
		//	iDamageProbability = 1000;

		//if( _rand(GetRoom()) % 1000 < iDamageProbability )
		//{
		//	// HP 0 이하이면 die 알아서 되나?
		//	m_hActor->SetHP( m_hActor->GetHP() - (INT64)m_fValue );
		//}

		//m_LastDamageCheckTime = LocalTime;
	//}

}



void CDnPoisonBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
//#ifdef _GAMESERVER
	// LocalTime 이 0으로 호출되면 삭제, 중첩처리 등을 위한 강제 종료임.
	if( LocalTime != 0 )
		m_IntervalChecker.OnEnd( LocalTime, fDelta );

#ifndef _GAMESERVER
	_SetDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
	
	// 이펙트 삭제. 이미 사라졌다해도 괜찮다.
	//m_hActor->DetachSEEffect( m_ParentSkillInfo, m_StateBlow.emBlowIndex, GetBlowID() );
	_DetachGraphicEffect();
#endif
	OutputDebug( "CDnPoisonBlow::OnEnd, Value:%2.2f (HP : %d ) \n", m_fValue, m_hActor->GetHP());
//#endif
}

#if defined(_GAMESERVER)
void CDnPoisonBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// 독 데미지 처음에 한 번 계산해 두고 계속 깍아 줌.
	if( m_ParentSkillInfo.hSkillUser )
	{
		if( -1.0f == m_fDamageProb )
		{
			CDnDamageBase::SHitParam HitParam;
			HitParam.cAttackType = 1;					// 스킬 데미지는 마법 공격임
			HitParam.HasElement = CDnState::Dark;
			m_fIntervalDamage = m_hActor->PreCalcDamage( m_ParentSkillInfo.hSkillUser.GetPointer(), HitParam, 1.0f, m_fValue );
		}
	}
}

bool CDnPoisonBlow::CalcDuplicateValue( const char* szValue )
{
	float fValue = 0.0f;
	float fDamageProb = 0.0f;

#if defined(PRE_FIX_53266)
	float fAddDamageValue = 0.0f;

	std::string str = szValue;//"데미지확률;데미지비율;추가데미지", 또는 "데미지";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);

	int nTokenSize = (int)tokens.size();
	//설정 값이 1이면 기존대로 공격력 반영?
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

		m_fIntervalDamage = CalcIntervalDamage(CDnState::Dark);
	}
	else
	{
		OutputDebug("%s Invalid Value %s\n", __FUNCTION__, szValue);
	}
#else
	string strValue(szValue);
	int iSemiColonIndex = (int)strValue.find_first_of( ';', 0 );
	bool bDamageProbMode = (string::npos != iSemiColonIndex);
	if( bDamageProbMode )
	{
		string strProb = strValue.substr( 0, iSemiColonIndex );
		string strDamageProb = strValue.substr( iSemiColonIndex+1, strValue.length() );

		// 확률
		fValue = (float)atof( strProb.c_str() );
		fDamageProb = (float)atof( strDamageProb.c_str() );

		m_fValue += fValue;
		m_fDamageProb += fDamageProb;
		if( 1.0f < m_fDamageProb )
			m_fDamageProb = 1.0f;

		_ASSERT( 0.0f <= m_fDamageProb );
#if defined(PRE_ADD_SKILLBUF_RENEW)
		m_fIntervalDamage = CalcIntervalDamage(CDnState::Dark);
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
float CDnPoisonBlow::CalcIntervalDamage(CDnState::ElementEnum haElement)
{
	float fCalcDamage = 1.0f;

	//마법 공격력..
	int HitterAttackValue[2] = {0, };
	CDnState *pState = NULL;
	if (m_ParentSkillInfo.hSkillUser)
		pState = static_cast<CDnActorState *>(m_ParentSkillInfo.hSkillUser.GetPointer());

	if (pState)
	{
		//스킬 사용자의 마법 공격력을 가져 온다.(스킬에 의한 증가량?은 무시, 버프는 포함?)
		if (m_ParentSkillInfo.hSkillUser)
		{
			HitterAttackValue[0] = m_ParentSkillInfo.hSkillUser->GetAttackMMinWithoutSkill();//pState->GetAttackMMin();
			HitterAttackValue[1] = m_ParentSkillInfo.hSkillUser->GetAttackMMaxWithoutSkill();//pState->GetAttackMMax();
		}

		//////////////////////////////////////////////////////////////////////////
		// 속성 가중치
		float fElementWeight = 1.f;
		fElementWeight = ( ( 1.f + pState->GetElementAttack( haElement ) ) * ( 1.f - m_hActor->GetElementDefense( haElement ) ) );
#ifndef PRE_ADD_BUFF_STATE_LIMIT
		fElementWeight = max( fElementWeight, CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::ElementMinRevision ) );
#endif
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//방어력..
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

void CDnPoisonBlow::OnSetParentSkillInfo()
{
	m_fIntervalDamage = CalcIntervalDamage(CDnState::Dark);
}
#endif // PRE_ADD_SKILLBUF_RENEW

#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnPoisonBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	int nValueCount = 0;

	//필요한 값 변수
	float fValue[2];
	float fDamageProb[2];


	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
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
	//두번째 문자열 파싱
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

	//두 값을 더한다.
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

void CDnPoisonBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	int nValueCount = 0;

	//필요한 값 변수
	float fValue[2];
	float fDamageProb[2];


	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
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
	//두번째 문자열 파싱
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

	//두 값을 더한다.
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