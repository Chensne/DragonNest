#include "StdAfx.h"
#include "DnProvocationBlow.h"
#if defined( _GAMESERVER )
#include "DNAggroSystem.h"
#endif // #if defined( _GAMESERVER )

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

// 일정 거리 이상 벌어지면 곧바로 상태효과를 종료시켜야 함. 안그러면 계속 쫓아감.
const float PROVOKE_BOUNDARY = 1000.0f * 1000.0f;

CDnProvocationBlow::CDnProvocationBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_132;

	SetValue( szValue );
	m_fValue = (float)atof(szValue);

	//// 인자가 3개임. value;거리값;캐릭터의 전방기준 각도값.
	//string strValue( szValue );

	//// 스트링을 공백단위로 분할한 후
	//std::vector<string> vlTokens;
	//TokenizeA( strValue, vlTokens, ";" );

	//bool bValidArgument = (3 == (int)vlTokens.size());
	//_ASSERT( bValidArgument && "132번 프로보크 상태효과 인자 셋팅이 잘못되었습니다." );
	//if( bValidArgument )
	//{
	//	m_fValue = (float)atoi(vlTokens.at( 0 ).c_str());
	//	m_fRange = (float)atof(vlTokens.at( 1 ).c_str());
	//	m_fRangeAngleByRadian = EtToRadian((float)atof(vlTokens.at( 2 ).c_str()));
	//	m_fRangeSQ = m_fRange * m_fRange;
	//}
}

CDnProvocationBlow::~CDnProvocationBlow(void)
{

}

void CDnProvocationBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnProvocationBlow::OnBegin\n" );
	
//#if defined( _GAMESERVER )
//	if( IsPermanent() )
//	{
//		MatrixEx Cross = *m_hActor->GetMatEx();
//		EtVector3 vPlayerDir = Cross.m_vZAxis;
//		EtVec3Normalize( &vPlayerDir, &vPlayerDir );
//
//		CMultiRoom *pRoom = m_hActor->GetRoom();
//		for( DWORD i=0; i<STATIC_INSTANCE_(CDnActor::s_pVecProcessList).size(); i++ ) 
//		{
//			DnActorHandle hActor = STATIC_INSTANCE_(CDnActor::s_pVecProcessList)[i]->GetMySmartPtr();
//			if( hActor && hActor->IsMonsterActor() )
//			{
//				// 일정 거리 안에 있는 애들만 적용시켜준다.
//				EtVector3 vDistance = *hActor->GetPosition() - *m_hActor->GetPosition();
//				float fDistanceSQ = EtVec3LengthSq( &vDistance );
//				if( fDistanceSQ <= m_fRangeSQ )
//				{
//					// 캐릭터 전방 기준 일정 각도값 안에 들어온 애들만 적용시켜준다.
//					EtVector3 vDirToMonster;
//					EtVec3Normalize( &vDirToMonster, &vDistance );
//					float fDot = EtVec3Dot( &vPlayerDir, &vDistance );
//					if( fDot <= m_fRangeAngleByRadian )
//					{
//						if( hActor->GetAggroSystem() )
//						{
//#if defined( PRE_FIX_22526 )
//							hActor->GetAggroSystem()->AddAggro( m_hActor, static_cast<int>(GetFloatValue()) );
//#else
//							CDNAggroSystem::AggroStruct* pStruct = hActor->GetAggroSystem()->GetAggroStruct( m_hActor );
//							if( pStruct )
//							{
//								pStruct->iAggro += static_cast<int>(GetFloatValue());
//							}
//#endif // #if defined( PRE_FIX_22526 )
//						}
//					}
//				}
//			}
//		}
//	}
//#endif // #if defined( _GAMESERVER )
}


void CDnProvocationBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	// 스킬을 쓴 플레이어와 이 상태효과가 발동중인 몬스터와의 거리가 10 미터 이상 벌어지면 상태효과 곧바로 제거.
	bool bValid = true;
	if( m_ParentSkillInfo.hSkillUser )
	{
		// 죽었을 때도 곧바로 제거.
		if( m_ParentSkillInfo.hSkillUser->IsDie() )
			bValid = false;

		//float fDistanceSQ = EtVec3LengthSq( &EtVector3(*m_hActor->GetPosition() - *m_ParentSkillInfo.hSkillUser->GetPosition()) );

		// 기획팀의 요청으로 거리체크로 풀리는 것은 없앤다. #26329
		// 추후에 컨트롤이 필요할 때 외부로 빼던지 조정이 필요함.
		//if( PROVOKE_BOUNDARY < fDistanceSQ )
		//{
		//	bValid = false;
		//}
	}
	else
		bValid = false;		// 플레이어 객체가 없을 때도 곧바로 제거.

	if( false == bValid )
	{
		// 상태효과 삭제 예약.
		m_hActor->AddReserveRemoveBlow( GetMySmartPtr() );
	}
#endif
}


void CDnProvocationBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

	OutputDebug( "CDnProvocationBlow::OnEnd\n" );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnProvocationBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnProvocationBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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