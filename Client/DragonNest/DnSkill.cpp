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

	m_iSelectedSkillLevelDataApplyType = PVE;		// 디폴트는 pve 이다.

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
	assert( hActor && "CDnSkill::SetHasActor() 액터 핸들이 NULL 임" );

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

			// 사용하는 액션이 있으면 외부에서 조회용으로 데이터 채워놓음
			// 스킬 액션 이름 자체는 현재 pvp/pve 같기 때문에 pve 일 때 기준으로 한번만 셋팅해준다.
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

	// 패시브 스킬이라면 관련된 액션이 있는지 찾아서 추가.
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

// static 팩토리 함수
DnSkillHandle CDnSkill::CreateSkill( DnActorHandle hActor, int iSkillTableID, int iLevel )
{
	if( false == CDnSkillTask::IsActive() )
		return DnSkillHandle();

#if defined(_DEBUG)
	if( false == ValidateSkillData( iSkillTableID, iLevel ) )
		return DnSkillHandle();
#endif

	// 레벨 0으로 생성 요청 들어오면 언락만 된 미획득 스킬임.
	bool bAcquired = (iLevel != 0);
	if( false == bAcquired )
		iLevel = 1;		// 레벨 0으로는 생성할 수 없으므로 1로 셋팅

	CDnSkill* pNewSkill = NULL;

	// 스킬 발동 조건, 발동 프로세서들을 달아준다.
	// 각각 5개씩 있고 파라메터는 전부 합쳐 10개임. 변경될 가능성도 있다.
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
	// 액션 프로세서를 뒤져서 사용하는 액션.. 걍 깔끔하게 인자 받는게 나을듯. ㅠㅠ
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
					// 시그널을 뒤져서 프로젝타일이 있는지 검사
					int iNumSignals = (int)pElement->pVecSignalList.size();
					for( int iSignal = 0; iSignal < iNumSignals; ++iSignal )
					{
						CEtActionSignal* pSignal = pElement->pVecSignalList.at(iSignal);
						if( STE_Projectile == (SignalTypeEnum)pSignal->GetSignalIndex() )
						{
							// 자신의 무기 사거리를 더해서 범위 설정
							ProjectileStruct* pSignalData = static_cast<ProjectileStruct*>(pSignal->GetData());

							// 무기 테이블에서 사거리 정보 가져 옴	
							DNTableFileFormat*  pWeaponTable = GetDNTable( CDnTableDB::TWEAPON );
							int iLength = pWeaponTable->GetFieldFromLablePtr( pSignalData->nWeaponTableID, "_Length" )->GetInteger();

							if( pSignalData->bIncludeMainWeaponLength )
							{
								DnWeaponHandle hWeapon = m_hActor->GetWeapon( 0 );
								if( hWeapon )
									iLength += hWeapon->GetWeaponLength();
							}

							// 스킬 사거리까지 더해줌
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
	//스킬 레벨 테이블 데이타가 있는 지 확인..
	{
		vector<int> vlSkillLevelList;
		if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) == 0 ) 
			return false;

		if ( vlSkillLevelList.empty() )
			return false;
	}

	SKILL_LEVEL_INFO* pTableInfo = GetSkillTask().GetSkillLevelTableIDList(iSkillTableID, iSkillLevelDataApplyType);

	// 해당 대상의 데이터가 없는 경우엔 그냥 아무것도 안하고 리턴하면 된다.
	if (pTableInfo->_SkillLevelTableIDs.empty())
		return true;

	int iSkillLevelTableID = -1;
	//NextLevel이 연속으로 있지 않을 수 있다.. 그래서 여기서 다음 레벨 데이타 테이블 ID를 찾아 놓는다.
	int iMinSkillLevelTableID = -1;
	int iNextSkillLevelTableID = -1;

	//현재 레벨의 테이블 ID를 검색.
	SKILL_LEVEL_TABLE_IDS::iterator findIter = pTableInfo->_SkillLevelTableIDs.find(iLevel);
	if (findIter != pTableInfo->_SkillLevelTableIDs.end())
		iSkillLevelTableID = findIter->second;

	//다음 레벨
	findIter = pTableInfo->_SkillLevelTableIDs.find(iLevel+1);
	if (findIter != pTableInfo->_SkillLevelTableIDs.end())
		iNextSkillLevelTableID = findIter->second;

	//최소 레벨
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

	// pve, pvp 대상인지 확인하여 걸러냄.
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

	// 해당 대상의 데이터가 없는 경우엔 그냥 아무것도 안하고 리턴하면 된다.
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

	// 스킬 발동 조건, 발동 프로세서들을 달아준다.
	// 각각 5개씩 있고 파라메터는 전부 합쳐 10개임. 변경될 가능성도 있다.
	char caLabel[ 32 ];
	int iCheckerParamOffset = 0;
	int iProcessorParamOffset = 0;
	for( int i = 0; i < MAX_PROCESSOR_COUNT; ++i )
	{
		// 발동조건 객체 이름을 찾는다. 파라메터 필드가 비어있으면 생성 함수들에서 NULL 리턴됨
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
				OutputDebug( "[SkillLevelTable Error!] %d 의 파라메터 개수가 잘못되었습니다.\n", iSkillLevelTableID );
				_ASSERT( !"스킬 레벨 테이블 파라메터 잘못됨. OutputDebug 출력 확인!" );
			}

			this->AddUsableCheckers( pUsableChecker, iSkillLevelDataApplyType );
		}


		// 클라에서는 사용하지 않음. 지역변수로 그냥 넣어준다.
		set<string> setUseActionName;
		iOffsetCheck = iProcessorParamOffset;
		IDnSkillProcessor* pSkillProcessor = IDnSkillProcessor::Create( m_hActor, iProcessor, iSkillLevelTableID, &iProcessorParamOffset, setUseActionName );

		if( NULL != pSkillProcessor )
		{
			if( (iProcessorParamOffset - iOffsetCheck) != pSkillProcessor->GetNumArgument() )
			{
				OutputDebug( "[SkillLevelTable Error!] %d 의 파라메터 개수가 잘못되었습니다.\n", iSkillLevelTableID );
				_ASSERT( !"스킬 레벨 테이블 파라메터 잘못됨. OutputDebug 출력 확인!" );
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

	// 스킬 테이블의 최대 레벨은 신뢰할 수 없다. -_-
	// 실제 갯수로 업데이트. 이전에 초기화 되어 이미 값이 있다면 값이 다른 경우 assert!!
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

	// 만약 글로벌 스킬 그룹이 설정되어있다면 스킬 쿨타임을 글로벌 쿨타임으로 대체 시켜준다.
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

	// 스킬 설명의 기본 틀은 안바뀌고 인자값만 바뀔 것이다. 따라서 인자값만 모드별로 나눈다.
	m_iSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationID" )->GetInteger();

	char *szParam = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationIDParam" )->GetString();
	MakeUIStringUseVariableParam( m_strSkillExplainString[ iSkillLevelDataApplyType ], m_iSkillExplainStringID, szParam );
	

	// 스킬 레벨 테이블에서 같은 스킬은 레벨 데이터가 연속해서 있다는 가정 하에.
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
			OutputDebug("스킬레벨 테이블에서 같은 스킬은 레벨데이터가 연속이 아니네???\n");
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
			OutputDebug("스킬레벨 테이블에서 같은 스킬은 레벨데이터가 연속이 아니네???\n");
			m_iNextSkillExplainStringID = -1;
		}
#endif // PRE_FIX_NEXTSKILLINFO
	}
	else
		m_iNextSkillExplainStringID = -1;

	// 상태 효과 정보 로딩
	StateEffectStruct StateEffect;
	for( int i = 0; i < MAX_STATE_EFFECT_COUNT; ++i )
	{
		sprintf_s( caLable, "_EffectClass%d", i + 1 );
		StateEffect.nID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();

		if( StateEffect.nID < 1 ) 
			continue;

		sprintf_s( caLable, "_EffectClass%dApplyType", i + 1 );
		int iApplyType = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();
		bool bApplyAll = (StateEffectApplyType::ApplyAll == iApplyType);		// 모두 적용임. 타겟만 다르게 해서 똑같은 상태효과 2개를 추가해준다.

		if( bApplyAll )
			StateEffect.ApplyType = StateEffectApplyType::ApplySelf;
		else
			StateEffect.ApplyType = (StateEffectApplyType)iApplyType;

		sprintf_s( caLable, "_EffectClassValue%d", i + 1 );
		StateEffect.szValue = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetString();

		sprintf_s( caLable, "_EffectClassValue%dDuration", i + 1 );
		StateEffect.nDurationTime = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetInteger();

		m_vlStateEffectList[ iSkillLevelDataApplyType ].push_back( StateEffect );

		// 모두 적용이면 타겟으로 바꿔서 똑같이 한 번 더 넣어줌.
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

	// 각 모드별로 데이터 로드. 먼저 PVP 를 읽는다.
	// PVE 는 디폴트 값이기 때문에 모드별로 나뉘어지는 값이 아니면 디폴트 값으로 채워지게 된다.

	// 몬스터인 경우엔 pvp 데이터가 없으므로 함수 안에서 아무것도 안되고 리턴된다.
	// 만약 플레이어인데 아무것도 없다면 잘못된 거임.
	if( false == _LoadSkillLevelData( iSkillTableID, iLevel, PVP ) )
		return false;

	if( false == _LoadSkillLevelData( iSkillTableID, iLevel, PVE ) )
		return false;

	// 초기화 이후에 따로 모아놓을 정보들.
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
	
	// 아이콘 이미지 셋
	SetIconImageIndex( pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_IconImageIndex" )->GetInteger() );

	// 초기화했을 땐 PVE 기준으로 초기화 해준다. 디폴트 값.
	m_iSelectedSkillLevelDataApplyType = PVE;
	m_bEnableTooltip = ( pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillTip" )->GetInteger() ) ? true : false;

	if (m_eDurationType == SummonOnOff)
		OnInitializeSummonMonsterInfo();

	// 클라 메모리 최적화 관련 작업. 마을인 경우 마을에서 쓸 수 있는 스킬만 허용한다.
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
			if( CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 0 ] ) == CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 1 ] ) )	// 1. 둘다 주무기이거나 보조무기 인 경우 or
			{
				int nWeapon = CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 0 ] ) ? 1 : 0;
				if( m_hActor->GetWeapon( nWeapon, false ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( nWeapon, false )->GetEquipType()
					|| m_hActor->GetWeapon( nWeapon, false ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( nWeapon, false )->GetEquipType() )
					bSatisfy = true;
			}
			else	// 2. 주무기, 보조무기인 경우 and
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
						// bActionMatchWeapon 를 디폴트로 true 로 호출하니까 액션에 따라 무기가 다르게 나오는 듯 하여 항상 차고 있는 메인 무기 위주로 체크토록..
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

		//마을에서 스탠스 변경 상태효과 스킬 사용 못함..
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

	//아이템 스킬이면 아래 허용 스킬 체크를 하지 않도록한다..
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

	// 대포 모드일 땐 플레이어는 스킬 사용 못한다.
	if( m_hActor->IsPlayerActor() )
	{
		if( static_cast<CDnPlayerActor*>(m_hActor.GetPointer())->IsCannonMode() )
			return UsingResult::Failed;
	}

	// 잔여 SP 체크, 체력 체크, 레벨 체크 등등
	// 오라나 토글은 현재 활성화 중이라면 MP 상관 없이 끌 수 있다.
	if( !IsToggleOn() && !IsAuraOn() )
		if( m_hActor->GetSP() < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;

	// 워리어의 릴리브같은 스킬은 수면, 스턴 중일때도 사용가능해야 한다.
	// 테이블에 공격불가 무시 발현타입을 추가할까하다가 현재 릴리브에서만 의미있는 것이므로
	// 다른 곳에서 어떤 식으로 쓰이게 될지 좀 더 지켜보고 규격화 시키도록 한다.
	// 우선 Dissolve 상태효과 있는 스킬은 행동 불가 체크를 건너뛴다.
	// 스킬에 해당하는 액션의 State 시그널에 반드시 IgnorectCantAction 이 켜져 있어야 스킬 액션이 나간다.
	bool bPassCheckCantAction = false;
	for( DWORD i = 0; i < GetStateEffectCount(); ++i )
	{
		//#40480 결빙상태에서 단축슬롯 활성화를 위해서..
		CDnSkill::StateEffectStruct* pSE = GetStateEffectFromIndex( i );

		if( STATE_BLOW::BLOW_069 == pSE->nID )
		{
			// 76643 ( Dissolvable 이 2면 그냥 비활성화 해달라고 합니다 )
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

	// 스킬 사용불가 상태효과가 있으면 mp 소모하는 스킬은 사용할 수 없다.
	if( 0 < m_hActor->GetCantUseSkillSEReferenceCount() )
		if( 0 < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;

	// 67번 스킬 제한 상태효과가 있으면 액티브 스킬은 사용할 수 없음. 
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
	//스킬 레벨업 아이템에 의해 스킬레벨업이 된 스킬은 캐릭터 레벨 체크 하지 않도록 한다.
	isCharacterLevelCheck = (GetLevelUpValue() == 0);

	if (isCharacterLevelCheck)
	{
		if( m_hActor->GetLevel() < m_iLevelLimit )
			return UsingResult::Failed;
	}

	// 필요 무기가 있다면 장착했는지 확인. 최대 2개임. 둘 중 하나만 충족되도 스킬 사용 가능.
	if( false == IsSatisfyWeapon() )
		return UsingResult::Failed;

	// 화살 같은 소모성 아이템의 갯수 확인
	bool bIsCheckNeedItem = true;
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP && static_cast<CDnPvPGameTask*>(pGameTask)->IsLadderMode() && bIsExtremitySkill() )	// 래더에서 궁극기 스킬
		bIsCheckNeedItem = false;

	if( bIsCheckNeedItem && m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ] > 0 )
	{
		int iNumNeedItem = GetItemTask().GetCharInventory().ScanItemFromID( m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ], NULL );
		if( iNumNeedItem < m_iNeedItemDecreaseCount[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;
	}
	// 직업 체크. 
	// 현재 마을에서는 시야에 들어온 유저의 직업 풀 히스토리가 있지 않고 최근직업만 셋팅되므로 
	// 마을에서 만난 다른 유저들의 직업까지는 체크하지 않습니다.
	// 던전에서는 파티원의 풀 히스토리를 받아오기 때문에 모두 체크함.
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

		// 죽었으면 불가
		if( m_hActor->IsDie() ) 
			return UsingResult::Failed;
		// 배틀 모드가 아니면 스킬 발동 불가!
		if( !pActor->IsBattleMode() )
			return UsingResult::Failed;
	}

	// 쿨타임이 끝나지 않았다면 스킬 발동 불가
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
	{ // m_fAnotherGlobalSkillCoolTime이 적용 되어 있으면 적용 안 하도록..
		
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
	// 시작할때 LeftDelayTime이 0.0이 되면 m_fCoolTime을 0.0f로 변경해야한다.
	// 그렇지 않으면 쿨타임이 1.0으로 계속 유지됨.(Process함수에서 m_fLeftDelayTime이 0이면 CoolTime갱신안됨.
	if (m_fLeftDelayTime == 0.0f)
	{
		m_fCoolTime = 0.0f;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
		m_fDeltaGlobalCoolTime = 0.0f;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
	}

	// 쿨타임 무시 치트키가 있다면 쿨타임 무시토록 조치.
#ifndef _FINAL_BUILD
	if( CGlobalValue::GetInstance().m_bIgnoreCoolTime ) {
		m_fCoolTime = 0.f;
		m_fLeftDelayTime = 0.f;
	}
#endif
}

void CDnSkill::_OnBeginProcessException( void )
{
	// 결빙상태에서 워리어 릴리브 스킬을 쓰는 경우 바로 프레임 고정을 풀어준다. #12438
	// 결빙상태효과가 걸린 상태에서 디버프 상태효과 해제 상태효과가 self 로 적용되는 스킬을 
	// 사용하는 경우로 일반화 시킴.
	if( m_hActor && 
		(
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_041 ) ||
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_146 ) ||	//#58432
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_218 ) //#53900 Escape스킬 추가
		))
	{
		int iNumStateBlow = (int)m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size();
		for( int i = 0; i < iNumStateBlow; ++i )
		{
			const StateEffectStruct& SE = m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].at( i );
			if( STATE_BLOW::BLOW_069 == SE.nID &&
				StateEffectApplyType::ApplySelf == SE.ApplyType )
			{
				// 프레임 정지 상태효과 참조 인덱스를 모두 풀어준다. 프레임이 돌아야 액션이 돌아가고
				// 액션이 돌아가야 릴리브 상태효과가 들어간다.
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
	// 같은 글로벌 ID 를 사용하는 다른 글로벌 스킬을 사용하여 셋팅된 쿨타임 값을 실제로 이 스킬 사용될 때는 제거한다.
	m_fAnotherGlobalSkillCoolTime = 0.0f;

	m_nAnotherGlobakSkillID = 0;

#if defined( PRE_ADD_ACADEMIC ) // 소환형 몬스터 액티브 토글 스킬은 쿨타임이 돌필요가 없습니다.
	if(m_eDurationType != DurationTypeEnum::ActiveToggleForSummon)
	{
		//--------------------------------------------------
		//[debug_skill]
		//세콘늴鑒�阮�
		//祺꼍鸞=6001   槿近랐땄=6003  莖슉黛듐=6204   섐똑댔샌=6208

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
                SetSkillNowNum( nNowNum ); //賈痰늴鑒헌쥐
				OnBeginCoolTime();
			} 
			else
			{
				nNowNum++;
                SetSkillNowNum( nNowNum ); //賈痰늴鑒藤속1
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

	// #31909 129번 액션 셋 상태효과를 쓰는 경우 (세이크리드 해머링) 기본 공격 액션을 변경해서 구현토록 되어있는데
	// 무기를 들고 있으면 무기 액션이 나가므로 CDnPlayerActor::SetActionQueue, SetAction 등에서 
	// 무기 변경 액션을 하고 있는 경우 기본 액션으로 다시 바꿔줘서 흘려보내는데 현재 무기 액션으로 
	// 변경되었다는 플래그가 OnChangeAction 에서 false 로 리셋되므로 해머링 4타 때린 후에 
	// 스킬 퀵슬롯과 동시에 눌렀을 경우 플래그가 켜져 있는 상태기 때문에 스킬 쪽에서 셋팅한 스킬 액션을 
	// 실행시킬 때 CDnPlayer::SetActionQueue 에서 스킬 액션이 씹히게 된다. 따라서 스킬 쓸 때 
	// 해당 무기 변경 플래그가 켜져 있는 경우 여기서 다시 꺼주고 스킬을 나가게 해준다. 
	// (현재 문제점은 서버는 정상적으로 스킬 액션을 하지만 클라에서 해머링 액션이 나오는 것임)
	if( m_hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( pPlayerActor->IsChangedEquipAction() )
		{
			pPlayerActor->ClearChangedEuipAction();
		}
	}

	// 스킬 갖고 있는 액터의 팀이 셋팅되어있는가.
	// 몬스터인 경우 액터 셋팅이 안되어있는 경우가 있으므로 여기서 해줌.
	if( !m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].hSkillUser )
		m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].hSkillUser = m_hActor;

	// pvp 같은 거 하면 팀셋팅이 바뀔 수 있으므로 항상 스킬 사용시에 팀셋팅 제대로 한다.
	m_SkillInfo[ m_iSelectedSkillLevelDataApplyType ].iSkillUserTeam = m_hActor->GetTeam();

	m_LastTimeToggleMPDecreaseTime = LocalTime;
	m_bFinished = false;

//	// [2010/12/15 semozz]
//	// 마을에서는 서버 패킷을 받을 수 없어서
//	// 기존 클라이언트가 자신의 상태효과 처리하는 부분 그대로 동작하도록 한다.
	CDnWorld* pWorld = CDnWorld::GetInstancePtr();
	bool bVillage = (CDnWorld::MapTypeEnum::MapTypeVillage == pWorld->GetMapType());

	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->OnBegin( LocalTime, fDelta, GetMySmartPtr() );
	}

	_OnBeginProcessException();

	//마을에서는 
	if (m_eDurationType == SummonOnOff && !bVillage)
	{
		//몬스터 소환 되면 Toggle On..시킴..
		EnableToggle(true);
	}
}

// 스킬에 달려있는 Processor 들 중에 선처리 해야하는 것들 먼저 처리.
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
			// 액션을 바꿔준다.
			CDnPlayAniProcess* pPlayAniProcessor = static_cast<CDnPlayAniProcess*>(GetProcessor( IDnSkillProcessor::PLAY_ANI ));
			pPlayAniProcessor->ChangeActionNameOnce( pChangedActionName );
		}
	}
}

void CDnSkill::CheckProcessorOnEnd( void )
{

}



// 서버측에서는 상태효과 패킷을 날려주고 아이템에 달려있는 액션을 실행시킴.
// 클라에서는 액션만 실행시킨다.
void CDnSkill::OnBeginForceSync( LOCAL_TIME LocalTime, float fDelta )
{
	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->OnBegin( LocalTime, fDelta, GetMySmartPtr() );
	}
}

// 실제로 스킬이 발동 되었을 시에 처리
void CDnSkill::ProcessExecute( LOCAL_TIME LocalTime, float fDelta )
{
	// 딸려 있는 프로세서 처리
	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->Process( LocalTime, fDelta );
	}

	// -1 인 경우는 대시 처럼 방향키를 누르고 있는동안 계속 발동되는 스킬.
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
			// #27257, #27272관련 수정 [2011/01/07 semozz]
			// 게임 서버에서는 여기 등록 되는 상태 효과 없음(게임 서버상에서는 모든 상태효과 패킷으로 동기화됨)
			// 캐릭터에서 패킷 없이 추가된 상태효과를 스킬이 끝나는 시점에서 제거하는데..
			// 이 상태효과들은 바로 제거 되어야 함..
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

// 쿨 타임 등등의 처리
void CDnSkill::Process( LOCAL_TIME LocalTime, float fDelta )
{
	// 쿨타임 조절 기본값은 1.0 임.
	// 현재 m_fCoolTimeAdjustBlowValue 값은 사용하지 않는다. 1.0 기본값으로 그대로 있고 현재 쿨타임 감소 상태효과 구현이 바뀌었다. (#21146)
	// 길드전 스킬은 쿨타임 감소하지 않음
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
						// 윈드워커의 스킬중 쇼타임에 의해 fTempAdjustValue가 0이되는 경우가 발생..
						// 예외 처리 추가
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
		case DurationTypeEnum::ActiveToggleForSummon: // 몬스터가 쓰는 스킬이고 엑셀같은거는 영향안받는다 그래서 코드가 좀틀리다. 서버에는 돌지않음.
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

	// 쿨이 종료된 상태라면 셋팅된 글로벌 스킬 쿨타임을 다시 없애준다.
	if( 0.0f < m_fAnotherGlobalSkillCoolTime && 0.0f == m_fCoolTime )
	{
		m_fAnotherGlobalSkillCoolTime = 0.0f;

		m_nAnotherGlobakSkillID = 0;
	}
}

bool CDnSkill::IsFinished( void )
{
	bool bResult = false;

	// 오라 스킬이라면 맵 이동 하기 전엔 끝내지 않음.
	if( Aura == m_eDurationType )
		return IsFinishedAuraSkill();

	if( m_bFinished )
		return m_bFinished;

	// -1 이면 대쉬 같은 키 뗄 때까지 쓰는 무한 스킬.
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

		// 플레이어가 획득하지 않은 스킬은 사용 불가.
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

		//쿨타임이 돌고 있는 중
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
				// 토글 스킬이고 토글 된 상태라면 
				// ItemConditionEnum::Toggle 리턴
				if( m_bNowToggled )
				{
					return ItemConditionEnum::Toggle;
				}
			}
			break;

		case Aura:
			{
				// 오라 스킬이고 오라가 켜진 상태라면 
				// ItemConditionEnum::Toggle 리턴
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
			_ASSERT( "최대 레벨 초과하여 CDnSkill::LevelUp() 호출" );
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
		//실제 레벨에서의 증가/감소의 스킬 포인트값을 얻어 와야한다.
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
			_ASSERT( "최대 레벨 초과하여 CDnSkill::LevelUp() 호출" );
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

		//실제 레벨에서의 증가/감소의 스킬 포인트값을 얻어 와야한다.
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
		if( !m_hActor || m_hActor->IsMonsterActor() )		// 게임서버와 동일하게 맞춤.
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
		// 같은 것이 오면 자동으로 추가 되지 않음.
		m_setUseActionNames.insert( string(pActionName) );
	}
}

// 액션 재생 프로세서가 반드시 있는 액티브 스킬의 동작 계승 요청!
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

	// 스킬 강화 문장 같은 경우 pvp/pve 모두 셋팅해준다.
	for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
	{
		// 원래 상태효과 갖고 있던 갯수
		int iNumOriginalStateEffect = (int)m_vlStateEffectList[ iSkillLevelApplyType ].size();

		// 상태 효과 추가
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
			bool bApplyAll = (StateEffectApplyType::ApplyAll == iApplyType);		// 모두 적용임. 타겟만 다르게 해서 똑같은 상태효과 2개를 추가해준다.

			if( bApplyAll )
				StateEffect.ApplyType = StateEffectApplyType::ApplySelf;
			else
				StateEffect.ApplyType = (StateEffectApplyType)iApplyType;

			sprintf_s( caLable, "_EffectClassValue%d", i + 1 );
			StateEffect.szValue = pGlyphTable->GetFieldFromLablePtr( nGlyphID, caLable )->GetString();

			// 지속시간은 기존에 있던 상태효과에서 그대로 대체한다.
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

			// 만약 같은 상태효과가 없다면 지속시간은 그냥 0 으로 해준다. 
			// 그럼 액션과 동일한 간격만큼 상태효과가 지속됨.
			if( false == bFound )
				StateEffect.nDurationTime = 0;

			m_vlStateEffectList[ iSkillLevelApplyType ].push_back( StateEffect ); 

			// 모두 적용이면 타겟으로 바꿔서 똑같이 한 번 더 넣어줌.
			if( bApplyAll )
			{
				StateEffect.ApplyType = StateEffectApplyType::ApplyTarget;
				m_vlStateEffectList[ iSkillLevelApplyType ].push_back( StateEffect );
			}
		}
	}

	//스킬 속성 값 추가
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
				// pair<int, int> <상태효과, 기간>
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

	// 스킬 강화 문장 같은 경우 pvp/pve 모두 셋팅해준다.
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
					// pair<int, int> <상태효과, 기간>
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

// 게임서버도 있으니 동기화 맞추어주세요 CheckAndDivideStateEffectArgument 이 함수도 수정해주세요 
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
			// 기존에 존재하는 상태효과엔 인자값을 더해주고 새로운 상태효과는 리스트에 추가.
			// 쿨타임은 강화 패시브 스킬의 쿨타임으로 덮어씌워준다.
			SelectLevelDataType( iSkillLevelApplyType );
			hEnchantPassiveSkill->SelectLevelDataType( iSkillLevelApplyType );
			int iNumStateEffect = (int)hEnchantPassiveSkill->GetStateEffectCount();
			for( int i = 0; i < iNumStateEffect; ++i )
			{
				StateEffectStruct* pEnchantPassiveSE = hEnchantPassiveSkill->GetStateEffectFromIndex( i );
				
				// #40643 핑퐁밤EX 처럼 특수하게 구현된 상태효과의 인자를 건드리는 경우..
				// 어쩔 수 없이 합쳐줘야 한다. 실제로 m_vlStateEffectList 에 추가되는 것이 아님.
				if( false == CheckAndUnifyStateEffectArgument( iSkillLevelApplyType, pEnchantPassiveSE ) )
				{
					// 공격력 강화 상태효과던 아니건 추가해주면 스킬 사용시 자동으로 능력치 리프레시 된다.
					m_vlStateEffectList[ iSkillLevelApplyType ].push_back( *pEnchantPassiveSE );
					m_vlStateEffectList[ iSkillLevelApplyType ].back().nFromEnchantPassiveSkillID = hEnchantPassiveSkill->GetClassID();
				}
			}

			// 액션 프로세서를 강화 스킬의 것으로 변경한다.
			IDnSkillProcessor* pMyPlayAniProcessor = GetProcessor( IDnSkillProcessor::PLAY_ANI );
			if( pMyPlayAniProcessor )
			{
				IDnSkillProcessor* pEnchantPassiveSkillPlayAniProcessor =  hEnchantPassiveSkill->GetProcessor( IDnSkillProcessor::PLAY_ANI );
				if( pEnchantPassiveSkillPlayAniProcessor )
				{
					// 기존 것 백업.
					IDnSkillProcessor* pBackup = new CDnPlayAniProcess;
					pBackup->CopyFrom( pMyPlayAniProcessor );
					m_vlpProcessorBackup[ iSkillLevelApplyType ].push_back( pBackup );

					// 강화 패시브 스킬의 것으로 데이터 적용.
					pMyPlayAniProcessor->CopyFrom( pEnchantPassiveSkillPlayAniProcessor );
				}
			}

			IDnSkillProcessor* pMyPartialPlayAniProcessor = GetProcessor( IDnSkillProcessor::PARTIAL_PLAY_ANI );
			if( pMyPartialPlayAniProcessor )
			{
				IDnSkillProcessor* pEnchantPassivePartialSkillPlayAniProcessor =  hEnchantPassiveSkill->GetProcessor( IDnSkillProcessor::PARTIAL_PLAY_ANI );
				if( pEnchantPassivePartialSkillPlayAniProcessor )
				{
					// 기존 것 백업.
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
					// 기존 것 백업.
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
			//UsableChecker를 베이스 스킬과 EX스킬 교체한다...
			vector<IDnSkillUsableChecker*>& enchangeSkillUsableChecker = hEnchantPassiveSkill->GetUsableChecker(iSkillLevelApplyType);

			//EX스킬에 UsableChecker가 설정 되어 있는 경우만 교체 하도록 한다...
			if (enchangeSkillUsableChecker.size() > 0)
			{
				vector<IDnSkillUsableChecker*>& baseSkillUsableChecker = GetUsableChecker(iSkillLevelApplyType);

				{
					//자신의 Checker들을 백업 리스트에 담아 놓는다.
					vector<IDnSkillUsableChecker*>::iterator iter = baseSkillUsableChecker.begin();
					vector<IDnSkillUsableChecker*>::iterator endIter = baseSkillUsableChecker.begin();

					//백업 리스트에 저장 해놓고...
					m_vlUsableCheckersBackup[iSkillLevelApplyType].clear();

					for (; iter != endIter; ++iter)
					{
						m_vlUsableCheckersBackup[iSkillLevelApplyType].push_back((*iter));
					}

				}				
				
				//EX스킬의 usableChecker를 베이스 스킬에 적용한다.
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

			//EffectIDs도 변경..
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
			// 기존에 존재하는 상태효과엔 인자값을 더해주고 새로운 상태효과는 리스트에 추가.
			// 쿨타임은 강화 패시브 스킬의 쿨타임으로 덮어씌워준다.
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

			//EffectIDs도 변경..
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
	// 스킬 쿨타임이 스킬 애니메이션길이 보다 짧을 경우 CanExecute에서 걸러 지지만
	// SetActionQueue에서 동작 변경이 되질 않는다. 그래서 스킬 사용 가능 체크에서
	// 현재 스킬 동작이 끝나지 않은 시점에서 똑같은 스킬을 사용 못 하도록 체크하도록한다.
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
	//오라 스킬일때 지속 절대/비율 마나 증가가 있을때 총 마나 변화량을 합산해서 지금 남은 마나와 비교해서
	//스킬 지속 가능 여부를 판단한다.
	if (m_hActor)
	{
		float fIncManaDelta = 0.0f;

		//절대 증가 분
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

		//비율 증가 분
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

		//#46381 엘리멘탈로드 - mp 부족으로 인하여 쉴드가 계속 유지됨
#if defined(PRE_FIX_46381)
		//231번 상태효과 적용.
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

		//최종 -증가분이 현재 남은 마나보다 많으면 마나 모자름..
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
		//서버로 소환몬스터 해제 요청을 보내고, 서버에서 해당 몬스터 객체를 찾아서 CmdSuicide호출
		m_hActor->RequestSummonOff(hSummonMonster->GetUniqueID());
		//소환 몬스터 소환 해제시 토글 해제..
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

	//스킬 Checker중에 소환몬스터 체크가 있으면 소환몬스터에 사용 하는 스킬???(기획의견..)
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
	//글로벌 쿨타임이 설정 되어 있고, 쿨타임이 이미 시작 되었을 경우
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
	//글로벌 쿨타임이 설정 되어 있고, 쿨타임이 이미 시작 되었을 경우
	if (m_iGlobalSkillGroupID > 0 )
	{
		//글로벌 쿨타임 변경 수치가 설정 된 경우
		if (m_fDeltaGlobalCoolTime > 0.0)
		{
			float fOrigDelayTime = GetOrigDelayTime();

			//적용된 수치가 지금 리셋 하려는 수치보다 작은 값인 경우
			if (m_fDeltaGlobalCoolTime < fRate)
				fRate = m_fDeltaGlobalCoolTime;

			//적용된 수치는 감소 시켜준다..
			m_fDeltaGlobalCoolTime -= fRate;
			if (m_fDeltaGlobalCoolTime < 0.0f)
				m_fDeltaGlobalCoolTime = 0.0f;

			//현재 쿨타임이 남아 있는 경우
			if (m_fLeftDelayTime > 0)
				m_fLeftDelayTime += fOrigDelayTime * fRate;		
		}
	}
}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#if defined(PRE_ADD_65808)
bool CDnSkill::IsSummonMonsterRecall(int& monsterID)
{
	//PlayAniProcess를 이용하고, 해당 동작에 SummonMonster시그널이 존재 하면 true를 리턴..
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