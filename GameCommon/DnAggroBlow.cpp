#include "StdAfx.h"
#include "DnAggroBlow.h"
#if defined( _GAMESERVER )
#include "DNAggroSystem.h"
#endif // #if defined( _GAMESERVER )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnAggroBlow::CDnAggroBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), m_fRange( 0.0f ), 
																		  m_fRangeSQ( 0.0f ), m_fRangeAngleByRadian( 0.0f )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_066;

	SetValue( szValue );
	
	// 인자가 3개임. value;거리값;캐릭터의 전방기준 각도값.
	string strValue( szValue );

	// 스트링을 공백단위로 분할한 후
	std::vector<string> vlTokens;
	TokenizeA( strValue, vlTokens, ";" );

	bool bValidArgument = (3 == (int)vlTokens.size());
	_ASSERT( bValidArgument && "132번 프로보크 상태효과 인자 셋팅이 잘못되었습니다." );
	if( bValidArgument )
	{
		m_fValue = (float)atoi(vlTokens.at( 0 ).c_str());
		m_fRange = (float)atof(vlTokens.at( 1 ).c_str());
		m_fRangeAngleByRadian = EtToRadian((float)atof(vlTokens.at( 2 ).c_str())) / 2.0f;		// 전방기준 좌우 각도값이므로 2로 나눠줌.
		m_fRangeSQ = m_fRange * m_fRange;
	}
}

CDnAggroBlow::~CDnAggroBlow(void)
{

}

void CDnAggroBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#if defined( _GAMESERVER )
	if( IsPermanent() )
	{
		MatrixEx Cross = *m_hActor->GetMatEx();
		EtVector3 vPlayerDir = Cross.m_vZAxis;
		EtVec3Normalize( &vPlayerDir, &vPlayerDir );

		CMultiRoom *pRoom = m_hActor->GetRoom();
		for( DWORD i=0; i<STATIC_INSTANCE_(CDnActor::s_pVecProcessList).size(); i++ ) 
		{
			DnActorHandle hActor = STATIC_INSTANCE_(CDnActor::s_pVecProcessList)[i]->GetMySmartPtr();
			if( hActor && hActor->IsMonsterActor() )
			{
				// 일정 거리 안에 있는 애들만 적용시켜준다.
				EtVector3 vDistance = *hActor->GetPosition() - *m_hActor->GetPosition();
				float fDistanceSQ = EtVec3LengthSq( &vDistance );
				if( fDistanceSQ <= m_fRangeSQ )
				{
					// 캐릭터 전방 기준 일정 각도값 안에 들어온 애들만 적용시켜준다.
					EtVector3 vDirToMonster;
					EtVec3Normalize( &vDirToMonster, &vDistance );
					float fDot = EtVec3Dot( &vPlayerDir, &vDirToMonster );
					if( fDot <= m_fRangeAngleByRadian )
					{
						m_vlhApplyMonsters.push_back( hActor );
					}
				}
			}
		}

		for( UINT i=0 ; i<m_vlhApplyMonsters.size() ; ++i )
		{
			DnActorHandle hActor = m_vlhApplyMonsters[i];
			if( hActor && hActor->GetAggroSystem() )
				hActor->GetAggroSystem()->AddAggro( m_hActor, static_cast<int>(m_fValue) );
		}
	}
#endif // #if defined( _GAMESERVER )

	OutputDebug( "CDnAggroBlow::OnBegin\n" );
}


void CDnAggroBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnAggroBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

	OutputDebug( "CDnAggroBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAggroBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 변수
	float fValue[2] = {0.0f, };
	float fRange[2] = {0.0f, };
	float fRangeAngle[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) {
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		fRange[0] = (float)atof( vlTokens[0][1].c_str() );
		fRangeAngle[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱.
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) {
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		fRange[1] = (float)atof( vlTokens[1][1].c_str() );
		fRangeAngle[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	float fResultValue = fValue[0] + fValue[1];
	float fResultRange = max(fRange[0], fRange[1]);
	float fResultRangeAngle = fRangeAngle[0] + fRangeAngle[1];

	sprintf_s(szBuff, "%f;%f;%f", fResultValue, fResultRange, fResultRangeAngle);

	szNewValue = szBuff;
}

void CDnAggroBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 변수
	float fValue[2] = {0.0f, };
	float fRange[2] = {0.0f, };
	float fRangeAngle[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) {
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		fRange[0] = (float)atof( vlTokens[0][1].c_str() );
		fRangeAngle[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱.
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) {
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		fRange[1] = (float)atof( vlTokens[1][1].c_str() );
		fRangeAngle[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	float fResultValue = fValue[0] - fValue[1];
	float fResultRange = min(fRange[0], fRange[1]);
	float fResultRangeAngle = fRangeAngle[0] - fRangeAngle[1];

	sprintf_s(szBuff, "%f;%f;%f", fResultValue, fResultRange, fResultRangeAngle);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
