#include "StdAfx.h"
#include "DnFrostbiteBlow.h"
#include "DnFrameBlow.h"
#include "DnBasicBlow.h"
#include "DnTableDB.h"
#include "DnMonsterActor.h"
#include "DnWorld.h"
#ifdef _GAMESERVER
#include "DnGameRoom.h"
#include "DnGameDataManager.h"
#include "DnUserSession.h"
#endif


#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnFrostbiteBlow::CDnFrostbiteBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																				  m_fFrameBlowArg( 0.0f ),
																				  m_pMoveSpeedBlow( NULL ),
																				  m_pIceDefenseBlow( NULL ),
																				  m_bNestMap( false )
																				  
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_144;

	SetValue( szValue );
	m_fValue = 0.0f;

#ifdef _GAMESERVER
	CDNGameRoom* pGameRoom = static_cast<CDNGameRoom*>(m_hActor->GetRoom());
	if( false == pGameRoom->bIsPvPRoom() )
	{
		UINT uiSessionID = 0;
		pGameRoom->GetLeaderSessionID( uiSessionID );
		CDNUserSession *pUserSession = pGameRoom ? pGameRoom->GetUserSession(uiSessionID) : NULL;
		if( pUserSession )
		{
			const TMapInfo* pMapData = g_pDataManager->GetMapInfo( pUserSession->GetMapIndex() );
			if( pMapData )
#if defined(PRE_ADD_DRAGON_FELLOWSHIP)
				m_bNestMap = CDnBlow::CheckEffectIgnoreMapType(pMapData->MapType, pMapData->MapSubType);
#else	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
				m_bNestMap = (GlobalEnum::MAP_DUNGEON == pMapData->MapType) && 
							 ((GlobalEnum::MAPSUB_NEST == pMapData->MapSubType) ||(GlobalEnum::MAPSUB_NESTNORMAL ==  pMapData->MapSubType));
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
		}
	}
#else
#if defined(PRE_ADD_DRAGON_FELLOWSHIP)
	m_bNestMap = CDnBlow::CheckEffectIgnoreMapType();
#else	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
	CDnWorld* pWorld = CDnWorld::GetInstancePtr();
	m_bNestMap = (CDnWorld::MapTypeEnum::MapTypeDungeon == pWorld->GetMapType()) && 
				 ((CDnWorld::MapSubTypeEnum::MapSubTypeNest == pWorld->GetMapSubType() || CDnWorld::MapSubTypeEnum::MapSubTypeNestNormal == pWorld->GetMapSubType()));
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP)
#endif

	string strValue( szValue );
	std::vector<string> vlTokens;
	TokenizeA( strValue, vlTokens, ";" );

	// 인자는 3개를 받는다. 이동 속도 / 프레임 증감 / 냉기 저항 증감 % 임. 모두 (1.0f -> 100%)
	bool bValidArgument = (3 == (int)vlTokens.size());
	_ASSERT( bValidArgument && "얼음 감옥 상태효과 인자 셋팅이 잘못되었습니다." );

	if( bValidArgument )
	{
		string strMoveSpeedArg = vlTokens.at( 0 );
		string strFrameBlowArg = vlTokens.at( 1 );
		string strIceDefenseArg = vlTokens.at( 2 );

		m_pIceDefenseBlow = new CDnBasicBlow( hActor, strIceDefenseArg.c_str() );
		m_pIceDefenseBlow->SetBlow( STATE_BLOW::BLOW_037 );

		//bool bIgnoreActionEffect = false;
		if( m_bNestMap )
		{
			// #27679 네임드, 보스, 8인 네스트보스에게는 효과는 적용되나 실제로 결빙의 부가효과가 적용되진 않는다.
			bool bBossMonster = true;
			if( m_hActor->IsMonsterActor() )
			{
				CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
				if( (CDnMonsterState::Boss == pMonsterActor->GetGrade() ||
					CDnMonsterState::BossHP4 == pMonsterActor->GetGrade() ||
					CDnMonsterState::NestBoss == pMonsterActor->GetGrade() ||
					CDnMonsterState::NestBoss8 == pMonsterActor->GetGrade()) )
				{
					m_pMoveSpeedBlow = NULL;
					bBossMonster = true;
					m_bIgnoreEffectAction = true;

					// 냉기 저항 감소는 네임드급 이상의 몹에게도 적용시킴..
				}
			}
		}

		// #51048 상태효과가 있는 경우 네스트 네임드급 몬스터들과 동일한 취급을 받는다.
		// 독립적으로 상태효과가 박히는 것이므로 맵과 관계 없다.
		if( m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_235) )
		{
			m_bIgnoreEffectAction = true;
		}

		if( false == m_bIgnoreEffectAction )
		{
			// 일반 던전에서는 상관없이 동상의 모든 효과가 다 들어감.
			m_pMoveSpeedBlow = new CDnBasicBlow( hActor, strMoveSpeedArg.c_str() );
			m_pMoveSpeedBlow->SetBlow( STATE_BLOW::BLOW_076 );		// 이동속도 변화 비율.
			m_fFrameBlowArg = (float)atof( strFrameBlowArg.c_str() );
		}
	}
}

CDnFrostbiteBlow::~CDnFrostbiteBlow( void )
{
	SAFE_DELETE( m_pMoveSpeedBlow );
	SAFE_DELETE( m_pIceDefenseBlow );
}

void CDnFrostbiteBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// 동상 지속시간 = <효과지속시간> * ( 1 - 물속성내성 * 상태보전 weight 값 )
	float fGlobalStateEffectWeight = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StateRevision );
	float fIceResistance = m_hActor->GetElementDefense(CDnState::Ice);

	// 지속시간을 내성에 따라 줄여준다.
	m_StateBlow.fDurationTime = m_StateBlow.fDurationTime * (1.0f - fIceResistance*fGlobalStateEffectWeight);

	if( m_pMoveSpeedBlow )
	{
		m_pMoveSpeedBlow->SetParentSkillInfo(&m_ParentSkillInfo);
		// 사실 포함되어 있는 상태효과들은 이 객체와 운명을 같이 하기 떄문에 지속시간을 사용하진 않지만
		// 정확하게 맞춰주기 위해 지속시간을 셋팅해준다.
		m_hActor->UpdateFPS();
		m_pMoveSpeedBlow->SetDurationTime( m_StateBlow.fDurationTime );
		m_pMoveSpeedBlow->OnBegin( LocalTime, fDelta );
	}

	if( m_pIceDefenseBlow )
	{
		m_pIceDefenseBlow->SetParentSkillInfo(&m_ParentSkillInfo);
		m_pIceDefenseBlow->SetDurationTime( m_StateBlow.fDurationTime );
		m_pIceDefenseBlow->OnBegin( LocalTime, fDelta );
	}

#ifdef _GAMESERVER
#ifdef PRE_MOD_FROSTBITE_EXCEPTION
	// 아이시프랙션에서 강제로 느려지게한 25, 76번 상태효과를 제거함
	// 나중에 아이시프랙션 관련해서 수정할 때 고려할 것!
	DNVector( DnBlowHandle ) vlFrameSpeedBlow;
	m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_025, vlFrameSpeedBlow );
	for( DWORD n=0; n<vlFrameSpeedBlow.size(); n++ )
	{
		if ( vlFrameSpeedBlow[n]->GetParentSkillInfo()->iSkillID == 2212 )
			m_hActor->CmdRemoveStateEffectFromID( vlFrameSpeedBlow[n]->GetBlowID() );
	}

	DNVector( DnBlowHandle ) vlMoveSpeedBlow;
	m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_076, vlMoveSpeedBlow );
	for( DWORD n=0; n<vlMoveSpeedBlow.size(); n++ )
	{
		if ( vlMoveSpeedBlow[n]->GetParentSkillInfo()->iSkillID == 2212 )
			m_hActor->CmdRemoveStateEffectFromID( vlMoveSpeedBlow[n]->GetBlowID() );
	}
#endif // PRE_MOD_FROSTBITE_EXCEPTION
	// 클라이언트에게도 이 상태효과의 지속시간을 방금 업데이트 된 것으로 바꿔준다.
	// PVP 같은 보정테이블 값을 참조받는 경우 , 서버와 클라이언트의 동기가 틀어지는경우가 생긴다.
	m_hActor->CmdModifyStateEffect( GetBlowID(), m_StateBlow );
#endif

	OutputDebug( "CDnFrostbiteBlow::OnBegin Value:%2.2f\n", m_fValue );
}

void CDnFrostbiteBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// MoveSpeedBlow 가 NULL 이면 네임드급 이상 보스.
	if( m_pMoveSpeedBlow )
	{
		m_hActor->UpdateFPS();
		m_pMoveSpeedBlow->Duplicate( StateBlowInfo );
	}

	if( m_pIceDefenseBlow )
		m_pIceDefenseBlow->Duplicate( StateBlowInfo );
}

void CDnFrostbiteBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

	if( m_pMoveSpeedBlow )
		m_pMoveSpeedBlow->Process( LocalTime, fDelta );

	if( m_pIceDefenseBlow )
		m_pIceDefenseBlow->Process( LocalTime, fDelta );
}

void CDnFrostbiteBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_pMoveSpeedBlow )
	{
		// 제한시간이 정해져 있지 않을 경우 STATE_DURATION 으로 되어있는 경우가 있어서 UpdateFPS() 에서 유효한 상태효과로
		// 분류되어 프레임 속도값이 적용되어 버리는 경우가 생김.
		SetState( STATE_BLOW::STATE_END );
		m_hActor->UpdateFPS();

		m_pMoveSpeedBlow->OnEnd( LocalTime, fDelta );
	}

	if( m_pIceDefenseBlow )
		m_pIceDefenseBlow->OnEnd( LocalTime, fDelta );

	OutputDebug( "CDnFrostbiteBlow::OnEnd Value:%2.2f\n", m_fValue );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFrostbiteBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	float fMoveSpeed[2] = {0.0f, };
	float fFrame[2] = {0.0f, };
	float fIceDefense[2] = {0.0f, };
	
	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) 
	{
		fMoveSpeed[0] = (float)atof( vlTokens[0][0].c_str() );
		fFrame[0] = (float)atof( vlTokens[0][1].c_str() );
		fIceDefense[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) 
	{
		fMoveSpeed[1] = (float)atof( vlTokens[1][0].c_str() );
		fFrame[1] = (float)atof( vlTokens[1][1].c_str() );
		fIceDefense[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultMoveSpeed = fMoveSpeed[0] + fMoveSpeed[1];
	float fResultFrame = fFrame[0] + fFrame[1];
	float fResultIceDefence = fIceDefense[0] + fIceDefense[1];

	sprintf_s(szBuff, "%f;%f;%f", fResultMoveSpeed, fResultFrame, fResultIceDefence);

	szNewValue = szBuff;
}

void CDnFrostbiteBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	float fMoveSpeed[2] = {0.0f, };
	float fFrame[2] = {0.0f, };
	float fIceDefense[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 3 ) 
	{
		fMoveSpeed[0] = (float)atof( vlTokens[0][0].c_str() );
		fFrame[0] = (float)atof( vlTokens[0][1].c_str() );
		fIceDefense[0] = (float)atof( vlTokens[0][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) 
	{
		fMoveSpeed[1] = (float)atof( vlTokens[1][0].c_str() );
		fFrame[1] = (float)atof( vlTokens[1][1].c_str() );
		fIceDefense[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultMoveSpeed = fMoveSpeed[0] - fMoveSpeed[1];
	float fResultFrame = fFrame[0] - fFrame[1];
	float fResultIceDefence = fIceDefense[0] - fIceDefense[1];

	sprintf_s(szBuff, "%f;%f;%f", fResultMoveSpeed, fResultFrame, fResultIceDefence);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

#if defined(PRE_FIX_51048)
void CDnFrostbiteBlow::RemoveDebufAction(LOCAL_TIME LocalTime, float fDelta)
{
	//이 상태효과가 적용 되어 있는 경우 아래 변수 값으로 CDnActor::UpdateFPS함수에서 Frame변경을 적용시킨다.
	//MoveSpeed상태효과 제거 될때 이 값도 초기화가 필요 하다.
	m_fFrameBlowArg = 0.0f;

	if (m_pMoveSpeedBlow) 
	{
		m_hActor->UpdateFPS();
		m_pMoveSpeedBlow->OnEnd( LocalTime, fDelta );
	}

	SAFE_DELETE(m_pMoveSpeedBlow);
}
#endif // PRE_FIX_51048