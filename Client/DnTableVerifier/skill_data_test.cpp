#include "StdAfx.h"
#include "skill_data_test.h"
#include "EtActionBase.h"
#include "EtActionSignal.h"
#include "SignalHeader.h"


const int MAX_STATE_EFEFCT_COUNT = 5;
const int MAX_PROCESSOR = 5;

DNTableFileFormat* skill_data_test::s_pSkillTable = NULL;
DNTableFileFormat* skill_data_test::s_pSkillLevelTable = NULL;
DNTableFileFormat* skill_data_test::s_pItemTable = NULL;
set<int> skill_data_test::s_setSkillBookItemID;

CEtActionBase* skill_data_test::s_pWarrior = NULL;
CEtActionBase* skill_data_test::s_pArcher = NULL;
CEtActionBase* skill_data_test::s_pCleric = NULL;
CEtActionBase* skill_data_test::s_pSoceress = NULL;


#define SKILL_T skill_data_test::s_pSkillTable
#define SKILL_LEVEL_T skill_data_test::s_pSkillLevelTable

skill_data_test::skill_data_test(void)
{
	
}

skill_data_test::~skill_data_test(void)
{

}


void skill_data_test::SetUpTestCase( void )
{
	s_pSkillTable = LoadExtFile( "SkillTable.dnt" );
	s_pSkillLevelTable = LoadExtFile( "SkillLevelTable.dnt", "_SkillIndex" );
	s_pItemTable = LoadExtFile( "ItemTable.dnt" );

	for( int i = 0; i < s_pItemTable->GetItemCount(); ++i )
	{
		int iItemID = s_pItemTable->GetItemID( i );

		// 스킬북 타입은 10
		if( 10 == s_pItemTable->GetFieldFromLablePtr( iItemID, "_Type" )->GetInteger() )
		{
			s_setSkillBookItemID.insert( iItemID );
		}
	}

	s_pWarrior = new CEtActionBase;
	s_pWarrior->LoadAction( (g_strResourcePathA+"\\Resource\\Char\\Player\\Warrior\\Warrior.act").c_str() );

	s_pArcher = new CEtActionBase;
	s_pArcher->LoadAction( (g_strResourcePathA+"\\Resource\\Char\\Player\\Archer\\Archer.act").c_str() );

	s_pCleric = new CEtActionBase;
	s_pCleric->LoadAction( (g_strResourcePathA+"\\Resource\\Char\\Player\\Cleric\\Cleric.act").c_str() );

	s_pSoceress = new CEtActionBase;
	s_pSoceress->LoadAction( (g_strResourcePathA+"\\Resource\\Char\\Player\\Soceress\\Soceress.act").c_str() );
}

void skill_data_test::TearDownTestCase( void )
{
	SAFE_DELETE( s_pSkillTable );
	SAFE_DELETE( s_pSkillLevelTable );

	// 프로그램 종료될 때 ActionCoreMng 에서 뻗네 ㅠㅠ
	// 일단 delete 하지 않고 끝냄..
	//SAFE_DELETE( s_pWarrior );
	//SAFE_DELETE( s_pArcher );
	//SAFE_DELETE( s_pCleric );
	//SAFE_DELETE( s_pSoceress );
}


void skill_data_test::SetUp( void )
{
	//m_pTestActionLoad = shared_ptr<CEtActionBase>(new CEtActionBase);
	//m_pTestActionLoad->LoadAction( "R:/GameRes/Resource/Char/Player/Cleric/Cleric.act" );
}


void skill_data_test::TearDown( void )
{

}

TEST_F( skill_data_test, PASSIVESKILL_MUST_NOT_HAVE_PLAYANI_PROCESSOR )
{
	int iNumItemCount = SKILL_T->GetItemCount();
	for( int iSkill = 0; iSkill < iNumItemCount; ++iSkill )
	{
		int iSkillTableID = SKILL_T->GetItemID( iSkill );
		int iSkillType = SKILL_T->GetFieldFromLablePtr( iSkillTableID, "_SkillType" )->GetInteger();

		TCHAR atcBuf[ 512 ];
		char acLabel[ 64 ];

		// 1이 패시브 스킬임
		if( 1 == iSkillType )
		{
			int iProcessor = -1;

			for( int i = 0; i < MAX_PROCESSOR; ++i )
			{
				sprintf_s( acLabel, "_Processor%d", i + 1 );
				iProcessor = SKILL_T->GetFieldFromLablePtr( iSkillTableID, acLabel )->GetInteger();

				// 1 번이 PlayAni
				ZeroMemory( atcBuf, sizeof(atcBuf) );
				_stprintf_s( atcBuf, _T("{TableName:SkillTable, Index:%d, Note:패시브 스킬은 액션툴에서 직접 InputHasPassiveSkill 로 처리됩니다...}"), iSkillTableID );

				SCOPED_TRACE( atcBuf );

				EXPECT_NE( 1, iProcessor );
			}
		}
	}
}



void ValidateProcessArgument( int iLevelTableID, int iArgument )
{
	char acBuf[ 64 ];
	sprintf_s( acBuf, "_ProcessParam%d", iArgument+1 );
	const char* pArgument = SKILL_LEVEL_T->GetFieldFromLablePtr( iLevelTableID, acBuf )->GetString();
	//EXPECT_STRNE( "0", pArgument );
	EXPECT_STRNE( "", pArgument );	
}


// PlayAni 발현 타입에 인자가 제대로 없는 경우
TEST_F( skill_data_test, PROCESSOR_ARGUMENT_VERIFY )
{
	const char* apProcessTypeName [] = { "PlayAni", "TimePlay", "ChangePrj", "DivideSEArgByTargetNum", "ImpactBlow", "Aura", "ChangeAction", "RangeStateEffect" };
	int iNumItemCount = SKILL_T->GetItemCount();
	for( int iSkill = 0; iSkill < iNumItemCount; ++iSkill )
	{
		int iSkillTableID = SKILL_T->GetItemID( iSkill );

		// PlayAni 발현 타입이 있는 경우.
		int aiUseProcessorType[ 5 ];
		ZeroMemory( aiUseProcessorType, sizeof(aiUseProcessorType) );
		for( int iProcessor = 0; iProcessor < MAX_PROCESSOR; ++iProcessor )
		{
			char acBuf[ 64 ];
			sprintf_s( acBuf, "_Processor%d", iProcessor+1 );
			int iUseType = SKILL_T->GetFieldFromLablePtr( iSkillTableID, acBuf )->GetInteger();
			aiUseProcessorType[ iProcessor ] = iUseType;
		}

		vector<int> vlSkillLevelList;
		if( SKILL_LEVEL_T->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) == 0 ) 
			FAIL();

		int iNumSkillLevel = (int)vlSkillLevelList.size();
		for( int iLevel = 0; iLevel < iNumSkillLevel; ++iLevel )
		{
			bool bChangeActionStr = false;			// 액션 문자열 변경 발현타입은 가변인자기 때문에 갯수 추측이 안됨.
			int iNowProcessor = 0;
			int iSkillLevelID = vlSkillLevelList.at( iLevel );
			for( int iArgument = 0; iArgument < 10; )
			{
				// 발현타입 설정 최대 갯수는 5개임.
				if( MAX_PROCESSOR == iNowProcessor )
					break;

				// NONE 으로 셋팅되어 있으면 다음 인덱스로 패스
				if( 0 == aiUseProcessorType[ iNowProcessor ] )
				{
					++iNowProcessor;
					continue;
				}

				TCHAR atcTraceBuf[ 1024 ];

				if( bChangeActionStr )
				{
					_stprintf_s( atcTraceBuf, _T("{TableName:SkillTable, Index:%d, Level:%d, Note:ChangeActionStr 발현 타입 다음에 다른 발현타입이 오면 안됩니다.}"), iSkillTableID, iLevel+1 );
					EXPECT_TRUE( bChangeActionStr );
				}

				ZeroMemory( atcTraceBuf, sizeof(atcTraceBuf) );
				const char* pNowProcessorTypeName = NULL;
				int iNumArgument = 0;
				switch( aiUseProcessorType[ iNowProcessor ] )
				{
						// playani 인자는 1개
					case 1:
						iNumArgument = 1;
						pNowProcessorTypeName = apProcessTypeName[ 0 ];
						break;

						// timeplay 인자는 5개
					case 2:
						iNumArgument = 5;
						pNowProcessorTypeName = apProcessTypeName[ 1 ];
						break;

						// changeproj 인자는 2개
					case 3:
						iNumArgument = 2;
						pNowProcessorTypeName = apProcessTypeName[ 2 ];
						break;

						// divideseargbytargetnum 인자는 1개
					case 4:
						iNumArgument = 1;
						pNowProcessorTypeName = apProcessTypeName[ 3 ];
						break;

						// impactblow 인자 없음
					case 5:
						iNumArgument = 0;
						pNowProcessorTypeName = apProcessTypeName[ 4 ];
						break;

						// aura 인자는 1개
					case 6:
						iNumArgument = 1;
						pNowProcessorTypeName = apProcessTypeName[ 5 ];
						break;

						// change action 인자는 정해져 있지 않음.
						// 인자가 가변이기 때문에 현재 구조상 뒤에 다른 발현 타입이 있으면 안된다.
					case 7:
						bChangeActionStr = true;
						//iNumArgument = 7;
						//pNowProcessorTypeName = apProcessTypeName[ 6 ];
						break; 

						// RangeStateEffect 인자는 4개
					case 8:
						iNumArgument = 4;
						pNowProcessorTypeName = apProcessTypeName[ 7 ];
						break;

					default:
						break;

				}

				TCHAR atcProcessorTypeName[256];
				MultiByteToWideChar( CP_ACP, 0, pNowProcessorTypeName, -1, atcProcessorTypeName, 256 );

				for( int i = 0; i < iNumArgument; ++i )
				{
					_stprintf_s( atcTraceBuf, _T("{TableName:SkillTable, Index:%d, Level:%d, Note:발현타입(%s) 인자인덱스(%d) 인자값이 없습니다.}"), iSkillTableID, iLevel+1, atcProcessorTypeName, iArgument+1 );
					SCOPED_TRACE( atcTraceBuf );
					ValidateProcessArgument( iSkillLevelID, iArgument );
					++iArgument;
				}

				++iNowProcessor;
			}
		}
	}
}


void ValidateSemiColonDivide( const char* pArgumentString, int iNumArgument )
{
	int iOffset = 0;
	int iSemiColonIndex = 0;
	string strArgument( pArgumentString );
	for( int i = 0; i < iNumArgument; ++i )
	{
		iOffset = iSemiColonIndex + 1;
		iSemiColonIndex = (int)strArgument.find_first_of( ';', iOffset );

		bool bInvalid = false;

		if( i != iNumArgument-1 )
			bInvalid = ( string::npos == iSemiColonIndex );						// 중간 인덱스에선 세미콜론 인덱스가 유효해야하고,
		else
			bInvalid = (strArgument.at( strArgument.length()-1 ) == ';');		// 마지막 인덱스에선 맨 끝이 ; 로 남아있으면 안된다.

		EXPECT_FALSE( bInvalid );
		if( bInvalid )
			break;
	}
}



// 상태효과의 인자가 없는 경우..
TEST_F( skill_data_test, STATE_EFFECT_ARGUMENT_VERIFY )
{
	int iNumItemCount = SKILL_T->GetItemCount();
	for( int iSkill = 0; iSkill < iNumItemCount; ++iSkill )
	{
		int iSkillTableID = SKILL_T->GetItemID( iSkill );

		// 상태효과가 있는 경우... 최대 5개
		bool abUseStateEffect[ MAX_STATE_EFEFCT_COUNT ];
		ZeroMemory( abUseStateEffect, sizeof(abUseStateEffect) );
		for( int iStateEffect = 0; iStateEffect < MAX_STATE_EFEFCT_COUNT; ++iStateEffect )
		{
			char acBuf[ 64 ];
			sprintf_s( acBuf, "_EffectClass%d", iStateEffect+1 );
			if( SKILL_T->GetFieldFromLablePtr( iSkillTableID, acBuf )->GetInteger() != 0 )
				abUseStateEffect[ iStateEffect ] = true;
		}

		vector<int> vlSkillLevelList;
		if( SKILL_LEVEL_T->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) == 0 ) 
			FAIL();

		int iNumSkillLevel = (int)vlSkillLevelList.size();
		for( int iLevel = 0; iLevel < iNumSkillLevel; ++iLevel )
		{
			int iSkillLevelID = vlSkillLevelList.at(iLevel);
			for( int iStateEffect = 0; iStateEffect < 5; ++iStateEffect )
			{
				if( abUseStateEffect[ iStateEffect ] )
				{
					char acBuf[ 64 ];
					sprintf_s( acBuf, "_EffectClass%d", iStateEffect+1 );
					int iStateEffectIndex = SKILL_T->GetFieldFromLablePtr( iSkillTableID, acBuf )->GetInteger();

					sprintf_s( acBuf, "_EffectClassValue%d", iStateEffect+1 );
					const char* pArgument = SKILL_LEVEL_T->GetFieldFromLablePtr( iSkillLevelID, acBuf )->GetString();

					TCHAR atcBuf[ 256 ] = {0};

					// 세미 콜론 들어간 상태효과는 제대로 인자 갯수가 맞는지 확인한다.
					switch( iStateEffectIndex )
					{
						 // 결빙(41) - 2개
						case 41:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:결빙 상태효과 인자가 잘못되었습니다. 세미콜론(;) 구분으로 (결빙확률(0.0~1.0);내구도횟수) 가 맞는지 확인해주십시오.}"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );

								ValidateSemiColonDivide( pArgument, 2 );
							}
							break;
						
						// 체인공격(60) - 3개
						case 60:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:체인공격 상태효과 인자가 잘못되었습니다. 세미콜론(;) 구분으로 (범위;최대히트카운트;데미지적용비율)이 맞는지 확인해주십시오.}"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );
								
								ValidateSemiColonDivide( pArgument, 3 );
							}
							break;

						// 페이백 마나(68) - 2개
						case 68:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:페이백 마나 상태효과 인자가 잘못되었습니다. 세미콜론(;) 구분으로 (적용확률(0.0~1.0);소모MP 감소비율(0.0-1.0)) 가 맞는지 확인해주십시오.}"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );

								ValidateSemiColonDivide( pArgument, 2 );
							}
							break;

						// 상태효과 면역(77) - 2개 
						case 77:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:상태효과 면역 상태효과 인자가 잘못되었습니다. 세미콜론(;) 구분으로 (면역될 상태효과 인덱스;면역확률(0.0~1.0) 가 맞는지 확인해주십시오.)"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );

								ValidateSemiColonDivide( pArgument, 2 );
							}
							break;

						// 전염병 상태효과(136) - 2개
						case 136:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:상태효과 전염병 상태효과 인자가 잘못되었습니다. 세미콜론(;) 구분으로 (데미지비율(0.0~1.0);전염될 반경)이 맞는지 확인해주십시오.)"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );

								ValidateSemiColonDivide( pArgument, 2 );
							}
							break;

						default:	
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:상태효과 인자가 비어있음}"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );

								//EXPECT_STRNE( "0", pArgument );
								EXPECT_STRNE( "", pArgument );
							}
							break;
					}
				}
			}
		}
	}
}


void ValidateSelfApplySESignal( CEtActionBase* pActor, const vector<int>& vlSkills )
{
	int iNumSkill = (int)vlSkills.size();
	for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
	{
		int iSkillTableID = vlSkills.at(iSkill);

		// 스킬에서 사용하는 자기 자신에게 사용하는 상태효과.
		deque<int> dqSelfApplyStateEffect;

		// 상태효과 데이터 긁어옴. 최대 5개
		bool abUseStateEffect[ MAX_STATE_EFEFCT_COUNT ];
		ZeroMemory( abUseStateEffect, sizeof(abUseStateEffect) );
		for( int iStateEffect = 0; iStateEffect < MAX_STATE_EFEFCT_COUNT; ++iStateEffect )
		{
			char StateEffectName[ 64 ];
			char StateEffectTarget[ 64 ];
			sprintf_s( StateEffectName, "_EffectClass%d", iStateEffect+1 );
			sprintf_s( StateEffectTarget, "_EffectClass%dApplyType", iStateEffect+1 );
			int iStateEffectIndex = SKILL_T->GetFieldFromLablePtr( iSkillTableID, StateEffectName )->GetInteger();
			if( iStateEffectIndex != 0 )
			{
				//자기 자신에게 사용하는 상태효과만 골라냄 (CDnSkill::StateEffectApplyType::ApplySelf or CDnSkill::StateEffectApplyType::ApplyAll)
				if( SKILL_T->GetFieldFromLablePtr( iSkillTableID, StateEffectTarget )->GetInteger() == 0 ||
					SKILL_T->GetFieldFromLablePtr( iSkillTableID, StateEffectTarget )->GetInteger() == 2 )
				{
					abUseStateEffect[ iStateEffect ] = true;
					dqSelfApplyStateEffect.push_back( iStateEffectIndex );
				}
			}
		}
		
		int aiUseProcessorType[ 5 ];
		ZeroMemory( aiUseProcessorType, sizeof(aiUseProcessorType) );
		for( int iProcessor = 0; iProcessor < MAX_PROCESSOR; ++iProcessor )
		{
			char acBuf[ 64 ];
			sprintf_s( acBuf, "_Processor%d", iProcessor+1 );
			int iUseType = SKILL_T->GetFieldFromLablePtr( iSkillTableID, acBuf )->GetInteger();
			aiUseProcessorType[ iProcessor ] = iUseType;
		}

		vector<int> vlSkillLevelList;
		if( SKILL_LEVEL_T->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) == 0 ) 
			FAIL();

		int iNumSkillLevel = (int)vlSkillLevelList.size();
		if( 0 == iNumSkillLevel ) 
			continue;

		for( int iLevel = 0; iLevel < 1; ++iLevel )					// 이건 스킬 종류 기준으로 검증하기 때문에 레벨별로 검증할 필요 없다.
		{
			bool abUseStateEffectInLevel[ MAX_STATE_EFEFCT_COUNT ];
			memcpy( abUseStateEffectInLevel, abUseStateEffect, sizeof(abUseStateEffect) );

			int iNowProcessor = 0;
			int iSkillLevelID = vlSkillLevelList.at( iLevel );
			for( int iArgument = 0; iArgument < 10; )
			{
				if( MAX_STATE_EFEFCT_COUNT == iNowProcessor )
					break;

				if( 0 == aiUseProcessorType[ iNowProcessor ] )
				{
					++iNowProcessor;
					continue;
				}

				int iNumArgument = 0;
				switch( aiUseProcessorType[ iNowProcessor ] )
				{
						// playani 인자는 1개
					case 1:
						{
							iNumArgument += 1;

							char acBuf[ 128 ];
							sprintf_s( acBuf, "_ProcessParam%d", iArgument+1 );
							const char* pArgument = SKILL_LEVEL_T->GetFieldFromLablePtr( iSkillLevelID, acBuf )->GetString();						
							CEtActionBase::ActionElementStruct* pActionElement = pActor->GetElement( pArgument );
							//EXPECT_TRUE( NULL != pActionElement );
							if( NULL == pActionElement )
								break;

							TCHAR atcBuf[ 512 ];
							int iApplyStateEffectSignalCount = 0;
							bool bHasApplySelfStateSignal = false;
							int iNumSignal = (int)pActionElement->pVecSignalList.size();
							for( int iSignal = 0; iSignal < iNumSignal; ++iSignal )
							{
								// 자신에게 사용하는 상태효과 타이밍 시그널이 하나도 없으면 시작하자마자 적용되는 것이므로 통과.
								CEtActionSignal* pSignal = pActionElement->pVecSignalList.at( iSignal );
								if( STE_ApplyStateEffect == pSignal->GetSignalIndex() )
								{
									++iApplyStateEffectSignalCount;
									bHasApplySelfStateSignal = true;
									void* pData = pSignal->GetData();
									ApplyStateEffectStruct* pSESignal = static_cast<ApplyStateEffectStruct*>(pData);
						
									{
										_stprintf_s( atcBuf, _T("{TableName:SkillTable, Index:%d, SkillLevel:%d, 입력값:%d, Note:자신에게 사용하는 상태효과 최대 상태효과 인덱스 벗어남.}"), iSkillTableID, iLevel+1, pSESignal->StateEffectIndex );
										SCOPED_TRACE( atcBuf );
										EXPECT_LT( pSESignal->StateEffectIndex, MAX_STATE_EFEFCT_COUNT );
									}
									
									{
										_stprintf_s( atcBuf,_T("{TableName:SkillTable, Index:%d, SkillLevel:%d, 입력값:%d, Note:자신에게 사용하는 상태효과 입력된 상태효과 갯수보다 큰 인덱스임.}"), iSkillTableID, iLevel+1, pSESignal->StateEffectIndex );
										SCOPED_TRACE( atcBuf );
										EXPECT_LT( pSESignal->StateEffectIndex, (int)dqSelfApplyStateEffect.size() );
									}


									{
										_stprintf_s( atcBuf,_T("{TableName:SkillTable, Index:%d, SkillLevel:%d, 입력값:%d, Note:자기 자신에게 사용하는 상태효과 인덱스 범위 벗어남.}"), iSkillTableID, iLevel+1, pSESignal->StateEffectIndex );
										SCOPED_TRACE( atcBuf );
										EXPECT_EQ( true, abUseStateEffectInLevel[pSESignal->StateEffectIndex] );
									}

									if( abUseStateEffectInLevel[ pSESignal->StateEffectIndex ] )
										abUseStateEffectInLevel[ pSESignal->StateEffectIndex ] = false;
								}
							}

							if( bHasApplySelfStateSignal )
							{
								_stprintf_s( atcBuf,_T("{TableName:SkillTable, Index:%d, SkillLevel:%d, Note:자기 자신에게 사용하는 상태효과 인덱스를 모두 적용하지 않음.}"), iSkillTableID, iLevel+1 );
								//_stprintf_s( atcBuf,_T("[자신에게 사용하는 상태효과 SkillID: %d, SkillLevel: %d] 자기 자신에게 사용하는 상태효과 인덱스를 모두 적용하지 않음."), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );
								EXPECT_EQ( iApplyStateEffectSignalCount, (int)dqSelfApplyStateEffect.size() );
							}

						}
						break;

						// timeplay 인자는 5개
					case 2:
						iNumArgument += 5;
						break;

						// changeproj 인자는 2개
					case 3:
						iNumArgument += 2;
						break;

						// divideseargbytargetnum 인자는 1개
					case 4:
						iNumArgument += 1;
						break;

						// impactblow 인자 없음
					case 5:
						iNumArgument += 0;
						break;

						// aura 인자는 1개
					case 6:
						iNumArgument += 1;
						break;

						// change action 현재는 인자 7개임
					case 7:
						iNumArgument += 7;
						break;

					default:
						break;
				}

				++iNowProcessor;
			}
		}
	}
}


// 자기 자신에게 사용하는 상태효과가 액션툴에 잘 셋팅이 되어있는지.
TEST_F( skill_data_test, SELF_STATE_EFFECT_SIGNAL_VERIFY )
{
	// 각 플레이어 캐릭터 별로 확인.
	// 해당 직업에 맞으며 PlayAni 발현 타입이 있는 스킬들중에,
	// 워리어는 스킬 아이디 0~
	// 아쳐는 1000~
	// 소서리스는 2000~
	// 클러릭은 3000~
	vector<int> vlPlayAniSkills[ 4 ];
	int iNowPlayerTypeIndex = 0;
	int iNumItemCount = SKILL_T->GetItemCount();
	for( int iSkill = 0; iSkill < iNumItemCount; ++iSkill )
	{
		int iSkillTableID = SKILL_T->GetItemID( iSkill );
		iNowPlayerTypeIndex = iSkillTableID / 1000;

		if( 4 <= iNowPlayerTypeIndex )
			break;

		// PlayAni 발현 타입이 있는 경우.
		int aiUseProcessorType[ 5 ];
		ZeroMemory( aiUseProcessorType, sizeof(aiUseProcessorType) );
		for( int iProcessor = 0; iProcessor < MAX_PROCESSOR; ++iProcessor )
		{
			char acBuf[ 64 ];
			sprintf_s( acBuf, "_Processor%d", iProcessor+1 );
			int iUseType = SKILL_T->GetFieldFromLablePtr( iSkillTableID, acBuf )->GetInteger();

			if( 1 == iUseType )
				vlPlayAniSkills[iNowPlayerTypeIndex].push_back( iSkillTableID );
		}
	}

	ValidateSelfApplySESignal( skill_data_test::s_pWarrior, vlPlayAniSkills[ 0 ] );
	ValidateSelfApplySESignal( skill_data_test::s_pArcher, vlPlayAniSkills[ 1 ] );	
	ValidateSelfApplySESignal( skill_data_test::s_pSoceress, vlPlayAniSkills[ 2 ] );
	ValidateSelfApplySESignal( skill_data_test::s_pCleric, vlPlayAniSkills[ 3 ] );
}

TEST_F( skill_data_test, SKILLBOOK_ITEM_ID_VERIFY )
{
	// 모든 플레이어 스킬들의 루프를 돌면서 잘못된 스킬북 itemid 를 적어놓은 것이 있나 확인한다.
	// 플레이어 스킬 ID 는 4000 이하이므로 4000 까지만 돈다.
	TCHAR atcBuf[ 512 ];
	for( int i = 0; i < SKILL_T->GetItemCount(); ++i )
	{
		int iSkillID = SKILL_T->GetItemID( i );
		if( iSkillID > 4000 )
			break;

		int iSkillBookItemID = SKILL_T->GetFieldFromLablePtr( iSkillID, "_UnlockSkillBookItemID" )->GetInteger();
		if( 0 != iSkillBookItemID )
		{
			set<int>::iterator iter = skill_data_test::s_setSkillBookItemID.find( iSkillBookItemID );
			if( skill_data_test::s_setSkillBookItemID.end() != iter )
				skill_data_test::s_setSkillBookItemID.erase( iter );
			else
			{
				// 진짜로 해당 스킬을 푸는 스킬 북이 없을 수도 있지만 만약 아이템 테이블에 있다면 
				// 스킬 테이블에서 같은 스킬북 아이템 쓰는 스킬이 두 개 이상 중복되어있다는 이야기.
				_stprintf_s( atcBuf,_T("{TableName:SkillTable, Index:%d, Note:해당 스킬을 푸는 스킬북이 없습니다.}"), iSkillID );
				SCOPED_TRACE( atcBuf );
				EXPECT_TRUE( false );
			}
		}
	}

	for( set<int>::iterator iter = skill_data_test::s_setSkillBookItemID.begin(); 
		iter != skill_data_test::s_setSkillBookItemID.end(); ++iter )
	{
		_stprintf_s( atcBuf,_T("{TableName:ItemTable, Index:%d, Note:사용되지 않는 스킬북 아이템 ID.}"), *iter );
		SCOPED_TRACE( atcBuf );
		EXPECT_TRUE( false );
	}
}