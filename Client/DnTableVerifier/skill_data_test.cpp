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

		// ��ų�� Ÿ���� 10
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

	// ���α׷� ����� �� ActionCoreMng ���� ���� �Ф�
	// �ϴ� delete ���� �ʰ� ����..
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

		// 1�� �нú� ��ų��
		if( 1 == iSkillType )
		{
			int iProcessor = -1;

			for( int i = 0; i < MAX_PROCESSOR; ++i )
			{
				sprintf_s( acLabel, "_Processor%d", i + 1 );
				iProcessor = SKILL_T->GetFieldFromLablePtr( iSkillTableID, acLabel )->GetInteger();

				// 1 ���� PlayAni
				ZeroMemory( atcBuf, sizeof(atcBuf) );
				_stprintf_s( atcBuf, _T("{TableName:SkillTable, Index:%d, Note:�нú� ��ų�� �׼������� ���� InputHasPassiveSkill �� ó���˴ϴ�...}"), iSkillTableID );

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


// PlayAni ���� Ÿ�Կ� ���ڰ� ����� ���� ���
TEST_F( skill_data_test, PROCESSOR_ARGUMENT_VERIFY )
{
	const char* apProcessTypeName [] = { "PlayAni", "TimePlay", "ChangePrj", "DivideSEArgByTargetNum", "ImpactBlow", "Aura", "ChangeAction", "RangeStateEffect" };
	int iNumItemCount = SKILL_T->GetItemCount();
	for( int iSkill = 0; iSkill < iNumItemCount; ++iSkill )
	{
		int iSkillTableID = SKILL_T->GetItemID( iSkill );

		// PlayAni ���� Ÿ���� �ִ� ���.
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
			bool bChangeActionStr = false;			// �׼� ���ڿ� ���� ����Ÿ���� �������ڱ� ������ ���� ������ �ȵ�.
			int iNowProcessor = 0;
			int iSkillLevelID = vlSkillLevelList.at( iLevel );
			for( int iArgument = 0; iArgument < 10; )
			{
				// ����Ÿ�� ���� �ִ� ������ 5����.
				if( MAX_PROCESSOR == iNowProcessor )
					break;

				// NONE ���� ���õǾ� ������ ���� �ε����� �н�
				if( 0 == aiUseProcessorType[ iNowProcessor ] )
				{
					++iNowProcessor;
					continue;
				}

				TCHAR atcTraceBuf[ 1024 ];

				if( bChangeActionStr )
				{
					_stprintf_s( atcTraceBuf, _T("{TableName:SkillTable, Index:%d, Level:%d, Note:ChangeActionStr ���� Ÿ�� ������ �ٸ� ����Ÿ���� ���� �ȵ˴ϴ�.}"), iSkillTableID, iLevel+1 );
					EXPECT_TRUE( bChangeActionStr );
				}

				ZeroMemory( atcTraceBuf, sizeof(atcTraceBuf) );
				const char* pNowProcessorTypeName = NULL;
				int iNumArgument = 0;
				switch( aiUseProcessorType[ iNowProcessor ] )
				{
						// playani ���ڴ� 1��
					case 1:
						iNumArgument = 1;
						pNowProcessorTypeName = apProcessTypeName[ 0 ];
						break;

						// timeplay ���ڴ� 5��
					case 2:
						iNumArgument = 5;
						pNowProcessorTypeName = apProcessTypeName[ 1 ];
						break;

						// changeproj ���ڴ� 2��
					case 3:
						iNumArgument = 2;
						pNowProcessorTypeName = apProcessTypeName[ 2 ];
						break;

						// divideseargbytargetnum ���ڴ� 1��
					case 4:
						iNumArgument = 1;
						pNowProcessorTypeName = apProcessTypeName[ 3 ];
						break;

						// impactblow ���� ����
					case 5:
						iNumArgument = 0;
						pNowProcessorTypeName = apProcessTypeName[ 4 ];
						break;

						// aura ���ڴ� 1��
					case 6:
						iNumArgument = 1;
						pNowProcessorTypeName = apProcessTypeName[ 5 ];
						break;

						// change action ���ڴ� ������ ���� ����.
						// ���ڰ� �����̱� ������ ���� ������ �ڿ� �ٸ� ���� Ÿ���� ������ �ȵȴ�.
					case 7:
						bChangeActionStr = true;
						//iNumArgument = 7;
						//pNowProcessorTypeName = apProcessTypeName[ 6 ];
						break; 

						// RangeStateEffect ���ڴ� 4��
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
					_stprintf_s( atcTraceBuf, _T("{TableName:SkillTable, Index:%d, Level:%d, Note:����Ÿ��(%s) �����ε���(%d) ���ڰ��� �����ϴ�.}"), iSkillTableID, iLevel+1, atcProcessorTypeName, iArgument+1 );
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
			bInvalid = ( string::npos == iSemiColonIndex );						// �߰� �ε������� �����ݷ� �ε����� ��ȿ�ؾ��ϰ�,
		else
			bInvalid = (strArgument.at( strArgument.length()-1 ) == ';');		// ������ �ε������� �� ���� ; �� ���������� �ȵȴ�.

		EXPECT_FALSE( bInvalid );
		if( bInvalid )
			break;
	}
}



// ����ȿ���� ���ڰ� ���� ���..
TEST_F( skill_data_test, STATE_EFFECT_ARGUMENT_VERIFY )
{
	int iNumItemCount = SKILL_T->GetItemCount();
	for( int iSkill = 0; iSkill < iNumItemCount; ++iSkill )
	{
		int iSkillTableID = SKILL_T->GetItemID( iSkill );

		// ����ȿ���� �ִ� ���... �ִ� 5��
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

					// ���� �ݷ� �� ����ȿ���� ����� ���� ������ �´��� Ȯ���Ѵ�.
					switch( iStateEffectIndex )
					{
						 // ���(41) - 2��
						case 41:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:��� ����ȿ�� ���ڰ� �߸��Ǿ����ϴ�. �����ݷ�(;) �������� (���Ȯ��(0.0~1.0);������Ƚ��) �� �´��� Ȯ�����ֽʽÿ�.}"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );

								ValidateSemiColonDivide( pArgument, 2 );
							}
							break;
						
						// ü�ΰ���(60) - 3��
						case 60:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:ü�ΰ��� ����ȿ�� ���ڰ� �߸��Ǿ����ϴ�. �����ݷ�(;) �������� (����;�ִ���Ʈī��Ʈ;�������������)�� �´��� Ȯ�����ֽʽÿ�.}"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );
								
								ValidateSemiColonDivide( pArgument, 3 );
							}
							break;

						// ���̹� ����(68) - 2��
						case 68:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:���̹� ���� ����ȿ�� ���ڰ� �߸��Ǿ����ϴ�. �����ݷ�(;) �������� (����Ȯ��(0.0~1.0);�Ҹ�MP ���Һ���(0.0-1.0)) �� �´��� Ȯ�����ֽʽÿ�.}"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );

								ValidateSemiColonDivide( pArgument, 2 );
							}
							break;

						// ����ȿ�� �鿪(77) - 2�� 
						case 77:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:����ȿ�� �鿪 ����ȿ�� ���ڰ� �߸��Ǿ����ϴ�. �����ݷ�(;) �������� (�鿪�� ����ȿ�� �ε���;�鿪Ȯ��(0.0~1.0) �� �´��� Ȯ�����ֽʽÿ�.)"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );

								ValidateSemiColonDivide( pArgument, 2 );
							}
							break;

						// ������ ����ȿ��(136) - 2��
						case 136:
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:����ȿ�� ������ ����ȿ�� ���ڰ� �߸��Ǿ����ϴ�. �����ݷ�(;) �������� (����������(0.0~1.0);������ �ݰ�)�� �´��� Ȯ�����ֽʽÿ�.)"), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );

								ValidateSemiColonDivide( pArgument, 2 );
							}
							break;

						default:	
							{
								_stprintf_s( atcBuf, _T("{TableName:SkillLevelTable, Index:%d, Level:%d, Note:����ȿ�� ���ڰ� �������}"), iSkillTableID, iLevel+1 );
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

		// ��ų���� ����ϴ� �ڱ� �ڽſ��� ����ϴ� ����ȿ��.
		deque<int> dqSelfApplyStateEffect;

		// ����ȿ�� ������ �ܾ��. �ִ� 5��
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
				//�ڱ� �ڽſ��� ����ϴ� ����ȿ���� ��� (CDnSkill::StateEffectApplyType::ApplySelf or CDnSkill::StateEffectApplyType::ApplyAll)
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

		for( int iLevel = 0; iLevel < 1; ++iLevel )					// �̰� ��ų ���� �������� �����ϱ� ������ �������� ������ �ʿ� ����.
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
						// playani ���ڴ� 1��
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
								// �ڽſ��� ����ϴ� ����ȿ�� Ÿ�̹� �ñ׳��� �ϳ��� ������ �������ڸ��� ����Ǵ� ���̹Ƿ� ���.
								CEtActionSignal* pSignal = pActionElement->pVecSignalList.at( iSignal );
								if( STE_ApplyStateEffect == pSignal->GetSignalIndex() )
								{
									++iApplyStateEffectSignalCount;
									bHasApplySelfStateSignal = true;
									void* pData = pSignal->GetData();
									ApplyStateEffectStruct* pSESignal = static_cast<ApplyStateEffectStruct*>(pData);
						
									{
										_stprintf_s( atcBuf, _T("{TableName:SkillTable, Index:%d, SkillLevel:%d, �Է°�:%d, Note:�ڽſ��� ����ϴ� ����ȿ�� �ִ� ����ȿ�� �ε��� ���.}"), iSkillTableID, iLevel+1, pSESignal->StateEffectIndex );
										SCOPED_TRACE( atcBuf );
										EXPECT_LT( pSESignal->StateEffectIndex, MAX_STATE_EFEFCT_COUNT );
									}
									
									{
										_stprintf_s( atcBuf,_T("{TableName:SkillTable, Index:%d, SkillLevel:%d, �Է°�:%d, Note:�ڽſ��� ����ϴ� ����ȿ�� �Էµ� ����ȿ�� �������� ū �ε�����.}"), iSkillTableID, iLevel+1, pSESignal->StateEffectIndex );
										SCOPED_TRACE( atcBuf );
										EXPECT_LT( pSESignal->StateEffectIndex, (int)dqSelfApplyStateEffect.size() );
									}


									{
										_stprintf_s( atcBuf,_T("{TableName:SkillTable, Index:%d, SkillLevel:%d, �Է°�:%d, Note:�ڱ� �ڽſ��� ����ϴ� ����ȿ�� �ε��� ���� ���.}"), iSkillTableID, iLevel+1, pSESignal->StateEffectIndex );
										SCOPED_TRACE( atcBuf );
										EXPECT_EQ( true, abUseStateEffectInLevel[pSESignal->StateEffectIndex] );
									}

									if( abUseStateEffectInLevel[ pSESignal->StateEffectIndex ] )
										abUseStateEffectInLevel[ pSESignal->StateEffectIndex ] = false;
								}
							}

							if( bHasApplySelfStateSignal )
							{
								_stprintf_s( atcBuf,_T("{TableName:SkillTable, Index:%d, SkillLevel:%d, Note:�ڱ� �ڽſ��� ����ϴ� ����ȿ�� �ε����� ��� �������� ����.}"), iSkillTableID, iLevel+1 );
								//_stprintf_s( atcBuf,_T("[�ڽſ��� ����ϴ� ����ȿ�� SkillID: %d, SkillLevel: %d] �ڱ� �ڽſ��� ����ϴ� ����ȿ�� �ε����� ��� �������� ����."), iSkillTableID, iLevel+1 );
								SCOPED_TRACE( atcBuf );
								EXPECT_EQ( iApplyStateEffectSignalCount, (int)dqSelfApplyStateEffect.size() );
							}

						}
						break;

						// timeplay ���ڴ� 5��
					case 2:
						iNumArgument += 5;
						break;

						// changeproj ���ڴ� 2��
					case 3:
						iNumArgument += 2;
						break;

						// divideseargbytargetnum ���ڴ� 1��
					case 4:
						iNumArgument += 1;
						break;

						// impactblow ���� ����
					case 5:
						iNumArgument += 0;
						break;

						// aura ���ڴ� 1��
					case 6:
						iNumArgument += 1;
						break;

						// change action ����� ���� 7����
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


// �ڱ� �ڽſ��� ����ϴ� ����ȿ���� �׼����� �� ������ �Ǿ��ִ���.
TEST_F( skill_data_test, SELF_STATE_EFFECT_SIGNAL_VERIFY )
{
	// �� �÷��̾� ĳ���� ���� Ȯ��.
	// �ش� ������ ������ PlayAni ���� Ÿ���� �ִ� ��ų���߿�,
	// ������� ��ų ���̵� 0~
	// ���Ĵ� 1000~
	// �Ҽ������� 2000~
	// Ŭ������ 3000~
	vector<int> vlPlayAniSkills[ 4 ];
	int iNowPlayerTypeIndex = 0;
	int iNumItemCount = SKILL_T->GetItemCount();
	for( int iSkill = 0; iSkill < iNumItemCount; ++iSkill )
	{
		int iSkillTableID = SKILL_T->GetItemID( iSkill );
		iNowPlayerTypeIndex = iSkillTableID / 1000;

		if( 4 <= iNowPlayerTypeIndex )
			break;

		// PlayAni ���� Ÿ���� �ִ� ���.
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
	// ��� �÷��̾� ��ų���� ������ ���鼭 �߸��� ��ų�� itemid �� ������� ���� �ֳ� Ȯ���Ѵ�.
	// �÷��̾� ��ų ID �� 4000 �����̹Ƿ� 4000 ������ ����.
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
				// ��¥�� �ش� ��ų�� Ǫ�� ��ų ���� ���� ���� ������ ���� ������ ���̺� �ִٸ� 
				// ��ų ���̺��� ���� ��ų�� ������ ���� ��ų�� �� �� �̻� �ߺ��Ǿ��ִٴ� �̾߱�.
				_stprintf_s( atcBuf,_T("{TableName:SkillTable, Index:%d, Note:�ش� ��ų�� Ǫ�� ��ų���� �����ϴ�.}"), iSkillID );
				SCOPED_TRACE( atcBuf );
				EXPECT_TRUE( false );
			}
		}
	}

	for( set<int>::iterator iter = skill_data_test::s_setSkillBookItemID.begin(); 
		iter != skill_data_test::s_setSkillBookItemID.end(); ++iter )
	{
		_stprintf_s( atcBuf,_T("{TableName:ItemTable, Index:%d, Note:������ �ʴ� ��ų�� ������ ID.}"), *iter );
		SCOPED_TRACE( atcBuf );
		EXPECT_TRUE( false );
	}
}