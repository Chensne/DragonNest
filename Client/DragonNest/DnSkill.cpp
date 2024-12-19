#include "StdAfx.h"
#include "DnSkill.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "IDnSkillUsableChecker.h"
#include "IDnSkillProcessor.h"
#include "DnLocalPlayerActor.h"
#include "DnBlow.h"
#include "DnRangeChecker.h"
#include "EtActionSignal.h"
#include "DnPlayAniProcess.h"
#include "DnPartialPlayProcessor.h"
#include "TaskManager.h"
#include "DnUIString.h"
#include "DnStateBlow.h"
#include "DnWorld.h"
#include "DnPvPGameTask.h"
#include "DnPingpongBlow.h"
#include "DnAllowedSkillsBlow.h"
#include "DnChangeActionStrByBubbleProcessor.h"
#include "DnChangeActionStrProcessor.h"
#include "DnSkillTask.h"
#include "DnMonsterActor.h"
#include "DnBasicBlow.h"
#include "DnCreateBlow.h"
#include "DnHealingBlow.h"
#include "DnHPIncBlow.h"
#include "DnTransformBlow.h"
#include "DnBloodSuckingBlow.h"
#include "DnOrderMySummonedMonsterBlow.h"

#if defined(PRE_FIX_46381)
#include "DnContinueBaseMPIncBlow.h"
#endif // PRE_FIX_46381

#include "DnPetTask.h"

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#include "TotalLevelSkillSystem.h"
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#include "DnDisableSkillBlow.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif


DECL_SMART_PTR_STATIC( CDnSkill, 200 )

CDnSkill::CDnSkill( DnActorHandle hActor ) : MIInventoryItem( MIInventoryItem::Skill ),
											 m_hActor( hActor ),
											 m_LastTimeToggleMPDecreaseTime( 0 ),
											 m_bItemSkill( false ),
											 m_fPassiveActionSkillLength( 0.0f ),
											 m_eElement( CDnState::ElementEnum_Amount ),
											 m_iNextLevelSkillPoint( -1 ),
											 m_iFirstLevelSkillPoint( -1 ),
											 m_bAppliedPassiveSelfBlows( false ),
											 m_bTempSkill( false ),

//--------------------------------------------------------------
											 //[debug_skill]
											 m_nMaxUseNum(3),
                                             m_nNowNum(0)
//--------------------------------------------------------------

{
	memset( m_iNeedItemID, 0, sizeof(m_iNeedItemID) );
	memset( m_iNeedItemDecreaseCount, 0, sizeof(m_iNeedItemDecreaseCount) );
	memset( m_fHPConsumeType, 0, sizeof(m_fHPConsumeType) );
	memset( m_fMPConsumeType, 0, sizeof(m_fMPConsumeType) );
	memset( m_iNeedHP, 0, sizeof(m_iNeedHP) );
	memset( m_iNeedMP, 0, sizeof(m_iNeedMP) );
	memset( m_iIncreaseRange, 0, sizeof(m_iIncreaseRange) );
	memset( m_iDecreaseHP, 0, sizeof(m_iDecreaseHP) );
	memset( m_iDecreaseMP, 0, sizeof(m_iDecreaseMP) );
	memset( m_fOriginalDelayTime, 0, sizeof(m_fOriginalDelayTime) );
	memset( m_fDelayTime, 0, sizeof(m_fDelayTime) );

	m_bLock = false;
	m_bAcquired = false;
	m_bAcquiredByBlow = false;
	m_iSkillID = 0;
	m_iSkillLevelID = 0;

	m_iSkillExplainStringID = 0;
	m_iNextSkillExplainStringID = 0;

	m_iDissolvable = 0;
	m_iDuplicateCount = 0;
	m_iSkillDuplicateMethod = 0;
	m_iEffectDuplicateMethod = 0;

	m_iLevel = 0;
	m_iMaxLevel = 0;
	m_iUsePoint = 0;

	m_eSkillType = SkillTypeEnum::Active;
	m_eDurationType = DurationTypeEnum::Instantly;
	m_eTargetType = TargetTypeEnum::Self;
	m_iLevelLimit = 0;
	m_iFirstLevelLimit = 0;
	m_fLeftDelayTime = 0.f;
	m_fCoolTime = 0.f;
	m_bNowToggled = false;
	m_bNowAura = false;

	m_iCPScore = 0;

	m_iUnlockSkillBookItemID = 0;

	m_fOnceDelayTime = 0.0f;
	m_fOnceElapsedDelayTime = 0.0f;
	m_bEquipItemSkill = false;

	m_fCoolTimeAdjustBlowValue = 1.0f;

	m_iUnlockPrice = 0;

	m_bNewAcquired = false;

	m_bResetSkill = false;

	m_iExclusiveID = 0;

	m_iSelectedSkillLevelDataApplyType = PVE;		// ����Ʈ�� pve �̴�.

	memset( m_aeNeedEquipType, CDnWeapon::EquipTypeEnum_Amount, sizeof(m_aeNeedEquipType) );

	m_iBaseSkillID = 0;
	m_iAppliedEnchantPassiveSkillID = 0;

	m_fCoolTimeMultipier = 1.0f;

	m_bEnableTooltip = true;

	m_iBuffIconImageIndex = 0;
#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF
	m_iDebuffIconImageIndx = 0;
#endif

	m_isCreatedByItem = false;

	m_nLevelUpValue = 0;

	m_iGlobalSkillGroupID = 0;
	SecureZeroMemory( m_afGlobalCoolTime, sizeof(m_afGlobalCoolTime) );
	m_fAnotherGlobalSkillCoolTime = 0.0f;
	m_iAnotherGlobalSkillLevelDataType = PVE;

	m_nAnotherGlobakSkillID = 0;

	m_bEnableActiveToggleCoolTime = false;

	m_bCantUseInVillage = true;

	m_bGlyphActiveSkillDisbale = false;

#if defined(PRE_FIX_45899)
	m_isDisableSkillByItemMove = false;
#endif // PRE_FIX_45899
	m_SummonMonsterID = 0;
	m_bFinished = false;
	m_iNeedJobClassID = 0;
	memset(&m_iOriginalNeedMP, 0, sizeof(m_iOriginalNeedMP));

#if defined(PRE_FIX_64312)
	m_isAppliedSummonMonsterEnchantSkill = false;
	m_bIsSummonMonsterSkill = false;
#endif // PRE_FIX_64312

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	m_fDeltaGlobalCoolTime = 0.0f;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
}

CDnSkill::~CDnSkill(void)
{
	for( int iSelectedLevelData = PVE; iSelectedLevelData < NUM_SKILLLEVEL_APPLY_TYPE; ++iSelectedLevelData )
	{
		SAFE_DELETE_PVEC( m_vlpUsableCheckers[ iSelectedLevelData ] );
	}

	for( int iSelectedLevelData = PVE; iSelectedLevelData < NUM_SKILLLEVEL_APPLY_TYPE; ++iSelectedLevelData )
	{
		SAFE_DELETE_PVEC( m_vlpProcessors[ iSelectedLevelData ] );
	} 

	for( int iSelectedLevelData = PVE; iSelectedLevelData < NUM_SKILLLEVEL_APPLY_TYPE; ++iSelectedLevelData )
	{
		SAFE_DELETE_PVEC( m_vlpProcessorBackup[ iSelectedLevelData ] );
	}

#if defined(PRE_FIX_66175)
	for( int iSelectedLevelData = PVE; iSelectedLevelData < NUM_SKILLLEVEL_APPLY_TYPE; ++iSelectedLevelData )
	{
		SAFE_DELETE_PVEC( m_vlUsableCheckersBackup[ iSelectedLevelData ] );
	}
#endif // PRE_FIX_66175
}

void CDnSkill::SetHasActor( DnActorHandle hActor )
{
	assert( hActor && "CDnSkill::SetHasActor() ���� �ڵ��� NULL ��" );

	m_hActor = hActor;

	for( int iSelectedLevelData = PVE; iSelectedLevelData < NUM_SKILLLEVEL_APPLY_TYPE; ++iSelectedLevelData )
	{
		int iNumChecker = (int)m_vlpUsableCheckers[ iSelectedLevelData ].size();
		for( int iChecker = 0; iChecker < iNumChecker; ++iChecker )
		{
			IDnSkillUsableChecker* pChecker = m_vlpUsableCheckers[ iSelectedLevelData ].at( iChecker );
			pChecker->SetHasActor( hActor );
		}
	}

	for( int iSelectedLevelData = PVE; iSelectedLevelData < NUM_SKILLLEVEL_APPLY_TYPE; ++iSelectedLevelData )
	{
		int iNumProcessor = (int)m_vlpProcessors[ iSelectedLevelData ].size();
		for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
		{
			IDnSkillProcessor* pProcessor = m_vlpProcessors[ iSelectedLevelData ].at( iProcessor );
			pProcessor->SetHasActor( hActor );

			// ����ϴ� �׼��� ������ �ܺο��� ��ȸ������ ������ ä������
			// ��ų �׼� �̸� ��ü�� ���� pvp/pve ���� ������ pve �� �� �������� �ѹ��� �������ش�.
			if( m_hActor && m_hActor->IsPlayerActor() )
			{
				if( PVE == iSelectedLevelData )
				{
					if( pProcessor->GetType() == IDnSkillProcessor::PLAY_ANI )
						_AddUsedActions( static_cast<CDnPlayAniProcess*>(pProcessor)->GetActionName() );
					else
					if( pProcessor->GetType() == IDnSkillProcessor::PARTIAL_PLAY_ANI )
					{
						CDnPartialPlayProcessor* pPartialPlayAni = static_cast<CDnPartialPlayProcessor*>(pProcessor);
						_AddUsedActions( pPartialPlayAni->GetStartActionName() );
						_AddUsedActions( pPartialPlayAni->GetLoopActionName() );
						_AddUsedActions( pPartialPlayAni->GetEndActionName() );
					}
				}
			}
		}
	}

	// �нú� ��ų�̶�� ���õ� �׼��� �ִ��� ã�Ƽ� �߰�.
	if( m_hActor )
	{
		if( Passive == m_eSkillType )
		{
			m_hActor->FillPassiveSkillActions( m_iSkillID, m_setUseActionNames );
		}
	}

	UpdateRangeChecker( m_iSelectedSkillLevelDataApplyType );

	RefreshDecreaseMP();

	if( 0.0f == m_fHPConsumeType[ m_iSelectedSkillLevelDataApplyType ] )
	{ 
		m_iNeedHP[ m_iSelectedSkillLevelDataApplyType ] = m_iDecreaseHP[ m_iSelectedSkillLevelDataApplyType ];
	}
	else
	{
		if( m_hActor )
			m_iNeedHP[ m_iSelectedSkillLevelDataApplyType ] = int((float)m_hActor->GetMaxHP() * m_fHPConsumeType[ m_iSelectedSkillLevelDataApplyType ]);
	}

	if (m_eDurationType == SummonOnOff)
		OnInitializeSummonMonsterInfo();
}

bool CDnSkill::ValidateSkillData( int iSkillTableID, int iLevel )
{
	return true;
}

// static ���丮 �Լ�
DnSkillHandle CDnSkill::CreateSkill( DnActorHandle hActor, int iSkillTableID, int iLevel )
{
	if( false == CDnSkillTask::IsActive() )
		return DnSkillHandle();

#if defined(_DEBUG)
	if( false == ValidateSkillData( iSkillTableID, iLevel ) )
		return DnSkillHandle();
#endif

	// ���� 0���� ���� ��û ������ ����� �� ��ȹ�� ��ų��.
	bool bAcquired = (iLevel != 0);
	if( false == bAcquired )
		iLevel = 1;		// ���� 0���δ� ������ �� �����Ƿ� 1�� ����

	CDnSkill* pNewSkill = NULL;

	// ��ų �ߵ� ����, �ߵ� ���μ������� �޾��ش�.
	// ���� 5���� �ְ� �Ķ���ʹ� ���� ���� 10����. ����� ���ɼ��� �ִ�.
	pNewSkill = new CDnSkill( hActor );

	// Initialize 
	bool bResult = pNewSkill->Initialize( iSkillTableID, iLevel );
	if( false == bResult )
	{
		SAFE_DELETE( pNewSkill );
		return CDnSkill::Identity();
	}

	//pNewSkill->SetSkillLevelIDOffset( iSkillLevelTableIDOffset );

	if( bAcquired )
	{
		pNewSkill->SetHasActor( hActor );
		pNewSkill->Acquire();
	}

	return pNewSkill->GetMySmartPtr();
}

void CDnSkill::UpdateRangeChecker( int iSkillLevelDataApplyType )
{
	// �׼� ���μ����� ������ ����ϴ� �׼�.. �� ����ϰ� ���� �޴°� ������. �Ф�
	if( !m_hActor )
		return;

	if( false == m_strAction.empty() )
	{
		int iNumChecker = (int)m_vlpUsableCheckers[ iSkillLevelDataApplyType ].size();
		for( int iChecker = 0; iChecker < iNumChecker; ++iChecker )
		{
			IDnSkillUsableChecker* pChecker = m_vlpUsableCheckers[ iSkillLevelDataApplyType ].at( iChecker );
			if( IDnSkillUsableChecker::RANGE_CHECKER == pChecker->GetType() )
			{
				CEtActionBase::ActionElementStruct* pElement = m_hActor->GetElement( m_strAction.c_str() );
				if( pElement )
				{
					// �ñ׳��� ������ ������Ÿ���� �ִ��� �˻�
					int iNumSignals = (int)pElement->pVecSignalList.size();
					for( int iSignal = 0; iSignal < iNumSignals; ++iSignal )
					{
						CEtActionSignal* pSignal = pElement->pVecSignalList.at(iSignal);
						if( STE_Projectile == (SignalTypeEnum)pSignal->GetSignalIndex() )
						{
							// �ڽ��� ���� ��Ÿ��� ���ؼ� ���� ����
							ProjectileStruct* pSignalData = static_cast<ProjectileStruct*>(pSignal->GetData());

							// ���� ���̺��� ��Ÿ� ���� ���� ��	
							DNTableFileFormat*  pWeaponTable = GetDNTable( CDnTableDB::TWEAPON );
							int iLength = pWeaponTable->GetFieldFromLablePtr( pSignalData->nWeaponTableID, "_Length" )->GetInteger();

							if( pSignalData->bIncludeMainWeaponLength )
							{
								DnWeaponHandle hWeapon = m_hActor->GetWeapon( 0 );
								if( hWeapon )
									iLength += hWeapon->GetWeaponLength();
							}

							// ��ų ��Ÿ����� ������
							iLength += m_iIncreaseRange[ iSkillLevelDataApplyType ];

							static_cast<CDnRangeChecker*>(pChecker)->SetRange( iLength );
							break;
						}
					}
				}

			}
		}
	}
}

bool CDnSkill::_LoadSkillLevelData( int iSkillTableID, int iLevel, int iSkillLevelDataApplyType )
{
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

#if defined(PRE_FIX_NEXTSKILLINFO)
	//��ų ���� ���̺� ����Ÿ�� �ִ� �� Ȯ��..
	{
		vector<int> vlSkillLevelList;
		if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) == 0 ) 
			return false;

		if ( vlSkillLevelList.empty() )
			return false;
	}

	SKILL_LEVEL_INFO* pTableInfo = GetSkillTask().GetSkillLevelTableIDList(iSkillTableID, iSkillLevelDataApplyType);

	// �ش� ����� �����Ͱ� ���� ��쿣 �׳� �ƹ��͵� ���ϰ� �����ϸ� �ȴ�.
	if (pTableInfo->_SkillLevelTableIDs.empty())
		return true;

	int iSkillLevelTableID = -1;
	//NextLevel�� �������� ���� ���� �� �ִ�.. �׷��� ���⼭ ���� ���� ����Ÿ ���̺� ID�� ã�� ���´�.
	int iMinSkillLevelTableID = -1;
	int iNextSkillLevelTableID = -1;

	//���� ������ ���̺� ID�� �˻�.
	SKILL_LEVEL_TABLE_IDS::iterator findIter = pTableInfo->_SkillLevelTableIDs.find(iLevel);
	if (findIter != pTableInfo->_SkillLevelTableIDs.end())
		iSkillLevelTableID = findIter->second;

	//���� ����
	findIter = pTableInfo->_SkillLevelTableIDs.find(iLevel+1);
	if (findIter != pTableInfo->_SkillLevelTableIDs.end())
		iNextSkillLevelTableID = findIter->second;

	//�ּ� ����
	findIter = pTableInfo->_SkillLevelTableIDs.find(pTableInfo->_MinLevel);
	if (findIter != pTableInfo->_SkillLevelTableIDs.end())
		iMinSkillLevelTableID = findIter->second;

	this->SetSkillLevelIDOffset( iMinSkillLevelTableID, iSkillLevelDataApplyType );

#else
	
	vector<int> vlSkillLevelList;
	if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) == 0 ) 
		return false;

	if ( vlSkillLevelList.empty() )
		return false;

	// pve, pvp ������� Ȯ���Ͽ� �ɷ���.
	vector<int>::iterator iterLevelList = vlSkillLevelList.begin();
	for( iterLevelList; iterLevelList != vlSkillLevelList.end(); )
	{
		int iSkillLevelTableID = *iterLevelList;
		int iApplyType = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_ApplyType" )->GetInteger();
		if( iApplyType != iSkillLevelDataApplyType )
			iterLevelList = vlSkillLevelList.erase( iterLevelList );
		else
			++iterLevelList;
	}

	// �ش� ����� �����Ͱ� ���� ��쿣 �׳� �ƹ��͵� ���ϰ� �����ϸ� �ȴ�.
	if( vlSkillLevelList.empty() )
		return true;

	int iSkillLevelTableIDOffset = vlSkillLevelList.front();
	this->SetSkillLevelIDOffset( iSkillLevelTableIDOffset, iSkillLevelDataApplyType );

	int iSkillLevelTableID = -1;
	for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
	{
		int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();
		if( iNowLevel == iLevel )
		{
			iSkillLevelTableID = vlSkillLevelList.at( i );
			break;
		}
	}
#endif // PRE_FIX_NEXTSKILLINFO

	if( -1 == iSkillLevelTableID )
		return false;

	// ��ų �ߵ� ����, �ߵ� ���μ������� �޾��ش�.
	// ���� 5���� �ְ� �Ķ���ʹ� ���� ���� 10����. ����� ���ɼ��� �ִ�.
	char caLabel[ 32 ];
	int iCheckerParamOffset = 0;
	int iProcessorParamOffset = 0;
	for( int i = 0; i < MAX_PROCESSOR_COUNT; ++i )
	{
		// �ߵ����� ��ü �̸��� ã�´�. �Ķ���� �ʵ尡 ��������� ���� �Լ��鿡�� NULL ���ϵ�
		sprintf_s( caLabel, "_UsableChecker%d", i + 1 );
		int iUsableChecker = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLabel )->GetInteger();

		sprintf_s( caLabel, "_Processor%d", i + 1 );
		int iProcessor = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLabel )->GetInteger();

		int iOffsetCheck = iCheckerParamOffset;
		IDnSkillUsableChecker* pUsableChecker = IDnSkillUsableChecker::Create( m_hActor, iUsableChecker, iSkillLevelTableID, &iCheckerParamOffset );

		if( NULL != pUsableChecker )
		{
			if( (iCheckerParamOffset - iOffsetCheck) != pUsableChecker->GetNumArgument() )
			{
				OutputDebug( "[SkillLevelTable Error!] %d �� �Ķ���� ������ �߸��Ǿ����ϴ�.\n", iSkillLevelTableID );
				_ASSERT( !"��ų ���� ���̺� �Ķ���� �߸���. OutputDebug ��� Ȯ��!" );
			}

			this->AddUsableCheckers( pUsableChecker, iSkillLevelDataApplyType );
		}


		// Ŭ�󿡼��� ������� ����. ���������� �׳� �־��ش�.
		set<string> setUseActionName;
		iOffsetCheck = iProcessorParamOffset;
		IDnSkillProcessor* pSkillProcessor = IDnSkillProcessor::Create( m_hActor, iProcessor, iSkillLevelTableID, &iProcessorParamOffset, setUseActionName );

		if( NULL != pSkillProcessor )
		{
			if( (iProcessorParamOffset - iOffsetCheck) != pSkillProcessor->GetNumArgument() )
			{
				OutputDebug( "[SkillLevelTable Error!] %d �� �Ķ���� ������ �߸��Ǿ����ϴ�.\n", iSkillLevelTableID );
				_ASSERT( !"��ų ���� ���̺� �Ķ���� �߸���. OutputDebug ��� Ȯ��!" );
			}

			this->AddProcessor( pSkillProcessor, iSkillLevelDataApplyType );
		}
	}


	// skill table
	int iStringID = 0;
	iStringID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_NameID" )->GetInteger();
	m_strName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID );
	m_strStaticName = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_StaticName" )->GetString();

	char caLable[ 64 ];
	ZeroMemory( caLable, sizeof(caLable) );
	for( int i = 0; i < 2; ++i )
	{
		sprintf_s( caLable,  "_NeedWeaponType%d", i+1 );
		int iEquipType = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();
		if( -1 != iEquipType )
			m_aeNeedEquipType[ i ] = (CDnWeapon::EquipTypeEnum)iEquipType;
		else
			m_aeNeedEquipType[ i ] = CDnWeapon::EquipTypeEnum_Amount;
	}

	// ��ų ���̺��� �ִ� ������ �ŷ��� �� ����. -_-
	// ���� ������ ������Ʈ. ������ �ʱ�ȭ �Ǿ� �̹� ���� �ִٸ� ���� �ٸ� ��� assert!!
#if defined(PRE_FIX_NEXTSKILLINFO)
	if( 0 == m_iMaxLevel )
	{
		m_iMaxLevel = (int)pTableInfo->_SkillLevelTableIDs.size();
	}
	else
	{
		_ASSERT( m_iMaxLevel == (int)pTableInfo->_SkillLevelTableIDs.size() );
	}
#else
	if( 0 == m_iMaxLevel )
	{
		m_iMaxLevel = (int)vlSkillLevelList.size();
	}
	else
	{
		_ASSERT( m_iMaxLevel == (int)vlSkillLevelList.size() );
	}
#endif // PRE_FIX_NEXTSKILLINFO

	m_eSkillType = (SkillTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillType" )->GetInteger();
	m_eDurationType = (DurationTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_DurationType" )->GetInteger();
	m_eTargetType = (TargetTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_TargetType" )->GetInteger();
	m_iBuffIconImageIndex = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_BuffIconImageIndex" )->GetInteger();
	m_EffectOutputIDs = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StateEffectTableID" )->GetString();
#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF
	m_iDebuffIconImageIndx = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_DeBuffIconImageIndex" )->GetInteger();
	m_DebuffEffectOutputIDs = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StateEffectOtherID" )->GetString();
#endif

	m_iDissolvable = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_Dissolvable" )->GetInteger();
	m_iDuplicateCount = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_EffectAmassCount" )->GetInteger();
	m_iSkillDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillDuplicate" )->GetInteger();
	m_iEffectDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_EffectDuplicate" )->GetInteger();
	m_iNeedJobClassID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_NeedJob" )->GetInteger();
	//m_eElement = (CDnState::ElementEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_Element" )->GetInteger();
	if( (CDnState::ElementEnum)-1 == m_eElement )
		m_eElement = CDnState::ElementEnum_Amount;

	m_iUnlockSkillBookItemID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_UnlockSkillBookItemID" )->GetInteger();
	m_iUnlockPrice = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_UnlockPrice" )->GetInteger();
	m_iExclusiveID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_DuplicatedSkillType" )->GetInteger();
	m_iBaseSkillID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_BaseSkillID" )->GetInteger();

	m_iGlobalSkillGroupID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_GlobalSkillGroup" )->GetInteger();

	float fGlobalCoolTime = 0.0f;
	if( PVE == iSkillLevelDataApplyType )
		fGlobalCoolTime = (float)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_GlobalCoolTimePvE" )->GetInteger() / 1000.0f;
	else
	if( PVP == iSkillLevelDataApplyType )
		fGlobalCoolTime = (float)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_GlobalCoolTimePvP" )->GetInteger() / 1000.0f;

	m_afGlobalCoolTime[ iSkillLevelDataApplyType ] = fGlobalCoolTime;

	// skill level table
	m_iSkillLevelID = iSkillLevelTableID;
	m_iLevel = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillLevel" )->GetInteger();

	if( m_iLevel < m_iMaxLevel )
#if defined(PRE_FIX_NEXTSKILLINFO)
		m_iNextLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
#else
		m_iNextLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_NeedSkillPoint" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO
	else
		m_iNextLevelSkillPoint = 0;

#if defined(PRE_FIX_NEXTSKILLINFO)
	m_iFirstLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iMinSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
#else
	m_iFirstLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID-(m_iLevel-1), "_NeedSkillPoint" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO

	m_iNeedItemID[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_NeedItem" )->GetInteger();
	m_iNeedItemDecreaseCount[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_NeedItemDecreaseCount" )->GetInteger();
	m_iIncreaseRange[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_AddRange" )->GetInteger();
	m_iDecreaseHP[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DecreaseHP" )->GetInteger();
	m_iDecreaseMP[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DecreaseSP" )->GetInteger();
	m_iLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_LevelLimit" )->GetInteger();
	m_fDelayTime[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DelayTime" )->GetInteger() / 1000.f;

	// ���� �۷ι� ��ų �׷��� �����Ǿ��ִٸ� ��ų ��Ÿ���� �۷ι� ��Ÿ������ ��ü �����ش�.
	if( 0 < m_iGlobalSkillGroupID )
		m_fDelayTime[ iSkillLevelDataApplyType ] = m_afGlobalCoolTime[ iSkillLevelDataApplyType ];

	m_fOriginalDelayTime[ iSkillLevelDataApplyType ] = m_fDelayTime[ iSkillLevelDataApplyType ];
	m_iCPScore = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_CPScore" )->GetInteger();

#if defined(PRE_FIX_NEXTSKILLINFO)
	m_iFirstLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iMinSkillLevelTableID, "_LevelLimit" )->GetInteger();
#else
	m_iFirstLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID-(m_iLevel-1), "_LevelLimit" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO

	m_fHPConsumeType[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_HPConsumeType" )->GetFloat();
	m_fMPConsumeType[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SPConsumeType" )->GetFloat();

	// ��ų ������ �⺻ Ʋ�� �ȹٲ�� ���ڰ��� �ٲ� ���̴�. ���� ���ڰ��� ��庰�� ������.
	m_iSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationID" )->GetInteger();

	char *szParam = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationIDParam" )->GetString();
	MakeUIStringUseVariableParam( m_strSkillExplainString[ iSkillLevelDataApplyType ], m_iSkillExplainStringID, szParam );
	

	// ��ų ���� ���̺��� ���� ��ų�� ���� �����Ͱ� �����ؼ� �ִٴ� ���� �Ͽ�.
	if( m_iLevel < m_iMaxLevel )
	{
#if defined(PRE_FIX_NEXTSKILLINFO)
		DNTableCell* pSoxField = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_SkillExplanationID" );
		if ( pSoxField )
		{
			m_iNextSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_SkillExplanationID" )->GetInteger();

			szParam = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_SkillExplanationIDParam" )->GetString();
			MakeUIStringUseVariableParam( m_strNextSkillExplainString[iSkillLevelDataApplyType ], m_iNextSkillExplainStringID, szParam );
		}
		else
		{
			OutputDebug("��ų���� ���̺��� ���� ��ų�� ���������Ͱ� ������ �ƴϳ�???\n");
			m_iNextSkillExplainStringID = -1;
		}
#else
		DNTableCell* pSoxField = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_SkillExplanationID" );
		if ( pSoxField )
		{
			m_iNextSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_SkillExplanationID" )->GetInteger();

			szParam = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_SkillExplanationIDParam" )->GetString();
			MakeUIStringUseVariableParam( m_strNextSkillExplainString[iSkillLevelDataApplyType ], m_iNextSkillExplainStringID, szParam );
		}
		else
		{
			OutputDebug("��ų���� ���̺��� ���� ��ų�� ���������Ͱ� ������ �ƴϳ�???\n");
			m_iNextSkillExplainStringID = -1;
		}
#endif // PRE_FIX_NEXTSKILLINFO
	}
	else
		m_iNextSkillExplainStringID = -1;

	// ���� ȿ�� ���� �ε�
	StateEffectStruct StateEffect;
	for( int i = 0; i < MAX_STATE_EFFECT_COUNT; ++i )
	{
		sprintf_s( caLable, "_EffectClass%d", i + 1 );
		StateEffect.nID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();

		if( StateEffect.nID < 1 ) 
			continue;

		sprintf_s( caLable, "_EffectClass%dApplyType", i + 1 );
		int iApplyType = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();
		bool bApplyAll = (StateEffectApplyType::ApplyAll == iApplyType);		// ��� ������. Ÿ�ٸ� �ٸ��� �ؼ� �Ȱ��� ����ȿ�� 2���� �߰����ش�.

		if( bApplyAll )
			StateEffect.ApplyType = StateEffectApplyType::ApplySelf;
		else
			StateEffect.ApplyType = (StateEffectApplyType)iApplyType;

		sprintf_s( caLable, "_EffectClassValue%d", i + 1 );
		StateEffect.szValue = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetString();

		sprintf_s( caLable, "_EffectClassValue%dDuration", i + 1 );
		StateEffect.nDurationTime = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetInteger();

		m_vlStateEffectList[ iSkillLevelDataApplyType ].push_back( StateEffect );

		// ��� �����̸� Ÿ������ �ٲ㼭 �Ȱ��� �� �� �� �־���.
		if( bApplyAll )
		{
			StateEffect.ApplyType = StateEffectApplyType::ApplyTarget;
			m_vlStateEffectList[ iSkillLevelDataApplyType ].push_back( StateEffect );
		}
	}

	m_SkillInfo[ iSkillLevelDataApplyType ].iSkillID = m_iSkillID;
	m_SkillInfo[ iSkillLevelDataApplyType ].iSkillLevelID = m_iSkillLevelID;
#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
	m_SkillInfo[ iSkillLevelDataApplyType ].iAppliedEnchantSkillID = m_iAppliedEnchantPassiveSkillID;
#endif
	m_SkillInfo[ iSkillLevelDataApplyType ].eDurationType = m_eDurationType;
	m_SkillInfo[ iSkillLevelDataApplyType ].eSkillType = m_eSkillType;
	m_SkillInfo[ iSkillLevelDataApplyType ].iSkillBufIcon = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_BuffIconImageIndex" )->GetInteger();
	m_SkillInfo[ iSkillLevelDataApplyType ].effectOutputIDs = m_EffectOutputIDs;
	m_SkillInfo[ iSkillLevelDataApplyType ].iBuffIconImageIndex = m_iBuffIconImageIndex;
#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF
	m_SkillInfo[ iSkillLevelDataApplyType ].iDebuffIconImageIndex = m_iDebuffIconImageIndx;
	m_SkillInfo[ iSkillLevelDataApplyType ].debuffEffectOutputIDs = m_DebuffEffectOutputIDs;
#endif
	m_SkillInfo[ iSkillLevelDataApplyType ].eTargetType = m_eTargetType;
	if( m_hActor )
		m_SkillInfo[ iSkillLevelDataApplyType ].iSkillUserTeam = m_hActor->GetTeam();

#if defined(PRE_FIX_NEXTSKILLINFO)
	m_SkillInfo[ iSkillLevelDataApplyType ].nSkillLevel = m_iLevel;
#endif // PRE_FIX_NEXTSKILLINFO

	UpdateRangeChecker( iSkillLevelDataApplyType );

	RefreshDecreaseMP( iSkillLevelDataApplyType );

	if( 0.0f == m_fHPConsumeType[ iSkillLevelDataApplyType ] )
	{
		m_iNeedHP[ iSkillLevelDataApplyType ] = m_iDecreaseHP[ iSkillLevelDataApplyType ];
	}
	else
	{
		if( m_hActor )
			m_iNeedHP[ iSkillLevelDataApplyType ] = int((float)m_hActor->GetMaxHP() * m_fHPConsumeType[ iSkillLevelDataApplyType ]);
	}

	return true;
}

void CDnSkill::_AddUsedActions( const char* pActionName )
{
	if( !m_hActor )
		return;

	if( NULL == pActionName || strlen(pActionName) <= 0 )
		return;

	m_setUseActionNames.insert( pActionName );

	CEtActionBase::ActionElementStruct* pElement = m_hActor->GetElement( pActionName );
	if( pElement )
	{
		for( int i = 0; i < (int)pElement->pVecSignalList.size(); ++i )
		{
			CEtActionSignal* pSignal = pElement->pVecSignalList.at( i );
			if( STE_Input == pSignal->GetSignalIndex() )
			{
				InputStruct* pInputSignal = static_cast<InputStruct*>(pSignal->GetData());
				m_setUseActionNames.insert( pInputSignal->szChangeAction );
			}
		}
	}
}

bool CDnSkill::Initialize( int iSkillTableID, int iLevel )
{
	m_iSkillID = iSkillTableID;

	// �� ��庰�� ������ �ε�. ���� PVP �� �д´�.
	// PVE �� ����Ʈ ���̱� ������ ��庰�� ���������� ���� �ƴϸ� ����Ʈ ������ ä������ �ȴ�.

	// ������ ��쿣 pvp �����Ͱ� �����Ƿ� �Լ� �ȿ��� �ƹ��͵� �ȵǰ� ���ϵȴ�.
	// ���� �÷��̾��ε� �ƹ��͵� ���ٸ� �߸��� ����.
	if( false == _LoadSkillLevelData( iSkillTableID, iLevel, PVP ) )
		return false;

	if( false == _LoadSkillLevelData( iSkillTableID, iLevel, PVE ) )
		return false;

	// �ʱ�ȭ ���Ŀ� ���� ��Ƴ��� ������.
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
	
	// ������ �̹��� ��
	SetIconImageIndex( pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_IconImageIndex" )->GetInteger() );

	// �ʱ�ȭ���� �� PVE �������� �ʱ�ȭ ���ش�. ����Ʈ ��.
	m_iSelectedSkillLevelDataApplyType = PVE;
	m_bEnableTooltip = ( pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillTip" )->GetInteger() ) ? true : false;

	if (m_eDurationType == SummonOnOff)
		OnInitializeSummonMonsterInfo();

	// Ŭ�� �޸� ����ȭ ���� �۾�. ������ ��� �������� �� �� �ִ� ��ų�� ����Ѵ�.
	DNTableFileFormat*  pVillageAllowedSkillTable = GetDNTable( CDnTableDB::TVILLAGEALLOWEDSKILL );
	if( pVillageAllowedSkillTable && pVillageAllowedSkillTable->GetItemIDFromField( "_villageAllowedSkill", m_iSkillID ) != -1 ) {
		m_bCantUseInVillage = false;
	}

	return true;
}

bool CDnSkill::AddUsableCheckers( IDnSkillUsableChecker* pUsableChecker, int iSelectedLevelData )
{
	bool bResult = false;

	if( pUsableChecker )
	{
		m_vlpUsableCheckers[ iSelectedLevelData ].push_back( pUsableChecker );
		bResult = true;
	}

	return bResult;
}

bool CDnSkill::AddProcessor( IDnSkillProcessor* pProcessor, int iSkillLevelDataType )
{
	bool bResult = false;

	if( pProcessor )
	{
		m_vlpProcessors[ iSkillLevelDataType ].push_back( pProcessor );

		if( IDnSkillProcessor::PLAY_ANI == pProcessor->GetType() && m_strAction.empty() )
			m_strAction.assign( static_cast<CDnPlayAniProcess*>(pProcessor)->GetActionName() );

		bResult = true;
	}

	return bResult;
}

bool CDnSkill::IsSatisfyWeapon( void )
{
	bool bSatisfy = true;

	if( (CDnWeapon::EquipTypeEnum_Amount != m_aeNeedEquipType[ 0 ]) ||
		(CDnWeapon::EquipTypeEnum_Amount != m_aeNeedEquipType[ 1 ]) )
	{
		bSatisfy = false;

		if( CDnWeapon::EquipTypeEnum_Amount != m_aeNeedEquipType[ 0 ] && CDnWeapon::EquipTypeEnum_Amount != m_aeNeedEquipType[ 1 ] )
		{
			if( CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 0 ] ) == CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 1 ] ) )	// 1. �Ѵ� �ֹ����̰ų� �������� �� ��� or
			{
				int nWeapon = CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 0 ] ) ? 1 : 0;
				if( m_hActor->GetWeapon( nWeapon, false ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( nWeapon, false )->GetEquipType()
					|| m_hActor->GetWeapon( nWeapon, false ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( nWeapon, false )->GetEquipType() )
					bSatisfy = true;
			}
			else	// 2. �ֹ���, ���������� ��� and
			{
				if( CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 0 ] ) )
				{
					if( m_hActor->GetWeapon( 1, false ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( 1, false )->GetEquipType()
						&& m_hActor->GetWeapon( 0, false ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( 0, false )->GetEquipType() )
						bSatisfy = true;
				}
				else
				{
					if( m_hActor->GetWeapon( 0, false ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( 0, false )->GetEquipType()
						&& m_hActor->GetWeapon( 1, false ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( 1, false )->GetEquipType() )
						bSatisfy = true;
				}
			}
		}
		else
		{
			for( int i=0; i<MAX_SKILL_NEED_EQUIP_COUNT; ++i )
			{
				if( CDnWeapon::EquipTypeEnum_Amount != m_aeNeedEquipType[ i ] )
				{
					for( int k=0; k<2; ++k )
					{
						// bActionMatchWeapon �� ����Ʈ�� true �� ȣ���ϴϱ� �׼ǿ� ���� ���Ⱑ �ٸ��� ������ �� �Ͽ� �׻� ���� �ִ� ���� ���� ���ַ� üũ���..
						if( m_hActor->GetWeapon( k, false ) && m_aeNeedEquipType[ i ] == m_hActor->GetWeapon( k, false )->GetEquipType() )
						{
							bSatisfy = true;
							break;
						}
					}
					if( bSatisfy )
						break;
				}
			}
		}
	}

	return bSatisfy;
}

CDnSkill::UsingResult CDnSkill::CanExecute( void )
{
	UsingResult eResult = UsingResult::Failed;

	if( !m_hActor )
		return UsingResult::Failed;

	if( m_hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( pPlayerActor && pPlayerActor->IsSwapSingleSkin() )
		{
			if( pPlayerActor->IsTransformModeSkill(GetClassID()) == false )
				return UsingResult::Failed;
		}
	}
#if defined(PRE_FIX_45899)
	if( m_isDisableSkillByItemMove == true )
		return UsingResult::Failed;
#endif // PRE_FIX_45899

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	if( m_hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		CDnTotalLevelSkillSystem* pTotalLevelSystem = NULL;

		pTotalLevelSystem = pPlayerActor ? pPlayerActor->GetTotalLevelSkillSystem() : NULL;

		if (pTotalLevelSystem && pTotalLevelSystem->IsTotalLevelSkill(m_iSkillID))
		{
			bool isUsable = pTotalLevelSystem->IsUsableSkill(GetMySmartPtr());

			if (isUsable == true)
				eResult = UsingResult::Success;
			else
				eResult = UsingResult::Failed;

			return eResult;
		}
	}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	bool bCheckVillage = false;
	if( CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType() ) bCheckVillage = true;
	//RLKT_FARMPVP
	//if( CDnWorld::MapTypeDungeon == CDnWorld::GetInstance().GetMapType() &&
	//	CDnWorld::MapSubTypeFarm == CDnWorld::GetInstance().GetMapSubType() ) bCheckVillage = true;

	if( bCheckVillage )
	{
		if( m_bCantUseInVillage )
		{
			return UsingResult::Failed;
		}

		//�������� ���Ľ� ���� ����ȿ�� ��ų ��� ����..
		bool bChangeStandAction = false;
		for( DWORD i = 0; i < GetStateEffectCount(); ++i )
		{
			CDnSkill::StateEffectStruct* pSE = GetStateEffectFromIndex( i );
			if (STATE_BLOW::BLOW_121 == pSE->nID)
			{
				bChangeStandAction = true;
				break;
			}
		}

		if( bChangeStandAction )
			return UsingResult::Failed;
	}

	bool bCheckAllowedSkill = true;

	//������ ��ų�̸� �Ʒ� ��� ��ų üũ�� ���� �ʵ����Ѵ�..
	if (IsCreatedByItem())
		bCheckAllowedSkill = false;

	if (bCheckAllowedSkill &&
		m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_176))
	{
		DNVector(DnBlowHandle) vlBlows;
		CDnAllowedSkillsBlow* pAllowedSkillBlow = NULL;
		m_hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_176, vlBlows);
		for (UINT i = 0; i < vlBlows.size(); ++i)
		{
			pAllowedSkillBlow = dynamic_cast<CDnAllowedSkillsBlow*>(vlBlows[i].GetPointer());
			if (!pAllowedSkillBlow)
				continue;

			if (!pAllowedSkillBlow->IsAllowSkill(m_iSkillID))
				return UsingResult::Failed;
		}
	}	
	
	if (m_hActor && //rlkt_BLOW_351
		m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_351))
	{
		DNVector(DnBlowHandle) vlBlows;
		CDnDisableSkillBlow* pDisableSkillBlow = NULL;
		m_hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_351, vlBlows);
		for (UINT i = 0; i < vlBlows.size(); ++i)
		{
			pDisableSkillBlow = dynamic_cast<CDnDisableSkillBlow*>(vlBlows[i].GetPointer());
			if (!pDisableSkillBlow)
				continue;

			if (pDisableSkillBlow->IsDisabledSkill(m_iSkillID))
				return UsingResult::Failed;
		}
	}

	// ���� ����� �� �÷��̾�� ��ų ��� ���Ѵ�.
	if( m_hActor->IsPlayerActor() )
	{
		if( static_cast<CDnPlayerActor*>(m_hActor.GetPointer())->IsCannonMode() )
			return UsingResult::Failed;
	}

	// �ܿ� SP üũ, ü�� üũ, ���� üũ ���
	// ���� ����� ���� Ȱ��ȭ ���̶�� MP ��� ���� �� �� �ִ�.
	if( !IsToggleOn() && !IsAuraOn() )
		if( m_hActor->GetSP() < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;

	// �������� �����갰�� ��ų�� ����, ���� ���϶��� ��밡���ؾ� �Ѵ�.
	// ���̺� ���ݺҰ� ���� ����Ÿ���� �߰��ұ��ϴٰ� ���� �����꿡���� �ǹ��ִ� ���̹Ƿ�
	// �ٸ� ������ � ������ ���̰� ���� �� �� ���Ѻ��� �԰�ȭ ��Ű���� �Ѵ�.
	// �켱 Dissolve ����ȿ�� �ִ� ��ų�� �ൿ �Ұ� üũ�� �ǳʶڴ�.
	// ��ų�� �ش��ϴ� �׼��� State �ñ׳ο� �ݵ�� IgnorectCantAction �� ���� �־�� ��ų �׼��� ������.
	bool bPassCheckCantAction = false;
	for( DWORD i = 0; i < GetStateEffectCount(); ++i )
	{
		//#40480 ������¿��� ���ཽ�� Ȱ��ȭ�� ���ؼ�..
		CDnSkill::StateEffectStruct* pSE = GetStateEffectFromIndex( i );

		if( STATE_BLOW::BLOW_069 == pSE->nID )
		{
			// 76643 ( Dissolvable �� 2�� �׳� ��Ȱ��ȭ �ش޶�� �մϴ� )
			for( int i=0 ; i<m_hActor->GetNumAppliedStateBlow() ; ++i )
			{
				DnBlowHandle hBlow = m_hActor->GetAppliedStateBlow( i );
				if( hBlow )
				{
					const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();
					if( pSkillInfo && pSkillInfo->iDissolvable == 2 )
						return UsingResult::Failed;
				}
			}

			bPassCheckCantAction = true;
			break;
		}

		if( STATE_BLOW::BLOW_155 == pSE->nID )
		{
			bPassCheckCantAction = true;
			break;
		}
	}

	if( false == bPassCheckCantAction )
	{
		if( 0 < m_hActor->GetCantActionSEReferenceCount() )
			return UsingResult::Failed;
	}

	// ��ų ���Ұ� ����ȿ���� ������ mp �Ҹ��ϴ� ��ų�� ����� �� ����.
	if( 0 < m_hActor->GetCantUseSkillSEReferenceCount() )
		if( 0 < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;

	// 67�� ��ų ���� ����ȿ���� ������ ��Ƽ�� ��ų�� ����� �� ����. 
	if( m_eSkillType == CDnSkill::Active )
	{
		int iAppliedStateBlow = m_hActor->GetNumAppliedStateBlow();
		for( int iBlow = 0; iBlow < iAppliedStateBlow; ++iBlow )
		{
			DnBlowHandle hBlow = m_hActor->GetAppliedStateBlow( iBlow );
			if( hBlow->GetBlowIndex() == STATE_BLOW::BLOW_067 )
				return UsingResult::Failed;
		}
	}

	// HP
	if( m_hActor->GetHP() < m_iNeedHP[ m_iSelectedSkillLevelDataApplyType ] )
		return UsingResult::Failed;

	bool isCharacterLevelCheck = true;
	//��ų ������ �����ۿ� ���� ��ų�������� �� ��ų�� ĳ���� ���� üũ ���� �ʵ��� �Ѵ�.
	isCharacterLevelCheck = (GetLevelUpValue() == 0);

	if (isCharacterLevelCheck)
	{
		if( m_hActor->GetLevel() < m_iLevelLimit )
			return UsingResult::Failed;
	}

	// �ʿ� ���Ⱑ �ִٸ� �����ߴ��� Ȯ��. �ִ� 2����. �� �� �ϳ��� �����ǵ� ��ų ��� ����.
	if( false == IsSatisfyWeapon() )
		return UsingResult::Failed;

	// ȭ�� ���� �Ҹ� �������� ���� Ȯ��
	bool bIsCheckNeedItem = true;
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP && static_cast<CDnPvPGameTask*>(pGameTask)->IsLadderMode() && bIsExtremitySkill() )	// �������� �ñر� ��ų
		bIsCheckNeedItem = false;

	if( bIsCheckNeedItem && m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ] > 0 )
	{
		int iNumNeedItem = GetItemTask().GetCharInventory().ScanItemFromID( m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ], NULL );
		if( iNumNeedItem < m_iNeedItemDecreaseCount[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;
	}
	// ���� üũ. 
	// ���� ���������� �þ߿� ���� ������ ���� Ǯ �����丮�� ���� �ʰ� �ֱ������� ���õǹǷ� 
	// �������� ���� �ٸ� �������� ���������� üũ���� �ʽ��ϴ�.
	// ���������� ��Ƽ���� Ǯ �����丮�� �޾ƿ��� ������ ��� üũ��.
	bool bNeedCheckJob = true;
	if( CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType() )
		if( NULL == dynamic_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer()) )
			bNeedCheckJob = false;

	if( bNeedCheckJob && m_hActor->GetClassID() <= CDnActor::Reserved6  )
	{
		CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( NULL == pActor )
			return UsingResult::Failed;
		if( 0 != m_iNeedJobClassID )
		{
			if( pActor->IsPassJob( m_iNeedJobClassID ) == false ) 
				return UsingResult::Failed;
		}

		// �׾����� �Ұ�
		if( m_hActor->IsDie() ) 
			return UsingResult::Failed;
		// ��Ʋ ��尡 �ƴϸ� ��ų �ߵ� �Ұ�!
		if( !pActor->IsBattleMode() )
			return UsingResult::Failed;
	}

	// ��Ÿ���� ������ �ʾҴٸ� ��ų �ߵ� �Ұ�
	switch( m_eDurationType )
	{
	    case CDnSkill::Instantly:
		case CDnSkill::Buff:
		case CDnSkill::Debuff:
		case CDnSkill::SummonOnOff:
		case CDnSkill::StanceChange:
			{
				if( m_fCoolTime > 0.0f )
					return UsingResult::FailedByCooltime;
			}
			break;

		case CDnSkill::TimeToggle: 
			break;

		case CDnSkill::ActiveToggle:
		case CDnSkill::ActiveToggleForSummon:
			break;

		case CDnSkill::Aura: 
			break;
	}

	if( m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].empty() )
		eResult = UsingResult::Success;
	else
	{
		int iNumChecker = (int)m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].size();

		for( int iChecker = 0; iChecker < iNumChecker; ++iChecker )
		{
			IDnSkillUsableChecker* pChecker = m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].at( iChecker );
			if( false == pChecker->CanUse() )
			{
				eResult = UsingResult::FailedByUsableChecker;
				break;
			}
			else
				eResult = UsingResult::Success;
		}
	}

	return eResult;
}


float CDnSkill::GetDelayTime( void ) 
{ 

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	float fDelayTime = 0.0f;

	if( 0.0f < m_fAnotherGlobalSkillCoolTime && 
		m_iSelectedSkillLevelDataApplyType == m_iAnotherGlobalSkillLevelDataType )
	{
		fDelayTime = m_fAnotherGlobalSkillCoolTime;

		DnSkillHandle hAnotherSkill;
		if (m_hActor)
			hAnotherSkill = m_hActor->FindSkill(m_nAnotherGlobakSkillID);

		if (hAnotherSkill)
		{
			if (hAnotherSkill->GetAnotherGlobalSkillID() != GetClassID())
				fDelayTime = hAnotherSkill->GetDelayTime();
		}
	}
	else
	{ // m_fAnotherGlobalSkillCoolTime�� ���� �Ǿ� ������ ���� �� �ϵ���..
		
		fDelayTime = m_fDelayTime[ m_iSelectedSkillLevelDataApplyType ]; 

		float fGlobalCoolTimeRate = 0.0f;
		if (m_hActor &&
			m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_269) && 
			m_iGlobalSkillGroupID > 0)
		{
			DNVector(DnBlowHandle) vlBlows;
			m_hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_269, vlBlows);
			{
				int nCount = (int)vlBlows.size();
				for (int i = 0; i < nCount; ++i)
				{
					DnBlowHandle hBlow = vlBlows[i];
					if (hBlow && hBlow->IsEnd() == false)
					{
						fGlobalCoolTimeRate += hBlow->GetFloatValue();
					}
				}
			}
		}

		fDelayTime -= fDelayTime * fGlobalCoolTimeRate;
	}

	return fDelayTime;
#else

	if( 0.0f < m_fAnotherGlobalSkillCoolTime && 
		m_iSelectedSkillLevelDataApplyType == m_iAnotherGlobalSkillLevelDataType )
		return m_fAnotherGlobalSkillCoolTime;

	return m_fDelayTime[ m_iSelectedSkillLevelDataApplyType ]; 

#endif // PRE_ADD_TOTAL_LEVEL_SKILL
}


void CDnSkill::OnBeginCoolTime()
{
	m_fCoolTimeAdjustBlowValue = 1.0f;

	m_fCoolTime = (GetDelayTime() == 0.f) ? 0.0f : 1.0f;
	m_fLeftDelayTime = GetDelayTime()*m_fCoolTimeAdjustBlowValue;

	m_fLeftDelayTime *= m_fCoolTimeMultipier;

	// [2011/03/09 semozz]
	// �����Ҷ� LeftDelayTime�� 0.0�� �Ǹ� m_fCoolTime�� 0.0f�� �����ؾ��Ѵ�.
	// �׷��� ������ ��Ÿ���� 1.0���� ��� ������.(Process�Լ����� m_fLeftDelayTime�� 0�̸� CoolTime���žȵ�.
	if (m_fLeftDelayTime == 0.0f)
	{
		m_fCoolTime = 0.0f;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
		m_fDeltaGlobalCoolTime = 0.0f;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
	}

	// ��Ÿ�� ���� ġƮŰ�� �ִٸ� ��Ÿ�� ������� ��ġ.
#ifndef _FINAL_BUILD
	if( CGlobalValue::GetInstance().m_bIgnoreCoolTime ) {
		m_fCoolTime = 0.f;
		m_fLeftDelayTime = 0.f;
	}
#endif
}

void CDnSkill::_OnBeginProcessException( void )
{
	// ������¿��� ������ ������ ��ų�� ���� ��� �ٷ� ������ ������ Ǯ���ش�. #12438
	// �������ȿ���� �ɸ� ���¿��� ����� ����ȿ�� ���� ����ȿ���� self �� ����Ǵ� ��ų�� 
	// ����ϴ� ���� �Ϲ�ȭ ��Ŵ.
	if( m_hActor && 
		(
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_041 ) ||
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_146 ) ||	//#58432
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_218 ) //#53900 Escape��ų �߰�
		))
	{
		int iNumStateBlow = (int)m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size();
		for( int i = 0; i < iNumStateBlow; ++i )
		{
			const StateEffectStruct& SE = m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].at( i );
			if( STATE_BLOW::BLOW_069 == SE.nID &&
				StateEffectApplyType::ApplySelf == SE.ApplyType )
			{
				// ������ ���� ����ȿ�� ���� �ε����� ��� Ǯ���ش�. �������� ���ƾ� �׼��� ���ư���
				// �׼��� ���ư��� ������ ����ȿ���� ����.
				while( 0 < m_hActor->GetFrameStopRefCount() )
					m_hActor->RemovedFrameStop();
				break;
			}
		}
	}

#ifdef PRE_FIX_STANDCHANGE_SKILL_START_ACTION
	if( GetDurationType() == CDnSkill::StanceChange )
	{
		int iNumStateBlow = (int)m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size();
		for( int i = 0; i < iNumStateBlow; ++i )
		{
			const StateEffectStruct& SE = m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].at( i );
			if( STATE_BLOW::BLOW_121 == SE.nID && StateEffectApplyType::ApplySelf == SE.ApplyType )
			{
				std::string strPefix;

				DNVector(std::string)	vSplitStr;
				TokenizeA(SE.szValue, vSplitStr, ";");

				if (vSplitStr.size() > 0)
				{	
					strPefix = vSplitStr[0].substr(0, vSplitStr[0].find_last_of("_"));
				}

				std::string strStartActionName = strPefix + "_Start";

				CEtActionBase::ActionElementStruct* pStartActionElement = m_hActor->GetElement( strStartActionName.c_str() );
				if( pStartActionElement )
					m_hActor->CmdStop( strStartActionName.c_str() );
				else
					m_hActor->CmdStop( SE.szValue.c_str() );
			}
		}
	}
#endif
}

void CDnSkill::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// ���� �۷ι� ID �� ����ϴ� �ٸ� �۷ι� ��ų�� ����Ͽ� ���õ� ��Ÿ�� ���� ������ �� ��ų ���� ���� �����Ѵ�.
	m_fAnotherGlobalSkillCoolTime = 0.0f;

	m_nAnotherGlobakSkillID = 0;

#if defined( PRE_ADD_ACADEMIC ) // ��ȯ�� ���� ��Ƽ�� ��� ��ų�� ��Ÿ���� ���ʿ䰡 �����ϴ�.
	if(m_eDurationType != DurationTypeEnum::ActiveToggleForSummon)
	{
		//--------------------------------------------------
		//[debug_skill]
		//���ܴ�������
		//Ѹ��ն=6001   ���ζ���=6003  �콵���=6204   ���ȴ��=6208

		if ( 6001 == GetClassID() || 6003 == GetClassID() || 6204 == GetClassID() || 6208 == GetClassID() )
		{
//------------------------------------------------------
//[test]
/*
			int nSkillLevel = GetSkillLevelID();
           
			if (nSkillLevel > 0)
            {
                OutputDebugString( L"88888888" );
            }
*/
//------------------------------------------------------

			int nNowNum = GetSkillNowNum();
            
			if ( nNowNum >= 2 )
			{
				//SetSkillMaxUseNum(3);
				nNowNum = 0;
                SetSkillNowNum( nNowNum ); //ʹ�ô�������
				OnBeginCoolTime();
			} 
			else
			{
				nNowNum++;
                SetSkillNowNum( nNowNum ); //ʹ�ô�������1
			}
		} 
		else
		{
			OnBeginCoolTime();
		}
		//--------------------------------------------------
	}
#else
	OnBeginCoolTime();
#endif

	CheckProcessorOnBegin();

	// #31909 129�� �׼� �� ����ȿ���� ���� ��� (����ũ���� �ظӸ�) �⺻ ���� �׼��� �����ؼ� ������� �Ǿ��ִµ�
	// ���⸦ ��� ������ ���� �׼��� �����Ƿ� CDnPlayerActor::SetActionQueue, SetAction ��� 
	// ���� ���� �׼��� �ϰ� �ִ� ��� �⺻ �׼����� �ٽ� �ٲ��༭ ��������µ� ���� ���� �׼����� 
	// ����Ǿ��ٴ� �÷��װ� OnChangeAction ���� false �� ���µǹǷ� �ظӸ� 4Ÿ ���� �Ŀ� 
	// ��ų �����԰� ���ÿ� ������ ��� �÷��װ� ���� �ִ� ���±� ������ ��ų �ʿ��� ������ ��ų �׼��� 
	// �����ų �� CDnPlayer::SetActionQueue ���� ��ų �׼��� ������ �ȴ�. ���� ��ų �� �� 
	// �ش� ���� ���� �÷��װ� ���� �ִ� ��� ���⼭ �ٽ� ���ְ� ��ų�� ������ ���ش�. 
	// (���� �������� ������ ���������� ��ų �׼��� ������ Ŭ�󿡼� �ظӸ� �׼��� ������ ����)
	if( m_hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( pPlayerActor->IsChangedEquipAction() )
		{
			pPlayerActor->ClearChangedEuipAction();
		}
	}

	// ��ų ���� �ִ� ������ ���� ���õǾ��ִ°�.
	// ������ ��� ���� ������ �ȵǾ��ִ� ��찡 �����Ƿ� ���⼭ ����.
	if( !m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].hSkillUser )
		m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].hSkillUser = m_hActor;

	// pvp ���� �� �ϸ� �������� �ٲ� �� �����Ƿ� �׻� ��ų ���ÿ� ������ ����� �Ѵ�.
	m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].iSkillUserTeam = m_hActor->GetTeam();

	m_LastTimeToggleMPDecreaseTime = LocalTime;
	m_bFinished = false;

//	// [2010/12/15 semozz]
//	// ���������� ���� ��Ŷ�� ���� �� ���
//	// ���� Ŭ���̾�Ʈ�� �ڽ��� ����ȿ�� ó���ϴ� �κ� �״�� �����ϵ��� �Ѵ�.
	CDnWorld* pWorld = CDnWorld::GetInstancePtr();
	bool bVillage = (CDnWorld::MapTypeEnum::MapTypeVillage == pWorld->GetMapType());

	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->OnBegin( LocalTime, fDelta, GetMySmartPtr() );
	}

	_OnBeginProcessException();

	//���������� 
	if (m_eDurationType == SummonOnOff && !bVillage)
	{
		//���� ��ȯ �Ǹ� Toggle On..��Ŵ..
		EnableToggle(true);
	}
}

// ��ų�� �޷��ִ� Processor �� �߿� ��ó�� �ؾ��ϴ� �͵� ���� ó��.
void CDnSkill::CheckProcessorOnBegin( void )
{
	CheckChangeActionStrByBubbleProcessor();
}

void CDnSkill::CheckChangeActionStrByBubbleProcessor( void )
{
	CDnChangeActionStrByBubbleProcessor* pProcessor = static_cast<CDnChangeActionStrByBubbleProcessor*>(GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR_BY_BUBBLE ));
	if( pProcessor )
	{
		bool bChanged = false;
		const char* pChangedActionName = pProcessor->GetChangeActionNameAndRemoveNeedBubble( &bChanged );
		if( bChanged )
		{
			// �׼��� �ٲ��ش�.
			CDnPlayAniProcess* pPlayAniProcessor = static_cast<CDnPlayAniProcess*>(GetProcessor( IDnSkillProcessor::PLAY_ANI ));
			pPlayAniProcessor->ChangeActionNameOnce( pChangedActionName );
		}
	}
}

void CDnSkill::CheckProcessorOnEnd( void )
{

}



// ������������ ����ȿ�� ��Ŷ�� �����ְ� �����ۿ� �޷��ִ� �׼��� �����Ŵ.
// Ŭ�󿡼��� �׼Ǹ� �����Ų��.
void CDnSkill::OnBeginForceSync( LOCAL_TIME LocalTime, float fDelta )
{
	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->OnBegin( LocalTime, fDelta, GetMySmartPtr() );
	}
}

// ������ ��ų�� �ߵ� �Ǿ��� �ÿ� ó��
void CDnSkill::ProcessExecute( LOCAL_TIME LocalTime, float fDelta )
{
	// ���� �ִ� ���μ��� ó��
	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->Process( LocalTime, fDelta );
	}

	// -1 �� ���� ��� ó�� ����Ű�� ������ �ִµ��� ��� �ߵ��Ǵ� ��ų.
	if( -1.0f != m_fPassiveActionSkillLength )
	{
		m_fPassiveActionSkillLength -= fDelta;
		if( m_fPassiveActionSkillLength < 0.0f )
		{
			m_fPassiveActionSkillLength = 0.0f;
		}
	}
}

void CDnSkill::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->OnEnd( LocalTime, fDelta );
	}

	int iNumBlow = (int)m_vlAppliedNoPacketInstantBlow.size();
	for( int iBlow = 0; iBlow < iNumBlow; ++iBlow )
	{
		DnBlowHandle hBlow = m_vlAppliedNoPacketInstantBlow.at( iBlow );
		if( hBlow )
		{
			// #27257, #27272���� ���� [2011/01/07 semozz]
			// ���� ���������� ���� ��� �Ǵ� ���� ȿ�� ����(���� �����󿡼��� ��� ����ȿ�� ��Ŷ���� ����ȭ��)
			// ĳ���Ϳ��� ��Ŷ ���� �߰��� ����ȿ���� ��ų�� ������ �������� �����ϴµ�..
			// �� ����ȿ������ �ٷ� ���� �Ǿ�� ��..
			CDnStateBlow *pStateBlow = NULL;
			if (m_hActor)
				pStateBlow = m_hActor->GetStateBlow();

			if (pStateBlow)
				pStateBlow->RemoveImediatlyStateEffectFromID(hBlow->GetBlowID());
		}
	}
	
	m_vlAppliedNoPacketInstantBlow.clear();

	if( m_bResetSkill ) {
		ResetCoolTime();
		m_bResetSkill = false;
	}

	CheckProcessorOnEnd();

#if defined(PRE_FIX_64312)
	if (m_isAppliedSummonMonsterEnchantSkill == true)
	{
		ReleaseEnchantSkill();
		m_isAppliedSummonMonsterEnchantSkill = false;
	}
#endif // PRE_FIX_64312
}

// �� Ÿ�� ����� ó��
void CDnSkill::Process( LOCAL_TIME LocalTime, float fDelta )
{
	// ��Ÿ�� ���� �⺻���� 1.0 ��.
	// ���� m_fCoolTimeAdjustBlowValue ���� ������� �ʴ´�. 1.0 �⺻������ �״�� �ְ� ���� ��Ÿ�� ���� ����ȿ�� ������ �ٲ����. (#21146)
	// ����� ��ų�� ��Ÿ�� �������� ����
	if( TIME_ACCELERATION_SKILL_ID != m_iSkillID && SPRIT_BOOST_SKILL_ID != m_iSkillID &&
		( CDnSkillTask::IsActive() && !GetSkillTask().IsGuildWarSkill( m_iSkillID ) ) )
		fDelta *= m_hActor->GetCoolTimeDeltaAdjustValue();

	float fTempAdjustValue = m_fCoolTimeAdjustBlowValue;

	fTempAdjustValue *= m_fCoolTimeMultipier;

	switch( m_eDurationType )
	{
		case DurationTypeEnum::Instantly:
		case DurationTypeEnum::Buff:
		case DurationTypeEnum::Debuff:
		case DurationTypeEnum::SummonOnOff:
		case DurationTypeEnum::StanceChange:
			{
				if( 0.0f == m_fOnceElapsedDelayTime )
				{
					if( m_fLeftDelayTime != 0.0f )
					{
						m_fLeftDelayTime -= fDelta;
						if( m_fLeftDelayTime < 0.f ) 
						{
							m_fLeftDelayTime = 0.f;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
							m_fDeltaGlobalCoolTime = 0.0f;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
						}

						// [2011/03/09 semozz]
						// �����Ŀ�� ��ų�� ��Ÿ�ӿ� ���� fTempAdjustValue�� 0�̵Ǵ� ��찡 �߻�..
						// ���� ó�� �߰�
						float fTempDelayTime = (GetDelayTime()*fTempAdjustValue);
						if (fTempDelayTime == 0.0f)
							m_fCoolTime = 0.0f;
						else
							m_fCoolTime = ( 1.0f / fTempDelayTime ) * m_fLeftDelayTime;
					}
				}
				else
				{
					if( m_fOnceElapsedDelayTime == 0.0f ) 
					{
						m_fOnceDelayTime = 0.0f;
						return;
					}

					m_fOnceElapsedDelayTime -= fDelta;
					if( m_fOnceElapsedDelayTime < 0.0f )
						m_fOnceElapsedDelayTime = 0.0f;

					m_fCoolTime = ( 1.0f / (GetDelayTime()*m_fCoolTimeAdjustBlowValue) ) * m_fOnceElapsedDelayTime;
				}
				CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer());
				if( m_eSkillType == AutoActive && pActor && pActor->IsPetSummonableMap() && CanExecute() && m_fCoolTime == 0.0f )
				{
					if( !GetPetTask().CheckRemovePetSkill( m_iSkillID ) )
						m_hActor->UseSkill( m_iSkillID );
				}
			}
			break;
		case DurationTypeEnum::ActiveToggleForSummon: // ���Ͱ� ���� ��ų�̰� ���������Ŵ� ����ȹ޴´� �׷��� �ڵ尡 ��Ʋ����. �������� ��������.
			{
				if(IsEanbleActiveToggleCoolTime())
				{
					if( m_fLeftDelayTime == 0.0f ) return;

					m_fLeftDelayTime -= fDelta;
					if( m_fLeftDelayTime < 0.f )
					{
						m_fLeftDelayTime = 0.f;
						m_bEnableActiveToggleCoolTime = false;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
						m_fDeltaGlobalCoolTime = 0.0f;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
					}

					m_fCoolTime = ((1.0f / GetDelayTime()) * m_fLeftDelayTime);
				}
			}
			break;

		case DurationTypeEnum::TimeToggle:
			break;
		case DurationTypeEnum::ActiveToggle:
			break;
		case DurationTypeEnum::Aura:
			break;
	}

	// ���� ����� ���¶�� ���õ� �۷ι� ��ų ��Ÿ���� �ٽ� �����ش�.
	if( 0.0f < m_fAnotherGlobalSkillCoolTime && 0.0f == m_fCoolTime )
	{
		m_fAnotherGlobalSkillCoolTime = 0.0f;

		m_nAnotherGlobakSkillID = 0;
	}
}

bool CDnSkill::IsFinished( void )
{
	bool bResult = false;

	// ���� ��ų�̶�� �� �̵� �ϱ� ���� ������ ����.
	if( Aura == m_eDurationType )
		return IsFinishedAuraSkill();

	if( m_bFinished )
		return m_bFinished;

	// -1 �̸� �뽬 ���� Ű �� ������ ���� ���� ��ų.
	if( 0.0f < m_fPassiveActionSkillLength || -1.0f == m_fPassiveActionSkillLength )
	{
		if( m_hActor->GetSP() < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] )
			return true;

		return false;
	}

#ifdef PRE_FIX_76603
	if( m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_232 ) )
	{
		DNVector(DnBlowHandle) vlBlows;
		m_hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_232, vlBlows);

		for (DWORD i = 0; i < vlBlows.size(); i++)
		{
			if( vlBlows[i] )
			{
				CDnTransformBlow *pTransformBlow = static_cast<CDnTransformBlow*>( vlBlows[i].GetPointer() );
				if( pTransformBlow && pTransformBlow->GetParentSkillInfo()->iSkillID == GetClassID() )
				{
					if( pTransformBlow->CheckSkillDefendency() == true )
						return false;
				}
			}
		}
	}
#endif

	if( m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].empty() )
		bResult = true;
	else
	{
		int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
		for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
		{
			IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
			
			if( true == pProcessor->IsFinished() )
				bResult = true;
			else
			{
				bResult = false;
				break;
			}
		}
	}

	// lazy evaluation
	m_bFinished = bResult;

	return bResult;
}

IDnSkillUsableChecker* CDnSkill::GetChecker( int iType )
{
	IDnSkillUsableChecker* pResult = NULL;

	int iNumChecker = (int)m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iChecker = 0; iChecker < iNumChecker; ++iChecker )
	{
		IDnSkillUsableChecker* pSkillChecker = m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].at( iChecker );
		if( iType == pSkillChecker->GetType() )
		{
			pResult = pSkillChecker;
			break;
		}
	}

	return pResult;
}

IDnSkillProcessor* CDnSkill::GetProcessor( int iType )
{
	IDnSkillProcessor* pResult = NULL;

	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pSkillProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		if( iType == pSkillProcessor->GetType() )
		{
			pResult = pSkillProcessor;
			break;
		}
	}

	return pResult;
}

MIInventoryItem::ItemConditionEnum CDnSkill::GetItemCondition( void )
{
	if( m_hActor ) {
		if( m_hActor->IsDie() ) 
			return ItemConditionEnum::Unusable;

		// �÷��̾ ȹ������ ���� ��ų�� ��� �Ұ�.
		if( false == IsAcquired()/*!m_hActor->FindSkill( m_iSkillID )*/ )
			return ItemConditionEnum::Unusable;
	}

	ItemConditionEnum eResult = ItemConditionEnum::Unusable;

	if( CDnSkill::UsingResult::Success != CanExecute() )
	{
		if( m_hActor ) {
			if( m_hActor->GetSP() < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] ||
				m_hActor->GetHP() <= m_iNeedHP[ m_iSelectedSkillLevelDataApplyType ] )
				return ItemConditionEnum::NotEnoughCondition;
		}

		//��Ÿ���� ���� �ִ� ��
		if (m_eDurationType == DurationTypeEnum::SummonOnOff)
		{
			if (m_bNowToggled)
				return ItemConditionEnum::Toggle;
		}

		//Add MechaMode 345
	///	if(m_hActor && m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_345))
		//		return ItemConditionEnum::Usable;

		return ItemConditionEnum::Unusable;
	}

	switch( m_eDurationType )
	{
		case TimeToggle:
		case ActiveToggle:
		case ActiveToggleForSummon:
		case DurationTypeEnum::SummonOnOff:
			{
				// ��� ��ų�̰� ��� �� ���¶�� 
				// ItemConditionEnum::Toggle ����
				if( m_bNowToggled )
				{
					return ItemConditionEnum::Toggle;
				}
			}
			break;

		case Aura:
			{
				// ���� ��ų�̰� ���� ���� ���¶�� 
				// ItemConditionEnum::Toggle ����
				if( m_bNowAura )
				{
					return ItemConditionEnum::Toggle;
				}
			}
			break;

	}

	return ItemConditionEnum::Usable;
}

DWORD CDnSkill::GetStateEffectCount( void )
{
	return (DWORD)m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size();
}


CDnSkill::StateEffectStruct* CDnSkill::GetStateEffectFromIndex( DWORD dwIndex )
{
	if( dwIndex < 0 || dwIndex >= m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size() ) return NULL;
	return &m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ][dwIndex];
}

bool CDnSkill::IsUseCheckerType( int iCheckerType )
{
	bool bResult = false;

	int iNumChecker = (int)m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iChecker = 0; iChecker < iNumChecker; ++iChecker )
	{
		if( m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].at( iChecker )->GetType() == iCheckerType )
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

bool CDnSkill::IsUseProcessorType( int iProcessorType )
{
	bool bResult = false;

	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		if( m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor )->GetType() == iProcessorType )
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

void CDnSkill::CalcLevelUpUsePoint( int iLevelCount ) 
{ 
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

#if defined(PRE_FIX_NEXTSKILLINFO)
	SKILL_LEVEL_TABLE_IDS::iterator findIter;
	SKILL_LEVEL_INFO* pTableInfo = GetSkillTask().GetSkillLevelTableIDList(m_iSkillID, 0);
#endif // PRE_FIX_NEXTSKILLINFO

	int iCount = 0;
	while( iLevelCount )
	{
		if( m_iMaxLevel <= m_iLevel )
		{
			_ASSERT( "�ִ� ���� �ʰ��Ͽ� CDnSkill::LevelUp() ȣ��" );
			break;
		}

		++m_iLevel;
		++m_iNextSkillExplainStringID;
		//m_iUsePoint += nPoint;

		--iLevelCount;
		++iCount;

#if defined(PRE_FIX_NEXTSKILLINFO)
		int nSkillLevelID = -1;

		int nSkillLevel = m_iLevel - m_nLevelUpValue;
		findIter = pTableInfo->_SkillLevelTableIDs.find(nSkillLevel);
		if (findIter != pTableInfo->_SkillLevelTableIDs.end())
			nSkillLevelID = findIter->second;

		m_iUsePoint += pSkillLevelTable->GetFieldFromLablePtr( nSkillLevelID, "_NeedSkillPoint" )->GetInteger();

		findIter = pTableInfo->_SkillLevelTableIDs.find(m_iLevel + 1);
		if (findIter != pTableInfo->_SkillLevelTableIDs.end())
			nSkillLevelID = findIter->second;
		m_iNextSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( nSkillLevelID, "_SkillExplanationID" )->GetInteger();
#else
		//���� ���������� ����/������ ��ų ����Ʈ���� ��� �;��Ѵ�.
		int nSkillLevelID = m_iSkillLevelID + iCount - m_nLevelUpValue;
		m_iUsePoint += pSkillLevelTable->GetFieldFromLablePtr( nSkillLevelID, "_NeedSkillPoint" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO

	}
}

void CDnSkill::ResetLevelUpUsePoint( int iLevelCount ) 
{ 
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	_ASSERT( m_iUsePoint > 0 );

#if defined(PRE_FIX_NEXTSKILLINFO)
	SKILL_LEVEL_TABLE_IDS::iterator findIter;
	SKILL_LEVEL_INFO* pTableInfo = GetSkillTask().GetSkillLevelTableIDList(m_iSkillID, 0);
#endif // PRE_FIX_NEXTSKILLINFO

	int iCount = 0;
	while( iLevelCount )
	{
		if( m_iLevel < 1 )
		{
			_ASSERT( "�ִ� ���� �ʰ��Ͽ� CDnSkill::LevelUp() ȣ��" );
			break;
		}

#if defined(PRE_FIX_NEXTSKILLINFO)
		int nSkillLevelID = -1;

		int nSkillLevel = m_iLevel - m_nLevelUpValue;
		findIter = pTableInfo->_SkillLevelTableIDs.find(nSkillLevel);
		if (findIter != pTableInfo->_SkillLevelTableIDs.end())
			nSkillLevelID = findIter->second;

		m_iUsePoint -= pSkillLevelTable->GetFieldFromLablePtr( nSkillLevelID, "_NeedSkillPoint" )->GetInteger();

		findIter = pTableInfo->_SkillLevelTableIDs.find(m_iLevel);
		if (findIter != pTableInfo->_SkillLevelTableIDs.end())
			nSkillLevelID = findIter->second;
		m_iNextSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( nSkillLevelID, "_SkillExplanationID" )->GetInteger();

		--m_iLevel;
		
		--iLevelCount;
		++iCount;

#else
		--m_iLevel;
		--m_iNextSkillExplainStringID;

		//���� ���������� ����/������ ��ų ����Ʈ���� ��� �;��Ѵ�.
		int nSkillLevelID = m_iSkillLevelID + iLevelCount - m_nLevelUpValue;
		m_iUsePoint -= pSkillLevelTable->GetFieldFromLablePtr( nSkillLevelID, "_NeedSkillPoint" )->GetInteger();

		--iLevelCount;
		++iCount;

#endif // PRE_FIX_NEXTSKILLINFO
	}

}

void CDnSkill::_RefreshDecreaseMP( int iSkillLevelDataApplyType )
{
	if( 0.0f == m_fMPConsumeType[ iSkillLevelDataApplyType ] )
	{
		if( !m_hActor || m_hActor->IsMonsterActor() )		// ���Ӽ����� �����ϰ� ����.
		{
			m_iNeedMP[ iSkillLevelDataApplyType ] = m_iDecreaseMP[ iSkillLevelDataApplyType ];
			return;
		}
#ifndef _ADD_NEW_MPCONSUME
		float fRatio = 1.f;
		if( m_hActor->IsPlayerActor() && m_hActor->GetLevel() > 0 )
		{
			int nJobClassID = ((CDnPlayerActor*)m_hActor.GetPointer())->GetJobClassID();
			float fDecreaseRatio = CPlayerLevelTable::GetInstance().GetValueFloat( nJobClassID , m_hActor->GetLevel(), CPlayerLevelTable::SPDecreaseRatio );
			fRatio =  fDecreaseRatio * m_hActor->GetLevelWeightValue();
		}

		m_iNeedMP[ iSkillLevelDataApplyType ] = (int)( m_iDecreaseMP[ iSkillLevelDataApplyType ] * fRatio );
#else
		m_iNeedMP[iSkillLevelDataApplyType] = (int)(m_iDecreaseMP[iSkillLevelDataApplyType]);

#endif
	}
	else
	{
		if( m_hActor ) 
			m_iNeedMP[iSkillLevelDataApplyType] = int((float)m_hActor->GetMaxSP() * m_fMPConsumeType[iSkillLevelDataApplyType]);

	}

	m_iOriginalNeedMP[ iSkillLevelDataApplyType ] = m_iNeedMP[ iSkillLevelDataApplyType ];
}

void CDnSkill::RefreshDecreaseMP( int iSkillLevelDataApplyType /*= NUM_SKILLLEVEL_APPLY_TYPE*/ )
{
	if( iSkillLevelDataApplyType == NUM_SKILLLEVEL_APPLY_TYPE )
	{
		for( int iApplyType = PVE; iApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iApplyType )
			_RefreshDecreaseMP( iApplyType );
	}
	else
	{
		_RefreshDecreaseMP( iSkillLevelDataApplyType );
	}
}

void CDnSkill::SetOnceCoolTime( float fDelayTime, float fElapsedDelayTime )
{
	_ASSERT( fElapsedDelayTime <= fDelayTime );
	
	m_fOnceDelayTime = fDelayTime;
	m_fOnceElapsedDelayTime = fElapsedDelayTime;
}

bool CDnSkill::IsUseActionNames( const set<string>& setUseActionNames )
{
	list<string> listIntersect;
	set_intersection( setUseActionNames.begin(), setUseActionNames.end(), 
					  m_setUseActionNames.begin(), m_setUseActionNames.end(), 
					  back_inserter(listIntersect) );

	return !listIntersect.empty();
}

void CDnSkill::SetPassiveSkillActionName( const char* pActionName )
{
	if( pActionName )
	{
		// ���� ���� ���� �ڵ����� �߰� ���� ����.
		m_setUseActionNames.insert( string(pActionName) );
	}
}

// �׼� ��� ���μ����� �ݵ�� �ִ� ��Ƽ�� ��ų�� ���� ��� ��û!
void CDnSkill::OnChainInput( const char* pActionName )
{
	IDnSkillProcessor* pProcessor = GetProcessor( IDnSkillProcessor::PLAY_ANI );
	_ASSERT( pProcessor );
	if( pProcessor )
	{
		CDnPlayAniProcess* pPlayAniProcessor = static_cast<CDnPlayAniProcess*>( pProcessor );
		pPlayAniProcessor->OnChainInput( pActionName );
	}
}

void CDnSkill::Acquire( void )
{
	_ASSERT( !m_bLock );
	
	m_bAcquired = true;
}

void CDnSkill::AddGlyphStateEffect(int nGlyphID)
{
	DNTableFileFormat* pGlyphTable = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pGlyphTable ) return;

#if defined(PRE_ADD_65808)
	int monsterID = -1;
	if (IsSummonMonsterRecall(monsterID) == true)
	{
		m_hActor->AddSummonMonsterGlyphInfo(monsterID, nGlyphID);
		return;
	}
#endif // PRE_ADD_65808

	char caLable[64];

	// ��ų ��ȭ ���� ���� ��� pvp/pve ��� �������ش�.
	for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
	{
		// ���� ����ȿ�� ���� �ִ� ����
		int iNumOriginalStateEffect = (int)m_vlStateEffectList[ iSkillLevelApplyType ].size();

		// ���� ȿ�� �߰�
		StateEffectStruct StateEffect;
		StateEffect.nGlyphID = nGlyphID;
		for( int i = 0; i < MAX_GLYPH_STATE_EFFECT_COUNT; ++i )
		{
			sprintf_s( caLable, "_EffectClass%d", i + 1 );
			StateEffect.nID = pGlyphTable->GetFieldFromLablePtr( nGlyphID, caLable )->GetInteger();

			if( StateEffect.nID < 1 ) 
				continue;

			sprintf_s( caLable, "_EffectClass%dApplyType", i + 1 );
			int iApplyType = pGlyphTable->GetFieldFromLablePtr( nGlyphID, caLable )->GetInteger();
			bool bApplyAll = (StateEffectApplyType::ApplyAll == iApplyType);		// ��� ������. Ÿ�ٸ� �ٸ��� �ؼ� �Ȱ��� ����ȿ�� 2���� �߰����ش�.

			if( bApplyAll )
				StateEffect.ApplyType = StateEffectApplyType::ApplySelf;
			else
				StateEffect.ApplyType = (StateEffectApplyType)iApplyType;

			sprintf_s( caLable, "_EffectClassValue%d", i + 1 );
			StateEffect.szValue = pGlyphTable->GetFieldFromLablePtr( nGlyphID, caLable )->GetString();

			// ���ӽð��� ������ �ִ� ����ȿ������ �״�� ��ü�Ѵ�.
			bool bFound = false;
			for( int k = 0; k < iNumOriginalStateEffect; ++k )
			{
				const StateEffectStruct& OriginalSE = m_vlStateEffectList[ iSkillLevelApplyType ].at( k );
				if( OriginalSE.nID == StateEffect.nID )
				{
					StateEffect.nDurationTime = OriginalSE.nDurationTime;
					bFound = true;
				}
			}

			// ���� ���� ����ȿ���� ���ٸ� ���ӽð��� �׳� 0 ���� ���ش�. 
			// �׷� �׼ǰ� ������ ���ݸ�ŭ ����ȿ���� ���ӵ�.
			if( false == bFound )
				StateEffect.nDurationTime = 0;

			m_vlStateEffectList[ iSkillLevelApplyType ].push_back( StateEffect ); 

			// ��� �����̸� Ÿ������ �ٲ㼭 �Ȱ��� �� �� �� �־���.
			if( bApplyAll )
			{
				StateEffect.ApplyType = StateEffectApplyType::ApplyTarget;
				m_vlStateEffectList[ iSkillLevelApplyType ].push_back( StateEffect );
			}
		}
	}

	//��ų �Ӽ� �� �߰�
	GlyphEffectStruct GlyphEffect;
	GlyphEffect.eEffectIndex = pGlyphTable->GetFieldFromLablePtr( nGlyphID, "_GlyphSkillEffect" )->GetInteger();
	GlyphEffect.fEffectValue = pGlyphTable->GetFieldFromLablePtr( nGlyphID, "_GlyphSkillEffectValue" )->GetFloat();
	GlyphEffect.nGlyphID = nGlyphID;

	switch( GlyphEffect.eEffectIndex )
	{
		case SKILLMP_PRO :
			{
				for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
					m_iNeedMP[ iSkillLevelApplyType ] += (int)( (float)m_iOriginalNeedMP[ iSkillLevelApplyType ] * GlyphEffect.fEffectValue );
				break;
			}

		case SKILLDELAY_PRO :
			{
				for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
					m_fDelayTime[ iSkillLevelApplyType ] += m_fOriginalDelayTime[ iSkillLevelApplyType ] * GlyphEffect.fEffectValue;
				break;
			}

		case SKILLDELAY_RESET_PRO :
			{
				break;
			}

		case SKILLDURATION_PRO :
			{
				// pair<int, int> <����ȿ��, �Ⱓ>
				for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
				{
					bool bEmpty = m_vlStateDurationList[iSkillLevelApplyType].empty();
					for( int itr = 0; itr < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++itr )
						if( bEmpty )
							m_vlStateDurationList[iSkillLevelApplyType].push_back( std::make_pair(m_vlStateEffectList[ iSkillLevelApplyType ][itr].nID, m_vlStateEffectList[ iSkillLevelApplyType ][itr].nDurationTime) );
						else m_vlStateEffectList[ iSkillLevelApplyType ][itr].nDurationTime = m_vlStateDurationList[iSkillLevelApplyType][itr].second;
				}


				float fRatio = GlyphEffect.fEffectValue;
				for( int itr = 0; itr < (int)m_vGlyphEffectList.size(); ++itr )
				{
					if( SKILLDURATION_PRO == m_vGlyphEffectList[itr].eEffectIndex )
						fRatio += m_vGlyphEffectList[itr].fEffectValue;
				}

				for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
				{
					for( int itr = 0; itr < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++itr )
						m_vlStateEffectList[ iSkillLevelApplyType ][itr].nDurationTime += (int)(m_vlStateEffectList[ iSkillLevelApplyType ][itr].nDurationTime * fRatio);
				}

				break;
			}
	}
	m_vGlyphEffectList.push_back( GlyphEffect );
}

void CDnSkill::DelGlyphStateEffect(int nGlyphID)
{
#if defined(PRE_ADD_65808)
	int monsterID = -1;
	if (IsSummonMonsterRecall(monsterID) == true)
	{
		m_hActor->RemoveSummonMonsterGlyphInfo(monsterID, nGlyphID);
		return;
	}
#endif // PRE_ADD_65808

	// ��ų ��ȭ ���� ���� ��� pvp/pve ��� �������ش�.
	for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
	{
		for( std::vector<StateEffectStruct>::iterator Itor = m_vlStateEffectList[ iSkillLevelApplyType ].begin(); 
			Itor < m_vlStateEffectList[ iSkillLevelApplyType ].end(); )
		{
			if( nGlyphID == Itor->nGlyphID )
				Itor = m_vlStateEffectList[ iSkillLevelApplyType ].erase(Itor);
			else
				++Itor;
		}
	}

	
	for( std::vector<GlyphEffectStruct>::iterator Itor = m_vGlyphEffectList.begin(); 
		Itor < m_vGlyphEffectList.end(); )
	{
		if( nGlyphID == Itor->nGlyphID )
		{
			switch( Itor->eEffectIndex )
			{
			case SKILLMP_PRO :
				{
					for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
						m_iNeedMP[ iSkillLevelApplyType ] -= (int)( (float)m_iOriginalNeedMP[ iSkillLevelApplyType ] * Itor->fEffectValue );
					break;
				}

			case SKILLDELAY_PRO :
				{
					for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
						m_fDelayTime[ iSkillLevelApplyType ] -= m_fOriginalDelayTime[ iSkillLevelApplyType ] * Itor->fEffectValue;
					break;
				}

			case SKILLDELAY_RESET_PRO :
				{
					break;
				}

			case SKILLDURATION_PRO :
				{
					// pair<int, int> <����ȿ��, �Ⱓ>
					bool bExist = false, bOnce = true;
					float fRatio = 0.0f;
					for( int itr = 0; itr < (int)m_vGlyphEffectList.size(); ++itr )
					{
						if( Itor->nGlyphID == m_vGlyphEffectList[itr].nGlyphID && bOnce) 
						{
							bOnce = false;
							continue;
						}
						if( SKILLDURATION_PRO == m_vGlyphEffectList[itr].eEffectIndex )
						{
							bExist = true;
							fRatio += m_vGlyphEffectList[itr].fEffectValue;
						}
					}

					if( !bExist )
					{
						for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
						{
							for( int itr = 0; itr < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++itr )
								m_vlStateEffectList[ iSkillLevelApplyType ][itr].nDurationTime = m_vlStateDurationList[iSkillLevelApplyType][itr].second;
							m_vlStateDurationList[iSkillLevelApplyType].clear();
						}
					}
					else
					{
						for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
						{
							for( int itr = 0; itr < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++itr )
								m_vlStateEffectList[ iSkillLevelApplyType ][itr].nDurationTime = m_vlStateDurationList[iSkillLevelApplyType][itr].second + (int)(m_vlStateDurationList[iSkillLevelApplyType][itr].second * fRatio);
						}
					}

					break;
				}
			}
			
			Itor = m_vGlyphEffectList.erase( Itor );
		}
		else
			++Itor;
	}
}

bool CDnSkill::SelectLevelDataType( int iSkillLevelDataType, bool bForce )
{
	if( false == ( (PVE <= iSkillLevelDataType) &&
				   (iSkillLevelDataType < NUM_SKILLLEVEL_APPLY_TYPE) ) ) 
		return false;

	if( !bForce ) {
		if( !m_hActor )
			return false;

		_ASSERT( m_hActor->IsPlayerActor() && (PVE <= iSkillLevelDataType) && (iSkillLevelDataType < NUM_SKILLLEVEL_APPLY_TYPE) );

		if( false == m_hActor->IsPlayerActor() ) 
			return false;
	}

	m_iSelectedSkillLevelDataApplyType = iSkillLevelDataType;
	return true;
}

int CDnSkill::GetBaseSkillID( void )
{
	int iResult = 0;

	if( EnchantPassive == m_eSkillType )
	{
		iResult = m_iBaseSkillID;
	}

	return iResult;
}

// ���Ӽ����� ������ ����ȭ ���߾��ּ��� CheckAndDivideStateEffectArgument �� �Լ��� �������ּ��� 
bool CDnSkill::CheckAndUnifyStateEffectArgument( int iSkillLevelApplyType, StateEffectStruct* pEnchantSkillSE )
{
	bool bResult = false;

	if( CDnCreateBlow::IsBasicBlow( (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID ) )
	{
		for( int i = 0; i < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++i )
		{
			StateEffectStruct* pDestSkillSE = &(m_vlStateEffectList[ iSkillLevelApplyType ].at( i ));
			if( pDestSkillSE->nID == pEnchantSkillSE->nID )
			{
				CDnBasicBlow::AddStateEffectValue( pDestSkillSE->szValue.c_str(), pEnchantSkillSE->szValue.c_str(), pDestSkillSE->szValue );
				pDestSkillSE->strEnchantSkillSEParam = pEnchantSkillSE->szValue;
				bResult = true;
			}
		}
	}
	else if( STATE_BLOW::BLOW_016 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID )
	{
		for( int i = 0; i < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++i )
		{
			StateEffectStruct* pDestSkillSE = &(m_vlStateEffectList[ iSkillLevelApplyType ].at( i ));
			if( pDestSkillSE->nID == pEnchantSkillSE->nID )
			{
				CDnHPIncBlow::AddStateEffectValue( pDestSkillSE->szValue.c_str(), pEnchantSkillSE->szValue.c_str(), pDestSkillSE->szValue );
				pDestSkillSE->strEnchantSkillSEParam = pEnchantSkillSE->szValue;
				bResult = true;
			}
		}
	}
	else if( STATE_BLOW::BLOW_208 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID )
	{
		for( int i = 0; i < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++i )
		{
			StateEffectStruct* pDestSkillSE = &(m_vlStateEffectList[ iSkillLevelApplyType ].at( i ));
			if( pDestSkillSE->nID == pEnchantSkillSE->nID )
			{
				CDnPingpongBlow::AddStateEffectValue( pDestSkillSE->szValue.c_str(), pEnchantSkillSE->szValue.c_str(), pDestSkillSE->szValue );
				pDestSkillSE->strEnchantSkillSEParam = pEnchantSkillSE->szValue;
				bResult = true;
			}
		}
	}
	else if( STATE_BLOW::BLOW_140 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID )
	{
		for( int i = 0; i < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++i )
		{
			StateEffectStruct* pDestSkillSE = &(m_vlStateEffectList[ iSkillLevelApplyType ].at( i ));
			if( pDestSkillSE->nID == pEnchantSkillSE->nID )
			{
				CDnHealingBlow::AddStateEffectValue( pDestSkillSE->szValue.c_str(), pEnchantSkillSE->szValue.c_str(), pDestSkillSE->szValue );
				pDestSkillSE->strEnchantSkillSEParam = pEnchantSkillSE->szValue;
				bResult = true;
			}
		}
	}
	else if( STATE_BLOW::BLOW_227 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID )
	{
		for( int i = 0; i < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++i )
		{
			StateEffectStruct* pDestSkillSE = &(m_vlStateEffectList[ iSkillLevelApplyType ].at( i ));
			if( pDestSkillSE->nID == pEnchantSkillSE->nID )
			{
				CDnBloodSuckingBlow::AddStateEffectValue( pDestSkillSE->szValue.c_str(), pEnchantSkillSE->szValue.c_str(), pDestSkillSE->szValue );
				pDestSkillSE->strEnchantSkillSEParam = pEnchantSkillSE->szValue;
				bResult = true;
			}
		}
	}
	else if( STATE_BLOW::BLOW_215 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID )
	{
		for( int i = 0; i < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++i )
		{
			StateEffectStruct* pDestSkillSE = &(m_vlStateEffectList[ iSkillLevelApplyType ].at( i ));
			if( pDestSkillSE->nID == pEnchantSkillSE->nID )
			{
				CDnOrderMySummonedMonsterBlow::AddStateEffectValue( pDestSkillSE->szValue.c_str(), pEnchantSkillSE->szValue.c_str(), pDestSkillSE->szValue );
				pDestSkillSE->strEnchantSkillSEParam = pEnchantSkillSE->szValue;
				bResult = true;
			}
		}
	}
	else if( STATE_BLOW::BLOW_179 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID
		|| STATE_BLOW::BLOW_121 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID
		|| STATE_BLOW::BLOW_129 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID
		|| STATE_BLOW::BLOW_062 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID 
		|| STATE_BLOW::BLOW_242 == (STATE_BLOW::emBLOW_INDEX)pEnchantSkillSE->nID )
	{
		for( int i = 0; i < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++i )
		{
			StateEffectStruct* pDestSkillSE = &(m_vlStateEffectList[ iSkillLevelApplyType ].at( i ));
			if( pDestSkillSE->nID == pEnchantSkillSE->nID )
			{
				pDestSkillSE->strEnchantSkillSEParam = pDestSkillSE->szValue;
				pDestSkillSE->szValue = pEnchantSkillSE->szValue;
				bResult = true;
				break;
			}
		}
	}

	return bResult;
}


bool CDnSkill::ApplyEnchantSkill( DnSkillHandle hEnchantPassiveSkill )
{
	bool bSuccess = false;

	if( 0 < m_iAppliedEnchantPassiveSkillID )
		return false;

	_ASSERT( m_iSkillID == hEnchantPassiveSkill->GetBaseSkillID() );
	_ASSERT( EnchantPassive != m_eSkillType );
	if( EnchantPassive != m_eSkillType )
	{
		int iNowLevelDataType = m_iSelectedSkillLevelDataApplyType;
		for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
		{
			// ������ �����ϴ� ����ȿ���� ���ڰ��� �����ְ� ���ο� ����ȿ���� ����Ʈ�� �߰�.
			// ��Ÿ���� ��ȭ �нú� ��ų�� ��Ÿ������ ������ش�.
			SelectLevelDataType( iSkillLevelApplyType );
			hEnchantPassiveSkill->SelectLevelDataType( iSkillLevelApplyType );
			int iNumStateEffect = (int)hEnchantPassiveSkill->GetStateEffectCount();
			for( int i = 0; i < iNumStateEffect; ++i )
			{
				StateEffectStruct* pEnchantPassiveSE = hEnchantPassiveSkill->GetStateEffectFromIndex( i );
				
				// #40643 ������EX ó�� Ư���ϰ� ������ ����ȿ���� ���ڸ� �ǵ帮�� ���..
				// ��¿ �� ���� ������� �Ѵ�. ������ m_vlStateEffectList �� �߰��Ǵ� ���� �ƴ�.
				if( false == CheckAndUnifyStateEffectArgument( iSkillLevelApplyType, pEnchantPassiveSE ) )
				{
					// ���ݷ� ��ȭ ����ȿ���� �ƴϰ� �߰����ָ� ��ų ���� �ڵ����� �ɷ�ġ �������� �ȴ�.
					m_vlStateEffectList[ iSkillLevelApplyType ].push_back( *pEnchantPassiveSE );
					m_vlStateEffectList[ iSkillLevelApplyType ].back().nFromEnchantPassiveSkillID = hEnchantPassiveSkill->GetClassID();
				}
			}

			// �׼� ���μ����� ��ȭ ��ų�� ������ �����Ѵ�.
			IDnSkillProcessor* pMyPlayAniProcessor = GetProcessor( IDnSkillProcessor::PLAY_ANI );
			if( pMyPlayAniProcessor )
			{
				IDnSkillProcessor* pEnchantPassiveSkillPlayAniProcessor =  hEnchantPassiveSkill->GetProcessor( IDnSkillProcessor::PLAY_ANI );
				if( pEnchantPassiveSkillPlayAniProcessor )
				{
					// ���� �� ���.
					IDnSkillProcessor* pBackup = new CDnPlayAniProcess;
					pBackup->CopyFrom( pMyPlayAniProcessor );
					m_vlpProcessorBackup[ iSkillLevelApplyType ].push_back( pBackup );

					// ��ȭ �нú� ��ų�� ������ ������ ����.
					pMyPlayAniProcessor->CopyFrom( pEnchantPassiveSkillPlayAniProcessor );
				}
			}

			IDnSkillProcessor* pMyPartialPlayAniProcessor = GetProcessor( IDnSkillProcessor::PARTIAL_PLAY_ANI );
			if( pMyPartialPlayAniProcessor )
			{
				IDnSkillProcessor* pEnchantPassivePartialSkillPlayAniProcessor =  hEnchantPassiveSkill->GetProcessor( IDnSkillProcessor::PARTIAL_PLAY_ANI );
				if( pEnchantPassivePartialSkillPlayAniProcessor )
				{
					// ���� �� ���.
					IDnSkillProcessor* pBackup = new CDnPartialPlayProcessor;
					pBackup->CopyFrom( pMyPartialPlayAniProcessor );
					m_vlpProcessorBackup[ iSkillLevelApplyType ].push_back( pBackup );

					pMyPartialPlayAniProcessor->CopyFrom( pEnchantPassivePartialSkillPlayAniProcessor );
				}
			}

			IDnSkillProcessor* pMyChangeActionStrProcessor = GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR );
			if( pMyChangeActionStrProcessor )
			{
				IDnSkillProcessor* pEnchantPassiveChangeActionStrProcessor =  hEnchantPassiveSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR );
				if( pEnchantPassiveChangeActionStrProcessor )
				{
					// ���� �� ���.
					IDnSkillProcessor* pBackup = new CDnChangeActionStrProcessor;
					pBackup->CopyFrom( pMyChangeActionStrProcessor );
					m_vlpProcessorBackup[ iSkillLevelApplyType ].push_back( pBackup );

					pMyChangeActionStrProcessor->CopyFrom( pEnchantPassiveChangeActionStrProcessor );
				}
			}

#ifdef PRE_ADD_ENCHANTSKILL_BUBBLE_ACTION
			IDnSkillProcessor* pMyChangeActionStrByBubbleProcessor = GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR_BY_BUBBLE );
			if( pMyChangeActionStrByBubbleProcessor )
			{
				IDnSkillProcessor* pEnchantPassiveChangeActionStrProcessor =  hEnchantPassiveSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR_BY_BUBBLE );
				if( pEnchantPassiveChangeActionStrProcessor )
				{
					IDnSkillProcessor* pBackup = new CDnChangeActionStrByBubbleProcessor( );
					pBackup->CopyFrom( pMyChangeActionStrByBubbleProcessor );
					m_vlpProcessorBackup[ iSkillLevelApplyType ].push_back( pBackup );

					pMyChangeActionStrByBubbleProcessor->CopyFrom( pEnchantPassiveChangeActionStrProcessor );
				}
			}
#endif

			m_eElement = hEnchantPassiveSkill->GetElement();

#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
			bool bSummonMonsterToggleSkill = ( m_isAppliedSummonMonsterEnchantSkill && hEnchantPassiveSkill->GetDurationType() == CDnSkill::ActiveToggleForSummon );
			if( bSummonMonsterToggleSkill == false )
			{
				m_eDurationType = hEnchantPassiveSkill->GetDurationType();
				m_SkillInfo[iSkillLevelApplyType].eDurationType = hEnchantPassiveSkill->GetDurationType();
			}
#endif

#if defined(PRE_FIX_66175)
			//#66175
			//UsableChecker�� ���̽� ��ų�� EX��ų ��ü�Ѵ�...
			vector<IDnSkillUsableChecker*>& enchangeSkillUsableChecker = hEnchantPassiveSkill->GetUsableChecker(iSkillLevelApplyType);

			//EX��ų�� UsableChecker�� ���� �Ǿ� �ִ� ��츸 ��ü �ϵ��� �Ѵ�...
			if (enchangeSkillUsableChecker.size() > 0)
			{
				vector<IDnSkillUsableChecker*>& baseSkillUsableChecker = GetUsableChecker(iSkillLevelApplyType);

				{
					//�ڽ��� Checker���� ��� ����Ʈ�� ��� ���´�.
					vector<IDnSkillUsableChecker*>::iterator iter = baseSkillUsableChecker.begin();
					vector<IDnSkillUsableChecker*>::iterator endIter = baseSkillUsableChecker.begin();

					//��� ����Ʈ�� ���� �س���...
					m_vlUsableCheckersBackup[iSkillLevelApplyType].clear();

					for (; iter != endIter; ++iter)
					{
						m_vlUsableCheckersBackup[iSkillLevelApplyType].push_back((*iter));
					}

				}				
				
				//EX��ų�� usableChecker�� ���̽� ��ų�� �����Ѵ�.
				baseSkillUsableChecker.clear();
				vector<IDnSkillUsableChecker*>::iterator iter = enchangeSkillUsableChecker.begin();
				vector<IDnSkillUsableChecker*>::iterator endIter = enchangeSkillUsableChecker.end();
				for (; iter != endIter; ++iter)
				{
					IDnSkillUsableChecker* pChecker = (*iter);
					baseSkillUsableChecker.push_back(pChecker->Clone());
				}
			}
#endif // PRE_FIX_66175

			//EffectIDs�� ����..
			string enchantEffectIDs = hEnchantPassiveSkill->GetEffectOutputIDs(iSkillLevelApplyType);
			string enchantDebuffEffectIDs = hEnchantPassiveSkill->GetDebuffEffectOutputIDs(iSkillLevelApplyType);

			m_BackupEffectOutputIDs[iSkillLevelApplyType] = m_SkillInfo[iSkillLevelApplyType].effectOutputIDs;
			m_BackupDebuffEffectOutputIDs[iSkillLevelApplyType] = m_SkillInfo[iSkillLevelApplyType].debuffEffectOutputIDs;

			m_SkillInfo[iSkillLevelApplyType].effectOutputIDs = enchantEffectIDs;
			m_SkillInfo[iSkillLevelApplyType].debuffEffectOutputIDs = enchantDebuffEffectIDs;

#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
			m_SkillInfo[iSkillLevelApplyType].iAppliedEnchantSkillID = hEnchantPassiveSkill->GetClassID();
#endif
		}

		SelectLevelDataType( iNowLevelDataType );

		m_iAppliedEnchantPassiveSkillID = hEnchantPassiveSkill->GetClassID();
		bSuccess = true;
	}

	return bSuccess;
}

bool CDnSkill::CheckAndDivideStateEffectArgument( StateEffectStruct* pDestSkillSE )
{
	if( pDestSkillSE->strEnchantSkillSEParam.empty() == true )
		return false;

	bool bResult = false;

	if( CDnCreateBlow::IsBasicBlow( (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID ) )
	{
		CDnBasicBlow::RemoveStateEffectValue( pDestSkillSE->szValue.c_str(), pDestSkillSE->strEnchantSkillSEParam.c_str(), pDestSkillSE->szValue );
		pDestSkillSE->strEnchantSkillSEParam.clear();
		bResult = true;
	}
	else if( STATE_BLOW::BLOW_016 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID )
	{
		CDnHPIncBlow::RemoveStateEffectValue( pDestSkillSE->szValue.c_str(), pDestSkillSE->strEnchantSkillSEParam.c_str(), pDestSkillSE->szValue );
		pDestSkillSE->strEnchantSkillSEParam.clear();
		bResult = true;
	}
	else if( STATE_BLOW::BLOW_208 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID )
	{
		CDnPingpongBlow::RemoveStateEffectValue( pDestSkillSE->szValue.c_str(), pDestSkillSE->strEnchantSkillSEParam.c_str(), pDestSkillSE->szValue );
		pDestSkillSE->strEnchantSkillSEParam.clear();
		bResult = true;
	}
	else if( STATE_BLOW::BLOW_140 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID )
	{
		CDnHealingBlow::RemoveStateEffectValue( pDestSkillSE->szValue.c_str(), pDestSkillSE->strEnchantSkillSEParam.c_str(), pDestSkillSE->szValue );
		pDestSkillSE->strEnchantSkillSEParam.clear();
		bResult = true;
	}
	else if( STATE_BLOW::BLOW_227 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID )
	{
		CDnBloodSuckingBlow::RemoveStateEffectValue( pDestSkillSE->szValue.c_str(), pDestSkillSE->strEnchantSkillSEParam.c_str(), pDestSkillSE->szValue );
		pDestSkillSE->strEnchantSkillSEParam.clear();
		bResult = true;
	}
	else if( STATE_BLOW::BLOW_215 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID )
	{
		CDnOrderMySummonedMonsterBlow::RemoveStateEffectValue( pDestSkillSE->szValue.c_str(), pDestSkillSE->strEnchantSkillSEParam.c_str(), pDestSkillSE->szValue );
		pDestSkillSE->strEnchantSkillSEParam.clear();
		bResult = true;
	}
	else if( STATE_BLOW::BLOW_179 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID
		|| STATE_BLOW::BLOW_121 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID
		|| STATE_BLOW::BLOW_129 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID
		|| STATE_BLOW::BLOW_062 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID 
		|| STATE_BLOW::BLOW_242 == (STATE_BLOW::emBLOW_INDEX)pDestSkillSE->nID )
	{
		pDestSkillSE->szValue = pDestSkillSE->strEnchantSkillSEParam;
		pDestSkillSE->strEnchantSkillSEParam.clear();
		bResult = true;
	}

	return bResult;
}


void CDnSkill::ReleaseEnchantSkill( void )
{
	if( 0 == m_iAppliedEnchantPassiveSkillID )
		return;

	_ASSERT( EnchantPassive != m_eSkillType );
	if( EnchantPassive != m_eSkillType )
	{
		int iNowLevelDataType = m_iSelectedSkillLevelDataApplyType;
		for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
		{
			// ������ �����ϴ� ����ȿ���� ���ڰ��� �����ְ� ���ο� ����ȿ���� ����Ʈ�� �߰�.
			// ��Ÿ���� ��ȭ �нú� ��ų�� ��Ÿ������ ������ش�.
			SelectLevelDataType( iSkillLevelApplyType );
			vector<StateEffectStruct>::iterator iter = m_vlStateEffectList[ iSkillLevelApplyType ].begin();
			for( iter; iter != m_vlStateEffectList[ iSkillLevelApplyType ].end(); )
			{
				if( 0 < iter->nFromEnchantPassiveSkillID )
				{
					if( false == CheckAndDivideStateEffectArgument( &(*iter) ) )
					{
						iter = m_vlStateEffectList[ iSkillLevelApplyType ].erase( iter );
						continue;
					}
				}

				++iter;
			}

			for( int i = 0; i < (int)m_vlpProcessorBackup[ iSkillLevelApplyType ].size(); ++i )
			{
				IDnSkillProcessor* pProcessor = m_vlpProcessorBackup[ iSkillLevelApplyType ].at( i );
				switch( pProcessor->GetType() )
				{
					case IDnSkillProcessor::PLAY_ANI:
						{
							IDnSkillProcessor* pMyPlayAniProcessor = GetProcessor( IDnSkillProcessor::PLAY_ANI );
							if( pMyPlayAniProcessor )
								pMyPlayAniProcessor->CopyFrom( pProcessor );
						}
						break;

					case IDnSkillProcessor::PARTIAL_PLAY_ANI:
						{
							IDnSkillProcessor* pMyPartialPlayAniProcessor = GetProcessor( IDnSkillProcessor::PARTIAL_PLAY_ANI );
							if( pMyPartialPlayAniProcessor )
								pMyPartialPlayAniProcessor->CopyFrom( pProcessor );
						}
						break;
					case IDnSkillProcessor::CHANGE_ACTIONSTR:
						{
							IDnSkillProcessor* pMyChangeActionStrProcessor = GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR );
							if( pMyChangeActionStrProcessor )
								pMyChangeActionStrProcessor->CopyFrom( pProcessor );
						}
						break;
#ifdef PRE_ADD_ENCHANTSKILL_BUBBLE_ACTION
					case IDnSkillProcessor::CHANGE_ACTIONSTR_BY_BUBBLE:
						{
							IDnSkillProcessor* pMyChangeActionStrByBubbleProcessor = GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR_BY_BUBBLE );
							if( pMyChangeActionStrByBubbleProcessor )
								pMyChangeActionStrByBubbleProcessor->CopyFrom( pProcessor );
						}
						break;
#endif
				}
			}
			
			SAFE_DELETE_PVEC( m_vlpProcessorBackup[ iSkillLevelApplyType ] );

#if defined(PRE_FIX_66175)
			if (m_vlUsableCheckersBackup[iSkillLevelApplyType].size() > 0)
			{
				std::swap(m_vlpUsableCheckers[iSkillLevelApplyType], m_vlUsableCheckersBackup[iSkillLevelApplyType]);
			}
#endif // PRE_FIX_66175

			//EffectIDs�� ����..
			m_SkillInfo[iSkillLevelApplyType].effectOutputIDs = m_BackupEffectOutputIDs[iSkillLevelApplyType];
			m_SkillInfo[iSkillLevelApplyType].debuffEffectOutputIDs = m_BackupDebuffEffectOutputIDs[iSkillLevelApplyType];

			m_BackupEffectOutputIDs[iSkillLevelApplyType].clear();
			m_BackupDebuffEffectOutputIDs[iSkillLevelApplyType].clear();
		}

		SelectLevelDataType( iNowLevelDataType );
		m_iAppliedEnchantPassiveSkillID = 0;
	}
}

bool CDnSkill::CheckAnimation()
{
	// ��ų ��Ÿ���� ��ų �ִϸ��̼Ǳ��� ���� ª�� ��� CanExecute���� �ɷ� ������
	// SetActionQueue���� ���� ������ ���� �ʴ´�. �׷��� ��ų ��� ���� üũ����
	// ���� ��ų ������ ������ ���� �������� �Ȱ��� ��ų�� ��� �� �ϵ��� üũ�ϵ����Ѵ�.
	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		if (!pProcessor->CheckAnimation())
			return false;
	}

	return true;

}

bool CDnSkill::IsFinishedAuraSkill()
{
	//���� ��ų�϶� ���� ����/���� ���� ������ ������ �� ���� ��ȭ���� �ջ��ؼ� ���� ���� ������ ���ؼ�
	//��ų ���� ���� ���θ� �Ǵ��Ѵ�.
	if (m_hActor)
	{
		float fIncManaDelta = 0.0f;

		//���� ���� ��
		DNVector( DnBlowHandle ) vlManaAbIncBlows;
		m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_013, vlManaAbIncBlows );
		if (!vlManaAbIncBlows.empty())
		{
			for( int i = 0; i < (int)vlManaAbIncBlows.size(); ++i )
			{
				DnBlowHandle hBlow = vlManaAbIncBlows.at( i );
				fIncManaDelta += hBlow->GetFloatValue();							
			}
		}

		//���� ���� ��
		float fMaxMP = (float)m_hActor->GetMaxSP();
		DNVector( DnBlowHandle ) vlManaRateIncBlows;
		m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_014, vlManaRateIncBlows );
		if (!vlManaRateIncBlows.empty())
		{
			for( int i = 0; i < (int)vlManaRateIncBlows.size(); ++i )
			{
				DnBlowHandle hBlow = vlManaRateIncBlows.at( i );
				fIncManaDelta += fMaxMP * hBlow->GetFloatValue();							
			}
		}

		//#46381 ������Ż�ε� - mp �������� ���Ͽ� ���尡 ��� ������
#if defined(PRE_FIX_46381)
		//231�� ����ȿ�� ����.
		DNVector(DnBlowHandle) vlContinueBaseMPIncBlows;
		m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_231, vlContinueBaseMPIncBlows);
		if (!vlContinueBaseMPIncBlows.empty())
		{
			for (int i = 0; i < (int)vlContinueBaseMPIncBlows.size(); ++i)
			{
				DnBlowHandle hBlow = vlContinueBaseMPIncBlows.at(i);
				fIncManaDelta += static_cast<CDnContinueBaseMPIncBlow*>(hBlow.GetPointer())->GetMPIncValue();
			}
		}
#endif // PRE_FIX_46381

		//���� -�������� ���� ���� �������� ������ ���� ���ڸ�..
		if (m_hActor->GetSP() < -(int)fIncManaDelta)
			return true;		
	}

	return false;
}

void CDnSkill::OnAnotherGlobalSkillBeginCoolTime( DnSkillHandle hSkill )
{
	if( hSkill )
	{
		m_fAnotherGlobalSkillCoolTime = hSkill->GetDelayTime();
		m_iAnotherGlobalSkillLevelDataType = m_iSelectedSkillLevelDataApplyType;

		m_nAnotherGlobakSkillID = hSkill->GetClassID();
	}
}

void CDnSkill::OnInitializeSummonMonsterInfo()
{
	if (!m_hActor)
		return;

	set<string>::iterator iter = m_setUseActionNames.begin();
	set<string>::iterator endIter = m_setUseActionNames.end();

	string actionName;
	CEtActionBase::ActionElementStruct *pActionElement = NULL;

	bool bFindMonsterID = false;

	for (; iter != endIter; ++iter)
	{
		actionName = *iter;
		pActionElement = m_hActor->GetElement(actionName.c_str());
		if (pActionElement)
		{
			CEtActionSignal *pSignal = NULL;
			for( DWORD i=0; i<pActionElement->pVecSignalList.size(); i++ ) 
			{
				pSignal = pActionElement->pVecSignalList[i];

				if (pSignal->GetSignalIndex() == STE_SummonMonster)
				{
					SummonMonsterStruct* pStruct = (SummonMonsterStruct *)pSignal->GetData();

					m_SummonMonsterID = pStruct ? pStruct->MonsterID : -1;

					if (m_SummonMonsterID != -1) 
					{
						bFindMonsterID = true;
						break;
					}
				}
			}
		}

		if (bFindMonsterID)
			break;
	}

	m_bEnableActiveToggleCoolTime = true;

}

DnActorHandle CDnSkill::FindSummonMonster(int nMonsterID)
{
	DnActorHandle hSummonMonster;


	if (m_hActor && m_hActor->IsPlayerActor())
	{

		DNVector(DnActorHandle) vlActors;

		CDnActor::ScanActor( (*m_hActor->GetPosition()), 10000.0f, vlActors );

		int iNumActors = (int)vlActors.size();
		for( int iActor = 0; iActor < iNumActors; ++iActor )
		{
			DnActorHandle hTargetActor = vlActors.at( iActor );

			if( hTargetActor 
				&&  hTargetActor->IsMonsterActor() 
				&& (hTargetActor->GetTeam() == m_hActor->GetTeam()) 
				&& (false == hTargetActor->IsDie()) )
			{
				CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hTargetActor.GetPointer());
				if (pMonsterActor && 
					pMonsterActor->GetMonsterClassID() == nMonsterID &&
					pMonsterActor->GetSummonerUniqueID() == m_hActor->GetUniqueID()
					)
				{
					hSummonMonster = hTargetActor;
					break;
				}
			}
		}
	}


	return hSummonMonster;
}

bool CDnSkill::SummonMonsterOff()
{
	DnActorHandle hSummonMonster;

	if (m_SummonMonsterID != -1)
		hSummonMonster = FindSummonMonster(m_SummonMonsterID);

	if (hSummonMonster)
	{
		//������ ��ȯ���� ���� ��û�� ������, �������� �ش� ���� ��ü�� ã�Ƽ� CmdSuicideȣ��
		m_hActor->RequestSummonOff(hSummonMonster->GetUniqueID());
		//��ȯ ���� ��ȯ ������ ��� ����..
		EnableToggle(false);
		return true;
	}
	else
		return false;
}

void CDnSkill::SetGlyphActiveSkillDisable( bool bActiveSkillDisable )
{
	m_bGlyphActiveSkillDisbale = bActiveSkillDisable;
}

bool CDnSkill::GetGlyphActiveSkillDisable()
{
	return m_bGlyphActiveSkillDisbale;
}

#if defined(PRE_FIX_64312)
bool CDnSkill::IsSummonMonsterSkill()
{
	m_bIsSummonMonsterSkill = false;

	//��ų Checker�߿� ��ȯ���� üũ�� ������ ��ȯ���Ϳ� ��� �ϴ� ��ų???(��ȹ�ǰ�..)
	int iNumChecker = (int)m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].size();

	for( int iChecker = 0; iChecker < iNumChecker; ++iChecker )
	{
		IDnSkillUsableChecker* pChecker = m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].at( iChecker );
		if( pChecker && pChecker->GetType() == IDnSkillUsableChecker::SUMMON_CHECKER )
		{
			m_bIsSummonMonsterSkill = true;
			break;
		}
	}

	return m_bIsSummonMonsterSkill;
}

void CDnSkill::AddSummonMonsterEnchantSkill(DnSkillHandle hSkill)
{
	m_SummonMonsterEnchantSkill = hSkill;
}

void CDnSkill::RemoveSummonMonsterEnchantSkill()
{
	m_SummonMonsterEnchantSkill.Identity();
}

bool CDnSkill::ApplySummonMonsterEnchantSkill(DnSkillHandle hSkill)
{
	m_isAppliedSummonMonsterEnchantSkill = true;
	return ApplyEnchantSkill(hSkill);
}
#endif // PRE_FIX_64312

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
float CDnSkill::GetOrigDelayTime()
{
	float fDelayTime = 0.0f;

	if( 0.0f < m_fAnotherGlobalSkillCoolTime && m_iSelectedSkillLevelDataApplyType == m_iAnotherGlobalSkillLevelDataType )
		fDelayTime = m_fAnotherGlobalSkillCoolTime;
	else
		fDelayTime = m_fDelayTime[ m_iSelectedSkillLevelDataApplyType ]; 

	return fDelayTime;
}

void CDnSkill::UpdateGlobalCoolTime(float fRate)
{
	//�۷ι� ��Ÿ���� ���� �Ǿ� �ְ�, ��Ÿ���� �̹� ���� �Ǿ��� ���
	if (m_iGlobalSkillGroupID > 0)
	{
		if (m_fLeftDelayTime > 0)
		{
			m_fDeltaGlobalCoolTime += fRate;

			float fOrigDelayTime = GetOrigDelayTime();

			m_fLeftDelayTime -= fOrigDelayTime * fRate;		
		}
	}
}

void CDnSkill::ResetGlobalCoolTime(float fRate)
{
	//�۷ι� ��Ÿ���� ���� �Ǿ� �ְ�, ��Ÿ���� �̹� ���� �Ǿ��� ���
	if (m_iGlobalSkillGroupID > 0 )
	{
		//�۷ι� ��Ÿ�� ���� ��ġ�� ���� �� ���
		if (m_fDeltaGlobalCoolTime > 0.0)
		{
			float fOrigDelayTime = GetOrigDelayTime();

			//����� ��ġ�� ���� ���� �Ϸ��� ��ġ���� ���� ���� ���
			if (m_fDeltaGlobalCoolTime < fRate)
				fRate = m_fDeltaGlobalCoolTime;

			//����� ��ġ�� ���� �����ش�..
			m_fDeltaGlobalCoolTime -= fRate;
			if (m_fDeltaGlobalCoolTime < 0.0f)
				m_fDeltaGlobalCoolTime = 0.0f;

			//���� ��Ÿ���� ���� �ִ� ���
			if (m_fLeftDelayTime > 0)
				m_fLeftDelayTime += fOrigDelayTime * fRate;		
		}
	}
}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#if defined(PRE_ADD_65808)
bool CDnSkill::IsSummonMonsterRecall(int& monsterID)
{
	//PlayAniProcess�� �̿��ϰ�, �ش� ���ۿ� SummonMonster�ñ׳��� ���� �ϸ� true�� ����..
	CDnPlayAniProcess* pAniProcess = static_cast<CDnPlayAniProcess*>(GetProcessor( IDnSkillProcessor::PLAY_ANI ));
	if (pAniProcess && m_hActor)
	{
		const char* szActionName = pAniProcess->GetActionName();
		if (szActionName == NULL)
			return false;

		CEtActionBase::ActionElementStruct *pStruct = m_hActor->GetElement(szActionName);
		if (pStruct)
		{
			int selectedMonsterID = -1;

			CEtActionSignal *pSignal = NULL;
			for (int i = 0; i < (int)pStruct->pVecSignalList.size(); ++i)
			{
				pSignal = pStruct->pVecSignalList[i];
				if (pSignal == NULL)
					continue;
				
				int signalIndex = pSignal->GetSignalIndex();
				switch(signalIndex)
				{
				case STE_SummonMonster:
					{
						SummonMonsterStruct* pSummonMonsterInfo = (SummonMonsterStruct *)pSignal->GetData();
						if (pSummonMonsterInfo)
							selectedMonsterID = pSummonMonsterInfo->MonsterID;
					}
					break;
				case STE_SummonMonsterInfo:
					{
						SummonMonsterInfoStruct* pInfo = (SummonMonsterInfoStruct*)pSignal->GetData();
						if (pInfo)
							selectedMonsterID = pInfo->MonsterID;
					}
					break;
				}

				if (selectedMonsterID != -1)
					break;
			}

			if (selectedMonsterID != -1)
			{
				monsterID = selectedMonsterID;
				return true;
			}
		}
	}

	return false;
}
#endif // PRE_ADD_65808