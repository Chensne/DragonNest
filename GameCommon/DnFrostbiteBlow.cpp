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

	// ���ڴ� 3���� �޴´�. �̵� �ӵ� / ������ ���� / �ñ� ���� ���� % ��. ��� (1.0f -> 100%)
	bool bValidArgument = (3 == (int)vlTokens.size());
	_ASSERT( bValidArgument && "���� ���� ����ȿ�� ���� ������ �߸��Ǿ����ϴ�." );

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
			// #27679 ���ӵ�, ����, 8�� �׽�Ʈ�������Դ� ȿ���� ����ǳ� ������ ����� �ΰ�ȿ���� ������� �ʴ´�.
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

					// �ñ� ���� ���Ҵ� ���ӵ�� �̻��� �����Ե� �����Ŵ..
				}
			}
		}

		// #51048 ����ȿ���� �ִ� ��� �׽�Ʈ ���ӵ�� ���͵�� ������ ����� �޴´�.
		// ���������� ����ȿ���� ������ ���̹Ƿ� �ʰ� ���� ����.
		if( m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_235) )
		{
			m_bIgnoreEffectAction = true;
		}

		if( false == m_bIgnoreEffectAction )
		{
			// �Ϲ� ���������� ������� ������ ��� ȿ���� �� ��.
			m_pMoveSpeedBlow = new CDnBasicBlow( hActor, strMoveSpeedArg.c_str() );
			m_pMoveSpeedBlow->SetBlow( STATE_BLOW::BLOW_076 );		// �̵��ӵ� ��ȭ ����.
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
	// ���� ���ӽð� = <ȿ�����ӽð�> * ( 1 - ���Ӽ����� * ���º��� weight �� )
	float fGlobalStateEffectWeight = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StateRevision );
	float fIceResistance = m_hActor->GetElementDefense(CDnState::Ice);

	// ���ӽð��� ������ ���� �ٿ��ش�.
	m_StateBlow.fDurationTime = m_StateBlow.fDurationTime * (1.0f - fIceResistance*fGlobalStateEffectWeight);

	if( m_pMoveSpeedBlow )
	{
		m_pMoveSpeedBlow->SetParentSkillInfo(&m_ParentSkillInfo);
		// ��� ���ԵǾ� �ִ� ����ȿ������ �� ��ü�� ����� ���� �ϱ� ������ ���ӽð��� ������� ������
		// ��Ȯ�ϰ� �����ֱ� ���� ���ӽð��� �������ش�.
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
	// ���̽������ǿ��� ������ ���������� 25, 76�� ����ȿ���� ������
	// ���߿� ���̽������� �����ؼ� ������ �� ����� ��!
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
	// Ŭ���̾�Ʈ���Ե� �� ����ȿ���� ���ӽð��� ��� ������Ʈ �� ������ �ٲ��ش�.
	// PVP ���� �������̺� ���� �����޴� ��� , ������ Ŭ���̾�Ʈ�� ���Ⱑ Ʋ�����°�찡 �����.
	m_hActor->CmdModifyStateEffect( GetBlowID(), m_StateBlow );
#endif

	OutputDebug( "CDnFrostbiteBlow::OnBegin Value:%2.2f\n", m_fValue );
}

void CDnFrostbiteBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// MoveSpeedBlow �� NULL �̸� ���ӵ�� �̻� ����.
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
		// ���ѽð��� ������ ���� ���� ��� STATE_DURATION ���� �Ǿ��ִ� ��찡 �־ UpdateFPS() ���� ��ȿ�� ����ȿ����
		// �з��Ǿ� ������ �ӵ����� ����Ǿ� ������ ��찡 ����.
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

	//�Ľ̿� �ʿ��� ���� ����
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//�ʿ��� �� ����
	float fMoveSpeed[2] = {0.0f, };
	float fFrame[2] = {0.0f, };
	float fIceDefense[2] = {0.0f, };
	
	//////////////////////////////////////////////////////////////////////////
	//ù��° ���ڿ� �Ľ�.
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
	//�ι�° ���ڿ� �Ľ�
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) 
	{
		fMoveSpeed[1] = (float)atof( vlTokens[1][0].c_str() );
		fFrame[1] = (float)atof( vlTokens[1][1].c_str() );
		fIceDefense[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultMoveSpeed = fMoveSpeed[0] + fMoveSpeed[1];
	float fResultFrame = fFrame[0] + fFrame[1];
	float fResultIceDefence = fIceDefense[0] + fIceDefense[1];

	sprintf_s(szBuff, "%f;%f;%f", fResultMoveSpeed, fResultFrame, fResultIceDefence);

	szNewValue = szBuff;
}

void CDnFrostbiteBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�Ľ̿� �ʿ��� ���� ����
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//�ʿ��� �� ����
	float fMoveSpeed[2] = {0.0f, };
	float fFrame[2] = {0.0f, };
	float fIceDefense[2] = {0.0f, };

	//////////////////////////////////////////////////////////////////////////
	//ù��° ���ڿ� �Ľ�.
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
	//�ι�° ���ڿ� �Ľ�
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 3 ) 
	{
		fMoveSpeed[1] = (float)atof( vlTokens[1][0].c_str() );
		fFrame[1] = (float)atof( vlTokens[1][1].c_str() );
		fIceDefense[1] = (float)atof( vlTokens[1][2].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
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
	//�� ����ȿ���� ���� �Ǿ� �ִ� ��� �Ʒ� ���� ������ CDnActor::UpdateFPS�Լ����� Frame������ �����Ų��.
	//MoveSpeed����ȿ�� ���� �ɶ� �� ���� �ʱ�ȭ�� �ʿ� �ϴ�.
	m_fFrameBlowArg = 0.0f;

	if (m_pMoveSpeedBlow) 
	{
		m_hActor->UpdateFPS();
		m_pMoveSpeedBlow->OnEnd( LocalTime, fDelta );
	}

	SAFE_DELETE(m_pMoveSpeedBlow);
}
#endif // PRE_FIX_51048