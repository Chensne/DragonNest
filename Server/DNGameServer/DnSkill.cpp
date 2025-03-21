#include "StdAfx.h"
#include "DnSkill.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "IDnSkillUsableChecker.h"
#include "IDnSkillProcessor.h"
#include "DnDivideSEArgumentByTargets.h"
#include "DnPlayerActor.h"
#include "DnBlow.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DNUserSession.h"
#include "DnPlayAniProcess.h"
#include "DnPartialPlayProcessor.h"
#include "DnApplySEWhenActionSetBlowEnabledProcessor.h"
#include "DnChangeActionStrProcessor.h"
#include "DnChangeActionStrByBubbleProcessor.h"
#include "DnStateEffectApplyOnOffByBubbleProcessor.h"
#include "DnStateBlow.h"
#include "DnBubbleSystem.h"
#include "DnObserverEventMessage.h"
#include "DnPingpongBlow.h"
#include "DnBasicBlow.h"
#include "DnHealingBlow.h"
#include "DnHPIncBlow.h"
#include "DnInvincibleAtBlow.h"
#include "DnProbabilityChecker.h"
#include "DnCreateBlow.h"
#include "DnAllowedSkillsBlow.h"
#include "DnMonsterActor.h"
#include "DnProjectile.h"
#include "DnAdditionalStateInfoBlow.h"
#include "DnAddStateBySkillGroupBlow.h"
#include "DnTransformBlow.h"
#include "DnBloodSuckingBlow.h"
#include "DnOrderMySummonedMonsterBlow.h"

#if defined(PRE_FIX_NEXTSKILLINFO)
#include "DNGameDataManager.h"
#endif // PRE_FIX_NEXTSKILLINFO

#if defined(PRE_FIX_46381)
#include "DnContinueBaseMPIncBlow.h"
#endif // PRE_FIX_46381

using namespace BubbleSystem;
using namespace boost;

DECL_MULTISMART_PTR_STATIC( CDnSkill, MAX_SESSION_COUNT, 100 )


CDnSkill::CDnSkill( DnActorHandle hActor ) : CMultiSmartPtrBase< CDnSkill, MAX_SESSION_COUNT >(hActor->GetRoom()),
											 m_hActor( hActor ),
											 m_LastTimeToggleMPDecreaseTime( 0 ),
											 m_bToggle( false ),
											 m_bAura( false ),
											 m_bItemSkill( false ),
											 m_fPassiveActionSkillLength( 0.0f ),
											 m_eElement( CDnState::ElementEnum_Amount ),
											 m_iNextLevelSkillPoint( -1 ),
											 m_bChainingPassiveSkill( false ),
											 m_bAppliedPassiveSelfBlows( false ),
											 m_bTempSkill( false )
{
	SecureZeroMemory( m_iNeedItemID, sizeof(m_iNeedItemID) );
	SecureZeroMemory( m_iNeedItemDecreaseCount, sizeof(m_iNeedItemDecreaseCount) );
	SecureZeroMemory( m_fHPConsumeType, sizeof(m_fHPConsumeType) );
	SecureZeroMemory( m_fMPConsumeType, sizeof(m_fMPConsumeType) );
	SecureZeroMemory( m_iNeedHP, sizeof(m_iNeedHP) );
	SecureZeroMemory( m_iNeedMP, sizeof(m_iNeedMP) );
	SecureZeroMemory( m_iIncreaseRange, sizeof(m_iIncreaseRange) );
	SecureZeroMemory( m_iDecreaseHP, sizeof(m_iDecreaseHP) );
	SecureZeroMemory( m_iDecreaseMP, sizeof(m_iDecreaseMP) );
	SecureZeroMemory( m_fOriginalDelayTime, sizeof(m_fOriginalDelayTime) );
	SecureZeroMemory( m_fDelayTime, sizeof(m_fDelayTime) );

	m_iSkillID = 0;
	m_iSkillLevelID = 0;
	m_iDissolvable = 0;
	m_iDuplicateCount = 0;
	m_iSkillDuplicateMethod = 0;
	m_iEffectDuplicateMethod = 0;

	m_iLevel = 0;
	m_iMaxLevel = 0;

	m_eSkillType = SkillTypeEnum::Active;
	m_eDurationType = DurationTypeEnum::Instantly;
	m_eTargetType = TargetTypeEnum::Self;
	m_iLevelLimit = 0;

	m_fLeftDelayTime = 0.f;
	m_fCoolTime = 0.f;
	m_iAdditionalThreat = 0;

	m_iCPScore = 0;

	m_pEffectAction = NULL;
	
	m_fOnceDelayTime = 0.0f;
	m_fOnceElapsedDelayTime = 0.0f;
	m_bEquipItemSkill = false;
	m_iEquipIndex = -1;

	m_fCoolTimeAdjustBlowValue = 1.0f;

	m_fStartSuperArmor = 0.0f;
	m_bStartCanHit = true;

	m_fResetCooltime = 0.0f;

	m_dwLastUseSkillTimeStamp = 0;

	m_iExclusiveID = 0;

	m_iSelectedSkillLevelDataApplyType = PVE;		// 디폴트는 pve 이다.

	m_iBaseSkillID = 0;
	m_iAppliedEnchantPassiveSkillID = 0;

	m_nPriority = 0;

	m_nItemID = -1;

	m_fCoolTimeMultipier = 1.0f;

	m_isIgnoreImmuneBackup = false;

	m_nLevelUpValue = 0;

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
	m_nPrefixSkillType = -1;
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

	m_iGlobalSkillGroupID = 0;
	SecureZeroMemory( m_afGlobalCoolTime, sizeof(m_afGlobalCoolTime) );
	m_fAnotherGlobalSkillCoolTime = 0.0f;

	m_nAnotherGlobakSkillID = 0;

#if defined( PRE_ADD_ACADEMIC )
	m_iSummonerDecreaseSP = 0;
	m_iSummonerDecreaseSPSkillID = 0;
#endif // #if defined( PRE_ADD_ACADEMIC )

	m_SummonMonsterID = -1;

	m_bAddStateEffectQueue = false;

	m_bEnchantedFromBubble = false;

	m_SkillStartTime = 0;
	m_bFinished = false;
	m_bIsPrefixTriggerSkill = false;
	m_iNeedJobClassID = 0;
	m_iNowLevelSkillPoint = 0;

	memset (m_iOriginalNeedMP, 0x00, sizeof(m_iOriginalNeedMP));
	memset (m_aeNeedEquipType, 0x00, sizeof(m_aeNeedEquipType));

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
	_ASSERT( hActor && "CDnSkill::SetHasActor() 액터 핸들이 NULL 임" );

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
			pProcessor->SetParentSkill( GetMySmartPtr() );
		}
	}
}


DnSkillHandle CDnSkill::CreateSkill( DnActorHandle hActor, int iSkillTableID, int iLevel )
{
	CDnSkill* pNewSkill = NULL;

	// 스킬 발동 조건, 발동 프로세서들을 달아준다.
	// 각각 5개씩 있고 파라메터는 전부 합쳐 10개임. 변경될 가능성도 있다.
	pNewSkill = new CDnSkill( hActor );
	
	bool bResult = pNewSkill->Initialize( iSkillTableID, iLevel );
	if( false == bResult )
	{
		delete pNewSkill;
		return CDnSkill::Identity();
	}

	if( hActor && hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());
		pNewSkill->RegisterObserver( pPlayerActor->GetBubbleSystem() );
	}
	
	return pNewSkill->GetMySmartPtr();
}

#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
bool CDnSkill::_LoadMonsterSkillLevelData( int iSkillTableID, int iLevel, int iSkillLevelDataApplyType )
{
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	vector<int> vlSkillLevelList;
	if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) <= 0 ) 
		return false;

	if( !pSkillTable->IsExistItem( iSkillTableID) )
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

	if( -1 == iSkillLevelTableID )
		return false;

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

		iOffsetCheck = iProcessorParamOffset;
		IDnSkillProcessor* pSkillProcessor = IDnSkillProcessor::Create( m_hActor, iProcessor, iSkillLevelTableID, &iProcessorParamOffset, this->GetUseActionNames() );

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
	// 실제 갯수로 업데이트.
	if( 0 == m_iMaxLevel )
	{
		m_iMaxLevel = (int)vlSkillLevelList.size();
	}
	else
	{
		_ASSERT( m_iMaxLevel == (int)vlSkillLevelList.size() );
	}

	m_eSkillType = (SkillTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillType" )->GetInteger();
	m_eDurationType = (DurationTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_DurationType" )->GetInteger();
	m_eTargetType = (TargetTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_TargetType" )->GetInteger();

	m_iDissolvable = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_Dissolvable" )->GetInteger();
	m_iDuplicateCount = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_EffectAmassCount" )->GetInteger();
	m_iSkillDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillDuplicate" )->GetInteger();
	m_iEffectDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_EffectDuplicate" )->GetInteger();
	m_eElement = (CDnState::ElementEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_Element" )->GetInteger();
	m_iNeedJobClassID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_NeedJob" )->GetInteger();
	if( (CDnState::ElementEnum)-1 == m_eElement )
		m_eElement = CDnState::ElementEnum_Amount;

	// skill level table
	m_iSkillLevelID = iSkillLevelTableID;
	m_iLevel = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillLevel" )->GetInteger();
	m_iNowLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();

#if defined(PRE_FIX_NEXTSKILLINFO)
	SKILL_LEVEL_INFO* pTabelInfo = g_pDataManager->GetSkillLevelTableIDList(iSkillTableID, iSkillLevelDataApplyType);
	int nextSkillLevel = m_iLevel + 1;

	if( m_iLevel < m_iMaxLevel )
	{
		int nextLevelTableID = -1;
		SKILL_LEVEL_TABLE_IDS::iterator findIter = pTabelInfo->_SkillLevelTableIDs.find(nextSkillLevel);
		if (findIter != pTabelInfo->_SkillLevelTableIDs.end())
			nextLevelTableID = findIter->second;

		m_iNextLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( nextLevelTableID, "_NeedSkillPoint" )->GetInteger();
	}
	else
		m_iNextLevelSkillPoint = 0;

#else
	if( m_iLevel < m_iMaxLevel )
		m_iNextLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_NeedSkillPoint" )->GetInteger();
	else
		m_iNextLevelSkillPoint = 0;
#endif // PRE_FIX_NEXTSKILLINFO

	m_iIncreaseRange[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_AddRange" )->GetInteger();

	m_iAdditionalThreat = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_AddThreat" )->GetInteger();

	m_iCPScore = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_CPScore" )->GetInteger();
	m_fStartSuperArmor = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StartSuperArmor" )->GetFloat();
	m_bStartCanHit = (pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StartCanHit" )->GetInteger() == 1) ? true : false;

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
		// 모두 적용임. 타겟만 다르게 해서 똑같은 상태효과 2개를 추가해준다.
		// 하지만 강화 패시브로 사용되는 스킬은 2개로 하지 않고 그냥 생성. 다른 스킬을 강화시키는 데이터 역할만 수행하기 때문.
		bool bApplyAll = (StateEffectApplyType::ApplyAll == iApplyType) && (SkillTypeEnum::EnchantPassive != m_eSkillType);

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

	SkillInfo& MySkillInfo = m_SkillInfo[ iSkillLevelDataApplyType ];
	MySkillInfo.iSkillID = m_iSkillID;
	MySkillInfo.iSkillLevelID = m_iSkillLevelID;
	MySkillInfo.iLevel = m_iLevel;
	MySkillInfo.iSkillDuplicateMethod = m_iSkillDuplicateMethod;
	MySkillInfo.iDuplicateCount = m_iDuplicateCount;

	MySkillInfo.eSkillType = m_eSkillType;
	MySkillInfo.eDurationType = m_eDurationType;
	MySkillInfo.eTargetType = m_eTargetType;
	MySkillInfo.eApplyType = StateEffect.ApplyType;
	MySkillInfo.iDissolvable = m_iDissolvable;
	MySkillInfo.eSkillElement = m_eElement;
	MySkillInfo.hSkillUser = m_hActor;				// Note: 액터가 항상 유효한 것은 아님
	if( m_hActor )
		MySkillInfo.iSkillUserTeam = m_hActor->GetTeam();

	if( m_hActor && _tcslen(m_hActor->GetName()) > 0 )
		MySkillInfo.strUserName = m_hActor->GetName();

	MySkillInfo.szEffectOutputIDs = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StateEffectTableID" )->GetString();

	return true;
}

bool CDnSkill::InitializeMonsterSkill( int iSkillTableID, int iLevel )
{
	m_iSkillID = iSkillTableID;

	// 각 모드별로 데이터 로드. 먼저 PVP 를 읽는다.
	// PVE 는 디폴트 값이기 때문에 모드별로 나뉘어지는 값이 아니면 디폴트 값으로 채워지게 된다.

	// 몬스터 스킬 생성 함수이므로 PVE 만 호출.
	if( false == _LoadSkillLevelData( iSkillTableID, iLevel, PVP ) )
		return false;

	if( false == _LoadSkillLevelData( iSkillTableID, iLevel, PVE ) )
		return false;

	int iNumProcessor = (int)m_vlpProcessors[ PVE ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ PVE ].at( iProcessor );

		// 사용하는 액션이 있으면 외부에서 조회용으로 데이터 채워놓음
		if( pProcessor->GetType() == IDnSkillProcessor::PLAY_ANI )
			m_setUseActionNames.insert( static_cast<CDnPlayAniProcess*>(pProcessor)->GetActionName() );
		else
			if( pProcessor->GetType() == IDnSkillProcessor::PARTIAL_PLAY_ANI )
			{
				CDnPartialPlayProcessor* pPartialPlayAni = static_cast<CDnPartialPlayProcessor*>(pProcessor);
				m_setUseActionNames.insert( pPartialPlayAni->GetStartActionName() );
				m_setUseActionNames.insert( pPartialPlayAni->GetLoopActionName() );
				m_setUseActionNames.insert( pPartialPlayAni->GetEndActionName() );
			}
	}

	// 초기화 했을 때는 pve 모드가 디폴트임.
	m_iSelectedSkillLevelDataApplyType = PVE;

	return true;
}

DnSkillHandle CDnSkill::CreateMonsterSkill( DnActorHandle hActor, int iSkillTableID, int iLevel )
{
	if( hActor )
	{
		CDnSkill* pNewSkill = pNewSkill = new CDnSkill( hActor );
		if( pNewSkill )
		{
			bool bResult = pNewSkill->InitializeMonsterSkill( iSkillTableID, iLevel );
			if( false == bResult )
			{
				SAFE_DELETE( pNewSkill );
				return CDnSkill::Identity();
			}

			if( hActor->IsPlayerActor() )
			{
				CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());
				pNewSkill->RegisterObserver( pPlayerActor->GetBubbleSystem() );
			}

			return pNewSkill->GetMySmartPtr();
		}
		else
		{
			SAFE_DELETE( pNewSkill );
			return CDnSkill::Identity();
		}
	}

	return CDnSkill::Identity();
}
#endif // #ifdef PRE_FIX_GAMESERVER_OPTIMIZE

void CDnSkill::CreateSkillInfo( int nSkillID, int nSkillLevel, CDnSkill::SkillInfo & sSkillInfo, std::vector<CDnSkill::StateEffectStruct> & vecSkillEffect, bool bUseBattleGround/* = false*/ )
{
	if( nSkillID == 0 || nSkillLevel == 0 )
		return;

	DNTableFileFormat* pSkillTable = NULL;
	DNTableFileFormat* pSkillLevelTable = NULL;
	if (bUseBattleGround)
	{
		pSkillTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILL );
		pSkillLevelTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );
	}
	else
	{
		pSkillTable = GetDNTable( CDnTableDB::TSKILL );
		pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
	}

	if (pSkillLevelTable == NULL || pSkillTable == NULL)
	{
		_DANGER_POINT();
		return;
	}
	std::vector<int> vlSkillLevelList;
	if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", nSkillID, vlSkillLevelList ) <= 0 ) 
		return;

	int iSkillLevelTableID = -1;
	for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
	{
		int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();
		if( iNowLevel == nSkillLevel )
		{
			iSkillLevelTableID = vlSkillLevelList.at( i );
			break;
		}
	}

	if( -1 == iSkillLevelTableID )
		return;

	sSkillInfo.iSkillID = nSkillID;
	sSkillInfo.iSkillLevelID = iSkillLevelTableID;
#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
	sSkillInfo.iAppliedEnchantSkillID = 0;
#endif
	sSkillInfo.iLevel = nSkillLevel;
	sSkillInfo.eDurationType = (CDnSkill::DurationTypeEnum)pSkillTable->GetFieldFromLablePtr( nSkillID, "_DurationType" )->GetInteger();
	sSkillInfo.eTargetType = (CDnSkill::TargetTypeEnum)pSkillTable->GetFieldFromLablePtr( nSkillID, "_TargetType" )->GetInteger();
	sSkillInfo.iSkillDuplicateMethod = pSkillTable->GetFieldFromLablePtr( nSkillID, "_SkillDuplicate" )->GetInteger();
	sSkillInfo.iDuplicateCount = pSkillTable->GetFieldFromLablePtr( nSkillID, "_EffectAmassCount" )->GetInteger();
	sSkillInfo.iDissolvable = pSkillTable->GetFieldFromLablePtr( nSkillID, "_Dissolvable" )->GetInteger();
	sSkillInfo.szEffectOutputIDToClient = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StateEffectTableID" )->GetString();

	sSkillInfo.bFromBuffProp = true;

	// 상태 효과 정보 로딩
	CDnSkill::StateEffectStruct StateEffect;
	char caLable[ 64 ];
	ZeroMemory( caLable, sizeof(caLable) );
	for( int i = 0; i < MAX_STATE_EFFECT_COUNT; ++i )
	{
		sprintf_s( caLable, "_EffectClass%d", i + 1 );
		StateEffect.nID = pSkillTable->GetFieldFromLablePtr( nSkillID, caLable )->GetInteger();

		if( StateEffect.nID < 1 ) 
			continue;

		sprintf_s( caLable, "_EffectClass%dApplyType", i + 1 );
		StateEffect.ApplyType = (CDnSkill::StateEffectApplyType)pSkillTable->GetFieldFromLablePtr( nSkillID, caLable )->GetInteger();

		sprintf_s( caLable, "_EffectClassValue%d", i + 1 );
		StateEffect.szValue = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetString();

		sprintf_s( caLable, "_EffectClassValue%dDuration", i + 1 );
		StateEffect.nDurationTime = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetInteger();

		vecSkillEffect.push_back( StateEffect );
	}
}

void CDnSkill::CreateBattleGroundSkillInfo( int nSkillID, int nSkillLevel, CDnSkill::SkillInfo & sSkillInfo, std::vector<CDnSkill::StateEffectStruct> & vecSkillEffect )
{
	CreateSkillInfo(nSkillID, nSkillLevel, sSkillInfo, vecSkillEffect, true);
}

bool CDnSkill::_LoadSkillLevelData( int iSkillTableID, int iLevel, int iSkillLevelDataApplyType )
{
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	vector<int> vlSkillLevelList;
	if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) <= 0 ) 
		return false;

	if( !pSkillTable->IsExistItem( iSkillTableID) )
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

	if( -1 == iSkillLevelTableID )
		return false;

#if defined(PRE_FIX_NEXTSKILLINFO)
	//NextLevel이 연속으로 있지 않을 수 있다.. 그래서 여기서 다음 레벨 데이타 테이블 ID를 찾아 놓는다.
	int iMinSkillLevelTableID = -1;
	int iNextSkillLevelTableID = -1;

	SKILL_LEVEL_INFO* pTableInfo = g_pDataManager->GetSkillLevelTableIDList(iSkillLevelTableID, iSkillLevelDataApplyType);
	SKILL_LEVEL_TABLE_IDS::iterator findIter = pTableInfo->_SkillLevelTableIDs.find(pTableInfo->_MinLevel);
	if (findIter != pTableInfo->_SkillLevelTableIDs.end())
		iMinSkillLevelTableID = findIter->second;

	findIter = pTableInfo->_SkillLevelTableIDs.find(iLevel + 1);
	if (findIter != pTableInfo->_SkillLevelTableIDs.end())
		iNextSkillLevelTableID = findIter->second;

#endif // PRE_FIX_NEXTSKILLINFO

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

		iOffsetCheck = iProcessorParamOffset;
		IDnSkillProcessor* pSkillProcessor = IDnSkillProcessor::Create( m_hActor, iProcessor, iSkillLevelTableID, &iProcessorParamOffset, this->GetUseActionNames() );

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
	// 실제 갯수로 업데이트.
	if( 0 == m_iMaxLevel )
	{
		m_iMaxLevel = (int)vlSkillLevelList.size();
	}
	else
	{
		_ASSERT( m_iMaxLevel == (int)vlSkillLevelList.size() );
	}

	m_eSkillType = (SkillTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillType" )->GetInteger();
	m_eDurationType = (DurationTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_DurationType" )->GetInteger();
	m_eTargetType = (TargetTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_TargetType" )->GetInteger();

	m_iDissolvable = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_Dissolvable" )->GetInteger();
	m_iDuplicateCount = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_EffectAmassCount" )->GetInteger();
	m_iSkillDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillDuplicate" )->GetInteger();
	m_iEffectDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_EffectDuplicate" )->GetInteger();
	m_eElement = (CDnState::ElementEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_Element" )->GetInteger();
	m_iNeedJobClassID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_NeedJob" )->GetInteger();
	if( (CDnState::ElementEnum)-1 == m_eElement )
		m_eElement = CDnState::ElementEnum_Amount;

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
	m_iNowLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
	if( m_iLevel < m_iMaxLevel )
#if defined(PRE_FIX_NEXTSKILLINFO)
		m_iNextLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
#else
		m_iNextLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_NeedSkillPoint" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO
	else
		m_iNextLevelSkillPoint = 0;

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
	m_iAdditionalThreat = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_AddThreat" )->GetInteger();

	m_fHPConsumeType[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_HPConsumeType" )->GetFloat();
	m_fMPConsumeType[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SPConsumeType" )->GetFloat();
	m_iCPScore = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_CPScore" )->GetInteger();
	m_fStartSuperArmor = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StartSuperArmor" )->GetFloat();
	m_bStartCanHit = (pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StartCanHit" )->GetInteger() == 1) ? true : false;

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
		// 모두 적용임. 타겟만 다르게 해서 똑같은 상태효과 2개를 추가해준다.
		// 하지만 강화 패시브로 사용되는 스킬은 2개로 하지 않고 그냥 생성. 다른 스킬을 강화시키는 데이터 역할만 수행하기 때문.
		bool bApplyAll = (StateEffectApplyType::ApplyAll == iApplyType) && (SkillTypeEnum::EnchantPassive != m_eSkillType);		

		if( bApplyAll )
		{
			StateEffect.ApplyType = StateEffectApplyType::ApplySelf;
			StateEffect.bApplyAllPair = true;
		}
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

	SkillInfo& MySkillInfo = m_SkillInfo[ iSkillLevelDataApplyType ];
	MySkillInfo.iSkillID = m_iSkillID;
	MySkillInfo.iSkillLevelID = m_iSkillLevelID;
	MySkillInfo.iLevel = m_iLevel;
	MySkillInfo.iSkillDuplicateMethod = m_iSkillDuplicateMethod;
	MySkillInfo.iDuplicateCount = m_iDuplicateCount;

	MySkillInfo.eSkillType = m_eSkillType;
	MySkillInfo.eDurationType = m_eDurationType;
	MySkillInfo.eTargetType = m_eTargetType;
	MySkillInfo.eApplyType = StateEffect.ApplyType;
	MySkillInfo.iDissolvable = m_iDissolvable;
	MySkillInfo.eSkillElement = m_eElement;
	MySkillInfo.hSkillUser = m_hActor;				// Note: 액터가 항상 유효한 것은 아님
	if( m_hActor )
		MySkillInfo.iSkillUserTeam = m_hActor->GetTeam();

	if( m_hActor && _tcslen(m_hActor->GetName()) > 0 )
		MySkillInfo.strUserName = m_hActor->GetName();

	MySkillInfo.szEffectOutputIDs = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StateEffectTableID" )->GetString();

	if( 0.0f == m_fHPConsumeType[ iSkillLevelDataApplyType ] )
	{
		m_iNeedHP[ iSkillLevelDataApplyType ] = m_iDecreaseHP[ iSkillLevelDataApplyType ];
	}
	else
	{
		m_iNeedHP[ iSkillLevelDataApplyType ] = int((float)m_hActor->GetMaxHP() * m_fHPConsumeType[ iSkillLevelDataApplyType ]);
	}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
	m_nPrefixSkillType = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_Group" )->GetInteger();

	//스킬 정보에 접미사 스킬 그룹ID를 설정 해 놓는다..(클라이언트는 몰라도 됨?...)
	SetPrefixSkillType(m_nPrefixSkillType);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

	return true;
}


void CDnSkill::_OnInitialize( void )
{
	// 129번 ChangeActionSet 상태효과와 DnApplySEWhenActionSetBlowEnabledProcessor 발현 타입이 있다면 ChangeActionSet 을 제외한 모든 상태효과를
	// 발현타입 객체에 몰아주고 삭제. 추후에 바뀐 액션에서만 상태효과가 유효하도록 발현타입에서 컨트롤 하게 된다.
	// 우선 그냥 포인터를 물려볼까..
	for( int k = PVE; k < NUM_SKILLLEVEL_APPLY_TYPE; ++k )
	{
		CDnApplySEWhenActionSetBlowEnabledProcessor* pApplySEWhenActionSetBlowEnableProcessor = 
			static_cast<CDnApplySEWhenActionSetBlowEnabledProcessor*>(GetProcessor( IDnSkillProcessor::APPLY_SE_WHEN_ACTIONSET_ENABLED, k ));
		if( pApplySEWhenActionSetBlowEnableProcessor )
		{
			// 이 발현타입이 있는데 ChangeActionSet 상태효과와 ChangeActionStr 발현타입이 없으면 오류..
			bool bValid = false;
			for( int i = 0; i < (int)m_vlStateEffectList[ k ].size(); ++i )
			{
				StateEffectStruct& StateEffect = m_vlStateEffectList[ k ].at( i );
				if( STATE_BLOW::BLOW_129 == StateEffect.nID )
				{
					bValid = true;
					break;
				}
			}

			if( bValid )
			{
				CDnChangeActionStrProcessor* pChangeActionStrProcessor = static_cast<CDnChangeActionStrProcessor*>(GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR, k ));
				if( pChangeActionStrProcessor )
					bValid = true;
				else
					bValid = false;
			}

			if( bValid )
			{
				for( int i = 0; i < (int)m_vlStateEffectList[ k ].size(); ++i )
				{
					StateEffectStruct& StateEffect = m_vlStateEffectList[ k ].at( i );
					if( STATE_BLOW::BLOW_129 != StateEffect.nID )
					{
						pApplySEWhenActionSetBlowEnableProcessor->AddStateEffect( &StateEffect );
						StateEffect.bApplyInProcessor = true;
					}
				}
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
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );

	RefreshDecreaseMP();

	int iNumProcessor = (int)m_vlpProcessors[ PVE ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ PVE ].at( iProcessor );

		// 사용하는 액션이 있으면 외부에서 조회용으로 데이터 채워놓음
		if( pProcessor->GetType() == IDnSkillProcessor::PLAY_ANI )
			m_setUseActionNames.insert( static_cast<CDnPlayAniProcess*>(pProcessor)->GetActionName() );
		else
		if( pProcessor->GetType() == IDnSkillProcessor::PARTIAL_PLAY_ANI )
		{
			CDnPartialPlayProcessor* pPartialPlayAni = static_cast<CDnPartialPlayProcessor*>(pProcessor);
			m_setUseActionNames.insert( pPartialPlayAni->GetStartActionName() );
			m_setUseActionNames.insert( pPartialPlayAni->GetLoopActionName() );
			m_setUseActionNames.insert( pPartialPlayAni->GetEndActionName() );
		}
	}

	// 초기화 했을 때는 pve 모드가 디폴트임.
	m_iSelectedSkillLevelDataApplyType = PVE;

	_OnInitialize();

	if (m_eDurationType == SummonOnOff)
		OnInitializeSummonMonsterInfo();

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


bool CDnSkill::AddProcessor( IDnSkillProcessor* pProcessor, int iSelectedLevelData )
{
	bool bResult = false;

	if( pProcessor )
	{
		m_vlpProcessors[ iSelectedLevelData ].push_back( pProcessor );
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
#ifdef _GAMESERVER
				if( m_hActor->GetWeapon( nWeapon ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( nWeapon )->GetEquipType()
					|| m_hActor->GetWeapon( nWeapon ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( nWeapon )->GetEquipType() )
#else // _GAMESERVER
				if( m_hActor->GetWeapon( nWeapon, false ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( nWeapon, false )->GetEquipType()
					|| m_hActor->GetWeapon( nWeapon, false ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( nWeapon, false )->GetEquipType() )
#endif // _GAMESERVER
					bSatisfy = true;
			}
			else	// 2. 주무기, 보조무기인 경우 and
			{
				if( CDnWeapon::IsSubWeapon( m_aeNeedEquipType[ 0 ] ) )
				{
#ifdef _GAMESERVER
					if( m_hActor->GetWeapon( 1 ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( 1 )->GetEquipType()
						&& m_hActor->GetWeapon( 0 ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( 0 )->GetEquipType() )
#else // _GAMESERVER
					if( m_hActor->GetWeapon( 1, false ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( 1, false )->GetEquipType()
						&& m_hActor->GetWeapon( 0, false ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( 0, false )->GetEquipType() )
#endif // _GAMESERVER
						bSatisfy = true;
				}
				else
				{
#ifdef _GAMESERVER
					if( m_hActor->GetWeapon( 0 ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( 0 )->GetEquipType()
						&& m_hActor->GetWeapon( 1 ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( 1 )->GetEquipType() )
#else // _GAMESERVER
					if( m_hActor->GetWeapon( 0, false ) && m_aeNeedEquipType[ 0 ] == m_hActor->GetWeapon( 0, false )->GetEquipType()
						&& m_hActor->GetWeapon( 1, false ) && m_aeNeedEquipType[ 1 ] == m_hActor->GetWeapon( 1, false )->GetEquipType() )
#endif // _GAMESERVER
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
#ifdef _GAMESERVER
						if( m_hActor->GetWeapon( k ) && m_aeNeedEquipType[ i ] == m_hActor->GetWeapon( k )->GetEquipType() )
#else // _GAMESERVER
						if( m_hActor->GetWeapon( k, false ) && m_aeNeedEquipType[ i ] == m_hActor->GetWeapon( k, false )->GetEquipType() )
#endif // _GAMESERVER
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
			if( pPlayerActor->IsTransformSkill(GetClassID()) == false )
				return UsingResult::Failed;
		}
	}
	if (m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_176))
	{
		DNVector(DnBlowHandle) vlBlows;
		CDnAllowedSkillsBlow* pAllowedSkillBlow = NULL;
		m_hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_176, vlBlows);
		for (int i = 0; i < (int)vlBlows.size(); ++i)
		{
			pAllowedSkillBlow = static_cast<CDnAllowedSkillsBlow*>(vlBlows[i].GetPointer());
			if (!pAllowedSkillBlow) continue;

			if (!pAllowedSkillBlow->IsAllowSkill(m_iSkillID))
				return UsingResult::Failed;
		}
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
			if( hBlow && hBlow->GetBlowIndex() == STATE_BLOW::BLOW_067 )
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
	if( (CDnWeapon::EquipTypeEnum_Amount != m_aeNeedEquipType[ 0 ]) ||
		(CDnWeapon::EquipTypeEnum_Amount != m_aeNeedEquipType[ 1 ]) )
	{
		bool bSatisfy = false;
		for( int i = 0; i < MAX_SKILL_NEED_EQUIP_COUNT; ++i )
		{
			if( CDnWeapon::EquipTypeEnum_Amount != m_aeNeedEquipType[ i ] )
			{
				for( int k = 0; k < 2; ++k )
				{
					// #11120 관련. 발차기 상태에서는 장착하고 있는 무기를 얻지 못하기 때문에 명시적으로 원래 함수로 장착하고 있는 무기를 얻어오자.
					if( m_hActor->CDnActor::GetWeapon( k ) && m_aeNeedEquipType[ i ] == m_hActor->CDnActor::GetWeapon( k )->GetEquipType() )
					{
						bSatisfy = true;
						break;
					}
				}
				if( bSatisfy )
					break;
			}
		}

		if( false == bSatisfy )
			return UsingResult::Failed;
	}

	// 화살 같은 소모성 아이템의 갯수 확인
	bool bCheckNeedItem = true;
	if( GetRoom() && bIsExtremitySkill() && static_cast<CDNGameRoom*>(GetRoom())->bIsLadderRoom() )	// 래더에서 궁극기 스킬
		bCheckNeedItem = false;

	if( bCheckNeedItem && m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ] > 0 )
	{
		int iNumNeedItem = CDnItemTask::GetInstance( m_hActor->GetRoom() ).ScanItemFromID( m_hActor, m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ], NULL );
		if( iNumNeedItem < m_iNeedItemDecreaseCount[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;
	}
	// 직업 체크
	//if( m_hActor->GetClassID() <= CDnActor::Reserved6  )
	if( m_hActor->IsPlayerActor() )
	{
		//CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		CDnPlayerActor* pActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( NULL == pActor )
			return UsingResult::Failed;
		if( 0 != m_iNeedJobClassID )
		{
			if( pActor->IsPassJob( m_iNeedJobClassID ) == false ) 
				return UsingResult::Failed;
		}

		// 배틀 모드가 아니면 스킬 발동 불가!
		if( !pActor->IsBattleMode() )
			return UsingResult::Failed;
	}

	switch( m_eDurationType )
	{
	    case CDnSkill::Instantly:
		case CDnSkill::Buff:
		case CDnSkill::Debuff:
		case CDnSkill::SummonOnOff:
		case CDnSkill::StanceChange:
			{
				// 쿨타임이 끝나지 않았다면 스킬 발동 불가.. 우선 클라에서 체크해서 보내주므로 서버에선 약간의 유예 시간을 남겨둔다.
				// 몬스터이거나 오토 패시브 스킬인 경우엔 칼같이 체크.
				if( m_hActor->IsMonsterActor() ||
					SkillTypeEnum::AutoPassive == m_eSkillType )
				{
					if( m_fCoolTime > 0.0f )
						return UsingResult::FailedByCooltime;
				}
				else
				{
					if( m_fLeftDelayTime > 0.5f )
						return UsingResult::FailedByCooltime;
				}
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

	if( 0.0f < m_fAnotherGlobalSkillCoolTime )
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
	//위m_fAnotherGlobalSkillCoolTime이 적용 되어 있으면 적용 안 하도록..
	{
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
	if( 0.0f < m_fAnotherGlobalSkillCoolTime )
		return m_fAnotherGlobalSkillCoolTime;

	return  m_fDelayTime[ m_iSelectedSkillLevelDataApplyType ]; 
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
}


void CDnSkill::OnBeginCoolTime()
{
	m_dwLastUseSkillTimeStamp = timeGetTime();

	m_fCoolTime = (GetDelayTime() == 0.0f) ? 0.0f : 1.0f;

	m_fCoolTimeAdjustBlowValue = 1.0f;

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
}



void CDnSkill::_OnBeginProcessException( void )
{
	// 결빙상태에서 워리어 릴리브 스킬을 쓰는 경우 바로 프레임 고정을 풀어준다. #12438
	// 결빙상태효과가 걸린 상태에서 디버프 상태효과 해제 상태효과가 self 로 적용되는 스킬을 
	// 사용하는 경우로 일반화 시킴.
	if( m_hActor && 
		(
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_041 ) ||
		m_hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_146 ) ||
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

	//낙인 상태효과 제거
	ClearStigmaStateEffect();
}

void CDnSkill::ClearStigmaStateEffect()
{
	//////////////////////////////////////////////////////////////////////////
	//낙인 상태효과가 있는 스킬인지 확인...
	bool hasStigmaState = false;
	int nListCount = (int)m_vlStateEffectList[m_iSelectedSkillLevelDataApplyType].size();
	for (int i = 0; i < nListCount; ++i)
	{
		const StateEffectStruct& SE = m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].at( i );
		if (SE.nID == STATE_BLOW::BLOW_246)
		{
			hasStigmaState = true;
			break;
		}
	}

	//주위 낙인 효과를 찾아서 제거 해준다...
	if (hasStigmaState == true)
	{
		DNVector(DnActorHandle) hVecList;
		m_hActor->ScanActor(m_hActor->GetRoom(), *m_hActor->GetPosition(), FLT_MAX, hVecList);

		int nActorCount = (int)hVecList.size();
		for (int i = 0; i < nActorCount; ++i)
		{
			DnActorHandle hActor = hVecList[i];
			if (hActor && hActor->IsDie() == false && hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_246))
			{
				DNVector(DnBlowHandle) vlhBlows;
				hActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_246, vlhBlows );
				int iNumBlow = (int)vlhBlows.size();
				for( int j = 0; j < iNumBlow; ++j )
				{
					DnBlowHandle hBlow = vlhBlows[j];
					if (hBlow && hBlow->IsEnd() == false)
					{
						CDnSkill::SkillInfo* pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
						if (pSkillInfo && pSkillInfo->hSkillUser == m_hActor)
						{
							int nBlowID = hBlow->GetBlowID();
							hActor->SendRemoveStateEffectFromID(nBlowID);	//제거 패킷 보내고
							hActor->GetStateBlow()->RemoveImediatlyStateEffectFromID(nBlowID);	//즉시 제거 한다.
						}
					}
				}
			}

		}

	}
	//////////////////////////////////////////////////////////////////////////
}

// 스킬에 달려있는 Processor 들 중에 선처리 해야하는 것들 먼저 처리.
void CDnSkill::CheckProcessorOnBegin( void )
{
	CheckChangeActionStrByBubbleProcessor();
	CheckStateEffectApplyOnOffByBubbleProcessor();
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
			if (pPlayAniProcessor)
				pPlayAniProcessor->ChangeActionNameOnce( pChangedActionName );
		}
	}
}


void CDnSkill::CheckStateEffectApplyOnOffByBubbleProcessor( void )
{
	// 버블 갯수에 따라 상태효과 갯수를 변경시키는 Processor
	CDnStateEffectApplyOnOffByBubbleProcessor* pProcessor = static_cast<CDnStateEffectApplyOnOffByBubbleProcessor*>(GetProcessor(IDnSkillProcessor::STATE_EFFECT_APPLY_ONOFF_BY_BUBBLE));
	if( pProcessor )
	{
		pProcessor->SelectAvailableSE( m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ] );
	}
}


void CDnSkill::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	ApplyAddtionalStateInfo();

	// 등록된 옵저버에게 스킬 사용 메시지 보냄.
	boost::shared_ptr<IDnObserverNotifyEvent> pNotifyEvent( new CDnUseSkillMessage );
    boost::shared_ptr<CDnUseSkillMessage> pSkillUseEvent = shared_polymorphic_downcast<CDnUseSkillMessage>( pNotifyEvent );
	pSkillUseEvent->SetSkillID( m_iSkillID );
	CDnObservable::Notify( pNotifyEvent );

	CheckProcessorOnBegin();

	// 같은 글로벌 ID 를 사용하는 다른 글로벌 스킬을 사용하여 셋팅된 쿨타임 값을 실제로 이 스킬 사용될 때는 제거한다.
	m_fAnotherGlobalSkillCoolTime = 0.0f;

	m_nAnotherGlobakSkillID = 0;

	m_SkillStartTime = LocalTime;
	//m_LastAuraCheckTime = 0;

#if defined( PRE_ADD_ACADEMIC ) // 소환형 몬스터 액티브 토글 스킬은 쿨타임이 돌필요가 없습니다.
	if(m_eDurationType != DurationTypeEnum::ActiveToggleForSummon)
		//OnBeginCoolTime();

	{
		//--------------------------------------------------
		//[debug_skill]  server
		//세콘늴鑒�阮�
		//祺꼍鸞=6001   槿近랐땄=6003  莖슉黛듐=6204   섐똑댔샌=6208

		if ( 6001 == GetClassID() || 6003 == GetClassID() || 6204 == GetClassID() || 6208 == GetClassID() )
		{
			//int nSkillID = GetClassID();
		} 
		else
		{
			OnBeginCoolTime();
		}
		//--------------------------------------------------
	}
#else
		//--------------------------------------------------
		//[debug_skill]  server
		//세콘늴鑒�阮�
	    //祺꼍鸞=6001   槿近랐땄=6003  莖슉黛듐=6204   섐똑댔샌=6208

	    if ( 6001 == GetClassID() || 6003 == GetClassID() || 6204 == GetClassID() || 6208 == GetClassID() )
		{
			int nSkillID = GetClassID();
		} 
		else
		{
			OnBeginCoolTime();
		}
		//--------------------------------------------------
#endif

	m_LastTimeToggleMPDecreaseTime = LocalTime;
	m_bFinished = false;

	// 아이템 소모로 셋팅되어있다면 소모하도록 처리.
	bool bCheckNeedItem = true;
	if( GetRoom() && bIsExtremitySkill() && static_cast<CDNGameRoom*>(GetRoom())->bIsLadderRoom() )	// 래더에서 궁극기 스킬
		bCheckNeedItem = false;

	if( bCheckNeedItem && m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ] > 0 )
	{
		if( m_hActor && m_hActor->IsPlayerActor() )
		{
			CDNUserItem* pUserItem = static_cast<CDnPlayerActor*>(m_hActor.GetPointer())->GetUserSession()->GetItem();
			//int iNeedItemSlotIndex = pUserItem->FindInventorySlot( m_iNeedItemID, m_iNeedItemDecreaseCount );
			if (!pUserItem->DeleteInventoryByItemID( m_iNeedItemID[ m_iSelectedSkillLevelDataApplyType ], m_iNeedItemDecreaseCount[ m_iSelectedSkillLevelDataApplyType ], DBDNWorldDef::UseItem::Use )) return;
		}
	}

	if( ActiveToggle != m_eDurationType && TimeToggle != m_eDurationType && ActiveToggleForSummon != m_eDurationType )
	{
		CheckAndAddSelfStateEffect();
	}

	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->OnBegin( LocalTime, fDelta, GetMySmartPtr() );
	}

	if( false == m_bItemSkill )
	{
		m_hActor->SetSkillSuperAmmor( int(m_hActor->GetSuperAmmor() * m_fStartSuperArmor) );
		m_hActor->SetHittable( m_bStartCanHit );
	}

	// 스킬 사용 관련 예외 상황 처리.
	_OnBeginProcessException();

	if( m_hActor->IsPlayerActor() && 0.0f != m_fResetCooltime )
	{
		int nRand = _rand(m_hActor->GetRoom())%100;

		if( nRand < (int)(m_fResetCooltime * 100) )
		{
			static_cast<CDnPlayerActor*>(m_hActor.GetPointer())->RequestCooltimeReset( m_iSkillID );
			m_fCoolTime = 0.0f; 
			m_fLeftDelayTime = 0.0f;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
			m_fDeltaGlobalCoolTime = 0.0f;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
		}
	}

#if defined(PRE_ADD_50903)
	//스킬 시작될때 리셋 시킴..
	SetHitCountForVarianceDamage(0);
#endif // PRE_ADD_50903
}


// 실제로 스킬이 발동 되었을 시에 처리
void CDnSkill::ProcessExecute( LOCAL_TIME LocalTime, float fDelta )
{
	switch( m_eDurationType )
	{
		case DurationTypeEnum::TimeToggle:
			break;

		case DurationTypeEnum::Aura:
			break;
	}

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
	CheckAndRemoveInstantApplySelfStateEffect();

	//접두어에서 추가된 상태 효과 제거
	RemovePrefixBlow();

	int iNumProcessor = (int)m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pProcessor = m_vlpProcessors[ m_iSelectedSkillLevelDataApplyType ].at( iProcessor );
		pProcessor->OnEnd( LocalTime, fDelta );
	}

	m_SkillInfo->bIgnoreImmune = m_isIgnoreImmuneBackup;

	CheckProcessorOnEnd();

	// 버블로 강화된 상태라면 원래대로.
	if( m_bEnchantedFromBubble )
	{
		ReleaseEnchantSkill();
		m_bEnchantedFromBubble = false;
	}

	RemoveAddtionalStateInfo();

#if defined(PRE_FIX_64312)
	if (m_isAppliedSummonMonsterEnchantSkill == true)
	{
		ReleaseEnchantSkill();
		m_isAppliedSummonMonsterEnchantSkill = false;
	}
#endif // PRE_FIX_64312

}


void CDnSkill::CheckProcessorOnEnd( void )
{
	// 버블 갯수에 따라 상태효과 갯수를 변경시키는 Processor
	CDnStateEffectApplyOnOffByBubbleProcessor* pProcessor = static_cast<CDnStateEffectApplyOnOffByBubbleProcessor*>(GetProcessor(IDnSkillProcessor::STATE_EFFECT_APPLY_ONOFF_BY_BUBBLE));
	if( pProcessor )
	{
		pProcessor->RestoreSEList( m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ] );
	}
}


// 쿨 타임 등등의 처리
void CDnSkill::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( TIME_ACCELERATION_SKILL_ID != m_iSkillID && SPRIT_BOOST_SKILL_ID != m_iSkillID )
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
					if( m_fLeftDelayTime == 0.0f ) return;

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

					m_fCoolTime = ( 1.0f / (m_fOnceDelayTime*m_fCoolTimeAdjustBlowValue) ) * m_fOnceElapsedDelayTime;

				}
			}
			break;

		case DurationTypeEnum::TimeToggle:

			break;

		case DurationTypeEnum::ActiveToggle:
		case DurationTypeEnum::ActiveToggleForSummon:

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

	if( m_bChainingPassiveSkill )
		return false;

	// MP 를 지속적으로 소모하는 스킬일경우 엠피를 모두 소진하였을때 스킬을 끝내줍니다.
	if( 0.0f < m_fPassiveActionSkillLength || -1.0f == m_fPassiveActionSkillLength )
	{
		float fDecreaseMPRatio = 0.f;
		DNVector( DnBlowHandle ) vlhBlows;
		m_hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_014, vlhBlows );

		for( DWORD i=0; i<vlhBlows.size(); i++ )
		{
			if( vlhBlows[i] && vlhBlows[i]->GetParentSkillInfo()->iSkillID == GetClassID() )
			{
				fDecreaseMPRatio = vlhBlows[i]->GetFloatValue();
				break;
			}
		}

		if( m_hActor->GetSP() < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] && fDecreaseMPRatio < 0.f )
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

IDnSkillProcessor* CDnSkill::GetProcessor( int iType, int iLevelDataApplyType )
{
	IDnSkillProcessor* pResult = NULL;

	int iNumProcessor = (int)m_vlpProcessors[ iLevelDataApplyType ].size();
	for( int iProcessor = 0; iProcessor < iNumProcessor; ++iProcessor )
	{
		IDnSkillProcessor* pSkillProcessor = m_vlpProcessors[ iLevelDataApplyType ].at( iProcessor );
		if( iType == pSkillProcessor->GetType() )
		{
			pResult = pSkillProcessor;
			break;
		}
	}

	return pResult;
}


void CDnSkill::CheckTargetCount( int iTargetActorCount )
{
	// 일단은 힐링 에이리어에서만 쓰이게 될 것임..
	IDnSkillProcessor* pProcessor = GetProcessor( IDnSkillProcessor::DIVIDE_STATE_EFFECT_ARG );
	
	if( pProcessor )
	{
		// 브레스 오브 가데스 스킬 테이블에서는 타겟으로만 설정하고 여기서 본인의 상태효과를 추가하게 합니다.
		_ASSERT( "아직 이 부분은 브레스 오브 가데스에서만 쓰입니다..." && m_iSkillID == 3004 );

		CDnDivideSEArgumentByTargets* pDivideSEProcessor = static_cast<CDnDivideSEArgumentByTargets*>(pProcessor);
		int iSEArgument = pDivideSEProcessor->GetStateEffectArgument();

		// 자기 자신까지 포함해서 하나 더 추가
		int iDividedValue = iSEArgument / (iTargetActorCount+1);

		int iNumStateEffect = (int)m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size();
		char acBuf[ 256 ];
		for( int iStateEffect = 0; iStateEffect < iNumStateEffect; ++iStateEffect )
		{
			ZeroMemory( acBuf, sizeof(acBuf) );
			StateEffectStruct& StateEffect = m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].at( iStateEffect );
			itoa( iDividedValue, acBuf, 10 );
			StateEffect.szValue.assign( acBuf );

			// 자기 자신에게도 추가
			int iID = m_hActor->CmdAddStateEffect( /*m_hActor,*/ GetInfo(), (STATE_BLOW::emBLOW_INDEX)StateEffect.nID, 
												   StateEffect.nDurationTime, StateEffect.szValue.c_str() );
		}
	}

#if defined(PRE_ADD_50903)
	SetHitCountForVarianceDamage(iTargetActorCount);
#endif // PRE_ADD_50903
}



void CDnSkill::CheckAndAddSelfStateEffect( void )
{
	map<int, bool> mapDuplicateResult;		// 자기 자신에게 거는 상태효과는 따로 확률 중첩 처리 하지 않는다.
	CanApply eResult = CDnSkill::CanApplySkillStateEffect( m_hActor, GetMySmartPtr(), mapDuplicateResult );

	if( CanApply::Fail != eResult )
	{
		int nID(-1);
		StateEffectStruct *pStruct(NULL);

		// Note 한기: 2009.12.21
		// 자신에게 사용하는 효과 적용 시간 관련 (#1911)
		// 자기 자신에게 사용하는 상태효과 적용 시그널을 찾아서 갯수를 지정한다.
		// 현재 패시브 스킬의 경우엔 playaniprocessor 를 설정하지 않기 때문에 이 부분에서 걸러진다. 
		// 자기 자신에게 사용하는 상태효과 타이밍 시그널을 사용할 수 없음.
		// 따라서 추후에 패시브로 사용하는 액션 스킬들도 상태효과 타이밍 시그널이 필요하다면
		// 추가적인 작업이 필요하다.
		bool bUseApplySelfStateBlowSignal = false;
		CDnPlayAniProcess* pPlayAniProcessor = static_cast<CDnPlayAniProcess*>(GetProcessor( IDnSkillProcessor::PLAY_ANI ));
		if( pPlayAniProcessor )
		{
			const char* pActionName = pPlayAniProcessor->GetActionName();

			CEtActionBase::ActionElementStruct* pActionElement = m_hActor->GetElement( pActionName );
			_ASSERT( pActionElement );
			if( pActionElement )
			{
				int iApplyStateEffectSignalCount = 0;
				int iNumSignal = (int)pActionElement->pVecSignalList.size();
				for( int iSignal = 0; iSignal < iNumSignal; ++iSignal )
				{
					CEtActionSignal* pSignal = pActionElement->pVecSignalList.at( iSignal );
					if( STE_ApplyStateEffect == pSignal->GetSignalIndex() )
					{
						bUseApplySelfStateBlowSignal = true;
						++iApplyStateEffectSignalCount;
					}
				}
			}
		}

		for( DWORD i=0; i<m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size(); i++ ) 
		{
			pStruct = &m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ][ i ];
			if( !pStruct ) 
				continue;
			
			// 발현타입에서 직접 상태효과 추가를 컨트롤 하는 경우가 있음. (세이크리드 해머링)
			if( pStruct->bApplyInProcessor )
				continue;

			if( pStruct->ApplyType == ApplySelf )
			{
				// 액션이 끝날 시점에 종료되는 자기 자신에게 사용하는 상태효과.
				if( 0 == pStruct->nDurationTime || -1 == pStruct->nDurationTime  )
				{
					// 테이블에선 0으로 하나 -1로 하나 프로그램쪽에선 -1로 기준시킴
					pStruct->nDurationTime = -1;

					// 한기 2009.7.27 
					// 자신에게 사용하는 효과 적용 시간 관련 (#1911)
					// 스킬 강화 문장으로 추가된 상태효과들은 원래 상태효과에 있던 상태효과들이 아니므로 
					// 자기 자신에게 거는 상태효과 타이밍 시그널을 무시하고 곧바로 추가.
					if( bUseApplySelfStateBlowSignal && (0 == pStruct->nGlyphID) )
					{
						StateEffectStruct InstantSelfStateEffect = *pStruct;
						InstantSelfStateEffect.nDurationTime = pStruct->nDurationTime;
						m_hActor->AddStateEffectQueue( *GetInfo(), InstantSelfStateEffect );

						SetAddStateEffectQueue(true);
					}
					else
					{
						// [2010/12/13 semozz]
						// 기존 패킷 없이 추가 되는 상태 효과들 처리를 위해 직접 생성하고 m_vlApplyNoPacketStateEffectList에 담아 놓는데,
						// 패킷으로 생성하고, m_vlApplyNoPacketStateEffectList에 담아 놓는다.

						// #32160 바뀐 스킬 중첩 처리 루틴이 아이템의 스킬에서도 적용되도록 처리.
						// 아이템 뿐만 아니라 ApplyStateEffect 시그널 안 붙어있는 스킬은 여기서 바로 상태효과 추가 되므로 빼야될 상태효과들 뺀다.
						m_hActor->RemoveResetStateBlow();

						nID = m_hActor->CmdAddStateEffect( GetInfo(), (STATE_BLOW::emBLOW_INDEX)pStruct->nID, pStruct->nDurationTime, pStruct->szValue.c_str() );
						
						if( -1 == nID ) 
							continue;

						// Note 한기: 물약 같은 거 빨고 나서 상태효과 1프레임 돌고 적용될 시에 데미지 받아서 죽으면 클라엔 
						// 먹은 거 적용되고 서버엔 죽으면서 상태효과 전부 비워서 적용안 될 수가 있다.. HP 동기 틀어질 수도 있음.
						if( m_bItemSkill )
						{
							DnBlowHandle hBlow = m_hActor->GetStateBlowFromID( nID );
							hBlow->OnBegin( 0, 0.0f );

#ifndef _GAMESERVER
							if( hBlow->IsUseTableDefinedGraphicEffect() )
								hBlow->AttachGraphicEffectDefaultType();
#endif
							m_hActor->OnBeginStateBlow( hBlow );

							hBlow->SetState( STATE_BLOW::STATE_DURATION );
						}

						m_vlApplySelfNoDurationStateEffectList.push_back( nID );
					}
				}
				else
				{
					// 지속 시간이 있는 self 상태효과
					if( bUseApplySelfStateBlowSignal && (0 == pStruct->nGlyphID) )
					{
						m_hActor->AddStateEffectQueue( *GetInfo(), *pStruct );
						SetAddStateEffectQueue(true);
					}
					else
					{
						// #32160 바뀐 스킬 중첩 처리 루틴이 아이템의 스킬에서도 적용되도록 처리.
						// 아이템 뿐만 아니라 ApplyStateEffect 시그널 안 붙어있는 스킬은 여기서 바로 상태효과 추가 되므로 빼야될 상태효과들 뺀다.
						m_hActor->RemoveResetStateBlow();

						nID = m_hActor->CmdAddStateEffect( /*m_hActor,*/ GetInfo(), (STATE_BLOW::emBLOW_INDEX)pStruct->nID, 
															   pStruct->nDurationTime, pStruct->szValue.c_str() );
						if( -1 == nID )
							continue;
					}
				}
			}
		}
	}
}



void CDnSkill::CheckAndRemoveInstantApplySelfStateEffect( void )
{
	for( DWORD i=0; i<m_vlApplySelfNoDurationStateEffectList.size(); i++ ) 
	{
		if( m_hActor )
			m_hActor->CmdRemoveStateEffectFromID( m_vlApplySelfNoDurationStateEffectList[i] );
	}

	if( m_hActor )
	{
		// #40768 아이템 스킬은 MASkillUser::m_hItemSkill 로 MASkillUser::m_hProcessSkill 과 따로 놀고 있으므로
		// m_hItemSkill 이 끝나지 않더라도 m_hProcessSkill 이 시작될 수 있으므로 그 상태에서 
		// m_hItemSkill 이 종료되어 이 함수가 호출되면 m_hProcessSkill 에 등록된 BlowQueue 를 날려버리게 된다.
		// 따라서 아이템 스킬은 구분해서 처리해주도록 한다.

		//#40760
		//스킬 사용 시점에 접미사 스킬 발동될때 기존 스킬에서 등록된 상태효과가
		//접미사 스킬 끝나는 시점에서 아래 함수 호출로 인해서 큐가 비워저 버리는 경우가 발생.
		//AddStateEffectQueue호출 시 SetAddStateEffectQueue(true)로 플래그 설정 해 놓고
		//플래그가 설정되어 있는 스킬만 상태효과 큐를 초기화 하도록 한다..

		if ( IsAddStateEffectQueue() )
			m_hActor->ClearSelfStateSignalBlowQueue( IsItemSkill() );
	}

	SAFE_DELETE_VEC( m_vlApplySelfNoDurationStateEffectList );
}


DWORD CDnSkill::GetStateEffectCount( void )
{
	return (DWORD)m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size();
}


CDnSkill::StateEffectStruct* CDnSkill::GetStateEffectFromIndex( DWORD dwIndex )
{
	if( dwIndex >= m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size() ) return NULL;
	return &m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ][ dwIndex ];
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

CDnSkill::SkillTypeEnum CDnSkill::GetSkillType( int nSkillID )
{
	if (nSkillID <= 0) return SkillTypeEnum::Active;

	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	if( pSkillTable->IsExistItem( nSkillID ) == false ) return SkillTypeEnum::Active;
	return (SkillTypeEnum)pSkillTable->GetFieldFromLablePtr( nSkillID, "_SkillType" )->GetInteger();
}


bool CDnSkill::IsUseActionNames( const set<string>& setUseActionNames )
{
	list<string> listIntersect;
	set_intersection( setUseActionNames.begin(), setUseActionNames.end(), 
					  m_setUseActionNames.begin(), m_setUseActionNames.end(), 
					  back_inserter(listIntersect) );

	return !listIntersect.empty();
}

void CDnSkill::_RefreshDecreaseMP( int iSkillLevelDataApplyType )
{
	if( 0.0f == m_fMPConsumeType[ iSkillLevelDataApplyType ] )
	{
		if( !m_hActor || m_hActor->IsMonsterActor() ) {
			m_iNeedMP[ iSkillLevelDataApplyType ] = m_iDecreaseMP[ iSkillLevelDataApplyType ];
			return;
		}

		if( m_hActor->IsPlayerActor() )
		{
#ifndef _ADD_NEW_MPCONSUME
			float fRatio = 1.f;
			if( m_hActor->GetLevel() > 0 )
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
	}
	else
	{
		m_iNeedMP[ iSkillLevelDataApplyType ] = int((float)m_hActor->GetMaxSP() * m_fMPConsumeType[ iSkillLevelDataApplyType ]);
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


void CDnSkill::SetPassiveSkillActionName( const char* pActionName )
{
	if( pActionName )
	{
		// 같은 것이 오면 자동으로 추가 되지 않음.
		m_setUseActionNames.insert( string(pActionName) );
	}
}


// 이슈 #6190 관련.
CDnSkill::CanApply CDnSkill::CanApplySkillStateEffect( const SkillInfo* pUsingSkillInfo, DnActorHandle hTargetActor, int iSkillID, int iLevel, 
													   int iSkillDuplicateMethod, int iDuplicateCount, map<int, bool>& mapDuplicateResult, bool isHitProcess )
{
	// 상태효과 검사할때 리셋리스트 초기화 [2010/12/09 semozz]
	if (hTargetActor)
		hTargetActor->InitStateBlowIDToRemove();

	CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;
	bool bExistingSameSkill = false;
	int iExistingSameSkillCount = 0;

	// 중복되어 같은 저렙 스킬의 상태효과가 제거될 시에 담아뒀다가 제거함.
	// 다른 스킬의 같은 상태효과가 걸려있는 경우도 있으므로 반드시 상태효과 생성시 부여받은 id 로 삭제한다.

	vector<int> vlStateBlowIDToRemove;
	DNVector( DnBlowHandle ) vlhSameSkillBlows;

	int iNumAppliedStateEffect = (int)hTargetActor->GetNumAppliedStateBlow();
	for( int iAppliedStateEffect = 0; iAppliedStateEffect < iNumAppliedStateEffect; ++iAppliedStateEffect )
	{
		DnBlowHandle hExistingBlow = hTargetActor->GetAppliedStateBlow( iAppliedStateEffect );

		const CDnSkill::SkillInfo* pExistingParentSkillInfo = NULL;
		if( hExistingBlow )
			pExistingParentSkillInfo = hExistingBlow->GetParentSkillInfo();

		// 모체 스킬 정보가 없는 상태효과는 스킬로 걸린 것이 아니므로 여기서 다룰 대상이 아님
		if( NULL == pExistingParentSkillInfo )
			continue;

		// 스킬 중복 카운트 체크
		// 같은 스킬 아이디에 동일한 상태효과가 또 나타난다면 중복된 스킬임.
		bool bIsSameSkillID = false;
#if defined(PRE_FIX_58505)
		//접미사 스킬 타입이 설정 되어 있지 않으면 스킬 아이디로 비교하고,
		//접미사 스킬 타입이 설정 되어 있으면 접미사 스킬 타입이 같은지 비교.
		if ( pUsingSkillInfo->bItemPrefixSkill == true && pUsingSkillInfo->nPrefixSkillType != CDnPrefixSkill::Prefix_Non )
			bIsSameSkillID = pUsingSkillInfo->nPrefixSkillType == pExistingParentSkillInfo->nPrefixSkillType;
		else
			bIsSameSkillID = iSkillID == pExistingParentSkillInfo->iSkillID;
#else
		if( iSkillID == pExistingParentSkillInfo->iSkillID )
			bIsSameSkillID = true;
#endif // PRE_FIX_58505

		if (bIsSameSkillID == true)
		{
			// 같은 스킬 아이디가 처음 나온다면 처음엔 적용중 스킬 카운트 하나 올려줌.

			// #27335 - 앵클샷 경우 [2011/01/11 semozz]
			// 상대방이 나에게 앵크샷을 쏘고 CantMove상태효과가 적용 되어 있을때
			// 내가 같은 앵클샷을 쏠때, 내 자신에 걸린 CantMove상태효과가 스킬ID가 같아서
			// 같은 스킬 중복 처리에 걸리게 됨..
			// 현재 상태효과의 ApplyType을 확인해서
			// 현재 상태효과를 건 Actor와 TargetActor를 비교해서
			// 유효한 Actor가 사용한게 아니면 같은 스킬이 아님으로 설정
			
			DnActorHandle hSkillUserActor = pUsingSkillInfo->hSkillUser;
			DnSkillHandle hSkill;
			if (hSkillUserActor)
				hSkill = hSkillUserActor->FindSkill( iSkillID );
			
			//Prop에의한 상태효과는 스킬 정보가 없을 수 있다.
			if (hSkill)
			{
				// 기존에 적용 중인, 지금 사용하려는 스킬의 상태효과 중 하나의 적용 대상 타입.
				StateEffectApplyType eExistingSEApplyType = ApplySelf;

				bExistingSameSkill = true;
				for( int i = 0; i < (int)hSkill->GetStateEffectCount(); ++i )
				{
					const StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex( i );
					if( pSE->nID == hExistingBlow->GetBlowIndex()
						&& pSE->ApplyType == pExistingParentSkillInfo->eApplyType )
					{
						eExistingSEApplyType = pSE->ApplyType;
						break;
					}
				}

				//현재 적용된 상태효과의 applyType에 따라
				switch(eExistingSEApplyType)
				{
				case StateEffectApplyType::ApplySelf:
					if (isHitProcess == false)
						bExistingSameSkill = (hTargetActor == hSkillUserActor);
					else
						bExistingSameSkill = false;
					break;
				case StateEffectApplyType::ApplyEnemy:
					bExistingSameSkill = (hTargetActor->GetTeam() != hSkillUserActor->GetTeam());
					break;
				case StateEffectApplyType::ApplyTarget:
					bExistingSameSkill = (hTargetActor != hSkillUserActor);
					break;
				case StateEffectApplyType::ApplyFriend:
					bExistingSameSkill = (hTargetActor->GetTeam() == hSkillUserActor->GetTeam());
					break;
				case StateEffectApplyType::ApplyAll:
					bExistingSameSkill = true;
					break;
				}
			}
			else
				bExistingSameSkill = true;
			
			if( bExistingSameSkill )
			{
				iExistingSameSkillCount += hExistingBlow->GetDuplicateCount();
				vlhSameSkillBlows.push_back( hExistingBlow );
			}
		}

		if (pUsingSkillInfo->eTargetType == All  //지금 사용 하는 스킬의 TargetType이 All이고, 
			&& (0 != iSkillDuplicateMethod && (iSkillDuplicateMethod == pExistingParentSkillInfo->iSkillDuplicateMethod)) //중첩 처리ID가 설정되어 있고, 기존 스킬과 동일한 ID일때,
			&& (hTargetActor == pUsingSkillInfo->hSkillUser) //자기 자신
			)
			continue;

		// 지속효과 구분 인덱스가 0이 아니라면 스킬 효과 중복 관련 처리가 필요함.
		// 지속효과 구분 인덱스가 0이면 그냥 중첩됨.
		// 혹은 같은 스킬이면서 최대 중첩 카운트가 1이하인경우엔 스킬 대체되도록 처리.
#if defined(PRE_FIX_58505)
		if( 0 != iSkillDuplicateMethod ||
			(bIsSameSkillID == true && iDuplicateCount <= 1) )
#else
		if( 0 != iSkillDuplicateMethod ||
			(iSkillID == pExistingParentSkillInfo->iSkillID && iDuplicateCount <= 1) )
#endif // PRE_FIX_58505
		{
			// 기존에 실행되고 있던 스킬과 같은 지속효과 인덱스인가. 그렇다면 중첩이 가능한지 체크들어간다.
			// 또한 스킬을 쓴 유저의 팀이 같을때만 중첩처리를 하도록 한다.
			// 다른 팀인데 같은 스킬을 쓸 경우 나에게 걸린 디버프 상태효과가 해제되어 버린다. (#19812)
			if( (iSkillDuplicateMethod == pExistingParentSkillInfo->iSkillDuplicateMethod) &&
				(pUsingSkillInfo->iSkillUserTeam == pExistingParentSkillInfo->iSkillUserTeam) )
			{
				if( iLevel >= pExistingParentSkillInfo->iLevel )
				{
					// 242번 상태효과는 한번 추가가 되면 리셋 되면 안된다.

					// 확률 체크에서 통과한다면 리셋시킬 상태효과에 넣어둠.
					// 실제 확률체크하는 객체는 기존에 적용된 상태효과 객체를 사용하지만 확률만 체크하는 것이기 때문에
					// 그대로 사용한다.
					if( hExistingBlow->CanBegin() )
					{
						bool isSelfStateEffect = false;

						//#60966 자신에게 적용하는 상태효과와 다른이에게 적용 하는 상태효과가 같이 있은 경우
						//스킬 시작에서는 상관 없지만, Hit처리시에는 self상태효과는 다시 적용 되지 않는다.
						//그래서 히트 처리되는 중에 여기서 self적용된 상태효과를 제거 리스트에 추가 해 놓으면
						//다시 추가 할 방법이 없다.
						//히트 처리시에 들어 온경우 Self적용 상태효과는 스킵..
						if (isHitProcess == true)
						{
							StateEffectApplyType eExistingSEApplyType = ApplySelf;

							bool bCheck = false;

							DnActorHandle hExistingSkillUserActor = pExistingParentSkillInfo->hSkillUser;
							DnSkillHandle hSkill;
							if (hExistingSkillUserActor)
								hSkill = hExistingSkillUserActor->FindSkill( pExistingParentSkillInfo->iSkillID );

							if (hSkill)
							{
								for( int i = 0; i < (int)hSkill->GetStateEffectCount(); ++i )
								{
									const StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex( i );
									if( pSE->nID == hExistingBlow->GetBlowIndex() 
										&& pSE->ApplyType == pExistingParentSkillInfo->eApplyType )
									{
										bCheck = true;
										eExistingSEApplyType = pSE->ApplyType;
										break;
									}
								}

								//자신에게 거는 상태효과는 적용 가능 하도록? 한다..
								if ( StateEffectApplyType::ApplySelf == eExistingSEApplyType && bCheck )
								{
									isSelfStateEffect = true;
								}
							}

						}

						if (isSelfStateEffect == false &&
							hExistingBlow->GetBlowIndex() != STATE_BLOW::BLOW_242 )
						{
							vlStateBlowIDToRemove.push_back( hExistingBlow->GetBlowID() );
							mapDuplicateResult[ hExistingBlow->GetBlowIndex() ] = true;

							// 리셋되어야할 상태들을 담아 놓는다. [2010/12/08 semozz]
							// 여기서 리스트에 담긴 상태들은 OnSignal의 STE_ApplyStateEffect 시점에 상태 적용할때
							// 기존의 상태를 제거 한다.
							hTargetActor->AddStateBlowIDToRemove( hExistingBlow->GetBlowID() );
						}
					}
					else
					{
						eResult = CDnSkill::CanApply::Fail;		// 효과 갱신 시 확률 체크에서 실패했으므로 추가하지 않는다.
						mapDuplicateResult[ hExistingBlow->GetBlowIndex() ] = false;
					}
				}
				else
				if( iLevel < pExistingParentSkillInfo->iLevel )
				{
					// 효과 적용 안됨.
					eResult = CDnSkill::CanApply::Fail;

					// Note 한기: 추후에 그래픽적으로 뭔가 표시해줘야 한다면 이 곳에서 처리하면 됨.

					// [2011/01/20 semozz]
					// 체인라이트닝 시전이 끝나기 전 헤븐스 저지먼트를 사용시 문제점.
					// A 스킬 레벨이 높고, 그 뒤에 사용되는 B스킬의 레벨이 낮을때, DuplicationMethod가 같으면
					// 여기서 스킬 추가 안됨으로 Self상태효과 적용이 되지 않음. Target은 상관 없을듯..

					// 현재 상태효과를 사용한 액터를 선택
					DnActorHandle hExistingSkillUserActor = pExistingParentSkillInfo->hSkillUser;
					DnSkillHandle hSkill;
					if (hExistingSkillUserActor)
						hSkill = hExistingSkillUserActor->FindSkill( pExistingParentSkillInfo->iSkillID );

					if (hSkill)
					{
						// 기존에 적용 중인, 지금 사용하려는 스킬의 상태효과 중 하나의 적용 대상 타입.
						StateEffectApplyType eExistingSEApplyType = ApplySelf;

						for( int i = 0; i < (int)hSkill->GetStateEffectCount(); ++i )
						{
							const StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex( i );
							if( pSE->nID == hExistingBlow->GetBlowIndex() 
								&& pSE->ApplyType == pExistingParentSkillInfo->eApplyType )
							{
								eExistingSEApplyType = pSE->ApplyType;
								break;
							}
						}

						//자신에게 거는 상태효과는 적용 가능 하도록? 한다..
						if (StateEffectApplyType::ApplySelf == eExistingSEApplyType)
						{
							eResult = CDnSkill::CanApply::Apply;

							vlStateBlowIDToRemove.push_back( hExistingBlow->GetBlowID() );
							mapDuplicateResult[ hExistingBlow->GetBlowIndex() ] = true;

							hTargetActor->AddStateBlowIDToRemove( hExistingBlow->GetBlowID() );
						}
					}
				}
			}
		}
	}

	if( false == vlStateBlowIDToRemove.empty() )
	{
		// 스킬 효과 대체. 중첩 카운트 세었던 것 처리할 필요 없다.
		bExistingSameSkill = false;
		eResult = CDnSkill::CanApply::ApplyDuplicateSameSkill;
	}

	// 상태 효과를 중첩해서 적용해야하는 경우, 최대 중첩 갯수가 넘으면 안된다.
	// 높은 레벨의 상태효과가 이미 적용중이라면 중첩처리가 되지 않는다.
	if( CDnSkill::CanApply::Fail != eResult )
	{
		if( bExistingSameSkill )
		{
			if( iDuplicateCount <= iExistingSameSkillCount )
			{
				// 최대 중첩 갯수를 넘는 경우 기존 상태효과의 시간만 초기화 한다.
				int iNumSameSkillBlowsToResetDurationTime = (int)vlhSameSkillBlows.size();
				for( int iBlow = 0; iBlow < iNumSameSkillBlowsToResetDurationTime; ++iBlow )
				{
					DnBlowHandle hBlow = vlhSameSkillBlows.at( iBlow );

					//////////////////////////////////////////////////////////////////////////
					//#53448
					//249번 상태효과 경우 중첩 갯수 이상은 추가 될 수 없다.
					//최대 중첩 갯수를 넘은 경우 아래 루프를 돌면서 기존 상태효과 제거 하고, 다시 추가 작업 필요 없음.
					if (hBlow && 
						(hBlow->GetBlowIndex() == STATE_BLOW::BLOW_249 || hBlow->GetBlowIndex() == STATE_BLOW::BLOW_242)
						)
						continue;
					//////////////////////////////////////////////////////////////////////////

					// 아예 삭제했다가 새로 추가하는 것이 명확함.
					int nDurationTime = int(hBlow->GetDurationTime() * 1000.0f);
					hBlow->ResetDurationTime();

					CDnSkill::SkillInfo SkillInfo = *(hBlow->GetParentSkillInfo());
					STATE_BLOW::emBLOW_INDEX BlowIndex = hBlow->GetBlowIndex();
					string strValue( hBlow->GetValue() );

					// 이 시점에서 즉시 지워저야 한다. [2011/01/18 semozz]
					int nBlowID = hBlow->GetBlowID();
					// 패킷 보낼꺼 보내고
					hTargetActor->SendRemoveStateEffectFromID(nBlowID);
					// 바로 삭제한다..
					if (hTargetActor->GetStateBlow())
						hTargetActor->GetStateBlow()->RemoveImediatlyStateEffectFromID(nBlowID);

					// 여기에 결빙같은 확률 있는 상태효과를 100% 성공시켜야 하므로 
					// CanBegin 함수 호출하지 않도록 bCheckCanBegin 플래그를 꺼서 호출.
					hTargetActor->CmdAddStateEffect( &SkillInfo, BlowIndex, nDurationTime, strValue.c_str(), false, false );
				}

				eResult = CDnSkill::CanApply::Fail;
			}
		}
	}

	return eResult;
}


CDnSkill::CanApply CDnSkill::CanApplySkillStateEffect( DnActorHandle hTargetActor, DnSkillHandle hUsingSkill, map<int, bool>& mapDuplicateResult, bool isHitProcess )
{
	const SkillInfo* pUsingSkillInfo = hUsingSkill->GetInfo();
	int iSkillID = hUsingSkill->GetClassID();
	int iLevel = hUsingSkill->GetLevel();
	int iSkillDuplicateMethod = hUsingSkill->GetDuplicateMethod();
	int iDuplicateCount = hUsingSkill->GetMaxDuplicateCount();

	// 스킬에서 상태 효과들중에 지속시간이 하나라도 있으면 지속시간이 있는걸로 판단
	// 해당 스킬이 지속시간이 없다면 그냥 추가 될 수 있도록 한다. [2010/11/12 semozz]
	bool hasDuration = false;
	for( DWORD k = 0; k < hUsingSkill->GetStateEffectCount(); k++ ) 
	{
		CDnSkill::StateEffectStruct *pLocalStruct = hUsingSkill->GetStateEffectFromIndex(k);
		if( pLocalStruct && (IsNeedCheckApplyStateBlow((STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID) || pLocalStruct->nDurationTime > 0) )
		{
			hasDuration = true;
			break;
		}
#if defined(PRE_FIX_52267)
		//#52267
		//상태효과 ApplyType이 Self가 아니어도 지속시간이 0인 경우 CanApplySkillStateEffect함수 호출이 되지 않아서
		//의도치 않게 중첩이 되는 경우가 발생한다.
		//ApplyType이 self가 아니고, durationTime이 0인경우도 체크 가능 하도록 hasDuration을 true로 설정 한다.
		else if (pLocalStruct->nDurationTime == 0 && pLocalStruct->ApplyType != StateEffectApplyType::ApplySelf)
		{
			hasDuration = true;
			break;
		}
#endif // PRE_FIX_52267
	}

	bool bResist = false;
	if( !hTargetActor->IsHittableSkill( iSkillID , bResist ) )
	{
		if( bResist )
		{
			hTargetActor->SendAddSEFail( CDnStateBlow::ADD_FAIL_BY_IMMUNE , STATE_BLOW::BLOW_154 );
		}

		return  CDnSkill::CanApply::Fail;
	}

	if ( false == hasDuration )
	{
		return CDnSkill::CanApply::Apply;
	}

	return CanApplySkillStateEffect( pUsingSkillInfo, hTargetActor, iSkillID, iLevel, iSkillDuplicateMethod, iDuplicateCount, mapDuplicateResult, isHitProcess );
}


// 액션 재생 프로세서가 반드시 있는 액티브 스킬의 동작 계승 요청!
void CDnSkill::OnChainInput( const char* pActionName )
{
	if( GetSkillType() == CDnSkill::Active &&
		( GetDurationType() == CDnSkill::Instantly ||
		  GetDurationType() == CDnSkill::Buff ||
		  GetDurationType() == CDnSkill::Debuff || 
		  GetDurationType() == CDnSkill::StanceChange ) )
	{
		IDnSkillProcessor* pProcessor = GetProcessor( IDnSkillProcessor::PLAY_ANI );
		_ASSERT( pProcessor );
		if( pProcessor )
		{
			CDnPlayAniProcess* pPlayAniProcessor = static_cast<CDnPlayAniProcess*>( pProcessor );
			pPlayAniProcessor->OnChainInput( pActionName );
		}
	}
	else
	if( GetSkillType() == CDnSkill::Passive )
	{
		CEtActionBase::ActionElementStruct* pStruct = m_hActor->GetElement( pActionName );
		if( pStruct )
		{
			m_fPassiveActionSkillLength += (float)pStruct->dwLength / s_fDefaultFps;
			m_bChainingPassiveSkill = true;
		}
	}
}

bool CDnSkill::IsChainInputAction( const char* pActionName )
{
	IDnSkillProcessor* pProcessor = GetProcessor( IDnSkillProcessor::PLAY_ANI );
	if( pProcessor )
	{
		CDnPlayAniProcess* pPlayAniProcessor = static_cast<CDnPlayAniProcess*>( pProcessor );
		if( strcmp( pActionName , pPlayAniProcessor->GetChainActionName() ) == 0 )
		{
			return true;
		}
	}

	return false;
}

bool CDnSkill::CheckChainingPassiveSkill( void )
{
	bool bResult = m_bChainingPassiveSkill;
	m_bChainingPassiveSkill = false;

	return bResult;
}

void CDnSkill::AddGlyphStateEffect( int nGlyphID )
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
			{
				StateEffect.ApplyType = StateEffectApplyType::ApplySelf;
				StateEffect.bApplyAllPair = true;
			}
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

	// 스킬 속성 추가
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
			m_fResetCooltime += GlyphEffect.fEffectValue;
			break;
		}

	case SKILLDURATION_PRO :
		{
			// pair<int, int> <상태효과, 기간>
			for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
			{
				bool bEmpty = m_vlStateDurationList[iSkillLevelApplyType].empty();
				for( int itr = 0; itr < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++itr ) {
					if( bEmpty )
						m_vlStateDurationList[iSkillLevelApplyType].push_back( make_pair(m_vlStateEffectList[ iSkillLevelApplyType ][itr].nID, m_vlStateEffectList[ iSkillLevelApplyType ][itr].nDurationTime) );
					else m_vlStateEffectList[ iSkillLevelApplyType ][itr].nDurationTime = m_vlStateDurationList[iSkillLevelApplyType][itr].second;
				}
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
	case SKILLATTACKHEAL_ABSOLUTE:
		{
			float fValue = GlyphEffect.fEffectValue;

			for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
			{
				for( int itr = 0; itr < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++itr )
				{
					if( m_vlStateEffectList[ iSkillLevelApplyType ][itr].nID == STATE_BLOW::BLOW_248 )
					{
						m_vlStateEffectList[ iSkillLevelApplyType ][itr].szValue = FormatA( "%f", fValue + atof(m_vlStateEffectList[ iSkillLevelApplyType ][itr].szValue.c_str()) ).c_str();
					}
				}
			}
		}
		break;
	}

	m_vGlyphEffectList.push_back( GlyphEffect );
}

void CDnSkill::DelGlyphStateEffect( int nGlyphID )
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
		for( DNVector(StateEffectStruct)::iterator Itor = m_vlStateEffectList[ iSkillLevelApplyType ].begin(); 
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
					m_fResetCooltime -= Itor->fEffectValue;
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
			case SKILLATTACKHEAL_ABSOLUTE:
				{
					float fValue = Itor->fEffectValue;
					for( int iSkillLevelApplyType = PVE; iSkillLevelApplyType < NUM_SKILLLEVEL_APPLY_TYPE; ++iSkillLevelApplyType )
					{
						for( int itr = 0; itr < (int)m_vlStateEffectList[ iSkillLevelApplyType ].size(); ++itr )
						{
							if( m_vlStateEffectList[ iSkillLevelApplyType ][itr].nID == STATE_BLOW::BLOW_248 )
								m_vlStateEffectList[ iSkillLevelApplyType ][itr].szValue = FormatA( "%f", atof(m_vlStateEffectList[ iSkillLevelApplyType ][itr].szValue.c_str()) - fValue).c_str();
						}
					}
				}
				break;
			}

			Itor = m_vGlyphEffectList.erase( Itor );
		}
		else
			++Itor;
	}
}

void CDnSkill::UpdateSkillCoolTimeExactly( void )
{
	// 96, 171번 쿨타임 변경 상태효과가 걸려있는 경우 서버에서 체크하지 않도록 처리.
	// 겜서버에서 쿨타임 갱신이 10프레임으로 줄어들었으므로 조작감 문제때문에 이렇게 풀어놓음. 2011.07.25
	DWORD dwNowTime = timeGetTime();
	if(	DWORD(GetDelayTime()*1000.0f) < dwNowTime - m_dwLastUseSkillTimeStamp )
	{
		ResetCoolTime();
	}
}


bool CDnSkill::SelectLevelDataType( int iSkillLevelDataType, bool bPlayerSummonedMonster/* = false*/ )
{ 
	_ASSERT( (m_hActor->IsPlayerActor() || bPlayerSummonedMonster) && (PVE <= iSkillLevelDataType) && (iSkillLevelDataType < NUM_SKILLLEVEL_APPLY_TYPE) );

	if( (m_hActor->IsPlayerActor() || bPlayerSummonedMonster) &&
		(PVE <= iSkillLevelDataType) &&
		(iSkillLevelDataType < NUM_SKILLLEVEL_APPLY_TYPE) ) 
	{
		m_iSelectedSkillLevelDataApplyType = iSkillLevelDataType;
		return true;
	}

	return false;
};


bool CDnSkill::IsNeedCheckApplyStateBlow( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	bool bResult = false;

	if( STATE_BLOW::BLOW_030 == emBlowIndex )
		bResult = true;

	return bResult;
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

// 클라이언트도 동기화 맞추어주세요 CheckAndDivideStateEffectArgument 이 함수도 수정해주세요 
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
			m_SkillInfo[iSkillLevelApplyType].eSkillElement = hEnchantPassiveSkill->GetElement();

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
			string enchantEffectIDToClient = hEnchantPassiveSkill->GetEffectOutputIDToClient(iSkillLevelApplyType);

			m_BackupEffectOutputIDs[iSkillLevelApplyType] = m_SkillInfo[iSkillLevelApplyType].szEffectOutputIDs;
			m_BackupEffectOutputIDToClient[iSkillLevelApplyType] = m_SkillInfo[iSkillLevelApplyType].szEffectOutputIDToClient;

			//SC_CMDADDSTATEEFFECT 패킷을 클라이언트로 보낼때, szEffectOutputIDToClient값이 없으면 클라이언트는 테이블 값을 읽어서 사용한다.
			//EX스킬을 사용 시 스킬 ID는 베이스 스킬 ID가 전달 되기 때문에, EX스킬에의해 변경된 값을 사용 하지 못하게 됨.
			//eX스킬 적용시 szEffectOutputIDToClient값을 EX스킬 값으로 변경 해놓는다.
			m_SkillInfo[iSkillLevelApplyType].szEffectOutputIDs = enchantEffectIDs;
			m_SkillInfo[iSkillLevelApplyType].szEffectOutputIDToClient = enchantEffectIDs;

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


void CDnSkill::ApplyEnchantSkillOnceFromBubble( DnSkillHandle hEnchantPassiveSkill )
{
	m_bEnchantedFromBubble = true;
	ApplyEnchantSkill( hEnchantPassiveSkill );
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
			DNVector( StateEffectStruct )::iterator iter = m_vlStateEffectList[ iSkillLevelApplyType ].begin();
			for( iter; iter != m_vlStateEffectList[ iSkillLevelApplyType ].end(); )
			{
				if( false == CheckAndDivideStateEffectArgument( &(*iter) ) )
				{
					if( 0 < iter->nFromEnchantPassiveSkillID )
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
			m_SkillInfo[iSkillLevelApplyType].szEffectOutputIDs = m_BackupEffectOutputIDs[iSkillLevelApplyType];
			m_SkillInfo[iSkillLevelApplyType].szEffectOutputIDToClient = m_BackupEffectOutputIDToClient[iSkillLevelApplyType];

			m_BackupEffectOutputIDs[iSkillLevelApplyType].clear();
			m_BackupEffectOutputIDToClient[iSkillLevelApplyType].clear();
		}

		SelectLevelDataType( iNowLevelDataType );
		m_iAppliedEnchantPassiveSkillID = 0;
	}
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

bool CDnSkill::IsExistUsableChecker( int iType ) // 정의된 UsableChecker을 가지고있느냐?
{
	int iNumChecker = (int)m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].size();
	for( int iChecker = 0; iChecker < iNumChecker; ++iChecker )
	{
		IDnSkillUsableChecker* pSkillChecker = m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].at( iChecker );
		if( iType == pSkillChecker->GetType() )
			return true;
	}
	return false;
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
		float fMaxMP = static_cast<float>(m_hActor->GetMaxSP());
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

void CDnSkill::AddPrefixBlow(DnActorHandle hActor, int nBlowID)
{
	PREFIX_SKILL_BLOWLIST::iterator findIter = m_PrefixBlowList.find(hActor);
	if (findIter != m_PrefixBlowList.end())
	{
		findIter->second.push_back(nBlowID);
	}
	else
	{
		BLOWLIST blowList;
		blowList.push_back(nBlowID);

		m_PrefixBlowList.insert(PREFIX_SKILL_BLOWLIST::value_type(hActor, blowList));
	}
}

void CDnSkill::RemovePrefixBlow()
{
	PREFIX_SKILL_BLOWLIST::iterator iter = m_PrefixBlowList.begin();
	PREFIX_SKILL_BLOWLIST::iterator endIter = m_PrefixBlowList.end();

	for (; iter != endIter; ++iter)
	{
		DnActorHandle hActor = iter->first;
		if (!hActor)
			continue;

		BLOWLIST& blowList = iter->second;
		for (int i = 0; i < (int)blowList.size(); ++i)
		{
			// [2010/12/13 semozz]
			// 이제는 모든 상태효과는 패킷으로 생성 되므로, 패킷으로 상태효과 제거한다.
			hActor->CmdRemoveStateEffectFromID( blowList[i] );
		}

		blowList.clear();
	}

	m_PrefixBlowList.clear();
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
	
}
DnActorHandle CDnSkill::FindSummonMonster(int nMonsterID)
{
	DnActorHandle hSummonMonster;

	if (m_hActor && m_hActor->IsPlayerActor())
	{
		const list<DnMonsterActorHandle>& listSummonMonster = m_hActor->GetSummonedMonsterList();

		if( false == listSummonMonster.empty() )
		{
			list<DnMonsterActorHandle>::const_iterator iter = listSummonMonster.begin();
			for( iter; iter != listSummonMonster.end(); ++iter )
			{
				DnMonsterActorHandle hMonster = (*iter);
				if( hMonster && hMonster->GetMonsterClassID() == nMonsterID )
				{
					hSummonMonster = hMonster;
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
		hSummonMonster->CmdSuicide(false, false);
		return true;
	}
	else
		return false;
}

bool CDnSkill::IsSkipStateBlow(const char* szSkipStateBlows, STATE_BLOW::emBLOW_INDEX blowIndex)
{
	if (szSkipStateBlows == NULL)
		return false;

	std::string stringValue = szSkipStateBlows;
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	TokenizeA(stringValue, tokens, delimiters);

	for (UINT i = 0; i < tokens.size(); ++i)
	{
		STATE_BLOW::emBLOW_INDEX skipStateBlowIndex = (STATE_BLOW::emBLOW_INDEX)(atoi(tokens[i].c_str()));
		if (skipStateBlowIndex == blowIndex)
			return true;
	}

	return false;
}

void CDnSkill::AddProjectile(CDnProjectile* pProjectile)
{
	if (pProjectile)
	{
		LOCAL_TIME startTime = pProjectile->GetSkillStartTime();

		SKILL_PROJECTILE_LIST::iterator findIter = m_ProjectileList.find(startTime);
		
		if (findIter == m_ProjectileList.end())
		{
			PROJECTILE_LIST projectileList;
			projectileList.push_back(pProjectile);

			m_ProjectileList.insert(std::make_pair(startTime, projectileList));
		}
		else
		{
			findIter->second.push_back(pProjectile);
		}
	}
}

void CDnSkill::RemoveProjectile(CDnProjectile* pProjectile)
{
	if (pProjectile == NULL)
		return;

	LOCAL_TIME startTime = pProjectile->GetSkillStartTime();

	SKILL_PROJECTILE_LIST::iterator findIter = m_ProjectileList.find(startTime);
	if (findIter != m_ProjectileList.end())
	{
		PROJECTILE_LIST::iterator iter = findIter->second.begin();
		for ( ; iter != findIter->second.end(); ++iter)
		{
			CDnProjectile* pTemp = *iter;
			if (pTemp == pProjectile)
			{
				iter = findIter->second.erase(iter);

				//리스트가 비었다면 메인 리스트에서 제거한다.
				if (findIter->second.empty() == true)
					m_ProjectileList.erase(findIter);
				
				break;
			}
		}
	}
	
}

int CDnSkill::GetProjectileCount(LOCAL_TIME startTime)
{
	int nListCount = 0;

	SKILL_PROJECTILE_LIST::iterator findIter = m_ProjectileList.find(startTime);
	if (findIter != m_ProjectileList.end())
		nListCount = (int)findIter->second.size();
	
	return nListCount;
}

void CDnSkill::ApplyAddtionalStateInfo()
{
	if( !m_hActor ) return;

	//#52905 [칼리]"중첩 추가 효과"
	//상태효과가 적용 되어 있을때 조건에 맞는 상태효과가 적용 되어 있으면 스킬의 상태효과에 추가 한다..
	if( m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_253) )
	{
		DNVector(DnBlowHandle) vlBlows;
		m_hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_253, vlBlows);
		int nListCount = (int)vlBlows.size();

		for (int iIndex = 0; iIndex < nListCount; ++iIndex)
		{
			DnBlowHandle hBlow = vlBlows[iIndex];
			if (hBlow && hBlow->IsEnd() == false)
			{
				CDnAdditionalStateInfoBlow* pAddtionalStateInfoBlow = static_cast<CDnAdditionalStateInfoBlow*>(hBlow.GetPointer());
				if (pAddtionalStateInfoBlow)
				{
					STATE_BLOW::emBLOW_INDEX DestBlowIndex = pAddtionalStateInfoBlow->GetDestStateIndex();
					CDnSkill::StateEffectStruct addStateInfo = pAddtionalStateInfoBlow->GetTargetStateInfo();

					int iNumStateBlow = (int)m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].size();
					for( int i = 0; i < iNumStateBlow; ++i )
					{
						const StateEffectStruct& SE = m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].at( i );
						if( DestBlowIndex == SE.nID)
						{
							addStateInfo.nDurationTime = SE.nDurationTime;	//지속 시간은 조건 상태효과 시간으로 설정한다.

							//추가할 상태효과 리스트를 만든다.
							m_AddtionalStateInfoList.push_back(addStateInfo);

							OutputDebug("추가 상태효과 적용됨.... %d, %s\n", addStateInfo.nID, addStateInfo.szValue.c_str());
						}
					}
				}
			}
		}

		//추가할 상태효과 리스트 순회 하면서 실제 스킬 상태효과 리스트에 추가 한다.
		int nAddStateCount = (int)m_AddtionalStateInfoList.size();
		for (int i = 0; i < nAddStateCount; ++i)
		{
			CDnSkill::StateEffectStruct addStateEffect = m_AddtionalStateInfoList[i];
			 m_vlStateEffectList[m_iSelectedSkillLevelDataApplyType].push_back(addStateEffect);
		}
	}

	if( m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_276) )
	{
		DNVector(DnBlowHandle) vlBlows;
		m_hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_276, vlBlows);
		
		for( DWORD i=0; i<vlBlows.size(); i++ )
		{
			if( vlBlows[i] )
			{
				CDnAddStateBySkillGroupBlow* pAddStateBySkillGroupBlow = static_cast<CDnAddStateBySkillGroupBlow*>(vlBlows[i].GetPointer());
				if( pAddStateBySkillGroupBlow ) pAddStateBySkillGroupBlow->ApplyAddtionalStateBlowFromSkill( GetClassID() );
			}
		}
	}
}

void CDnSkill::RemoveAddtionalStateInfo()
{
	//스킬 사용시 추가 상태효과 추가된 것들 제거...
	int nListCount = (int)m_AddtionalStateInfoList.size();
	for (int iIndex = 0; iIndex < nListCount; ++iIndex)
	{
		StateEffectStruct addStateInfo = m_AddtionalStateInfoList[iIndex];

		std::vector<StateEffectStruct>::iterator iter = m_vlStateEffectList[ m_iSelectedSkillLevelDataApplyType ].begin();
		for( ; iter != m_vlStateEffectList[m_iSelectedSkillLevelDataApplyType].end(); ++iter)
		{
			StateEffectStruct SE = (*iter);

			if (SE.nID == addStateInfo.nID &&
				SE.ApplyType == addStateInfo.ApplyType &&
				/*SE.nDurationTime == addStateInfo.nDurationTime &&*/	//지속시간은 가변...
				SE.bAddtionalStateInfo == addStateInfo.bAddtionalStateInfo)
			{
				
				iter = m_vlStateEffectList[m_iSelectedSkillLevelDataApplyType].erase(iter);
				break;
			}			
		}
	}

	m_AddtionalStateInfoList.clear();


	if( m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_276) )
	{
		DNVector(DnBlowHandle) vlBlows;
		m_hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_276, vlBlows);

		for( DWORD i=0; i<vlBlows.size(); i++ )
		{
			if( vlBlows[i] )
			{
				CDnAddStateBySkillGroupBlow* pAddStateBySkillGroupBlow = static_cast<CDnAddStateBySkillGroupBlow*>(vlBlows[i].GetPointer());
				if( pAddStateBySkillGroupBlow )	pAddStateBySkillGroupBlow->RemoveAdditionalStateBlow();
			}
		}
	}
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

	if( 0.0f < m_fAnotherGlobalSkillCoolTime )
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


#if defined(_GAMESERVER)
bool CoolTimeInfo::Process(LOCAL_TIME localTime, float fDelta)
{
	switch( m_eDurationType )
	{
	case CDnSkill::DurationTypeEnum::Instantly:
	case CDnSkill::DurationTypeEnum::Buff:
	case CDnSkill::DurationTypeEnum::Debuff:
	case CDnSkill::DurationTypeEnum::SummonOnOff:
	case CDnSkill::DurationTypeEnum::StanceChange:
		{
			if( m_fLeftDelayTime == 0.0f ) return false;

			m_fLeftDelayTime -= fDelta;
			if( m_fLeftDelayTime < 0.f ) 
				m_fLeftDelayTime = 0.f;

			m_fCoolTime = ( 1.0f / (m_fDelayTime*m_fCoolTimeAdjustBlowValue) ) * m_fLeftDelayTime;
		}
		break;

	case CDnSkill::DurationTypeEnum::TimeToggle:
		break;
	case CDnSkill::DurationTypeEnum::ActiveToggle:
	case CDnSkill::DurationTypeEnum::ActiveToggleForSummon:
		break;
	case CDnSkill::DurationTypeEnum::Aura:
		break;
	}

	return (m_fCoolTime != 0.0f && m_fLeftDelayTime > 0.0f);
}

void CoolTimeInfo::OnBeginCoolTime()
{
	m_fCoolTime = (m_fDelayTime == 0.f) ? 0.0f : 1.0f;
	m_fCoolTimeAdjustBlowValue = 1.0f;

	m_fLeftDelayTime = m_fDelayTime * m_fCoolTimeAdjustBlowValue;
}

void CoolTimeInfo::SetInfo(DnSkillHandle &hSkill)
{
	m_nSkillID = hSkill->GetClassID();
	m_hActor = hSkill->GetActor();
	m_eDurationType = hSkill->GetDurationType();

	m_fCoolTimeAdjustBlowValue = 1.0f;

	m_fDelayTime = hSkill->GetDelayTime();
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CoolTimeInfo::SetInfo(CDnPrefixSkill* pPrefixSkill)
{
	m_nSkillID = pPrefixSkill->GetSkillType();

	DnSkillHandle hSkill = pPrefixSkill->GetSkillHandle();
	if (hSkill)
	{
		m_hActor = hSkill->GetActor();
		m_eDurationType = hSkill->GetDurationType();

		m_fCoolTimeAdjustBlowValue = 1.0f;

		m_fDelayTime = hSkill->GetDelayTime();
	}
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


bool CoolTimeManager::IsCoolTime(int nSkillID)
{
	COOLTIME_LIST::iterator findIter = m_CoolTimeList.find(nSkillID);

	//아직 등록되어 있다면 쿨타임이 끝나지 않았다??
	return (findIter != m_CoolTimeList.end());
}

void CoolTimeManager::AddCoolTime(DnSkillHandle hSkill)
{
	if (!hSkill)
		return;

	int nSkillID = hSkill->GetClassID();
	COOLTIME_LIST::iterator findIter = m_CoolTimeList.find(nSkillID);
	//같은 스킬 ID가 없을때만 등록한다.
	if (findIter == m_CoolTimeList.end())
	{
		CoolTimeInfo *pCoolTimeInfo = new CoolTimeInfo();
		pCoolTimeInfo->SetInfo(hSkill);
		pCoolTimeInfo->OnBeginCoolTime();

		m_CoolTimeList.insert(COOLTIME_LIST::value_type(nSkillID, pCoolTimeInfo));
	}
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CoolTimeManager::AddCoolTime(CDnPrefixSkill* pPrefixSkill)
{
	if (!pPrefixSkill)
		return;

	int nSkillType = pPrefixSkill->GetSkillType();
	COOLTIME_LIST::iterator findIter = m_CoolTimeList.find(nSkillType);
	//같은 스킬 ID가 없을때만 등록한다.
	if (findIter == m_CoolTimeList.end())
	{
		CoolTimeInfo *pCoolTimeInfo = new CoolTimeInfo();
		pCoolTimeInfo->SetInfo(pPrefixSkill);
		pCoolTimeInfo->OnBeginCoolTime();

		m_CoolTimeList.insert(COOLTIME_LIST::value_type(nSkillType, pCoolTimeInfo));
	}
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

void CoolTimeManager::RemoveCoolTime(int nSkillID)
{
	COOLTIME_LIST::iterator findIter = m_CoolTimeList.find(nSkillID);
	if (findIter != m_CoolTimeList.end())
	{
		CoolTimeInfo* pCoolTimeInfo = findIter->second;
		if (pCoolTimeInfo)
			delete pCoolTimeInfo;

		m_CoolTimeList.erase(findIter);
	}
}

void CoolTimeManager::Process(LOCAL_TIME localTime, float fDelta)
{
	COOLTIME_LIST::iterator iter = m_CoolTimeList.begin();
	COOLTIME_LIST::iterator endIter = m_CoolTimeList.end();

	std::list<int> removeCoolTimeList;

	for (; iter != endIter; ++iter)
	{
		if (!iter->second->Process(localTime, fDelta))
			removeCoolTimeList.push_back(iter->first);			
	}

	if (!removeCoolTimeList.empty())
	{
		std::list<int>::iterator iter = removeCoolTimeList.begin();
		std::list<int>::iterator endIter = removeCoolTimeList.end();

		for (; iter != endIter; ++iter)
		{
			RemoveCoolTime((*iter));
		}

		removeCoolTimeList.clear();
	}
}

void CoolTimeManager::InitList()
{
	COOLTIME_LIST::iterator iter = m_CoolTimeList.begin();
	COOLTIME_LIST::iterator endIter = m_CoolTimeList.end();

	for (; iter != endIter; ++iter)
	{
		CoolTimeInfo* pCoolTimeInfo = iter->second;
		if (pCoolTimeInfo)
			delete pCoolTimeInfo;
	}

	m_CoolTimeList.clear();
}
#endif // _GAMESERVER


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)

CDnPrefixSkill::CDnPrefixSkill(int nPrefixType)
{
	m_nPrefixType = nPrefixType;

	m_fProbability = 0.0f;
}

CDnPrefixSkill::~CDnPrefixSkill()
{

}

void CDnPrefixSkill::SetSkillHandle(DnSkillHandle hSkill)
{
	m_hSkill = hSkill;
}

void CDnPrefixSkill::UpdateCandidateSkill()
{
	std::list<DnSkillHandle>::iterator iter = m_SkillList.begin();
	std::list<DnSkillHandle>::iterator endIter = m_SkillList.end();

	float fDelayTime = FLT_MAX;

	DnSkillHandle hSelectedSkill;
	for (; iter != endIter; ++iter)
	{
		DnSkillHandle hSkill = *iter;
		if (!hSkill)
			continue;

		float fSkillDelayTime = hSkill->GetDelayTime();
		if (fSkillDelayTime < fDelayTime)
		{
			hSelectedSkill = hSkill;
			fDelayTime = fSkillDelayTime;
		}
	}

	if (hSelectedSkill)
		SetSkillHandle(hSelectedSkill);
}

void CDnPrefixSkill::AddSkill(DnSkillHandle hSkill)
{
	if (!hSkill) return;

	m_SkillList.push_back(hSkill);

	//해당 스킬의 확률값을 가져온다.
	CDnProbabilityChecker* pProbabilityChecker = static_cast<CDnProbabilityChecker*>(hSkill->GetChecker(IDnSkillUsableChecker::PROB_CHECKER));
	m_fProbability += pProbabilityChecker ? pProbabilityChecker->GetProbability() : 0.0f;

	UpdateCandidateSkill();

	AddStateEffectInfo(hSkill);
}

void CDnPrefixSkill::RemoveSkill(DnSkillHandle hSkill)
{
	std::list<DnSkillHandle>::iterator iter = m_SkillList.begin();
	std::list<DnSkillHandle>::iterator endIter = m_SkillList.end();

	for (; iter != endIter; ++iter)
	{
		if ((*iter) == hSkill)
		{
			//해당 스킬의 확률값을 가져온다.
			CDnProbabilityChecker* pProbabilityChecker = static_cast<CDnProbabilityChecker*>(hSkill->GetChecker(IDnSkillUsableChecker::PROB_CHECKER));
			m_fProbability -= pProbabilityChecker ? pProbabilityChecker->GetProbability() : 0.0f;
			m_SkillList.erase(iter);
			break;
		}
	}

	UpdateCandidateSkill();

	//스킬 제거 될때 스킬 리스트 돌면서 상태효과 다시 적용하도록..
	UpdateStateEffects();
}

void CDnPrefixSkill::UpdateStateEffects()
{
	//1.기존 상태효과 설정 값을 제거..
	m_vlStateEffectList.clear();

	//스킬 리스트 돌면서 상태효과 값 적용.
	std::list<DnSkillHandle>::iterator iter = m_SkillList.begin();
	std::list<DnSkillHandle>::iterator endIter = m_SkillList.end();

	for (; iter != endIter; ++iter)
	{
		DnSkillHandle hSkill = (*iter);
		if (hSkill)
		{
			AddStateEffectInfo(hSkill);
		}
	}
}

void CDnPrefixSkill::AddStateEffectInfo(DnSkillHandle hSkill)
{
	if (m_vlStateEffectList.empty())
	{
		//hSkill의 상태효과 설정값을 그대로 리스트에 추가 한다.
		int nStateEffectCount = hSkill->GetStateEffectCount();
		for (int i = 0; i < nStateEffectCount; ++i)
		{
			CDnSkill::StateEffectStruct* pAddStateEffect = hSkill->GetStateEffectFromIndex(i);

			if (pAddStateEffect)
				m_vlStateEffectList.push_back(*pAddStateEffect);
		}
	}
	else
	{
		//상태효과 갯수가 맞지 않으면 추가 못함..
		if (m_vlStateEffectList.size() != hSkill->GetStateEffectCount())
			return;

		switch(m_nPrefixType)
		{
		case Prefix_000: //파괴의
			AddStateEffectInfo_Prefix_000(hSkill);
			break;
		case Prefix_001: //마법의
			AddStateEffectInfo_Prefix_001(hSkill);
			break;
		case Prefix_002: //곰의 무기
			AddStateEffectInfo_Prefix_002(hSkill);
			break;
		case Prefix_003: //바람의 무기
			AddStateEffectInfo_Prefix_003(hSkill);
			break;
		case Prefix_004: //지혜의 무기
			AddStateEffectInfo_Prefix_004(hSkill);
			break;
		case Prefix_005: //가혹한
			AddStateEffectInfo_Prefix_005(hSkill);
			break;
		case Prefix_006: //어둠의 무기
			AddStateEffectInfo_Prefix_006(hSkill);
			break;
		case Prefix_007: //생명의 무기
			AddStateEffectInfo_Prefix_007(hSkill);
			break;
		case Prefix_008: //마나의 무기
			AddStateEffectInfo_Prefix_008(hSkill);
			break;
		case Prefix_009: //활기의
			AddStateEffectInfo_Prefix_009(hSkill);
			break;
		case Prefix_010: //치명적인
			AddStateEffectInfo_Prefix_010(hSkill);
			break;
		case Prefix_011: //구속의
			AddStateEffectInfo_Prefix_011(hSkill);
			break;
		case Prefix_012: //충격의
			AddStateEffectInfo_Prefix_012(hSkill);
			break;
		case Prefix_013: //용자의
			AddStateEffectInfo_Prefix_013(hSkill);
			break;
		case Prefix_014: //기사의
			AddStateEffectInfo_Prefix_014(hSkill);
			break;
		case Prefix_015: //철벽의 무기
			AddStateEffectInfo_Prefix_015(hSkill);
			break;
		case Prefix_016: //장막의 무기
			AddStateEffectInfo_Prefix_016(hSkill);
			break;
		case Prefix_017: //불의 무기
			AddStateEffectInfo_Prefix_017(hSkill);
			break;
		case Prefix_018: //물의 무기
			AddStateEffectInfo_Prefix_018(hSkill);
			break;
		case Prefix_019: //빛의 무기
			AddStateEffectInfo_Prefix_019(hSkill);
			break;
		case Prefix_020: //철벽의 방어구
			AddStateEffectInfo_Prefix_020(hSkill);
			break;
		case Prefix_021: //장막의 방어구
			AddStateEffectInfo_Prefix_021(hSkill);
			break;
		case Prefix_022: //곰의 방어구
			AddStateEffectInfo_Prefix_022(hSkill);
			break;
		case Prefix_023: //바람의 방어구
			AddStateEffectInfo_Prefix_023(hSkill);
			break;
		case Prefix_024: //지혜의 방어구
			AddStateEffectInfo_Prefix_024(hSkill);
			break;
		case Prefix_025: //건강한
			AddStateEffectInfo_Prefix_025(hSkill);
			break;
		case Prefix_026: //행운의
			AddStateEffectInfo_Prefix_026(hSkill);
			break;
		case Prefix_027: //생명의 방어구
			AddStateEffectInfo_Prefix_027(hSkill);
			break;
		case Prefix_028: //마나의 방어구
			AddStateEffectInfo_Prefix_028(hSkill);
			break;
// 		case Prefix_029: //견고한
// 			AddStateEffectInfo_Prefix_029(hSkill);
// 			break;
		case Prefix_030: //유연한
			AddStateEffectInfo_Prefix_030(hSkill);
			break;
		case Prefix_031: //의지의
			AddStateEffectInfo_Prefix_031(hSkill);
			break;
// 		case Prefix_032: //강인한
// 			AddStateEffectInfo_Prefix_032(hSkill);
// 			break;
// 		case Prefix_033: //불굴의
// 			AddStateEffectInfo_Prefix_033(hSkill);
// 			break;
		case Prefix_034: //불의 방어구
			AddStateEffectInfo_Prefix_034(hSkill);
			break;
		case Prefix_035: //물의 방어구
			AddStateEffectInfo_Prefix_035(hSkill);
			break;
		case Prefix_036: //빛의 방어구
			AddStateEffectInfo_Prefix_036(hSkill);
			break;
		case Prefix_037: //어둠의 방어구
			AddStateEffectInfo_Prefix_037(hSkill);
			break;

		default:
			break;

		}		
	}
}

void CDnPrefixSkill::AddStateEffectInfo( STATE_BLOW::emBLOW_INDEX blowList[], int nCount, DnSkillHandle hSkill )
{
	if (!hSkill)
		return;

	int nAddStateEffectCount = hSkill->GetStateEffectCount();

	//상태효과 갯수가 4개
	if (nAddStateEffectCount != nCount)
	{
		OutputDebug("접두사 스킬 Type[%d]의 상태효과 갯수가 다름...!!\n", m_nPrefixType);
		return;
	}

	for (int i = 0; i < nCount; ++i)
	{
		CDnSkill::StateEffectStruct* pAddStateEffect = hSkill->GetStateEffectFromIndex(i);
		CDnSkill::StateEffectStruct* pOrigStateEffect = &m_vlStateEffectList[i];

		//상태효과 Index가 BLOW_213이 아니면 잘못된거다..
		if ((STATE_BLOW::emBLOW_INDEX)pOrigStateEffect->nID != blowList[i])
		{
			OutputDebug("상태효과 Index가 다름..%d이어야 하는데 %d이네...!!!\n", blowList[i], pOrigStateEffect->nID);
			continue;
		}

		//두개 상태효과 Index가 다르면..
		if (pAddStateEffect->nID != pOrigStateEffect->nID)
		{
			OutputDebug("두 상태효과 Index가 다름..Orig[%d], Add[%d]..!!!\n", pOrigStateEffect->nID, pAddStateEffect->nID);
			continue;
		}

		std::string szNewValue = "";
		CDnCreateBlow::AddStateEffectValue((STATE_BLOW::emBLOW_INDEX)pOrigStateEffect->nID, pOrigStateEffect->szValue.c_str(), pAddStateEffect->szValue.c_str(), szNewValue);
		
		OutputDebug("원래 상태효과 설정값(%s), 추가 되는 상태효과 설정값(%s), 최종 결과(%s)\n", pOrigStateEffect->szValue.c_str(), pAddStateEffect->szValue.c_str(), szNewValue.c_str());

		pOrigStateEffect->szValue = szNewValue;
		//지속 시간은 큰 값으로 변경.
		pOrigStateEffect->nDurationTime = max(pOrigStateEffect->nDurationTime, pAddStateEffect->nDurationTime);
	}
}


void CDnPrefixSkill::AddStateEffectInfo_Prefix_000(DnSkillHandle hSkill) //파괴의
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_213,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}

void CDnPrefixSkill::AddStateEffectInfo_Prefix_001(DnSkillHandle hSkill) //마법의
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_214,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}

void CDnPrefixSkill::AddStateEffectInfo_Prefix_002(DnSkillHandle hSkill) //곰의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_221,
		STATE_BLOW::BLOW_124,
		STATE_BLOW::BLOW_128,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}

void CDnPrefixSkill::AddStateEffectInfo_Prefix_003(DnSkillHandle hSkill) //바람의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_221,
		STATE_BLOW::BLOW_126,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_004(DnSkillHandle hSkill) //지혜의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_214,
		STATE_BLOW::BLOW_036,
		STATE_BLOW::BLOW_037,
		STATE_BLOW::BLOW_038,
		STATE_BLOW::BLOW_039,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_005(DnSkillHandle hSkill) //가혹한
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_157,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_006(DnSkillHandle hSkill) //어둠의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_035,
		STATE_BLOW::BLOW_182,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_007(DnSkillHandle hSkill) //생명의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_016,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_008(DnSkillHandle hSkill) //마나의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_018,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_009(DnSkillHandle hSkill) //활기의
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_014,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_010(DnSkillHandle hSkill) //치명적인
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_158,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_011(DnSkillHandle hSkill) //구속의
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_070,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_012(DnSkillHandle hSkill) //충격의
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_221,
		STATE_BLOW::BLOW_175,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_013(DnSkillHandle hSkill) //용자의
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_079,
	};
	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_014(DnSkillHandle hSkill) //기사의
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_076,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_015(DnSkillHandle hSkill) //철벽의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_004,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_016(DnSkillHandle hSkill) //장막의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_094,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_017(DnSkillHandle hSkill) //불의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_032,
		STATE_BLOW::BLOW_182,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_018(DnSkillHandle hSkill) //물의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_033,
		STATE_BLOW::BLOW_182,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_019(DnSkillHandle hSkill) //빛의 무기
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_034,
		STATE_BLOW::BLOW_182,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_020(DnSkillHandle hSkill) //철벽의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_134,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);;
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_021(DnSkillHandle hSkill) //장막의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_135,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_022(DnSkillHandle hSkill) //곰의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_124,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_023(DnSkillHandle hSkill) //바람의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_126,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_024(DnSkillHandle hSkill) //지혜의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_036,
		STATE_BLOW::BLOW_037,
		STATE_BLOW::BLOW_038,
		STATE_BLOW::BLOW_039,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_025(DnSkillHandle hSkill) //건강한
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_124,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_026(DnSkillHandle hSkill) //행운의
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_111,
		STATE_BLOW::BLOW_112,
		STATE_BLOW::BLOW_113,
		STATE_BLOW::BLOW_114,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_027(DnSkillHandle hSkill) //생명의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_016,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_028(DnSkillHandle hSkill) //마나의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_018,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_029(DnSkillHandle hSkill) //견고한
{
// 	STATE_BLOW::emBLOW_INDEX blowList[] = 
// 	{
// 	};
// 
// 	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);
// 
// 	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_030(DnSkillHandle hSkill) //유연한
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_076,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_031(DnSkillHandle hSkill) //의지의
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_156,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_032(DnSkillHandle hSkill) //강인한
{
// 	STATE_BLOW::emBLOW_INDEX blowList[] = 
// 	{
// 	};
// 
// 	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);
// 
// 	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_033(DnSkillHandle hSkill) //불굴의
{
// 	STATE_BLOW::emBLOW_INDEX blowList[] = 
// 	{
// 	};
// 
// 	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);
// 
// 	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_034(DnSkillHandle hSkill) //불의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_036,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_035(DnSkillHandle hSkill) //물의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_037,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_036(DnSkillHandle hSkill) //빛의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_038,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
void CDnPrefixSkill::AddStateEffectInfo_Prefix_037(DnSkillHandle hSkill) //어둠의 방어구
{
	STATE_BLOW::emBLOW_INDEX blowList[] = 
	{
		STATE_BLOW::BLOW_039,
	};

	int nCount = sizeof(blowList) / sizeof(STATE_BLOW::emBLOW_INDEX);

	AddStateEffectInfo(blowList, nCount, hSkill);
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

void CDnSkill::OnAnotherGlobalSkillBeginCoolTime( DnSkillHandle hSkill )
{
	if( hSkill )
	{
		m_fAnotherGlobalSkillCoolTime = hSkill->GetDelayTime();

		m_nAnotherGlobakSkillID = hSkill->GetClassID();
	}
}