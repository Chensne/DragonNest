#include "StdAfx.h"
#include "DnSkillPropComponent.h"
#include "DnTableDB.h"
#include "EtMatrixEx.h"
#include "DnWorldProp.h"
#include "DnProjectile.h"
#include "DnMonsterActor.h"


CDnSkillPropComponent::CDnSkillPropComponent(void)
{
}

CDnSkillPropComponent::~CDnSkillPropComponent(void)
{
	SAFE_RELEASE_SPTR( m_hSkill );
}


void CDnSkillPropComponent::Initialize( int iSkillID, int iSkillLevel )
{
	if( iSkillID == 0 || iSkillLevel == 0 )
		return;

	// 사용할 상태효과를 미리 생성해 놓는다.
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	vector<int> vlSkillLevelList;
	if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillID, vlSkillLevelList ) <= 0 ) 
	{
		_ASSERT( !"프랍이 사용하는 스킬에서 스킬 인덱스를 스킬 레벨 테이블에서 찾을 수 없음." );
		//return false;
	}

	int iSkillLevelTableID = -1;
	for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
	{
		int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();
		if( iNowLevel == iSkillLevel )
		{
			iSkillLevelTableID = vlSkillLevelList.at( i );
			break;
		}
	}

	_ASSERT( -1 != iSkillLevelTableID && "프랍이 사용하는 스킬의 해당 레벨이 없음." );

	// 상태 효과 정보 로딩
	CDnSkill::StateEffectStruct StateEffect;
	char caLable[ 64 ];
	ZeroMemory( caLable, sizeof(caLable) );
	for( int i = 0; i < CDnSkill::MAX_STATE_EFFECT_COUNT; ++i )
	{
		sprintf_s( caLable, "_EffectClass%d", i + 1 );
		StateEffect.nID = pSkillTable->GetFieldFromLablePtr( iSkillID, caLable )->GetInteger();

		if( StateEffect.nID < 1 ) 
			continue;

		sprintf_s( caLable, "_EffectClass%dApplyType", i + 1 );
		StateEffect.ApplyType = (CDnSkill::StateEffectApplyType)pSkillTable->GetFieldFromLablePtr( iSkillID, caLable )->GetInteger();

		sprintf_s( caLable, "_EffectClassValue%d", i + 1 );
		StateEffect.szValue = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetString();

		sprintf_s( caLable, "_EffectClassValue%dDuration", i + 1 );
		StateEffect.nDurationTime = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetInteger();

		m_vlStateEffectList.push_back( StateEffect );
	}

	m_SkillInfo.iSkillID = iSkillID;
	m_SkillInfo.iSkillLevelID = iSkillLevelTableID;
	m_SkillInfo.iLevel = iSkillLevel;
	m_SkillInfo.eDurationType = (CDnSkill::DurationTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillID, "_DurationType" )->GetInteger();
	m_SkillInfo.eTargetType = (CDnSkill::TargetTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillID, "_TargetType" )->GetInteger();
	m_SkillInfo.iSkillDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iSkillID, "_SkillDuplicate" )->GetInteger();
	m_SkillInfo.iDuplicateCount = pSkillTable->GetFieldFromLablePtr( iSkillID, "_EffectAmassCount" )->GetInteger();
	m_SkillInfo.iDissolvable = pSkillTable->GetFieldFromLablePtr( iSkillID, "_Dissolvable" )->GetInteger();

	// 버프 프랍에서 사용하는 모듈이므로 프랍에서 비롯된 스킬이라는 정보를 셋팅.
	m_SkillInfo.bFromBuffProp = true;
}	


void CDnSkillPropComponent::OnSignal( DnActorHandle hActor, DnPropHandle hProp, SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, 
									  LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type )
	{
		case STE_Projectile:
			{
				ProjectileStruct *pStruct = (ProjectileStruct *)pPtr;

				MatrixEx LocalCross = *hProp->GetMatEx();
				CDnProjectile *pProjectile = CDnProjectile::CreateProjectile( hProp->GetRoom(), hActor, LocalCross, pStruct );
				if( pProjectile == NULL ) break;

				pProjectile->SetShooterProp( hProp );

				if( m_SkillInfo.iSkillID != 0 )
				{
					int iNumStateEffect = (int)m_vlStateEffectList.size();
					for( int iStateEffect = 0; iStateEffect < iNumStateEffect; ++iStateEffect )
						pProjectile->AddStateEffect( m_vlStateEffectList.at(iStateEffect) );

					if( !m_hSkill )
						m_hSkill = CDnSkill::CreateSkill( hActor, m_SkillInfo.iSkillID, m_SkillInfo.iLevel );

					pProjectile->SetParentSkill( m_hSkill );
					pProjectile->SetParentSkillInfo( m_SkillInfo );

					//bool bActorAttachWeapon = false;
					//if( pStruct->nWeaponTableID == 0 && GetWeapon(1) ) bActorAttachWeapon = true;

					//OnProjectile( pProjectile, bActorAttachWeapon );
					//OnSkillProjectile( pProjectile );
				}
			}
			break;

	default:
		hActor->OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
		break;
	}
}

#ifdef PRE_ADD_HIT_PROP_ADD_SE
//void CDnSkillPropComponent::ApplyStateBlowsToTheseTargets( DnActorHandle hPropActor, DNVector(DnActorHandle)& vlhActors )
void CDnSkillPropComponent::ApplyStateBlowsToTheseTargets( DnActorHandle hPropActor, DNVector(DnActorHandle)& vlhActors, HitStruct * pStruct )
{
	if( !m_hSkill )
		m_hSkill = CDnSkill::CreateSkill( hPropActor, m_SkillInfo.iSkillID, m_SkillInfo.iLevel );
	
	if( !m_hSkill )
		return;

	int iNumActorApplyStateEffect = (int)vlhActors.size();
	for( int i = 0; i < iNumActorApplyStateEffect; ++i )
	{
		// 대상이 얼음감옥 상태일때는 상태효과 적용 무시
		if (vlhActors[ i ]->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
			continue;

		// 프랍에서는 히트 시그널을 통과한 대상에게
		// 상태효과를 넣어주도록 처리한다.
		// 나중에 몬스터나 플레이어처럼 스킬 중첩 처리 등 복잡한 기능이 요구되면 
		// 액터의 hit signal 처리와 동일한 형식으로 가져가야 한다.

		//// 스킬 대상 설정이 아군인가 타겟인가에 따라 상태효과 적용을 구분한다.
		//switch( m_hSkill->GetTargetType() )
		//{
		//	case CDnSkill::Enemy:
		//	case CDnSkill::Self:
		//		if( hPropActor->GetTeam() == vlhActors[ i ]->GetTeam() )
		//			continue;
		//		break;

		//	case CDnSkill::Friend:
		//	case CDnSkill::Party:
		//		if( hPropActor->GetTeam() != vlhActors[ i ]->GetTeam() )
		//			continue;
		//		break;

		//		// 스킬 적용 대상이 아군/적군 전부 다 라면 상태효과 적용 쪽에서 적용 여부를 구분해야 한다.
		//	case CDnSkill::All:
		//		break;
		//}

//		// 상태이상 Add 시에 지속 구분 인덱스 처리 필요.
//		// 해당 Actor의 지속효과 구분 인덱스를 구분하여 성공한 경우에 상태이상 추가 시킴.
//		map<int, bool> mapDuplicateResult;
//		CDnSkill::CanApply eResult = CDnSkill::CanApplySkillStateEffect( vlhActors[i], m_hSkill, mapDuplicateResult );
//		if( CDnSkill::CanApply::Fail != eResult )
//		{
			for( DWORD k = 0; k < m_hSkill->GetStateEffectCount(); k++ ) 
			{
				CDnSkill::StateEffectStruct *pLocalStruct = m_hSkill->GetStateEffectFromIndex(k);
//
//#if defined(PRE_ADD_SKIP_STATEBLOW)				
				// #74389 [데저트 드래곤] HitStateEffect 속성 프랍 버그.
				if( pStruct && pStruct->szSkipStateBlows && CDnSkill::IsSkipStateBlow(pStruct->szSkipStateBlows, (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID) )
					continue;
//#endif // PRE_ADD_SKIP_STATEBLOW
//
//				// 아군까지 힐 시켜주는 스킬의 경우엔 Self 힐과 Target 힐 두 개의 상태효과가 선언되어있다.
//				// Self 상태효과는 자신에게 이미 적용되었고 여긴 Hit 시그널 판정되는 곳이기 떄문에 Target 만 적용된다.
//				switch( pLocalStruct->ApplyType )
//				{
//					case CDnSkill::ApplySelf:
//						continue;
//						break;
//
//					case CDnSkill::ApplyTarget:
//						break;
//
//					case CDnSkill::ApplyEnemy:
//						if( hPropActor->GetTeam() == vlhActors[ i ]->GetTeam() )
//							continue;
//						break;
//
//					case CDnSkill::ApplyFriend:
//						if( hPropActor->GetTeam() != vlhActors[ i ]->GetTeam() )
//							continue;
//						break;
//				}
//
//				// 같은 스킬 중첩일 경우엔 스킬 효과 중에 확률 체크하는 것들은 이미 CanApplySkillStateEffect 에서 확률체크되고
//				// 통과된 상태이다. 따라서 여기선 확률 체크 된건지 확인하고 된거라면 다시 확률 체크 안하도록 함수 호출 해준다.
//				bool bAllowAddThisSE = true;
//				bool bCheckCanBegin = true;
//				if( CDnSkill::ApplyDuplicateSameSkill == eResult )
//				{
//					map<int, bool>::iterator iter = mapDuplicateResult.find( pLocalStruct->nID );
//					// 맵에 없는 경우 현재 액터가 상태효과에 걸려있지 않으므로 그냥 정상적으로 상태효과 추가 루틴 실행.
//					if( mapDuplicateResult.end() != iter )
//					{
//						// 같은 스킬의 확률있는 상태효과가 현재 걸려있어서 CanAdd 를 호출해보았으나 실패했음.
//						// 이런 경우엔 상태효과 추가하지 않는다.
//						if( false == (iter->second) )
//							bAllowAddThisSE = false;
//						else
//							// 이미 CanAdd 를 통과한 상태이므로 CmdAddStateEffect 호출 시 따로 체크하지 않도록 해준다.
//							bCheckCanBegin = false;
//					}
//				}

				//if( bAllowAddThisSE )
				//{
					//// 여기서 등록되어 있는 제거 되어야할 상태효과들 없앤다. [2010/12/08 semozz]
					//vlhActors[i]->RemoveResetStateBlow();

					vlhActors[i]->CmdAddStateEffect( m_hSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID, 
													 pLocalStruct->nDurationTime, pLocalStruct->szValue.c_str()/*, false, bCheckCanBegin*/ );
				//}
			}
		//}
		// 스킬 사용 끝났으면 리셋리스트 초기화 [2010/12/09 semozz]
		//vlhActors[i]->InitStateBlowIDToRemove();
	}
}
#endif // #ifdef PRE_ADD_HIT_PROP_ADD_SE   