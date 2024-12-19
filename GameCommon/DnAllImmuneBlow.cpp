#include "StdAfx.h"
#include "DnAllImmuneBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnAllImmuneBlow::CDnAllImmuneBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), 
																			m_fResistPercent( 0 )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_150;
	SetValue( szValue );
	
	// ; 를 구분자로 면역이 되지 않는 예외 상태효과 인덱스를 셋팅한다.
	string strValue( szValue );
	std::vector<string> vlTokens;
	TokenizeA( strValue, vlTokens, ";" );

	for( int i = 0; i < (int)vlTokens.size(); ++i )
	{

		const char* pToken = vlTokens.at( i ).c_str();
		int iBlowIndex = atoi( pToken );
		m_setExceptionBlowIndices.insert( iBlowIndex );
	}
}

CDnAllImmuneBlow::~CDnAllImmuneBlow(void)
{
}



void CDnAllImmuneBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
//	OutputDebug( "CDnAllImmuneBlow::OnBegin, Value: %d\n", m_nValue );
}


void CDnAllImmuneBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	// 이 녀석은 면역 말고 하는 게 없음..	
	CDnBlow::Process( LocalTime, fDelta );
}


bool CDnAllImmuneBlow::IsImmuned( const CDnSkill::SkillInfo* pParentSkillInfo, STATE_BLOW::emBLOW_INDEX BlowIndex )
{
	if( NULL == pParentSkillInfo )
		return false;

	bool bResult = true;

	// 예외 셋에 존재하는 상태효과 인덱스는 면역이 되지 않고 통과. 
	// 나머지는 전부 면역.
	set<int>::iterator iter = m_setExceptionBlowIndices.find( (int)BlowIndex );
	if( m_setExceptionBlowIndices.end() != iter )
		bResult = false;

	return bResult;
};


void CDnAllImmuneBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
//	OutputDebug( "CDnAllImmuneBlow::OnEndn" );
}

void CDnAllImmuneBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// 상태효과 면역 상태효과는 아무것도 하지 않는다.
}

bool CDnAllImmuneBlow::IsImmuned( STATE_BLOW::emBLOW_INDEX BlowIndex )
{
	bool bResult = true;

	// 예외 셋에 존재하는 상태효과 인덱스는 면역이 되지 않고 통과. 
	// 나머지는 전부 면역.
	set<int>::iterator iter = m_setExceptionBlowIndices.find( (int)BlowIndex );
	if( m_setExceptionBlowIndices.end() != iter )
		bResult = false;

	return bResult;
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAllImmuneBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
	//szOrigValue 마지막에 ";"가 없으면 추가 해서 szAddValue를 추가 한다.
	int nLength = (int)strlen(szOrigValue);
	
	if (nLength == 0 || szOrigValue[nLength - 1] == ';')
		szNewValue += szAddValue;
	else
	{
		szNewValue += ";";
		szNewValue += szAddValue;
	}
	
}

void CDnAllImmuneBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//"###;###;###;###;....;##;##;##"
	//|<--이전 문자열----->|<-AddValue->|

	//szOrigValue에서 szAddValue 문자열길이 만큼 뒤에서 자른다.
	int nOrigLength = (int)strlen(szOrigValue);
	int nAddLength = (int)strlen(szAddValue);
	int nCount = nOrigLength - nAddLength;

	sprintf_s(szBuff, "%s", szOrigValue);
	szBuff[nCount] = 0;

	szNewValue = szBuff;	
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
