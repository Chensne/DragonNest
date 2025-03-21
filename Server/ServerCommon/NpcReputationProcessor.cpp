
#include "Stdafx.h"
#include "NpcReputationProcessor.h"
#include "DNUserSession.h"
#include "DNGameDataManager.h"
#include "ReputationSystemRepository.h"
#include "DNMissionSystem.h"
#include "DNMailSender.h"
#include "TimeSet.h"
#include "DNGuildSystem.h"

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#if defined(_GAMESERVER)
#include "DnBlow.h"
#endif // _GAMESERVER
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#if defined(PRE_ADD_WORLD_EVENT)
#include "DNEvent.h"
#endif

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

// Transaction/Commit 관리하는 Auto 클리스
class CAutoTC
{
public:
	CAutoTC( CDNUserSession* pSession, int iNpcID )
		:m_pSession(pSession),m_iNpcID(iNpcID),m_pRepository(pSession->GetReputationSystem())
	{
		m_pRepository->Transaction();
	}
	~CAutoTC()
	{
		m_pRepository->Commit();
		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnNpcReputaionChange, 3, 
												 EventSystem::NpcID, m_iNpcID, 
												 EventSystem::NpcReputaionFavor, m_pRepository->GetNpcReputation( m_iNpcID, IReputationSystem::NpcFavor ),
												 EventSystem::NpcReputaionMalice, m_pRepository->GetNpcReputation( m_iNpcID, IReputationSystem::NpcMalice ) );
	}

private:

	CDNUserSession*	m_pSession;
	CReputationSystemRepository* m_pRepository;
	int m_iNpcID;
};

#if defined ( PRE_ADD_REPUTATION_EXPOSURE )
bool CNpcReputationProcessor::_bIsCheckMission( CDNUserSession* pSession, const int iNpcID )
{
	TReputeTableData* pReputeTable = g_pDataManager->GetReputeTableData( iNpcID );
	if( NULL == pReputeTable )
	{
		return false;
	}

	bool bIsClearMission	= pSession->GetMissionSystem()->bIsAchieveMission( pReputeTable->iMissionID );
	if( false == bIsClearMission )
	{
#if defined( _WORK )
		WCHAR wszBuf[MAX_PATH];
		wsprintf( wszBuf, L"NpcID:%d, MissionID:%d 미션 클리어 안되서 호감도 작용안함", iNpcID, pReputeTable->iMissionID );
		pSession->SendDebugChat( wszBuf );
#endif // #if defined( _WORK )
		return false;
	}

	return true;
}
#else
bool CNpcReputationProcessor::_bIsCheckQuest( CDNUserSession* pSession, const int iNpcID )
{
	TReputeTableData* pReputeTable = g_pDataManager->GetReputeTableData( iNpcID );
	if( pReputeTable == NULL )
		return false;

	bool bIsClearQuest = false;
	const std::vector<int>& questIds = pReputeTable->iQuestIDs;
	std::vector<int>::const_iterator iter = questIds.begin();
	for (; iter != questIds.end(); ++iter)
	{
		const int& curQuestId = (*iter);
		if (pSession->GetQuest()->IsClearQuest(curQuestId))
		{
			bIsClearQuest = true;
			break;
		}
	}

	if (bIsClearQuest == false)
	{
#if defined( _WORK )
		WCHAR wszBuf[MAX_PATH];
		wsprintf( wszBuf, L"NpcID:%d 퀘스트 클리어 안되서 호감도 작용안함", iNpcID );
		pSession->SendDebugChat( wszBuf );
#endif // #if defined( _WORK )
		return false;
	}

	return true;
}
#endif // #if defined ( PRE_ADD_REPUTATION_EXPOSURE )

BYTE CNpcReputationProcessor::_ConvertUnionToPointType(int nType)
{
	switch (nType)
	{
	case NpcReputation::UnionType::Commercial:	return DBDNWorldDef::EtcPointCode::Union_Commercial;
	case NpcReputation::UnionType::Liberty:		return DBDNWorldDef::EtcPointCode::Union_Liberty;
	case NpcReputation::UnionType::Royal:		return DBDNWorldDef::EtcPointCode::Union_Royal;
	}
	return DBDNWorldDef::EtcPointCode::None;
}


void CNpcReputationProcessor::_AddUnionReputePoint( CDNUserSession* pSession, int iUnionID, int nUnionPoint )
{
	if (nUnionPoint <= 0)
		return;

	BYTE cPointType = _ConvertUnionToPointType(iUnionID);
	if( pSession->GetGuildUID().IsSet() )
	{
		CDNGuildBase* pGuild = g_pGuildManager->At( pSession->GetGuildUID() );
		if(pGuild)
		{
			TGuildRewardItem* GuildRewardItem = pGuild->GetGuildRewardItem();
			if( GuildRewardItem[GUILDREWARDEFFECT_TYPE_EXTRAUNIONPOINT].nItemID > 0 &&
				GuildRewardItem[GUILDREWARDEFFECT_TYPE_EXTRAUNIONPOINT].nEffectValue > 0 )
			{
				nUnionPoint += (int)(nUnionPoint * GuildRewardItem[GUILDREWARDEFFECT_TYPE_EXTRAUNIONPOINT].nEffectValue * 0.01);
			}

		}
	}
#if defined(PRE_ADD_WORLD_EVENT)
	const TEventListInfo* pEventInfo = g_pEvent->GetEventByType(pSession->GetWorldSetID(), WorldEvent::EVENT4/*4. 연합 포인트획득량 증가*/, pSession->GetClassID());
	if (pEventInfo && pEventInfo->nAtt1 > 0 )
		nUnionPoint += (int)(nUnionPoint * (pEventInfo->nAtt1/100.0f));
#else //#if defined(PRE_ADD_WORLD_EVENT)
#if defined(PRE_ADD_WEEKLYEVENT)
	int nThreadID = 0;
	float fEventValue = g_pDataManager->GetWeeklyEventValuef(WeeklyEvent::Player, pSession->GetClassID(), WeeklyEvent::Event_9, nThreadID);
	if (fEventValue != 0.f)
		nUnionPoint += (int)(nUnionPoint * fEventValue);
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)
#endif //#if defined(PRE_ADD_WORLD_EVENT)

	pSession->AddEtcPoint(cPointType, nUnionPoint);
}

void CNpcReputationProcessor::Process( CDNUserSession* pSession, const int iNpcID, const IReputationSystem::eType Type, REPUTATION_TYPE value )
{
#if defined ( PRE_ADD_REPUTATION_EXPOSURE )
	if( _bIsCheckMission( pSession, iNpcID ) == false )
		return;
#else
	if( _bIsCheckQuest( pSession, iNpcID ) == false )
		return;
#endif // #if defined ( PRE_ADD_REPUTATION_EXPOSURE )

	CAutoTC TC( pSession, iNpcID );
	pSession->GetReputationSystem()->AddNpcReputation( iNpcID, Type, value );
}

void CNpcReputationProcessor::PresentProcess( CDNUserSession* pSession, const int iNpcID, const int iPresentID, const int iPresentCount )
{
	// 선물 수 검사
	if (iPresentCount <= 0 || iPresentCount > NPCPRESENTMAX)
		return;

#if defined ( PRE_ADD_REPUTATION_EXPOSURE )
	if( _bIsCheckMission( pSession, iNpcID ) == false )
		return;
#else
	if( _bIsCheckQuest( pSession, iNpcID ) == false )
		return;
#endif // #if defined ( PRE_ADD_REPUTATION_EXPOSURE )

	CAutoTC TC( pSession, iNpcID );

	TReputeTableData*  pReputationTable	= g_pDataManager->GetReputeTableData( iNpcID );
	if( pReputationTable == NULL )
		return;
	TPresentTableData* pPresentTable	= g_pDataManager->GetPresentTableData( iPresentID );
	if( pPresentTable == NULL )
		return;

	// 받을 수 있는 선물인지 검사
	if( pReputationTable->CheckPresentID( iPresentID ) == false )
		return;

	switch( pPresentTable->Type )
	{
	case TPresentTableData::Normal:
		{
			// ItemID가지고 inven에 빼기 (db 저장 포함)
			if( pSession->GetItem()->DeleteInventoryByItemID( pPresentTable->iTypeID, iPresentCount, DBDNWorldDef::UseItem::Present, iNpcID ) == false )
				return;
			break;
		}
	case TPresentTableData::Cash:
		{
			// ItemID가지고 inven에 빼기 (db 저장 포함)
			if( pSession->GetItem()->DeleteCashInventoryByItemID( pPresentTable->iTypeID, iPresentCount, DBDNWorldDef::UseItem::Present, iNpcID ) == false )
				return;
			break;
		}
	case TPresentTableData::Coin:
		{
			if( pSession->CheckEnoughCoin( pPresentTable->iTypeID ) == false )
				return;
			pSession->DelCoin( pPresentTable->iTypeID, DBDNWorldDef::CoinChangeCode::Present, iNpcID );
			break;
		}
	default:
		{
			_ASSERT(0);
			return;
		}
	}

	bool bPlusItem = false;

#if defined ( PRE_ADD_REPUTATION_EXPOSURE )
	// Npc가 주는 선물 아이템이 활성화 되었는지 확인한다.
	int nPlusItemID = pReputationTable->iPlusItemID;
	if( 0 != nPlusItemID )
	{
		const REPUTATION_TYPE nCurrentRepute = pSession->GetReputationSystem()->GetNpcReputation( iNpcID, IReputationSystem::NpcFavor );

		if( nPlusItemID == pReputationTable->iNpcPresentID1 && nCurrentRepute >= pReputationTable->iNpcPresentRepute1 )
		{
			bPlusItem = true;
		}
		else if( nPlusItemID == pReputationTable->iNpcPresentID2 && nCurrentRepute >= pReputationTable->iNpcPresentRepute2 )
		{
			bPlusItem = true;
		}
	}
#else
#endif // #if defined ( PRE_ADD_REPUTATION_EXPOSURE )

	// #48214 호감도 콜렉션 강화 적용 시 기존에 아이템을 지급받은 캐릭터 들을 위해서 해당 기능을 계속 유지 하기로 한다. 
	// 인벤토리에 특정 아이템을 소지하고 있는지 확인한다.
	CDNUserItem* pUserItem = pSession->GetItem();
	if (pUserItem)
	{
		int nItemID = pReputationTable->iPlusItemID;
		if (pUserItem->GetInventoryItemCount(nItemID) > 0)
			bPlusItem = true;
	}

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#if defined(_GAMESERVER)
	float incValue = 0.0f;
	DnActorHandle hActor = pSession->GetActorHandle();
	if (hActor && hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_260))
	{
		DNVector(DnBlowHandle) vlBlows;
		hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_260, vlBlows);
		{
			int nCount = (int)vlBlows.size();
			for (int i = 0; i < nCount; ++i)
			{
				DnBlowHandle hBlow = vlBlows[i];
				if (hBlow && hBlow->IsEnd() == false)
				{
					incValue += hBlow->GetFloatValue();
				}
			}
		}
	}
#endif // _GAMESERVER
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	// 아이템이 있으면 보너스 비율을 계산한다.
	float fPlusProbRate = 0;
	if (bPlusItem)
		fPlusProbRate = (float)pReputationTable->iPlusProb/100.0f;

	// 보너스 점수
	int iAddFavorBonus = (int)(pPresentTable->iAddFavorPoint * fPlusProbRate);
	int iTakeMaliceBonus = (int)(pPresentTable->iTakeMalicePoint * fPlusProbRate);
	int iFavorGroupBonus = (int)(pPresentTable->iFavorGroupPoint * fPlusProbRate);
	int iMaliceGroupBonus = (int)(pPresentTable->iMaliceGroupPoint * fPlusProbRate);

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#if defined(_GAMESERVER)
	iAddFavorBonus += (int)(pPresentTable->iAddFavorPoint * incValue);
#elif defined( _VILLAGESERVER )
	float fTotalLevel = pSession->GetTotalLevelSkillEffect(TotalLevelSkill::Common::RepuTationIncrease);
	iAddFavorBonus += (int)(pPresentTable->iAddFavorPoint * fTotalLevel);
#endif // _GAMESERVER
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	// 선물 받은 NPC 호감도 적용
	pSession->GetReputationSystem()->AddNpcReputation( iNpcID, IReputationSystem::NpcFavor, (pPresentTable->iAddFavorPoint+iAddFavorBonus)*iPresentCount );
	pSession->GetReputationSystem()->AddNpcReputation( iNpcID, IReputationSystem::NpcMalice, ((pPresentTable->iTakeMalicePoint+iTakeMaliceBonus)*-1)*iPresentCount );

	// 선물 받은 NPC 와 친한 NPC 호감도 적용
	for( UINT i=0 ; i<pReputationTable->vFavorNpcID.size() ; ++i )
	{
#if defined ( PRE_ADD_REPUTATION_EXPOSURE )
		if( _bIsCheckMission( pSession, pReputationTable->vFavorNpcID[i] ) == false )
			continue;
#else
		if( _bIsCheckQuest( pSession, pReputationTable->vFavorNpcID[i] ) == false )
			continue;
#endif // #if defined ( PRE_ADD_REPUTATION_EXPOSURE )

		pSession->GetReputationSystem()->AddNpcReputation( pReputationTable->vFavorNpcID[i], IReputationSystem::NpcFavor, (pPresentTable->iFavorGroupPoint+iFavorGroupBonus)*iPresentCount );
	}

	// 선물 받은 NPC 와 싫어하는 NPC 호감도 적용
	for( UINT i=0 ; i<pReputationTable->vMaliceNpcID.size() ; ++i )
	{
#if defined ( PRE_ADD_REPUTATION_EXPOSURE )
		if( _bIsCheckMission( pSession, pReputationTable->vMaliceNpcID[i] ) == false )
			continue;
#else
		if( _bIsCheckQuest( pSession, pReputationTable->vMaliceNpcID[i] ) == false )
			continue;
#endif // #if defined ( PRE_ADD_REPUTATION_EXPOSURE )
		
		pSession->GetReputationSystem()->AddNpcReputation( pReputationTable->vMaliceNpcID[i], IReputationSystem::NpcMalice, (pPresentTable->iMaliceGroupPoint+iMaliceGroupBonus)*iPresentCount );
	}

	// 선물메일
	int iMailID	= 0;
	for (int i=0; i<iPresentCount; i++)
	{
		iMailID	= 0;
#if defined( _VILLAGESERVER )
		int iRand	= (_rand()%NpcReputation::Common::MaxMailRandValue)+1;
#else
		int iRand	= (_rand(reinterpret_cast<CMultiRoom*>(pSession->GetGameRoom()))%NpcReputation::Common::MaxMailRandValue)+1;
#endif // #if defined( _VILLAGESERVER )
		for( int i=0 ;i<NpcReputation::Common::MaxMailCount ; ++i )
		{
			if( pReputationTable->iMailID[i] <= 0 || pReputationTable->iMailRand[i] <= 0 )
				break;

			if( iRand <= pReputationTable->iMailRand[i] )
			{
				iMailID = pReputationTable->iMailID[i];
				break;
			}
		}

		if( iMailID > 0 )
			CDNMailSender::Process( pSession, iMailID );
	}


	// 선물에 따른 연합포인트 누적
	int nUnionPoint = pReputationTable->GetUnionPointByPresent( iPresentID );
	_AddUnionReputePoint( pSession, pReputationTable->iUnionID, nUnionPoint*iPresentCount );

}
void CNpcReputationProcessor::CheckAndCalcStoreBenefit( CDNUserSession* pUserSession, int iNpcID, TStoreBenefitData::eType Type, /*IN OUT*/ int& iNeedCoin )
{
	// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
	// 혜택종류가 강화 비용 할인이 있다면 적용시켜준다.
	vector<TStoreBenefitData*> vlBenefitDatas;
	g_pDataManager->GetStoreBenefitData( iNpcID, vlBenefitDatas );
	for( int i = 0; i < (int)vlBenefitDatas.size(); ++i )
	{
		const TStoreBenefitData* pBenefitData = vlBenefitDatas.at( i );
		if( Type == pBenefitData->Type )
		{
			// 현재 이 npc 대한 호감도 percent 를 얻어와서 기준값 이상이 되는지 확인.
			CReputationSystemRepository* pRepository = pUserSession->GetReputationSystem();
			if( pRepository->IsExistNpcReputation( iNpcID ) )
			{
				int iNowFavorPercent = pRepository->GetNpcReputationPercent( iNpcID, IReputationSystem::NpcFavor );
				int iBenefitIndex = -1;
				for( int k = 0; k < STORE_BENEFIT_MAX; ++k )
				{
					if( 0 != pBenefitData->aiFavorThreshold[ k ] )
					{
						if( pBenefitData->aiFavorThreshold[ k ] <= iNowFavorPercent )
							iBenefitIndex = k;
					}
				}

				// 해당 npc 에 대한 현재 호감도 수치에 따른 혜택이 존재함. 적용시켜준다.
				if( -1 < iBenefitIndex )
				{
					// 물건 구입비, 강화 수수료, 수리비 등등은 할인 처리.
					// 물건 팔 때 비싸게 팔 수 있는 것은 가격을 올려주도록 처리.
					if( TStoreBenefitData::SellingPriceUp == Type )
					{
						iNeedCoin += int( (float)iNeedCoin * (float(pBenefitData->aiAdjustPercent[ iBenefitIndex ]) / 100.0f) );
					}
					else
					{
						iNeedCoin -= int( (float)iNeedCoin * (float(pBenefitData->aiAdjustPercent[ iBenefitIndex ]) / 100.0f) );
					}
				}
			}
			break;
		}
	}
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

void CNpcReputationProcessor::CheckAndCalcUnionBenefit( CDNUserSession* pUserSession, TStoreBenefitData::eType Type, /*IN OUT*/ int& iNeedPoint )
{
	// 원격아이템 사용시에는 호감도 강화 할인 적용안됨.
	if( Type == TStoreBenefitData::EnchantFeeDiscount )
	{
		if( pUserSession->bIsRemoteEnchant() == true )
			return;
	}

	CTimeSet timeSet;
	__time64_t tLocalTime = timeSet.GetTimeT64_LC();

	for (int i=0; i<NpcReputation::UnionType::Etc; i++)
	{
		TUnionMembership* pMembership = pUserSession->GetItem()->GetUnionMembership(i);
		if (!pMembership) continue;
		if (pMembership->nItemID == 0) continue;
		
		// 기간 체크
		if (pMembership->tExpireDate == 0 ||
			pMembership->tExpireDate < tLocalTime)
		{
			// 만료
			pMembership->nItemID = 0;
			pMembership->tExpireDate = 0;
			continue;
		}

		// 혜택정보를 얻어온다.
		TUnionReputeBenefitData* pBenefitData = g_pDataManager->GetUnionReputeBenefitByItemID(pMembership->nItemID);
		if (!pBenefitData) continue;

		for (int j=0; j<NpcReputation::Common::MaxBenefitCount; j++)
		{
			if (pBenefitData->nBenefitType[j] == -1) continue;
			if (Type != pBenefitData->nBenefitType[j]) continue;
			if (pBenefitData->nBenefitNum[j] <= 0) continue;

			switch (Type)
			{
			case TStoreBenefitData::SellingPriceUp:
			case TStoreBenefitData::FishProficiencyUp:
			case TStoreBenefitData::CookProficiencyUp:
			case TStoreBenefitData::CultivateProficiencyUp:
				{
					iNeedPoint += int( (float)iNeedPoint * (float(pBenefitData->nBenefitNum[j]) / 100.0f) );
				}
				break;
			
			default:	// Discount
				{
					iNeedPoint -= int( (float)iNeedPoint * (float(pBenefitData->nBenefitNum[j]) / 100.0f) );
				}
			}
		}
	}
}

void CNpcReputationProcessor::UseUnionReputePoint( CDNUserSession* pSession, int nType, int nUsePoint )
{
	const_cast<TUnionReputePointInfo*>(pSession->GetUnionReputePointInfoPtr())->DelUnionReputePoint(nType, nUsePoint);

	BYTE cPointType = _ConvertUnionToPointType(nType);
	pSession->UseEtcPoint(cPointType, nUsePoint);
}