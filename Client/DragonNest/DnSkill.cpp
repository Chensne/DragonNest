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

	m_iSelectedSkillLevelDataApplyType = PVE;		// µðÆúÆ®´Â pve ÀÌ´Ù.

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
	assert( hActor && "CDnSkill::SetHasActor() ¾×ÅÍ ÇÚµéÀÌ NULL ÀÓ" );

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

			// »ç¿ëÇÏ´Â ¾×¼ÇÀÌ ÀÖÀ¸¸é ¿ÜºÎ¿¡¼­ Á¶È¸¿ëÀ¸·Î µ¥ÀÌÅÍ Ã¤¿ö³õÀ½
			// ½ºÅ³ ¾×¼Ç ÀÌ¸§ ÀÚÃ¼´Â ÇöÀç pvp/pve °°±â ¶§¹®¿¡ pve ÀÏ ¶§ ±âÁØÀ¸·Î ÇÑ¹ø¸¸ ¼ÂÆÃÇØÁØ´Ù.
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

	// ÆÐ½Ãºê ½ºÅ³ÀÌ¶ó¸é °ü·ÃµÈ ¾×¼ÇÀÌ ÀÖ´ÂÁö Ã£¾Æ¼­ Ãß°¡.
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

// static ÆÑÅä¸® ÇÔ¼ö
DnSkillHandle CDnSkill::CreateSkill( DnActorHandle hActor, int iSkillTableID, int iLevel )
{
	if( false == CDnSkillTask::IsActive() )
		return DnSkillHandle();

#if defined(_DEBUG)
	if( false == ValidateSkillData( iSkillTableID, iLevel ) )
		return DnSkillHandle();
#endif

	// ·¹º§ 0À¸·Î »ý¼º ¿äÃ» µé¾î¿À¸é ¾ð¶ô¸¸ µÈ ¹ÌÈ¹µæ ½ºÅ³ÀÓ.
	bool bAcquired = (iLevel != 0);
	if( false == bAcquired )
		iLevel = 1;		// ·¹º§ 0À¸·Î´Â »ý¼ºÇÒ ¼ö ¾øÀ¸¹Ç·Î 1·Î ¼ÂÆÃ

	CDnSkill* pNewSkill = NULL;

	// ½ºÅ³ ¹ßµ¿ Á¶°Ç, ¹ßµ¿ ÇÁ·Î¼¼¼­µéÀ» ´Þ¾ÆÁØ´Ù.
	// °¢°¢ 5°³¾¿ ÀÖ°í ÆÄ¶ó¸ÞÅÍ´Â ÀüºÎ ÇÕÃÄ 10°³ÀÓ. º¯°æµÉ °¡´É¼ºµµ ÀÖ´Ù.
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
	// ¾×¼Ç ÇÁ·Î¼¼¼­¸¦ µÚÁ®¼­ »ç¿ëÇÏ´Â ¾×¼Ç.. °Á ±ò²ûÇÏ°Ô ÀÎÀÚ ¹Þ´Â°Ô ³ªÀ»µí. ¤Ð¤Ð
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
					// ½Ã±×³ÎÀ» µÚÁ®¼­ ÇÁ·ÎÁ§Å¸ÀÏÀÌ ÀÖ´ÂÁö °Ë»ç
					int iNumSignals = (int)pElement->pVecSignalList.size();
					for( int iSignal = 0; iSignal < iNumSignals; ++iSignal )
					{
						CEtActionSignal* pSignal = pElement->pVecSignalList.at(iSignal);
						if( STE_Projectile == (SignalTypeEnum)pSignal->GetSignalIndex() )
						{
							// ÀÚ½ÅÀÇ ¹«±â »ç°Å¸®¸¦ ´õÇØ¼­ ¹üÀ§ ¼³Á¤
							ProjectileStruct* pSignalData = static_cast<ProjectileStruct*>(pSignal->GetData());

							// ¹«±â Å×ÀÌºí¿¡¼­ »ç°Å¸® Á¤º¸ °¡Á® ¿È	
							DNTableFileFormat*  pWeaponTable = GetDNTable( CDnTableDB::TWEAPON );
							int iLength = pWeaponTable->GetFieldFromLablePtr( pSignalData->nWeaponTableID, "_Length" )->GetInteger();

							if( pSignalData->bIncludeMainWeaponLength )
							{
								DnWeaponHandle hWeapon = m_hActor->GetWeapon( 0 );
								if( hWeapon )
									iLength += hWeapon->GetWeaponLength();
							}

							// ½ºÅ³ »ç°Å¸®±îÁö ´õÇØÁÜ
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
	//½ºÅ³ ·¹º§ Å×ÀÌºí µ¥ÀÌÅ¸°¡ ÀÖ´Â Áö È®ÀÎ..
	{
		vector<int> vlSkillLevelList;
		if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) == 0 ) 
			return false;

		if ( vlSkillLevelList.empty() )
			return false;
	}

	SKILL_LEVEL_INFO* pTableInfo = GetSkillTask().GetSkillLevelTableIDList(iSkillTableID, iSkillLevelDataApplyType);

	// ÇØ´ç ´ë»óÀÇ µ¥ÀÌÅÍ°¡ ¾ø´Â °æ¿ì¿£ ±×³É ¾Æ¹«°Íµµ ¾ÈÇÏ°í ¸®ÅÏÇÏ¸é µÈ´Ù.
	if (pTableInfo->_SkillLevelTableIDs.empty())
		return true;

	int iSkillLevelTableID = -1;
	//NextLevelÀÌ ¿¬¼ÓÀ¸·Î ÀÖÁö ¾ÊÀ» ¼ö ÀÖ´Ù.. ±×·¡¼­ ¿©±â¼­ ´ÙÀ½ ·¹º§ µ¥ÀÌÅ¸ Å×ÀÌºí ID¸¦ Ã£¾Æ ³õ´Â´Ù.
	int iMinSkillLevelTableID = -1;
	int iNextSkillLevelTableID = -1;

	//ÇöÀç ·¹º§ÀÇ Å×ÀÌºí ID¸¦ °Ë»ö.
	SKILL_LEVEL_TABLE_IDS::iterator findIter = pTableInfo->_SkillLevelTableIDs.find(iLevel);
	if (findIter != pTableInfo->_SkillLevelTableIDs.end())
		iSkillLevelTableID = findIter->second;

	//´ÙÀ½ ·¹º§
	findIter = pTableInfo->_SkillLevelTableIDs.find(iLevel+1);
	if (findIter != pTableInfo->_SkillLevelTableIDs.end())
		iNextSkillLevelTableID = findIter->second;

	//ÃÖ¼Ò ·¹º§
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

	// pve, pvp ´ë»óÀÎÁö È®ÀÎÇÏ¿© °É·¯³¿.
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

	// ÇØ´ç ´ë»óÀÇ µ¥ÀÌÅÍ°¡ ¾ø´Â °æ¿ì¿£ ±×³É ¾Æ¹«°Íµµ ¾ÈÇÏ°í ¸®ÅÏÇÏ¸é µÈ´Ù.
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

	// ½ºÅ³ ¹ßµ¿ Á¶°Ç, ¹ßµ¿ ÇÁ·Î¼¼¼­µéÀ» ´Þ¾ÆÁØ´Ù.
	// °¢°¢ 5°³¾¿ ÀÖ°í ÆÄ¶ó¸ÞÅÍ´Â ÀüºÎ ÇÕÃÄ 10°³ÀÓ. º¯°æµÉ °¡´É¼ºµµ ÀÖ´Ù.
	char caLabel[ 32 ];
	int iCheckerParamOffset = 0;
	int iProcessorParamOffset = 0;
	for( int i = 0; i < MAX_PROCESSOR_COUNT; ++i )
	{
		// ¹ßµ¿Á¶°Ç °´Ã¼ ÀÌ¸§À» Ã£´Â´Ù. ÆÄ¶ó¸ÞÅÍ ÇÊµå°¡ ºñ¾îÀÖÀ¸¸é »ý¼º ÇÔ¼öµé¿¡¼­ NULL ¸®ÅÏµÊ
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
				OutputDebug( "[SkillLevelTable Error!] %d ÀÇ ÆÄ¶ó¸ÞÅÍ °³¼ö°¡ Àß¸øµÇ¾ú½À´Ï´Ù.\n", iSkillLevelTableID );
				_ASSERT( !"½ºÅ³ ·¹º§ Å×ÀÌºí ÆÄ¶ó¸ÞÅÍ Àß¸øµÊ. OutputDebug Ãâ·Â È®ÀÎ!" );
			}

			this->AddUsableCheckers( pUsableChecker, iSkillLevelDataApplyType );
		}


		// Å¬¶ó¿¡¼­´Â »ç¿ëÇÏÁö ¾ÊÀ½. Áö¿ªº¯¼ö·Î ±×³É ³Ö¾îÁØ´Ù.
		set<string> setUseActionName;
		iOffsetCheck = iProcessorParamOffset;
		IDnSkillProcessor* pSkillProcessor = IDnSkillProcessor::Create( m_hActor, iProcessor, iSkillLevelTableID, &iProcessorParamOffset, setUseActionName );

		if( NULL != pSkillProcessor )
		{
			if( (iProcessorParamOffset - iOffsetCheck) != pSkillProcessor->GetNumArgument() )
			{
				OutputDebug( "[SkillLevelTable Error!] %d ÀÇ ÆÄ¶ó¸ÞÅÍ °³¼ö°¡ Àß¸øµÇ¾ú½À´Ï´Ù.\n", iSkillLevelTableID );
				_ASSERT( !"½ºÅ³ ·¹º§ Å×ÀÌºí ÆÄ¶ó¸ÞÅÍ Àß¸øµÊ. OutputDebug Ãâ·Â È®ÀÎ!" );
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

	// ½ºÅ³ Å×ÀÌºíÀÇ ÃÖ´ë ·¹º§Àº ½Å·ÚÇÒ ¼ö ¾ø´Ù. -_-
	// ½ÇÁ¦ °¹¼ö·Î ¾÷µ¥ÀÌÆ®. ÀÌÀü¿¡ ÃÊ±âÈ­ µÇ¾î ÀÌ¹Ì °ªÀÌ ÀÖ´Ù¸é °ªÀÌ ´Ù¸¥ °æ¿ì assert!!
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

	// ¸¸¾à ±Û·Î¹ú ½ºÅ³ ±×·ìÀÌ ¼³Á¤µÇ¾îÀÖ´Ù¸é ½ºÅ³ ÄðÅ¸ÀÓÀ» ±Û·Î¹ú ÄðÅ¸ÀÓÀ¸·Î ´ëÃ¼ ½ÃÄÑÁØ´Ù.
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

	// ½ºÅ³ ¼³¸íÀÇ ±âº» Æ²Àº ¾È¹Ù²î°í ÀÎÀÚ°ª¸¸ ¹Ù²ð °ÍÀÌ´Ù. µû¶ó¼­ ÀÎÀÚ°ª¸¸ ¸ðµåº°·Î ³ª´«´Ù.
	m_iSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationID" )->GetInteger();

	char *szParam = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationIDParam" )->GetString();
	MakeUIStringUseVariableParam( m_strSkillExplainString[ iSkillLevelDataApplyType ], m_iSkillExplainStringID, szParam );
	

	// ½ºÅ³ ·¹º§ Å×ÀÌºí¿¡¼­ °°Àº ½ºÅ³Àº ·¹º§ µ¥ÀÌÅÍ°¡ ¿¬¼ÓÇØ¼­ ÀÖ´Ù´Â °¡Á¤ ÇÏ¿¡.
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
			OutputDebug("½ºÅ³·¹º§ Å×ÀÌºí¿¡¼­ °°Àº ½ºÅ³Àº ·¹º§µ¥ÀÌÅÍ°¡ ¿¬¼ÓÀÌ ¾Æ´Ï³×???\n");
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
			OutputDebug("½ºÅ³·¹º§ Å×ÀÌºí¿¡¼­ °°Àº ½ºÅ³Àº ·¹º§µ¥ÀÌÅÍ°¡ ¿¬¼ÓÀÌ ¾Æ´Ï³×???\n");
			m_iNextSkillExplainStringID = -1;
		}
#endif // PRE_FIX_NEXTSKILLINFO
	}
	else
		m_iNextSkillExplainStringID = -1;

	// »óÅÂ È¿°ú Á¤º¸ ·Îµù
	StateEffectStruct StateEffect;
	for( int i = 0; i < MAX_STATE_EFFECT_COUNT; ++i )
	{
		sprintf_s( caLable, "_EffectClass%d", i + 1 );
		StateEffect.nID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();

		if( StateEffect.nID < 1 ) 
			continue;

		sprintf_s( caLable, "_EffectClass%dApplyType", i + 1 );
		int iApplyType = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();
		bool bApplyAll = (StateEffectApplyType::ApplyAll == iApplyType);		// ¸ðµÎ Àû¿ëÀÓ. Å¸°Ù¸¸ ´Ù¸£°Ô ÇØ¼­ ¶È°°Àº »óÅÂÈ¿°ú 2°³¸¦ Ãß°¡ÇØÁØ´Ù.

		if( bApplyAll )
			StateEffect.ApplyType = StateEffectApplyType::ApplySelf;
		else
			StateEffect.ApplyType = (StateEffectApplyType)iApplyType;

		sprintf_s( caLable, "_EffectClassValue%d", i + 1 );
		StateEffect.szValue = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetString();

		sprintf_s( caLable, "_EffectClassValue%dDuration", i + 1 );
		StateEffect.nDurationTime = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetInteger();

		m_vlStateEffectList[ iSkillLevelDataApplyType ].push_back( StateEffect );

		// ¸ðµÎ Àû¿ëÀÌ¸é Å¸°ÙÀ¸·Î ¹Ù²ã¼­ ¶È°°ÀÌ ÇÑ ¹ø ´õ ³Ö¾îÁÜ.
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

	// °¢ ¸ðµåº°·Î µ¥ÀÌÅÍ ·Îµå. ¸ÕÀú PVP ¸¦ ÀÐ´Â´Ù.
	// PVE ´Â µðÆúÆ® °ªÀÌ±â ¶§¹®¿¡ ¸ðµåº°·Î ³ª´µ¾îÁö´Â °ªÀÌ ¾Æ´Ï¸é µðÆúÆ® °ªÀ¸·Î Ã¤¿öÁö°Ô µÈ´Ù.

	// ¸ó½ºÅÍÀÎ °æ¿ì¿£ pvp µ¥ÀÌÅÍ°¡ ¾øÀ¸¹Ç·Î ÇÔ¼ö ¾È¿¡¼­ ¾Æ¹«°Íµµ ¾ÈµÇ°í ¸®ÅÏµÈ´Ù.
	// ¸¸¾à ÇÃ·¹ÀÌ¾îÀÎµ¥ ¾Æ¹«°Íµµ ¾ø´Ù¸é Àß¸øµÈ °ÅÀÓ.
	if( false == _LoadSkillLevelData( iSkillTableID, iLevel, PVP ) )
		return false;

	if( false == _LoadSkillLevelData( iSkillTableID, iLevel, PVE ) )
		return false;

	// ÃÊ±âÈ­ ÀÌÈÄ¿¡ µû·Î ¸ð¾Æ³õÀ» Á¤º¸µé.
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
	
	// ¾ÆÀÌÄÜ ÀÌ¹ÌÁö ¼Â
	SetIconImageIndex( pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_IconImageIndex" )->GetInteger() );

	// ÃÊ±âÈ­ÇßÀ» ¶© PVE ±âÁØÀ¸·Î ÃÊ±âÈ­ ÇØÁØ´Ù. µðÆúÆ® °ª.
	m_iSelectedSkillLevelDataApplyType = PVE;
	m_bEnableTooltip = ( pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillTip" )->GetInteger() ) ? true : false;

	if (m_eDurationType == SummonOnOff)
		OnInitializeSummonMonsterInfo();

	// Å¬¶ó ¸Þ¸ð¸® ÃÖÀûÈ­ °ü·Ã ÀÛ¾÷. ¸¶À»ÀÎ °æ¿ì ¸¶À»¿¡¼­ ¾µ ¼ö ÀÖ´Â ½ºÅ³¸¸ Çã¿ëÇÑ´Ù.
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
			if( CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 0 ] ) == CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 1 ] ) )	// 1. µÑ´Ù ÁÖ¹«±âÀÌ°Å³ª º¸Á¶¹«±â ÀÎ °æ¿ì or
			{
				int nWeapon = CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 0 ] ) ? 1 : 0;
				if( m_hActor->GetWeapon( nWeapon, false ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( nWeapon, false )->GetEquipType()
					|| m_hActor->GetWeapon( nWeapon, false ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( nWeapon, false )->GetEquipType() )
					bSatisfy = true;
			}
			else	// 2. ÁÖ¹«±â, º¸Á¶¹«±âÀÎ °æ¿ì and
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
						// bActionMatchWeapon ¸¦ µðÆúÆ®·Î true ·Î È£ÃâÇÏ´Ï±î ¾×¼Ç¿¡ µû¶ó ¹«±â°¡ ´Ù¸£°Ô ³ª¿À´Â µí ÇÏ¿© Ç×»ó Â÷°í ÀÖ´Â ¸ÞÀÎ ¹«±â À§ÁÖ·Î Ã¼Å©Åä·Ï..
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

		//¸¶À»¿¡¼­ ½ºÅÄ½º º¯°æ »óÅÂÈ¿°ú ½ºÅ³ »ç¿ë ¸øÇÔ..
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

	//¾ÆÀÌÅÛ ½ºÅ³ÀÌ¸é ¾Æ·¡ Çã¿ë ½ºÅ³ Ã¼Å©¸¦ ÇÏÁö ¾Êµµ·ÏÇÑ´Ù..
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

	// ´ëÆ÷ ¸ðµåÀÏ ¶© ÇÃ·¹ÀÌ¾î´Â ½ºÅ³ »ç¿ë ¸øÇÑ´Ù.
	if( m_hActor->IsPlayerActor() )
	{
		if( static_cast<CDnPlayerActor*>(m_hActor.GetPointer())->IsCannonMode() )
			return UsingResult::Failed;
	}

	// ÀÜ¿© SP Ã¼Å©, Ã¼·Â Ã¼Å©, ·¹º§ Ã¼Å© µîµî
	// ¿À¶ó³ª Åä±ÛÀº ÇöÀç È°¼ºÈ­ ÁßÀÌ¶ó¸é MP »ó°ü ¾øÀÌ ²ø ¼ö ÀÖ´Ù.
	if( !IsToggleOn() && !IsAuraOn() )
		if( m_hActor->GetSP() < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;

	// ¿ö¸®¾îÀÇ ¸±¸®ºê°°Àº ½ºÅ³Àº ¼ö¸é, ½ºÅÏ ÁßÀÏ¶§µµ »ç¿ë°¡´ÉÇØ¾ß ÇÑ´Ù.
	// Å×ÀÌºí¿¡ °ø°ÝºÒ°¡ ¹«½Ã ¹ßÇöÅ¸ÀÔÀ» Ãß°¡ÇÒ±îÇÏ´Ù°¡ ÇöÀç ¸±¸®ºê¿¡¼­¸¸ ÀÇ¹ÌÀÖ´Â °ÍÀÌ¹Ç·Î
	// ´Ù¸¥ °÷¿¡¼­ ¾î¶² ½ÄÀ¸·Î ¾²ÀÌ°Ô µÉÁö Á» ´õ ÁöÄÑº¸°í ±Ô°ÝÈ­ ½ÃÅ°µµ·Ï ÇÑ´Ù.
	// ¿ì¼± Dissolve »óÅÂÈ¿°ú ÀÖ´Â ½ºÅ³Àº Çàµ¿ ºÒ°¡ Ã¼Å©¸¦ °Ç³Ê¶Ú´Ù.
	// ½ºÅ³¿¡ ÇØ´çÇÏ´Â ¾×¼ÇÀÇ State ½Ã±×³Î¿¡ ¹Ýµå½Ã IgnorectCantAction ÀÌ ÄÑÁ® ÀÖ¾î¾ß ½ºÅ³ ¾×¼ÇÀÌ ³ª°£´Ù.
	bool bPassCheckCantAction = false;
	for( DWORD i = 0; i < GetStateEffectCount(); ++i )
	{
		//#40480 °áºù»óÅÂ¿¡¼­ ´ÜÃà½½·Ô È°¼ºÈ­¸¦ À§ÇØ¼­..
		CDnSkill::StateEffectStruct* pSE = GetStateEffectFromIndex( i );

		if( STATE_BLOW::BLOW_069 == pSE->nID )
		{
			// 76643 ( Dissolvable ÀÌ 2¸é ±×³É ºñÈ°¼ºÈ­ ÇØ´Þ¶ó°í ÇÕ´Ï´Ù )
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

	// ½ºÅ³ »ç¿ëºÒ°¡ »óÅÂÈ¿°ú°¡ ÀÖÀ¸¸é mp ¼Ò¸ðÇÏ´Â ½ºÅ³Àº »ç¿ëÇÒ ¼ö ¾ø´Ù.
	if( 0 < m_hActor->GetCantUseSkillSEReferenceCount() )
		if( 0 < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;

	// 67¹ø ½ºÅ³ Á¦ÇÑ »óÅÂÈ¿°ú°¡ ÀÖÀ¸¸é ¾×Æ¼ºê ½ºÅ³Àº »ç¿ëÇÒ ¼ö ¾øÀ½. 
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
	//½ºÅ³ ·¹º§¾÷ ¾ÆÀÌÅÛ¿¡ ÀÇÇØ ½ºÅ³·¹º§¾÷ÀÌ µÈ ½ºÅ³Àº Ä³¸¯ÅÍ ·¹º§ Ã¼Å© ÇÏÁö ¾Êµµ·Ï ÇÑ´Ù.
	isCharacterLevelCheck = (GetLevelUpValue() == 0);

	if (isCharacterLevelCheck)
	{
		if( m_hActor->GetLevel() < m_iLevelLimit )
			return UsingResult::Failed;
	}

	// ÇÊ¿ä ¹«±â°¡ ÀÖ´Ù¸é ÀåÂøÇß´ÂÁö È®ÀÎ. ÃÖ´ë 2°³ÀÓ. µÑ Áß ÇÏ³ª¸¸ ÃæÁ·µÇµµ ½ºÅ³ »ç¿ë °¡´É.
	if( false == IsSatisfyWeapon() )
		return UsingResult::Failed;

	// È­»ì °°Àº ¼Ò¸ð¼º ¾ÆÀÌÅÛÀÇ °¹¼ö È®ÀÎ
	bool bIsCheckNeedItem = true;
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP && static_cast<CDnPvPGameTask*>(pGameTask)->IsLadderMode() && bIsExtremitySkill() )	// ·¡´õ¿¡¼­ ±Ã±Ø±â ½ºÅ³
		bIsCheckNeedItem = false;

	if( bIsCheckNeedItem && m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ] > 0 )
	{
		int iNumNeedItem = GetItemTask().GetCharInventory().ScanItemFromID( m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ], NULL );
		if( iNumNeedItem < m_iNeedItemDecreaseCount[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;
	}
	// Á÷¾÷ Ã¼Å©. 
	// ÇöÀç ¸¶À»¿¡¼­´Â ½Ã¾ß¿¡ µé¾î¿Â À¯ÀúÀÇ Á÷¾÷ Ç® È÷½ºÅä¸®°¡ ÀÖÁö ¾Ê°í ÃÖ±ÙÁ÷¾÷¸¸ ¼ÂÆÃµÇ¹Ç·Î 
	// ¸¶À»¿¡¼­ ¸¸³­ ´Ù¸¥ À¯ÀúµéÀÇ Á÷¾÷±îÁö´Â Ã¼Å©ÇÏÁö ¾Ê½À´Ï´Ù.
	// ´øÀü¿¡¼­´Â ÆÄÆ¼¿øÀÇ Ç® È÷½ºÅä¸®¸¦ ¹Þ¾Æ¿À±â ¶§¹®¿¡ ¸ðµÎ Ã¼Å©ÇÔ.
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

		// Á×¾úÀ¸¸é ºÒ°¡
		if( m_hActor->IsDie() ) 
			return UsingResult::Failed;
		// ¹èÆ² ¸ðµå°¡ ¾Æ´Ï¸é ½ºÅ³ ¹ßµ¿ ºÒ°¡!
		if( !pActor->IsBattleMode() )
			return UsingResult::Failed;
	}

	// ÄðÅ¸ÀÓÀÌ ³¡³ªÁö ¾Ê¾Ò´Ù¸é ½ºÅ³ ¹ßµ¿ ºÒ°¡
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
	{ // m_fAnotherGlobalSkillCoolTimeÀÌ Àû¿ë µÇ¾î ÀÖÀ¸¸é Àû¿ë ¾È ÇÏµµ·Ï..
		
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
	// ½ÃÀÛÇÒ¶§ LeftDelayTimeÀÌ 0.0ÀÌ µÇ¸é m_fCoolTimeÀ» 0.0f·Î º¯°æÇØ¾ßÇÑ´Ù.
	// ±×·¸Áö ¾ÊÀ¸¸é ÄðÅ¸ÀÓÀÌ 1.0À¸·Î °è¼Ó À¯ÁöµÊ.(ProcessÇÔ¼ö¿¡¼­ m_fLeftDelayTimeÀÌ 0ÀÌ¸é CoolTime°»½Å¾ÈµÊ.
	if (m_fLeftDelayTime == 0.0f)
	{
		m_fCoolTime = 0.0f;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
		m_fDeltaGlobalCoolTime = 0.0f;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
	}

	// ÄðÅ¸ÀÓ ¹«½Ã Ä¡Æ®Å°°¡ ÀÖ´Ù¸é ÄðÅ¸ÀÓ ¹«½ÃÅä·Ï Á¶Ä¡.
#ifndef _FINAL_BUILD
	if( CGlobalValue::GetInstance().m_bIgnoreCoolTime ) {
		m_fCoolTime = 0.f;
		m_fLeftDelayTime = 0.f;
	}
#endif
}

void CDnSkill::_OnBeginProcessException( void )
{
	// °áºù»óÅÂ¿¡¼­ ¿ö¸®¾î ¸±¸®ºê ½ºÅ³À» ¾²´Â °æ¿ì ¹Ù·Î ÇÁ·¹ÀÓ °íÁ¤À» Ç®¾îÁØ´Ù. #12438
	// °áºù»óÅÂÈ¿°ú°¡ °É¸° »óÅÂ¿¡¼­ µð¹öÇÁ »óÅÂÈ¿°ú ÇØÁ¦ »óÅÂÈ¿°ú°¡ self ·Î Àû¿ëµÇ´Â ½ºÅ³À» 
	// »ç¿ëÇÏ´Â °æ¿ì·Î ÀÏ¹ÝÈ­ ½ÃÅ´.
	if( m_hActor && 
		(
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_041 ) ||
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_146 ) ||	//#58432
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_218 ) //#53900 Escape½ºÅ³ Ãß°¡
		))
	{
		int iNumStateBlow = (int)m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size();
		for( int i = 0; i < iNumStateBlow; ++i )
		{
			const StateEffectStruct& SE = m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].at( i );
			if( STATE_BLOW::BLOW_069 == SE.nID &&
				StateEffectApplyType::ApplySelf == SE.ApplyType )
			{
				// ÇÁ·¹ÀÓ Á¤Áö »óÅÂÈ¿°ú ÂüÁ¶ ÀÎµ¦½º¸¦ ¸ðµÎ Ç®¾îÁØ´Ù. ÇÁ·¹ÀÓÀÌ µ¹¾Æ¾ß ¾×¼ÇÀÌ µ¹¾Æ°¡°í
				// ¾×¼ÇÀÌ µ¹¾Æ°¡¾ß ¸±¸®ºê »óÅÂÈ¿°ú°¡ µé¾î°£´Ù.
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
	// °°Àº ±Û·Î¹ú ID ¸¦ »ç¿ëÇÏ´Â ´Ù¸¥ ±Û·Î¹ú ½ºÅ³À» »ç¿ëÇÏ¿© ¼ÂÆÃµÈ ÄðÅ¸ÀÓ °ªÀ» ½ÇÁ¦·Î ÀÌ ½ºÅ³ »ç¿ëµÉ ¶§´Â Á¦°ÅÇÑ´Ù.
	m_fAnotherGlobalSkillCoolTime = 0.0f;

	m_nAnotherGlobakSkillID = 0;

#if defined( PRE_ADD_ACADEMIC ) // ¼ÒÈ¯Çü ¸ó½ºÅÍ ¾×Æ¼ºê Åä±Û ½ºÅ³Àº ÄðÅ¸ÀÓÀÌ µ¹ÇÊ¿ä°¡ ¾ø½À´Ï´Ù.
	if(m_eDurationType != DurationTypeEnum::ActiveToggleForSummon)
	{
		//--------------------------------------------------
		//[debug_skill]
		//¼¼ÄÜ´ÎÊýÉèÖÃ
		//Ñ¸²½Õ¶=6001   ÐÇÐÎ¶ú¶¤=6003  Ìì½µÓêµã=6204   ¼«¶È´ò»÷=6208

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
                SetSkillNowNum( nNowNum ); //Ê¹ÓÃ´ÎÊýÇåÁã
				OnBeginCoolTime();
			} 
			else
			{
				nNowNum++;
                SetSkillNowNum( nNowNum ); //Ê¹ÓÃ´ÎÊýÔö¼Ó1
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

	// #31909 129¹ø ¾×¼Ç ¼Â »óÅÂÈ¿°ú¸¦ ¾²´Â °æ¿ì (¼¼ÀÌÅ©¸®µå ÇØ¸Ó¸µ) ±âº» °ø°Ý ¾×¼ÇÀ» º¯°æÇØ¼­ ±¸ÇöÅä·Ï µÇ¾îÀÖ´Âµ¥
	// ¹«±â¸¦ µé°í ÀÖÀ¸¸é ¹«±â ¾×¼ÇÀÌ ³ª°¡¹Ç·Î CDnPlayerActor::SetActionQueue, SetAction µî¿¡¼­ 
	// ¹«±â º¯°æ ¾×¼ÇÀ» ÇÏ°í ÀÖ´Â °æ¿ì ±âº» ¾×¼ÇÀ¸·Î ´Ù½Ã ¹Ù²ãÁà¼­ Èê·Áº¸³»´Âµ¥ ÇöÀç ¹«±â ¾×¼ÇÀ¸·Î 
	// º¯°æµÇ¾ú´Ù´Â ÇÃ·¡±×°¡ OnChangeAction ¿¡¼­ false ·Î ¸®¼ÂµÇ¹Ç·Î ÇØ¸Ó¸µ 4Å¸ ¶§¸° ÈÄ¿¡ 
	// ½ºÅ³ Äü½½·Ô°ú µ¿½Ã¿¡ ´­·¶À» °æ¿ì ÇÃ·¡±×°¡ ÄÑÁ® ÀÖ´Â »óÅÂ±â ¶§¹®¿¡ ½ºÅ³ ÂÊ¿¡¼­ ¼ÂÆÃÇÑ ½ºÅ³ ¾×¼ÇÀ» 
	// ½ÇÇà½ÃÅ³ ¶§ CDnPlayer::SetActionQueue ¿¡¼­ ½ºÅ³ ¾×¼ÇÀÌ ¾ÃÈ÷°Ô µÈ´Ù. µû¶ó¼­ ½ºÅ³ ¾µ ¶§ 
	// ÇØ´ç ¹«±â º¯°æ ÇÃ·¡±×°¡ ÄÑÁ® ÀÖ´Â °æ¿ì ¿©±â¼­ ´Ù½Ã ²¨ÁÖ°í ½ºÅ³À» ³ª°¡°Ô ÇØÁØ´Ù. 
	// (ÇöÀç ¹®Á¦Á¡Àº ¼­¹ö´Â Á¤»óÀûÀ¸·Î ½ºÅ³ ¾×¼ÇÀ» ÇÏÁö¸¸ Å¬¶ó¿¡¼­ ÇØ¸Ó¸µ ¾×¼ÇÀÌ ³ª¿À´Â °ÍÀÓ)
	if( m_hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( pPlayerActor->IsChangedEquipAction() )
		{
			pPlayerActor->ClearChangedEuipAction();
		}
	}

	// ½ºÅ³ °®°í ÀÖ´Â ¾×ÅÍÀÇ ÆÀÀÌ ¼ÂÆÃµÇ¾îÀÖ´Â°¡.
	// ¸ó½ºÅÍÀÎ °æ¿ì ¾×ÅÍ ¼ÂÆÃÀÌ ¾ÈµÇ¾îÀÖ´Â °æ¿ì°¡ ÀÖÀ¸¹Ç·Î ¿©±â¼­ ÇØÁÜ.
	if( !m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].hSkillUser )
		m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].hSkillUser = m_hActor;

	// pvp °°Àº °Å ÇÏ¸é ÆÀ¼ÂÆÃÀÌ ¹Ù²ð ¼ö ÀÖÀ¸¹Ç·Î Ç×»ó ½ºÅ³ »ç¿ë½Ã¿¡ ÆÀ¼ÂÆÃ Á¦´ë·Î ÇÑ´Ù.
	m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].iSkillUserTeam = m_hActor->GetTeam();

	m_LastTimeToggleMPDecreaseTime = LocalTime;
	m_bFinished = false;

//	// [2010/12/15 semozz]
//	// ¸¶À»¿¡¼­´Â ¼­¹ö ÆÐÅ¶À» ¹ÞÀ» ¼ö ¾ø¾î¼­
//	// ±âÁ¸ Å¬¶óÀÌ¾ðÆ®°¡ ÀÚ½ÅÀÇ »óÅÂÈ¿°ú Ã³¸®ÇÏ´Â ºÎºÐ ±×´ë·Î µ¿ÀÛÇÏµµ·Ï ÇÑ´Ù.
	CDnWorld* pWorld = CDnWorld::GetInstancePtr();
	bool bVillage = (CDnWorld::MapTypeEnum::MapTypeVillage == pWorld->GetMapType());

	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->OnBegin( LocalTime, fDelta, GetMySmartPtr() );
	}

	_OnBeginProcessException();

	//¸¶À»¿¡¼­´Â 
	if (m_eDurationType == SummonOnOff && !bVillage)
	{
		//¸ó½ºÅÍ ¼ÒÈ¯ µÇ¸é Toggle On..½ÃÅ´..
		EnableToggle(true);
	}
}

// ½ºÅ³¿¡ ´Þ·ÁÀÖ´Â Processor µé Áß¿¡ ¼±Ã³¸® ÇØ¾ßÇÏ´Â °Íµé ¸ÕÀú Ã³¸®.
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
			// ¾×¼ÇÀ» ¹Ù²ãÁØ´Ù.
			CDnPlayAniProcess* pPlayAniProcessor = static_cast<CDnPlayAniProcess*>(GetProcessor( IDnSkillProcessor::PLAY_ANI ));
			pPlayAniProcessor->ChangeActionNameOnce( pChangedActionName );
		}
	}
}

void CDnSkill::CheckProcessorOnEnd( void )
{

}



// ¼­¹öÃø¿¡¼­´Â »óÅÂÈ¿°ú ÆÐÅ¶À» ³¯·ÁÁÖ°í ¾ÆÀÌÅÛ¿¡ ´Þ·ÁÀÖ´Â ¾×¼ÇÀ» ½ÇÇà½ÃÅ´.
// Å¬¶ó¿¡¼­´Â ¾×¼Ç¸¸ ½ÇÇà½ÃÅ²´Ù.
void CDnSkill::OnBeginForceSync( LOCAL_TIME LocalTime, float fDelta )
{
	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->OnBegin( LocalTime, fDelta, GetMySmartPtr() );
	}
}

// ½ÇÁ¦·Î ½ºÅ³ÀÌ ¹ßµ¿ µÇ¾úÀ» ½Ã¿¡ Ã³¸®
void CDnSkill::ProcessExecute( LOCAL_TIME LocalTime, float fDelta )
{
	// µþ·Á ÀÖ´Â ÇÁ·Î¼¼¼­ Ã³¸®
	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->Process( LocalTime, fDelta );
	}

	// -1 ÀÎ °æ¿ì´Â ´ë½Ã Ã³·³ ¹æÇâÅ°¸¦ ´©¸£°í ÀÖ´Âµ¿¾È °è¼Ó ¹ßµ¿µÇ´Â ½ºÅ³.
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
			// #27257, #27272°ü·Ã ¼öÁ¤ [2011/01/07 semozz]
			// °ÔÀÓ ¼­¹ö¿¡¼­´Â ¿©±â µî·Ï µÇ´Â »óÅÂ È¿°ú ¾øÀ½(°ÔÀÓ ¼­¹ö»ó¿¡¼­´Â ¸ðµç »óÅÂÈ¿°ú ÆÐÅ¶À¸·Î µ¿±âÈ­µÊ)
			// Ä³¸¯ÅÍ¿¡¼­ ÆÐÅ¶ ¾øÀÌ Ãß°¡µÈ »óÅÂÈ¿°ú¸¦ ½ºÅ³ÀÌ ³¡³ª´Â ½ÃÁ¡¿¡¼­ Á¦°ÅÇÏ´Âµ¥..
			// ÀÌ »óÅÂÈ¿°úµéÀº ¹Ù·Î Á¦°Å µÇ¾î¾ß ÇÔ..
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

// Äð Å¸ÀÓ µîµîÀÇ Ã³¸®
void CDnSkill::Process( LOCAL_TIME LocalTime, float fDelta )
{
	// ÄðÅ¸ÀÓ Á¶Àý ±âº»°ªÀº 1.0 ÀÓ.
	// ÇöÀç m_fCoolTimeAdjustBlowValue °ªÀº »ç¿ëÇÏÁö ¾Ê´Â´Ù. 1.0 ±âº»°ªÀ¸·Î ±×´ë·Î ÀÖ°í ÇöÀç ÄðÅ¸ÀÓ °¨¼Ò »óÅÂÈ¿°ú ±¸ÇöÀÌ ¹Ù²î¾ú´Ù. (#21146)
	// ±æµåÀü ½ºÅ³Àº ÄðÅ¸ÀÓ °¨¼ÒÇÏÁö ¾ÊÀ½
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
						// À©µå¿öÄ¿ÀÇ ½ºÅ³Áß ¼îÅ¸ÀÓ¿¡ ÀÇÇØ fTempAdjustValue°¡ 0ÀÌµÇ´Â °æ¿ì°¡ ¹ß»ý..
						// ¿¹¿Ü Ã³¸® Ãß°¡
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
		case DurationTypeEnum::ActiveToggleForSummon: // ¸ó½ºÅÍ°¡ ¾²´Â ½ºÅ³ÀÌ°í ¿¢¼¿°°Àº°Å´Â ¿µÇâ¾È¹Þ´Â´Ù ±×·¡¼­ ÄÚµå°¡ Á»Æ²¸®´Ù. ¼­¹ö¿¡´Â µ¹Áö¾ÊÀ½.
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

	// ÄðÀÌ Á¾·áµÈ »óÅÂ¶ó¸é ¼ÂÆÃµÈ ±Û·Î¹ú ½ºÅ³ ÄðÅ¸ÀÓÀ» ´Ù½Ã ¾ø¾ÖÁØ´Ù.
	if( 0.0f < m_fAnotherGlobalSkillCoolTime && 0.0f == m_fCoolTime )
	{
		m_fAnotherGlobalSkillCoolTime = 0.0f;

		m_nAnotherGlobakSkillID = 0;
	}
}

bool CDnSkill::IsFinished( void )
{
	bool bResult = false;

	// ¿À¶ó ½ºÅ³ÀÌ¶ó¸é ¸Ê ÀÌµ¿ ÇÏ±â Àü¿£ ³¡³»Áö ¾ÊÀ½.
	if( Aura == m_eDurationType )
		return IsFinishedAuraSkill();

	if( m_bFinished )
		return m_bFinished;

	// -1 ÀÌ¸é ´ë½¬ °°Àº Å° ¶¿ ¶§±îÁö ¾²´Â ¹«ÇÑ ½ºÅ³.
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

		// ÇÃ·¹ÀÌ¾î°¡ È¹µæÇÏÁö ¾ÊÀº ½ºÅ³Àº »ç¿ë ºÒ°¡.
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

		//ÄðÅ¸ÀÓÀÌ µ¹°í ÀÖ´Â Áß
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
				// Åä±Û ½ºÅ³ÀÌ°í Åä±Û µÈ »óÅÂ¶ó¸é 
				// ItemConditionEnum::Toggle ¸®ÅÏ
				if( m_bNowToggled )
				{
					return ItemConditionEnum::Toggle;
				}
			}
			break;

		case Aura:
			{
				// ¿À¶ó ½ºÅ³ÀÌ°í ¿À¶ó°¡ ÄÑÁø »óÅÂ¶ó¸é 
				// ItemConditionEnum::Toggle ¸®ÅÏ
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
			_ASSERT( "ÃÖ´ë ·¹º§ ÃÊ°úÇÏ¿© CDnSkill::LevelUp() È£Ãâ" );
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
		//½ÇÁ¦ ·¹º§¿¡¼­ÀÇ Áõ°¡/°¨¼ÒÀÇ ½ºÅ³ Æ÷ÀÎÆ®°ªÀ» ¾ò¾î ¿Í¾ßÇÑ´Ù.
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
			_ASSERT( "ÃÖ´ë ·¹º§ ÃÊ°úÇÏ¿© CDnSkill::LevelUp() È£Ãâ" );
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

		//½ÇÁ¦ ·¹º§¿¡¼­ÀÇ Áõ°¡/°¨¼ÒÀÇ ½ºÅ³ Æ÷ÀÎÆ®°ªÀ» ¾ò¾î ¿Í¾ßÇÑ´Ù.
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
		if( !m_hActor || m_hActor->IsMonsterActor() )		// °ÔÀÓ¼­¹ö¿Í µ¿ÀÏÇÏ°Ô ¸ÂÃã.
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
		// °°Àº °ÍÀÌ ¿À¸é ÀÚµ¿À¸·Î Ãß°¡ µÇÁö ¾ÊÀ½.
		m_setUseActionNames.insert( string(pActionName) );
	}
}

// ¾×¼Ç Àç»ý ÇÁ·Î¼¼¼­°¡ ¹Ýµå½Ã ÀÖ´Â ¾×Æ¼ºê ½ºÅ³ÀÇ µ¿ÀÛ °è½Â ¿äÃ»!
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

	// ½ºÅ³ °­È­ ¹®Àå °°Àº °æ¿ì pvp/pve ¸ðµÎ ¼ÂÆÃÇØÁØ´Ù.
	for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
	{
		// ¿ø·¡ »óÅÂÈ¿°ú °®°í ÀÖ´ø °¹¼ö
		int iNumOriginalStateEffect = (int)m_vlStateEffectList[ iSkillLevelApplyType ].size();

		// »óÅÂ È¿°ú Ãß°¡
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
			bool bApplyAll = (StateEffectApplyType::ApplyAll == iApplyType);		// ¸ðµÎ Àû¿ëÀÓ. Å¸°Ù¸¸ ´Ù¸£°Ô ÇØ¼­ ¶È°°Àº »óÅÂÈ¿°ú 2°³¸¦ Ãß°¡ÇØÁØ´Ù.

			if( bApplyAll )
				StateEffect.ApplyType = StateEffectApplyType::ApplySelf;
			else
				StateEffect.ApplyType = (StateEffectApplyType)iApplyType;

			sprintf_s( caLable, "_EffectClassValue%d", i + 1 );
			StateEffect.szValue = pGlyphTable->GetFieldFromLablePtr( nGlyphID, caLable )->GetString();

			// Áö¼Ó½Ã°£Àº ±âÁ¸¿¡ ÀÖ´ø »óÅÂÈ¿°ú¿¡¼­ ±×´ë·Î ´ëÃ¼ÇÑ´Ù.
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

			// ¸¸¾à °°Àº »óÅÂÈ¿°ú°¡ ¾ø´Ù¸é Áö¼Ó½Ã°£Àº ±×³É 0 À¸·Î ÇØÁØ´Ù. 
			// ±×·³ ¾×¼Ç°ú µ¿ÀÏÇÑ °£°Ý¸¸Å­ »óÅÂÈ¿°ú°¡ Áö¼ÓµÊ.
			if( false == bFound )
				StateEffect.nDurationTime = 0;

			m_vlStateEffectList[ iSkillLevelApplyType ].push_back( StateEffect ); 

			// ¸ðµÎ Àû¿ëÀÌ¸é Å¸°ÙÀ¸·Î ¹Ù²ã¼­ ¶È°°ÀÌ ÇÑ ¹ø ´õ ³Ö¾îÁÜ.
			if( bApplyAll )
			{
				StateEffect.ApplyType = StateEffectApplyType::ApplyTarget;
				m_vlStateEffectList[ iSkillLevelApplyType ].push_back( StateEffect );
			}
		}
	}

	//½ºÅ³ ¼Ó¼º °ª Ãß°¡
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
				// pair<int, int> <»óÅÂÈ¿°ú, ±â°£>
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

	// ½ºÅ³ °­È­ ¹®Àå °°Àº °æ¿ì pvp/pve ¸ðµÎ ¼ÂÆÃÇØÁØ´Ù.
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
					// pair<int, int> <»óÅÂÈ¿°ú, ±â°£>
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

// °ÔÀÓ¼­¹öµµ ÀÖÀ¸´Ï µ¿±âÈ­ ¸ÂÃß¾îÁÖ¼¼¿ä CheckAndDivideStateEffectArgument ÀÌ ÇÔ¼öµµ ¼öÁ¤ÇØÁÖ¼¼¿ä 
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
			// ±âÁ¸¿¡ Á¸ÀçÇÏ´Â »óÅÂÈ¿°ú¿£ ÀÎÀÚ°ªÀ» ´õÇØÁÖ°í »õ·Î¿î »óÅÂÈ¿°ú´Â ¸®½ºÆ®¿¡ Ãß°¡.
			// ÄðÅ¸ÀÓÀº °­È­ ÆÐ½Ãºê ½ºÅ³ÀÇ ÄðÅ¸ÀÓÀ¸·Î µ¤¾î¾º¿öÁØ´Ù.
			SelectLevelDataType( iSkillLevelApplyType );
			hEnchantPassiveSkill->SelectLevelDataType( iSkillLevelApplyType );
			int iNumStateEffect = (int)hEnchantPassiveSkill->GetStateEffectCount();
			for( int i = 0; i < iNumStateEffect; ++i )
			{
				StateEffectStruct* pEnchantPassiveSE = hEnchantPassiveSkill->GetStateEffectFromIndex( i );
				
				// #40643 ÇÎÆþ¹ãEX Ã³·³ Æ¯¼öÇÏ°Ô ±¸ÇöµÈ »óÅÂÈ¿°úÀÇ ÀÎÀÚ¸¦ °Çµå¸®´Â °æ¿ì..
				// ¾îÂ¿ ¼ö ¾øÀÌ ÇÕÃÄÁà¾ß ÇÑ´Ù. ½ÇÁ¦·Î m_vlStateEffectList ¿¡ Ãß°¡µÇ´Â °ÍÀÌ ¾Æ´Ô.
				if( false == CheckAndUnifyStateEffectArgument( iSkillLevelApplyType, pEnchantPassiveSE ) )
				{
					// °ø°Ý·Â °­È­ »óÅÂÈ¿°ú´ø ¾Æ´Ï°Ç Ãß°¡ÇØÁÖ¸é ½ºÅ³ »ç¿ë½Ã ÀÚµ¿À¸·Î ´É·ÂÄ¡ ¸®ÇÁ·¹½Ã µÈ´Ù.
					m_vlStateEffectList[ iSkillLevelApplyType ].push_back( *pEnchantPassiveSE );
					m_vlStateEffectList[ iSkillLevelApplyType ].back().nFromEnchantPassiveSkillID = hEnchantPassiveSkill->GetClassID();
				}
			}

			// ¾×¼Ç ÇÁ·Î¼¼¼­¸¦ °­È­ ½ºÅ³ÀÇ °ÍÀ¸·Î º¯°æÇÑ´Ù.
			IDnSkillProcessor* pMyPlayAniProcessor = GetProcessor( IDnSkillProcessor::PLAY_ANI );
			if( pMyPlayAniProcessor )
			{
				IDnSkillProcessor* pEnchantPassiveSkillPlayAniProcessor =  hEnchantPassiveSkill->GetProcessor( IDnSkillProcessor::PLAY_ANI );
				if( pEnchantPassiveSkillPlayAniProcessor )
				{
					// ±âÁ¸ °Í ¹é¾÷.
					IDnSkillProcessor* pBackup = new CDnPlayAniProcess;
					pBackup->CopyFrom( pMyPlayAniProcessor );
					m_vlpProcessorBackup[ iSkillLevelApplyType ].push_back( pBackup );

					// °­È­ ÆÐ½Ãºê ½ºÅ³ÀÇ °ÍÀ¸·Î µ¥ÀÌÅÍ Àû¿ë.
					pMyPlayAniProcessor->CopyFrom( pEnchantPassiveSkillPlayAniProcessor );
				}
			}

			IDnSkillProcessor* pMyPartialPlayAniProcessor = GetProcessor( IDnSkillProcessor::PARTIAL_PLAY_ANI );
			if( pMyPartialPlayAniProcessor )
			{
				IDnSkillProcessor* pEnchantPassivePartialSkillPlayAniProcessor =  hEnchantPassiveSkill->GetProcessor( IDnSkillProcessor::PARTIAL_PLAY_ANI );
				if( pEnchantPassivePartialSkillPlayAniProcessor )
				{
					// ±âÁ¸ °Í ¹é¾÷.
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
					// ±âÁ¸ °Í ¹é¾÷.
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
			//UsableChecker¸¦ º£ÀÌ½º ½ºÅ³°ú EX½ºÅ³ ±³Ã¼ÇÑ´Ù...
			vector<IDnSkillUsableChecker*>& enchangeSkillUsableChecker = hEnchantPassiveSkill->GetUsableChecker(iSkillLevelApplyType);

			//EX½ºÅ³¿¡ UsableChecker°¡ ¼³Á¤ µÇ¾î ÀÖ´Â °æ¿ì¸¸ ±³Ã¼ ÇÏµµ·Ï ÇÑ´Ù...
			if (enchangeSkillUsableChecker.size() > 0)
			{
				vector<IDnSkillUsableChecker*>& baseSkillUsableChecker = GetUsableChecker(iSkillLevelApplyType);

				{
					//ÀÚ½ÅÀÇ CheckerµéÀ» ¹é¾÷ ¸®½ºÆ®¿¡ ´ã¾Æ ³õ´Â´Ù.
					vector<IDnSkillUsableChecker*>::iterator iter = baseSkillUsableChecker.begin();
					vector<IDnSkillUsableChecker*>::iterator endIter = baseSkillUsableChecker.begin();

					//¹é¾÷ ¸®½ºÆ®¿¡ ÀúÀå ÇØ³õ°í...
					m_vlUsableCheckersBackup[iSkillLevelApplyType].clear();

					for (; iter != endIter; ++iter)
					{
						m_vlUsableCheckersBackup[iSkillLevelApplyType].push_back((*iter));
					}

				}				
				
				//EX½ºÅ³ÀÇ usableChecker¸¦ º£ÀÌ½º ½ºÅ³¿¡ Àû¿ëÇÑ´Ù.
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

			//EffectIDsµµ º¯°æ..
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
			// ±âÁ¸¿¡ Á¸ÀçÇÏ´Â »óÅÂÈ¿°ú¿£ ÀÎÀÚ°ªÀ» ´õÇØÁÖ°í »õ·Î¿î »óÅÂÈ¿°ú´Â ¸®½ºÆ®¿¡ Ãß°¡.
			// ÄðÅ¸ÀÓÀº °­È­ ÆÐ½Ãºê ½ºÅ³ÀÇ ÄðÅ¸ÀÓÀ¸·Î µ¤¾î¾º¿öÁØ´Ù.
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

			//EffectIDsµµ º¯°æ..
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
	// ½ºÅ³ ÄðÅ¸ÀÓÀÌ ½ºÅ³ ¾Ö´Ï¸ÞÀÌ¼Ç±æÀÌ º¸´Ù ÂªÀ» °æ¿ì CanExecute¿¡¼­ °É·¯ ÁöÁö¸¸
	// SetActionQueue¿¡¼­ µ¿ÀÛ º¯°æÀÌ µÇÁú ¾Ê´Â´Ù. ±×·¡¼­ ½ºÅ³ »ç¿ë °¡´É Ã¼Å©¿¡¼­
	// ÇöÀç ½ºÅ³ µ¿ÀÛÀÌ ³¡³ªÁö ¾ÊÀº ½ÃÁ¡¿¡¼­ ¶È°°Àº ½ºÅ³À» »ç¿ë ¸ø ÇÏµµ·Ï Ã¼Å©ÇÏµµ·ÏÇÑ´Ù.
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
	//¿À¶ó ½ºÅ³ÀÏ¶§ Áö¼Ó Àý´ë/ºñÀ² ¸¶³ª Áõ°¡°¡ ÀÖÀ»¶§ ÃÑ ¸¶³ª º¯È­·®À» ÇÕ»êÇØ¼­ Áö±Ý ³²Àº ¸¶³ª¿Í ºñ±³ÇØ¼­
	//½ºÅ³ Áö¼Ó °¡´É ¿©ºÎ¸¦ ÆÇ´ÜÇÑ´Ù.
	if (m_hActor)
	{
		float fIncManaDelta = 0.0f;

		//Àý´ë Áõ°¡ ºÐ
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

		//ºñÀ² Áõ°¡ ºÐ
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

		//#46381 ¿¤¸®¸àÅ»·Îµå - mp ºÎÁ·À¸·Î ÀÎÇÏ¿© ½¯µå°¡ °è¼Ó À¯ÁöµÊ
#if defined(PRE_FIX_46381)
		//231¹ø »óÅÂÈ¿°ú Àû¿ë.
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

		//ÃÖÁ¾ -Áõ°¡ºÐÀÌ ÇöÀç ³²Àº ¸¶³ªº¸´Ù ¸¹À¸¸é ¸¶³ª ¸ðÀÚ¸§..
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
		//¼­¹ö·Î ¼ÒÈ¯¸ó½ºÅÍ ÇØÁ¦ ¿äÃ»À» º¸³»°í, ¼­¹ö¿¡¼­ ÇØ´ç ¸ó½ºÅÍ °´Ã¼¸¦ Ã£¾Æ¼­ CmdSuicideÈ£Ãâ
		m_hActor->RequestSummonOff(hSummonMonster->GetUniqueID());
		//¼ÒÈ¯ ¸ó½ºÅÍ ¼ÒÈ¯ ÇØÁ¦½Ã Åä±Û ÇØÁ¦..
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

	//½ºÅ³ CheckerÁß¿¡ ¼ÒÈ¯¸ó½ºÅÍ Ã¼Å©°¡ ÀÖÀ¸¸é ¼ÒÈ¯¸ó½ºÅÍ¿¡ »ç¿ë ÇÏ´Â ½ºÅ³???(±âÈ¹ÀÇ°ß..)
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
	//±Û·Î¹ú ÄðÅ¸ÀÓÀÌ ¼³Á¤ µÇ¾î ÀÖ°í, ÄðÅ¸ÀÓÀÌ ÀÌ¹Ì ½ÃÀÛ µÇ¾úÀ» °æ¿ì
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
	//±Û·Î¹ú ÄðÅ¸ÀÓÀÌ ¼³Á¤ µÇ¾î ÀÖ°í, ÄðÅ¸ÀÓÀÌ ÀÌ¹Ì ½ÃÀÛ µÇ¾úÀ» °æ¿ì
	if (m_iGlobalSkillGroupID > 0 )
	{
		//±Û·Î¹ú ÄðÅ¸ÀÓ º¯°æ ¼öÄ¡°¡ ¼³Á¤ µÈ °æ¿ì
		if (m_fDeltaGlobalCoolTime > 0.0)
		{
			float fOrigDelayTime = GetOrigDelayTime();

			//Àû¿ëµÈ ¼öÄ¡°¡ Áö±Ý ¸®¼Â ÇÏ·Á´Â ¼öÄ¡º¸´Ù ÀÛÀº °ªÀÎ °æ¿ì
			if (m_fDeltaGlobalCoolTime < fRate)
				fRate = m_fDeltaGlobalCoolTime;

			//Àû¿ëµÈ ¼öÄ¡´Â °¨¼Ò ½ÃÄÑÁØ´Ù..
			m_fDeltaGlobalCoolTime -= fRate;
			if (m_fDeltaGlobalCoolTime < 0.0f)
				m_fDeltaGlobalCoolTime = 0.0f;

			//ÇöÀç ÄðÅ¸ÀÓÀÌ ³²¾Æ ÀÖ´Â °æ¿ì
			if (m_fLeftDelayTime > 0)
				m_fLeftDelayTime += fOrigDelayTime * fRate;		
		}
	}
}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#if defined(PRE_ADD_65808)
bool CDnSkill::IsSummonMonsterRecall(int& monsterID)
{
	//PlayAniProcess¸¦ ÀÌ¿ëÇÏ°í, ÇØ´ç µ¿ÀÛ¿¡ SummonMonster½Ã±×³ÎÀÌ Á¸Àç ÇÏ¸é true¸¦ ¸®ÅÏ..
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