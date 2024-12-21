#include "StdAfx.h"
#include "DnCreateBlow.h"
#include "DnBasicBlow.h"
#include "DnParryBlow.h"
#include "DnRebirthBlow.h"
#include "DnConcentrationBlow.h"
#include "DnFrameBlow.h"
#include "DnHPIncBlow.h"
#include "DnMPIncBlow.h"
#include "DnCantMoveBlow.h"
#include "DnCantActionBlow.h"
#include "DnDissolveDebufBlow.h"
#include "DnDissolveBufBlow.h"
#include "DnFreezingBlow.h"
#include "DnForceActionBlow.h"
#include "DnElectricShockBlow.h"
#include "DnAbContinueHPIncBlow.h"
#include "DnAbContinueMPIncBlow.h"
#include "DnClericMentalChargeBlow.h"
#include "DnFireBurnBlow.h"
#include "DnPoisonBlow.h"
#include "DnSleepBlow.h"
#include "DnBlockBlow.h"
#include "DnImmuneBlow.h"
#include "DnCantUseActiveSkillBlow.h"
#include "DnMaximumAttackBlow.h"
#include "DnInvincibleBlow.h"
#include "DnInvisibleBlow.h"
#include "DnAttackerHPAbsolute.h"
#include "DnAttackerHPRatio.h"
#include "DnReverseTeamBlow.h"
#include "DnRtContinueHPIncBlow.h"
#include "DnRtContinueMPIncBlow.h"
#include "DnNULLBlow.h"
#include "DnChainAttackBlow.h"
#include "DnCantUseItemBlow.h"
#include "DnCantUseMPUsingSkillBlow.h"
#include "DnChangeWeaponRangeBlow.h"
#include "DnAbMeleePDefenseBlow.h"
#include "DnRtMeleePDefenseBlow.h"
#include "DnAbRangePDefenseBlow.h"
#include "DnRtRangePDefenseBlow.h"
#include "DnAbMeleeMDefenseBlow.h"
#include "DnRtMeleeMDefenseBlow.h"
#include "DnAbRangeMDefenseBlow.h"
#include "DnRtRangeMDefenseBlow.h"
#include "DnChangeStandActionBlow.h"
#include "DnParryingProbBlow.h"
#include "DnCoolTimeBlow.h"
#include "DnUseHitterPAttackBlow.h"
#include "DnUseHitterMAttackBlow.h"
#include "DnMeleePReflectDamageBlow.h"
#include "DnMeleeMReflectDamageBlow.h"
#include "DnRangePReflectDamageBlow.h"
#include "DnRangeMReflectDamageBlow.h"
#include "DnPayBackMPBlow.h"
#include "DnPoisonExplosionBlow.h"
#include "DnFireBurnExplosionBlow.h"
#include "DnFreezingExplosionBlow.h"
#include "DnElectricExplosionBlow.h"
#include "DnChargerBlow.h"
#include "DnChangeActionSetBlow.h"
#include "DnCantAttackBlow.h"
#include "DnAggroBlow.h"
#include "DnChangeWeightBlow.h"
#include "DnProvocationBlow.h"
#include "DnForceStunBlow.h"
#include "DnChangePAttackDamageBlow.h"
#include "DnChangeMAttackDamageBlow.h"
#include "DnPlagueBlow.h"
#include "DnUsingSkillWhenDieBlow.h"
#include "DnIgnoreCanHitBlow.h"
#include "DnChangeMPConsumeRatioBlow.h"
#include "DnHealingBlow.h"
#include "DnChangeHealingRatioBlow.h"
#include "DnSuperArmorAttackRatioBlow.h"
#include "DnHighLanderBlow.h"
#include "DnAggroResetBlow.h"
#include "DnFrameStopBlow.h"
#include "DnFreezingPrisonBlow.h"
#include "DnAllImmuneBlow.h"
#include "DnCameraRoarBlow.h"
#include "DnMouseSensitiveBlow.h"
#include "DnCooltimeParryBlow.h"
#include "DnFrostbiteBlow.h"
#include "DnSummonMonsterChangeSkillLevelBlow.h"
#include "DnDamageRatioBlow.h"
#include "DnInvincibleAtBlow.h"
#include "DnDissolveStateBlow.h"
#include "DnSuperAmmorShield.h"
#include "DnDOTBlow.h"
#include "DnDamageDuplicateBlow.h"
#include "DnPushBlow.h"
#include "DnChangeElementBlow.h"
#include "DnAddDamageOnCriticalBlow.h"
#include "DnAddDamageOnStateBlow.h"
#include "DnAddStateOnConditionBlow.h"
#include "DnBoneCrushBlow.h"
#include "DnBreakSuperArmorBlow.h"
#include "DnCoolTimeChangeBlow.h"
#include "DnFinishAttackBlow.h"
#include "DnFreezingShieldBlow.h"
#include "DnIntelligenceToPhysicAttackBlow.h"
#include "DnMaxDamageAddBlow.h"
#include "DnShieldBlow.h"
#include "DnShockInfectionBlow.h"
#include "DnStealBuffBlow.h"
#include "DnStrengthToMagicAttackBlow.h"
#include "DnTimeBoomBlow.h"
#include "DnDamageBlow.h"
#include "DnAddStateOnHitBlow.h"
#include "DnPhysicalAttackToMagicalAttackBlow.h"
#include "DnActionChangeOnHitBlow.h"
#include "DnFinalPDamageBlow.h"
#include "DnFinalMDamageBlow.h"
#include "DnTransformBlow.h"
#include "DnKillBlow.h"
#include "DnAllowedSkillsBlow.h"
#include "DnHolyWaterBlow.h"
#include "DnIgnorePrefixSkillBlow.h"
#include "DnSpectatorBlow.h"
#include "DnAddMinimapRadarBlow.h"
#include "DnForceOutBlow.h"
#include "DnTeleportBlow.h"
#include "DnActionChangeRatioBlow.h"
#include "DnDamageTransitionBlow.h"
#include "DnPingpongBlow.h"
#include "DnBasisAttackDmgUpBlow.h"
#include "DnRecallSummonBlow.h"
#include "DnRemoveStateBlow.h"
#include "DnAddStateOnTimer.h"
#include "DnAddAttackPDamageBlow.h"
#include "DnAddAttackMDamageBlow.h"
#include "DnOrderMySummonedMonsterBlow.h"
#include "DnDestroySuperArmorBlow.h"
#include "DnGhoulTransformBlow.h"
#include "DnUnstableHolyWaterBlow.h"
#include "DnEscapeBlow.h"
#include "DnAccelBlow.h"
#include "DnPrefixDamageBlow.h"
#include "DnBurnCriticalBlow.h" 
#include "DnMPBurstBlow.h"     
#include "DnDisableActionBlow.h"
#include "DnConfuseBlow.h"
#include "DnProbInvincibleAtBlow.h"
#include "DnBloodSuckingBlow.h"
#include "DnIgnoreHitSignalBlow.h"
#include "DnApplyStateBlowOnTimeBlow.h"
#include "DnContinueBaseMPIncBlow.h"
#include "DnVarianceDamageBlow.h"
#include "DnDisarmamentBlow.h"
#include "DnChangeWeaponBlow.h"
#include "DnInvincibleTypeBlow.h"
#include "DnImmuneByTypeBlow.h"
#include "DnAvoidDebufActionBlow.h"
#include "DnFlashBlow.h"
#include "DnComboDamageLimitBlow.h"
#include "DnCurseBlow.h"
#include "DnManaShieldBlow.h"
#include "DnCurseBreakDownBlow.h"
#include "DnRecoverHPByAttackBlow.h"
#include "DnTargetCriticalBlow.h"
#include "DnFinalCriticalBlow.h"
#include "DnStigmaBlow.h"
#include "DnTransmitDamageBlow.h"
#include "DnPuppetBlow.h"
#include "DnPileAddEffectBlow.h"
#include "DnIgnoreEffectBlow.h"
#include "DnAdditionalStateInfoBlow.h"
#include "DnForceAggroTargetBlow.h"
#include "DnTotalLevelSkillBlows.h"
#include "DnBasicStateBaseAddState.h"
#include "DnIncreaseItemRecoverHPBlow.h"
#include "DnIgnoreGravitySignalBlow.h"
#include "DnDamageLimitBlow.h"
#include "DnIgnoreRestoreHPBlow.h"
#include "DnAddStateBySkillGroupBlow.h"
#include "DnTransformMonsterBlow.h"
#include "DnPartyFinishAttackBlow.h"
#include "DnPartyAddDamageBlow.h"
#include "DnDragonShieldBlow.h"
#ifdef PRE_ADD_HPBELOWDOSKILLBLOW
#include "DnHPBelowUseSkillBlow.h"
#endif // PRE_ADD_HPBELOWDOSKILLBLOW
#include "DnMechanicModeBlow.h"
#include "DnDarkTransformationBlow.h"
#include "DnDisableSkillBlow.h"
#include "DnConsumeBubbleBlow.h"
#include "DnCheckBubbleCancelSkillBlow.h"
#include "DnEmptyBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



CDnCreateBlow::CDnCreateBlow(void)
{
}
  
CDnCreateBlow::~CDnCreateBlow(void)
{
}

DnBlowHandle CDnCreateBlow::CreateBlow( STATE_BLOW::emBLOW_INDEX emBlowIndex, DnActorHandle hActor, const char *szParam )
{
	ASSERT(hActor&&"CDnCreateBlow::CreateBlow, hActor is NULL!");
	if( !hActor ) return CDnBlow::Identity();

#ifndef PRE_MOD_MAGICALBREEZE_CHANGE_BLOW
	// 물리 공격력 관련 상태효과는 전부 마법 공격력으로 돌린다.
	STATE_BLOW::emBLOW_INDEX emOriginalBlowIndex = emBlowIndex;
	bool bChangedByMagicalBreeze = false;
	if( hActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_180 ) )
	{
		switch( emBlowIndex )
		{
			// 물리 공격력 절대값 변경 -> 마법 공격력 절대값 변경
			case STATE_BLOW::BLOW_001:
				emBlowIndex = STATE_BLOW::BLOW_028;
				bChangedByMagicalBreeze = true;
				break;

			// 물리 공격력 절대값 변경 -> 마법 공격력 절대값 변경
			case STATE_BLOW::BLOW_002:
				emBlowIndex = STATE_BLOW::BLOW_029;
				bChangedByMagicalBreeze = true;
				break;

			// 물리 공격력 절대값 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
			// ->마법 공격력 절대값 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
			case STATE_BLOW::BLOW_200:
				emBlowIndex = STATE_BLOW::BLOW_202;
				bChangedByMagicalBreeze = true;
				break;

			// 물리 공격력 비율 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
			// ->마법 공격력 비율 변경 상태효과. 스킬 상태효과 계산 되기 전에 먼저 계산됨.
			case STATE_BLOW::BLOW_201:
				emBlowIndex = STATE_BLOW::BLOW_203;
				bChangedByMagicalBreeze = true;
				break;
		}
	}
#endif // PRE_MOD_MAGICALBREEZE_CHANGE_BLOW

	CDnBlow *pBlow(NULL);
 
	if( IsBasicBlow(emBlowIndex) )
	{
		pBlow = new CDnBasicBlow(hActor, szParam);
		pBlow->SetBlow( emBlowIndex );
		return pBlow->GetMySmartPtr();
	}

	OutputDebug("%s   BlowID: %d szParam: %s ", __FUNCTION__, emBlowIndex, szParam);

	switch( emBlowIndex )
	{
		case STATE_BLOW::BLOW_011:	pBlow = new CDnAbContinueHPIncBlow(hActor,szParam);	break;
		case STATE_BLOW::BLOW_012:	pBlow = new CDnRtContinueHPIncBlow(hActor,szParam); break;
		case STATE_BLOW::BLOW_013:	pBlow = new CDnAbContinueMPIncBlow(hActor,szParam);	break;
		case STATE_BLOW::BLOW_014:	pBlow = new CDnRtContinueMPIncBlow(hActor,szParam); break;
		case STATE_BLOW::BLOW_016:	pBlow = new CDnHPIncBlow(hActor,szParam);	break;
		case STATE_BLOW::BLOW_018:	pBlow = new CDnMPIncBlow(hActor,szParam);	break;
		case STATE_BLOW::BLOW_025:	pBlow = new CDnFrameBlow(hActor,szParam);	break;
		case STATE_BLOW::BLOW_028:	break;
		case STATE_BLOW::BLOW_029:	break;
		case STATE_BLOW::BLOW_030:	pBlow = new CDnBlockBlow(hActor,szParam);	break;
		case STATE_BLOW::BLOW_031:	pBlow = new CDnParryBlow(hActor, szParam);	break;
		case STATE_BLOW::BLOW_040:	pBlow = new CDnConcentrationBlow(hActor,szParam);	break;
		case STATE_BLOW::BLOW_041:	pBlow = new CDnFreezingBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_042:	pBlow = new CDnFireBurnBlow( hActor, szParam );	break;
		case STATE_BLOW::BLOW_043:	pBlow = new CDnElectricShockBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_044:	pBlow = new CDnPoisonBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_045:	pBlow = new CDnSleepBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_050:  pBlow = new CDnDamageRatioBlow( hActor , szParam ); break;
		case STATE_BLOW::BLOW_056:	pBlow = new CDnParryingProbBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_057:	pBlow = new CDnRebirthBlow(hActor,szParam);	break;
		case STATE_BLOW::BLOW_060:	pBlow = new CDnChainAttackBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_062:	pBlow = new CDnChargerBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_065:	pBlow = new CDnChangeWeaponRangeBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_066:	pBlow = new CDnAggroBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_067:	pBlow = new CDnCantUseActiveSkillBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_068:	pBlow = new CDnPayBackMPBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_069:	pBlow = new CDnDissolveDebufBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_070:	pBlow = new CDnCantMoveBlow(hActor,szParam); break;
		case STATE_BLOW::BLOW_071:	pBlow = new CDnCantActionBlow(hActor, szParam);	break;
		case STATE_BLOW::BLOW_072:	pBlow = new CDnClericMentalChargeBlow(hActor, szParam);	break;
		case STATE_BLOW::BLOW_073:	pBlow = new CDnInvisibleBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_074:	pBlow = new CDnMaximumAttackBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_077:	pBlow = new CDnImmuneBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_078:	pBlow = new CDnCantUseMPUsingSkillBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_079:	pBlow = new CDnDestroySuperArmorBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_083:	pBlow = new CDnDissolveBufBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_084:	pBlow = new CDnForceActionBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_086:	pBlow = new CDnReverseTeamBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_091:	pBlow = new CDnAttackerHPAbsolute(hActor,szParam); break;
		case STATE_BLOW::BLOW_092:	pBlow = new CDnAttackerHPRatio(hActor, szParam); break;
		case STATE_BLOW::BLOW_096:	pBlow = new CDnCoolTimeBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_099:	pBlow = new CDnInvincibleBlow( hActor, szParam );	break;
		case STATE_BLOW::BLOW_100:	pBlow = new CDnNULLBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_101:	pBlow = new CDnAbMeleePDefenseBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_102:	pBlow = new CDnRtMeleePDefenseBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_103:	pBlow = new CDnAbRangePDefenseBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_104:	pBlow = new CDnRtRangePDefenseBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_105:	pBlow = new CDnAbMeleeMDefenseBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_106:	pBlow = new CDnRtMeleeMDefenseBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_107:	pBlow = new CDnAbRangeMDefenseBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_108:	pBlow = new CDnRtRangeMDefenseBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_109:	pBlow = new CDnUseHitterPAttackBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_110:	pBlow = new CDnUseHitterMAttackBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_111:	pBlow = new CDnMeleePReflectDamageBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_112:	pBlow = new CDnRangePReflectDamageBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_113:	pBlow = new CDnMeleeMReflectDamageBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_114:	pBlow = new CDnRangeMReflectDamageBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_115:	pBlow = new CDnPoisonExplosionBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_120:	pBlow = new CDnCantUseItemBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_116:	pBlow = new CDnFireBurnExplosionBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_117:	pBlow = new CDnFreezingExplosionBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_118:	pBlow = new CDnElectricExplosionBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_121:	pBlow = new CDnChangeStandActionBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_129:	pBlow = new CDnChangeActionSetBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_130:	pBlow = new CDnCantAttackBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_131:	pBlow = new CDnChangeWeightBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_132:	pBlow = new CDnProvocationBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_133:	pBlow = new CDnForceStunBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_134:	pBlow = new CDnChangePAttackDamageBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_135:	pBlow = new CDnChangeMAttackDamageBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_136:	pBlow = new CDnPlagueBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_137:	pBlow = new CDnUsingSkillWhenDieBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_138:	pBlow = new CDnIgnoreCanHitBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_139:	pBlow = new CDnChangeMPConsumeRatioBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_140:	pBlow = new CDnHealingBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_141:	pBlow = new CDnChangeHealingRatioBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_142:	pBlow = new CDnSuperArmorAttackRatioBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_143:	pBlow = new CDnHighlanderBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_144:	pBlow = new CDnFrostbiteBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_146:	pBlow = new CDnFrameStopBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_148:	pBlow = new CDnAggroResetBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_149:	pBlow = new CDnFreezingPrisonBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_150:	pBlow = new CDnAllImmuneBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_151:	pBlow = new CDnCameraRoarBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_152:	pBlow = new CDnMouseSensitiveBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_153:	pBlow = new CDnCooltimeParryBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_154:	pBlow = new CDNInvincibleAtBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_155:	pBlow = new CDnDissolveStateBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_156:	pBlow = new CDnSuperAmmorShield(hActor, szParam); break;	//접두어 시스템용 상태효과 - 방어용
		case STATE_BLOW::BLOW_157:	pBlow = new CDnDOTBlow(hActor, szParam); break;				//접두어 시스템용 상태효과 - 공격용(DOT 일단 출혈용)
		case STATE_BLOW::BLOW_158:	pBlow = new CDnDamageDuplicateBlow(hActor, szParam); break;	//접두어 시스템용 상태효과 - 데미지 한번더..
		case STATE_BLOW::BLOW_175:	pBlow = new CDnPushBlow(hActor, szParam); break;			//넉백..
		case STATE_BLOW::BLOW_055:	pBlow = new CDnShieldBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_159:	pBlow = new CDnFinishAttackBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_160:	pBlow = new CDnMaxDamageAddBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_161:	pBlow = new CDnBoneCrushBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_162:	pBlow = new CDnBreakSuperArmorBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_164:	pBlow = new CDnStrengthToMagicAttackBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_165:	pBlow = new CDnIntelligenceToPhysicAttackBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_166:	pBlow = new CDnAddDamageOnStateBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_167:	pBlow = new CDnSummonMonsterChangeSkillLevelBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_168:	pBlow = new CDnShockInfectionBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_169:	pBlow = new CDnAddDamageOnCriticalBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_170:	pBlow = new CDnTimeBoomBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_171:	pBlow = new CDnCoolTimeChangeBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_172:	pBlow = new CDnAddStateOnConditionBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_173:	pBlow = new CDnFreezingShieldBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_174:	pBlow = new CDnStealBuffBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_176:	pBlow = new CDnAllowedSkillsBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_177:	pBlow = new CDnDamageBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_178:	pBlow = new CDnAddStateOnHitBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_179:	pBlow = new CDnActionChangeOnHitBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_180:	pBlow = new CDnPhysicalAttackToMagicalAttackBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_181:	pBlow = new CDnHolyWaterBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_182:	pBlow = new CDnChangeElementBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_183:	pBlow = new CDnIgnorePrefixSkillBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_184:	pBlow = new CDnFinalPDamageBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_185:	pBlow = new CDnFinalMDamageBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_204:	pBlow = new CDnForceOutBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_205:	pBlow = new CDnTeleportBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_206:	pBlow = new CDnActionChangeRatioBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_207:	pBlow = new CDnDamageTransitionBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_208:	pBlow = new CDnPingpongBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_209:	pBlow = new CDnBasisAttackDmgUpBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_210:	pBlow = new CDnMiniMapRadarBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_211:	pBlow = new CDnRemoveStateBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_212:	pBlow = new CDnAddStateOnTimerBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_213:	pBlow = new CDnAddAttackPDamageBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_214:	pBlow = new CDnAddAttackMDamageBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_215:	pBlow = new CDnOrderMySummonedMonsterBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_216:	pBlow = new CDnRecallSummonBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_217:  pBlow = new CDnGhoulTransformBlow( hActor , szParam ); break;
		case STATE_BLOW::BLOW_218:	pBlow = new CDnEscapeBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_219:  pBlow = new CDnUnstableHolyWaterBlow( hActor, szParam ); break;
		case STATE_BLOW::BLOW_220:	pBlow = new CDnAccelBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_221:	pBlow = new CDnPrefixDamageBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_222:  pBlow = new CDnBurnCriticalBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_223:  pBlow = new CDnMPBurstBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_224:  pBlow = new CDnDisableActionBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_225:	pBlow = new CDnConfuseBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_226:	pBlow = new CDnProbInvincibleAtBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_227:	pBlow = new CDnBloodSuckingBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_228:	pBlow = new CDnIgnoreHitSignalBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_229:	pBlow = new CDnApplyStateBlowOnTimeBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_230:  pBlow = new CDnSpectatorBlow(hActor , szParam ); break;
		case STATE_BLOW::BLOW_231:	pBlow = new CDnContinueBaseMPIncBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_232:  pBlow = new CDnTransformBlow(hActor , szParam); break;
		case STATE_BLOW::BLOW_233:  pBlow = new CDnKillBlow(hActor , szParam); break;
		case STATE_BLOW::BLOW_235:	pBlow = new CDnAvoidDebufActionBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_236:	pBlow = new CDnVarianceDamgeBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_237:	pBlow = new CDnDisarmamentBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_238:	pBlow = new CDnChangeWeaponBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_239:  pBlow = new CDnInvincibleTypeBlow(hActor , szParam); break;
		case STATE_BLOW::BLOW_240:  pBlow = new CDnImmuneByType(hActor , szParam); break;
		case STATE_BLOW::BLOW_241:  pBlow = new CDnFlashBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_242:  pBlow = new CDnComboDamageLimitBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_243:  pBlow = new CDnManaShieldBlow(hActor , szParam ); break;
		case STATE_BLOW::BLOW_244:  pBlow = new CDnCurseBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_245:  pBlow = new CDnCurseBreakDownBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_246:	pBlow = new CDnStigmaBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_051:	pBlow = new CDnTransmitDamageBlow(hActor, szParam); break;	//247상태효과 관련 추가 상태효과(데미지를 받으면 주인이 데미지 처리 되도록함)
		case STATE_BLOW::BLOW_247:	pBlow = new CDnPuppetBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_248:	pBlow = new CDnRecoverHPByAttackBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_249:	pBlow = new CDnPileAddEffectBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_250:	pBlow = new CDnTargetCriticalBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_251:	pBlow = new CDnFinalCriticalBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_252:	pBlow = new CDnIngnoreEffectBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_253:	pBlow = new CDnAdditionalStateInfoBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_254:	pBlow = new CDnForceAggroTargetBlow(hActor, szParam); break;

		//통합 레벨 스킬 기능용 상태효과들
		case STATE_BLOW::BLOW_255:	pBlow = new CDnExpIncBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_256:	pBlow = new CDnPetExpIncBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_257:	pBlow = new CDnMoveSpeedIncBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_258:	pBlow = new CDnVehicleMoveSpeedBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_259:	pBlow = new CDnIgnoreDurationBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_260:	pBlow = new CDnNpcReputationIncBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_261:	pBlow = new CDnAddMedalBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_262:	pBlow = new CDnFarmingTimeBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_263:	pBlow = new CDnLadderStoreSaleBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_264:	pBlow = new CDnAutoFishingTimeBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_265:	pBlow = new CDnCookingExpIncBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_266:	pBlow = new CDnGoldIncBlow(hActor, szParam); break;

		//통합 레벨 스킬 공격용 추가 상태효과
		case STATE_BLOW::BLOW_267:	pBlow = new CDnAddCriticalRateBlow(hActor, szParam); break;

		//통합 레벨 스킬 공격용 추가 상태효과
		case STATE_BLOW::BLOW_268:	pBlow = new CDnDamageChagneBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_269:	pBlow = new CDnGlobalCoolTimeBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_270:	pBlow = new CDnPartyDropRateBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_271:	pBlow = new CDnBasicStateBaseAddState(hActor, szParam); break;
		case STATE_BLOW::BLOW_272:  pBlow = new CDnIncreaseItemRecoverHPBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_273:	pBlow = new CDnIgnoreGravitySignalBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_274:  pBlow = new CDnDamageLimitBlow(hActor, szParam); break;

		case STATE_BLOW::BLOW_275:	pBlow = new CDnIgnoreRestoreHPBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_276:  pBlow = new CDnAddStateBySkillGroupBlow(hActor, szParam ); break;
		case STATE_BLOW::BLOW_277:  pBlow = new CDnTransformMonsterBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_278:  pBlow = new CDnPartyFinishAttackBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_279:  pBlow = new CDnPartyAddDamageBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_280:  pBlow = new CDnDragonShieldBlow(hActor, szParam); break;
#ifdef PRE_ADD_HPBELOWDOSKILLBLOW
		case STATE_BLOW::BLOW_281:  pBlow = new CDnHPBelowUseSkillBlow( hActor, szParam ); break;
#endif // PRE_ADD_HPBELOWDOSKILLBLOW

		case STATE_BLOW::BLOW_298:  pBlow = new CDnDarkTransformationBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_306:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_345:  pBlow = new CDnMechanicModeBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_350:  pBlow = new CDnConsumeBubbleBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_351:  pBlow = new CDnDisableSkillBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_370:  pBlow = new CDnCheckBubbleCancelSkillBlow(hActor, szParam); break;

		//EMPTY BLOWS,
		case STATE_BLOW::BLOW_282:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_283:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_284:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_285:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_286:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_287:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_288:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_289:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_290:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_291:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_292:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_293:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_294:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_295:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_296:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_297:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_299:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_300:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_301:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_302:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_303:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_304:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_305:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_307:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_308:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_309:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_310:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_311:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_312:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_313:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_314:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_315:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_316:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_317:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_318:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_319:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_320:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_321:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_322:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_323:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_324:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_325:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_326:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_327:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_328:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_329:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_330:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_331:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_332:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_333:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_334:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_335:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_336:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_337:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_338:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_339:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_340:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_341:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_342:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_343:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_344:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_346:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_347:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_348:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_349:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_352:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_353:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_354:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_355:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_356:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_357:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_358:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_359:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_360:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_361:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_362:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_363:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_364:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_365:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_366:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_367:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_368:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_369:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_371:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_372:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_373:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_374:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_375:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_376:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_377:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_378:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		case STATE_BLOW::BLOW_379:  pBlow = new CDnEmptyBlow(hActor, szParam); break;
		default:
			return CDnBlow::Identity();
	}

	if( pBlow )
	{
#ifndef PRE_MOD_MAGICALBREEZE_CHANGE_BLOW
		pBlow->SetOriginalBlowIndex( emOriginalBlowIndex );
#endif // PRE_MOD_MAGICALBREEZE_CHANGE_BLOW
		return pBlow->GetMySmartPtr();
	}
	else
	{
		return CDnBlow::Identity();
	}
}

bool CDnCreateBlow::IsBasicBlow( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{	
	switch( emBlowIndex )
	{
		case STATE_BLOW::BLOW_001:
		case STATE_BLOW::BLOW_002:
		case STATE_BLOW::BLOW_003:
		case STATE_BLOW::BLOW_004:
		case STATE_BLOW::BLOW_005:
		case STATE_BLOW::BLOW_006:
		case STATE_BLOW::BLOW_007:
		case STATE_BLOW::BLOW_008:
		case STATE_BLOW::BLOW_009:
		case STATE_BLOW::BLOW_010:
		case STATE_BLOW::BLOW_015:
		case STATE_BLOW::BLOW_017:
		case STATE_BLOW::BLOW_019:
		case STATE_BLOW::BLOW_020:
		case STATE_BLOW::BLOW_021:
		case STATE_BLOW::BLOW_022:
		case STATE_BLOW::BLOW_023:
		case STATE_BLOW::BLOW_024:
		case STATE_BLOW::BLOW_026:
		case STATE_BLOW::BLOW_028:
		case STATE_BLOW::BLOW_029:
		case STATE_BLOW::BLOW_032:
		case STATE_BLOW::BLOW_033:
		case STATE_BLOW::BLOW_034:
		case STATE_BLOW::BLOW_035:
		case STATE_BLOW::BLOW_036:
		case STATE_BLOW::BLOW_037:
		case STATE_BLOW::BLOW_038:
		case STATE_BLOW::BLOW_039:
		case STATE_BLOW::BLOW_048:
		case STATE_BLOW::BLOW_049:
		case STATE_BLOW::BLOW_058:
		case STATE_BLOW::BLOW_059:
		case STATE_BLOW::BLOW_061:
		case STATE_BLOW::BLOW_064:
		case STATE_BLOW::BLOW_075:
		case STATE_BLOW::BLOW_076:
		case STATE_BLOW::BLOW_082:
		case STATE_BLOW::BLOW_087:
		case STATE_BLOW::BLOW_088:
		case STATE_BLOW::BLOW_089:
		case STATE_BLOW::BLOW_090:
		case STATE_BLOW::BLOW_093:
		case STATE_BLOW::BLOW_094:
		case STATE_BLOW::BLOW_123:
		case STATE_BLOW::BLOW_124:
		case STATE_BLOW::BLOW_125:
		case STATE_BLOW::BLOW_126:
		case STATE_BLOW::BLOW_127:
		case STATE_BLOW::BLOW_128:
		case STATE_BLOW::BLOW_200:
		case STATE_BLOW::BLOW_201:
		case STATE_BLOW::BLOW_202:
		case STATE_BLOW::BLOW_203:
		case STATE_BLOW::BLOW_234:
			return true;
	}

	return false;
}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCreateBlow::AddStateEffectValue(STATE_BLOW::emBLOW_INDEX emBlowIndex, const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	if( IsBasicBlow(emBlowIndex) )
	{
		CDnBasicBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);
	}
	else
	{
		switch( emBlowIndex )
		{
		case STATE_BLOW::BLOW_011: CDnAbContinueHPIncBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_012: CDnRtContinueHPIncBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_013: CDnAbContinueMPIncBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_014: CDnRtContinueMPIncBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_016: CDnHPIncBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_018: CDnMPIncBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_025: CDnFrameBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_028:	break;
		case STATE_BLOW::BLOW_029:	break;
		case STATE_BLOW::BLOW_030: CDnBlockBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_031: CDnParryBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_040: CDnConcentrationBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_041: CDnFreezingBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_042: CDnFireBurnBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_043: CDnElectricShockBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_044: CDnPoisonBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_045: CDnSleepBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_050: CDnDamageRatioBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_056: CDnParryingProbBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_057: CDnRebirthBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_060: CDnChainAttackBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_062: CDnChargerBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_065: CDnChangeWeaponRangeBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_066: CDnAggroBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_067: CDnCantUseActiveSkillBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_068: CDnPayBackMPBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_069: CDnDissolveDebufBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_070: CDnCantMoveBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_071: CDnCantActionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_072: CDnClericMentalChargeBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_073: CDnInvisibleBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_074: CDnMaximumAttackBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_077: CDnImmuneBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_078: CDnCantUseMPUsingSkillBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_079: CDnDestroySuperArmorBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_083: CDnDissolveBufBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_084: CDnForceActionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_086: CDnReverseTeamBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_091: CDnAttackerHPAbsolute::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_092: CDnAttackerHPRatio::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_096: CDnCoolTimeBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_099: CDnInvincibleBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue);	break;
		case STATE_BLOW::BLOW_101: CDnAbMeleePDefenseBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_102: CDnRtMeleePDefenseBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_103: CDnAbRangePDefenseBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_104: CDnRtRangePDefenseBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_105: CDnAbMeleeMDefenseBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_106: CDnRtMeleeMDefenseBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_107: CDnAbRangeMDefenseBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_108: CDnRtRangeMDefenseBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_109: CDnUseHitterPAttackBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_110: CDnUseHitterMAttackBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_111: CDnMeleePReflectDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_112: CDnRangePReflectDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_113: CDnMeleeMReflectDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_114: CDnRangeMReflectDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_115: CDnPoisonExplosionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_120: CDnCantUseItemBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_116: CDnFireBurnExplosionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_117: CDnFreezingExplosionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_118: CDnElectricExplosionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_121: CDnChangeStandActionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_129: CDnChangeActionSetBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_130: CDnCantAttackBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_131: CDnChangeWeightBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_132: CDnProvocationBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_133: CDnForceStunBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_134: CDnChangePAttackDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_135: CDnChangeMAttackDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_136: CDnPlagueBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_137: CDnUsingSkillWhenDieBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_138: CDnIgnoreCanHitBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_139: CDnChangeMPConsumeRatioBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_140: CDnHealingBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_141: CDnChangeHealingRatioBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_142: CDnSuperArmorAttackRatioBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_143: CDnHighlanderBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_144: CDnFrostbiteBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_146: CDnFrameStopBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_148: CDnAggroResetBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_149: CDnFreezingPrisonBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_150: CDnAllImmuneBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_151: CDnCameraRoarBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_152: CDnMouseSensitiveBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_153: CDnCooltimeParryBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_154: CDNInvincibleAtBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_155: CDnDissolveStateBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_156: CDnSuperAmmorShield::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;	//접두어 시스템용 상태효과 - 방어용
		case STATE_BLOW::BLOW_157: CDnDOTBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;				//접두어 시스템용 상태효과 - 공격용(DOT 일단 출혈용)
		case STATE_BLOW::BLOW_158: CDnDamageDuplicateBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;	//접두어 시스템용 상태효과 - 데미지 한번더..
		case STATE_BLOW::BLOW_175: CDnPushBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;			//넉백..
		case STATE_BLOW::BLOW_055: CDnShieldBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_159: CDnFinishAttackBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_160: CDnMaxDamageAddBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_161: CDnBoneCrushBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_162: CDnBreakSuperArmorBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_164: CDnStrengthToMagicAttackBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_165: CDnIntelligenceToPhysicAttackBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_166: CDnAddDamageOnStateBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_167: CDnSummonMonsterChangeSkillLevelBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_168: CDnShockInfectionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_169: CDnAddDamageOnCriticalBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_170: CDnTimeBoomBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_171: CDnCoolTimeChangeBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_172: CDnAddStateOnConditionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_173: CDnFreezingShieldBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_174: CDnStealBuffBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_176: CDnAllowedSkillsBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_177: CDnDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_178: CDnAddStateOnHitBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_179: CDnActionChangeOnHitBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_180: CDnPhysicalAttackToMagicalAttackBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_181: CDnHolyWaterBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_182: CDnChangeElementBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_183: CDnIgnorePrefixSkillBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_184: CDnFinalPDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_185: CDnFinalMDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_204: CDnForceOutBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_205: CDnTeleportBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_206: CDnActionChangeRatioBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_207: CDnDamageTransitionBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_208: CDnPingpongBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_209: CDnBasisAttackDmgUpBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_210: CDnMiniMapRadarBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_211: CDnRemoveStateBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_212: CDnAddStateOnTimerBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_213: CDnAddAttackPDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_214: CDnAddAttackMDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_215: CDnOrderMySummonedMonsterBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_216: CDnRecallSummonBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_217: CDnGhoulTransformBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_218: CDnEscapeBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_219: CDnUnstableHolyWaterBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_220: CDnAccelBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_221: CDnPrefixDamageBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_222: CDnBurnCriticalBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_225: CDnConfuseBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		//case STATE_BLOW::BLOW_226: CDnProbInvincibleAtBlow::AddState(hActor, szParam); break;
		case STATE_BLOW::BLOW_227: CDnBloodSuckingBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_228: CDnIgnoreHitSignalBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_229: CDnApplyStateBlowOnTimeBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_230: CDnSpectatorBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_231: CDnContinueBaseMPIncBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_233: CDnKillBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;
		case STATE_BLOW::BLOW_243: CDnManaShieldBlow::AddStateEffectValue(szOrigValue, szAddValue, szNewValue); break;

		default: szNewValue = szOrigValue; break;
		}
	}
}

#endif // PRE_ADD_PREFIX_SYSTE_RENEW
