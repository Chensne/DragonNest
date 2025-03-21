#include "StdAfx.h"
#include "DnPayBackMPBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnPayBackMPBlow::CDnPayBackMPBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																				  m_fApplyProb( 0.0f ),
																				  m_fConsumeDecreaseProb( 0.0f )
#ifndef _GAMESERVER
																				  ,m_bShowActivateEffect( false )
																				  ,m_ShowActivateEffectTime( 0 )
																				  ,m_PrevFrameLocalTime( 0 )
#endif
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_068;

	SetValue( szValue );
	//m_fValue = (float)atof( szValue );

	string strValue( szValue );
	int iSemiColonIndex = (int)strValue.find_first_of( ';', 0 );
	bool bValidArgument = (string::npos != iSemiColonIndex);
	_ASSERT( bValidArgument && "PayBack Mana 상태효과 효과 수치가 잘못 되었습니다. 세미콜론을 찾을 수가 없음" );
	if( bValidArgument )
	{
		string strApplyProb = strValue.substr( 0, iSemiColonIndex );
		string strConsumeDecreaseProb = strValue.substr( iSemiColonIndex+1, strValue.length() );

		// 확률
		m_fApplyProb = (float)atof( strApplyProb.c_str() );
		m_fConsumeDecreaseProb = (float)atof( strConsumeDecreaseProb.c_str() );
	}

	AddCallBackType( SB_ONUSEMP );
}

CDnPayBackMPBlow::~CDnPayBackMPBlow(void)
{

}

void CDnPayBackMPBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	

	OutputDebug( "CDnPayBackMPBlow::OnBegin\n" );
}


void CDnPayBackMPBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifndef _GAMESERVER
	if( m_bShowActivateEffect )
	{
		// 1.5 초간만 보여준다.
		if( 1500 < LocalTime - m_ShowActivateEffectTime )
		{
			_DetachGraphicEffect();
			m_bShowActivateEffect = false;
		}
	}

	m_PrevFrameLocalTime = LocalTime;
#endif
}


void CDnPayBackMPBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	

	OutputDebug( "CDnPayBackMPBlow::OnEnd\n" );
}


void CDnPayBackMPBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// 페이백 마나 상태효과에서는 아무것도 하지 않는다.
}


#ifdef _GAMESERVER
int CDnPayBackMPBlow::OnUseMP( int iMPDelta )
{
	int iResult = 0;
	if( _rand(GetRoom())%10000 <= m_fApplyProb*10000.0f )
	{
		iResult = int(-iMPDelta * m_fConsumeDecreaseProb);
		m_hActor->RequestSEProbSuccess( m_ParentSkillInfo.iSkillID, m_StateBlow.emBlowIndex );
	}

	return iResult;
}
#else
void CDnPayBackMPBlow::OnActivate( void )
{
	m_bShowActivateEffect = true;
	_AttachGraphicEffect();

	m_ShowActivateEffectTime = m_PrevFrameLocalTime;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnPayBackMPBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];


	//필요한 값 변수
	float fApplyProb[2] = {0.0f, };
	float fConsumeDecreaseProb[2] = {0.0f, };


	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) {
		fApplyProb[0] = (float)atof( vlTokens[0][0].c_str() );
		fConsumeDecreaseProb[0] = (float)atof( vlTokens[0][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) {
		fApplyProb[1] = (float)atof( vlTokens[1][0].c_str() );
		fConsumeDecreaseProb[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultApplyProb = fApplyProb[0] + fApplyProb[1];
	float fResultConsumeDecreaseProb = fConsumeDecreaseProb[0] + fConsumeDecreaseProb[1];

	sprintf_s(szBuff, "%f;%f", fResultApplyProb, fResultConsumeDecreaseProb);
	
	szNewValue = szBuff;
}

void CDnPayBackMPBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];


	//필요한 값 변수
	float fApplyProb[2] = {0.0f, };
	float fConsumeDecreaseProb[2] = {0.0f, };


	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) {
		fApplyProb[0] = (float)atof( vlTokens[0][0].c_str() );
		fConsumeDecreaseProb[0] = (float)atof( vlTokens[0][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) {
		fApplyProb[1] = (float)atof( vlTokens[1][0].c_str() );
		fConsumeDecreaseProb[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultApplyProb = fApplyProb[0] - fApplyProb[1];
	float fResultConsumeDecreaseProb = fConsumeDecreaseProb[0] - fConsumeDecreaseProb[1];

	sprintf_s(szBuff, "%f;%f", fResultApplyProb, fResultConsumeDecreaseProb);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
