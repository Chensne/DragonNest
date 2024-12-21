#include "StdAfx.h"
#include "DnDarklairclearImp.h"
#include "DnDLGameTask.h"
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"
#include "TaskManager.h"
#include "DnPartyTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 


CDnDarklairClearImp::CDnDarklairClearImp( CDnGameTask *pTask )
: CDnStageClearImp( pTask )
{
	memset( &m_CurScore, 0, sizeof(TDLRankHistoryPartyInfo) );
	memset( &m_TopScore, 0, sizeof(TDLRankHistoryPartyInfo) );
	memset( m_HistoryScore, 0, sizeof(TDLRankHistoryPartyInfo) * DarkLair::Rank::SelectTop );
}

CDnDarklairClearImp::~CDnDarklairClearImp()
{
}

void CDnDarklairClearImp::OnCalcClearProcess( void *pData )
{
	SCDLDungeonClear *pPacket = (SCDLDungeonClear *)pData;

	// Note : 파티원들의 던젼 정보를 셋팅하고 던젼클리어창을 열자.
	m_vecDungeonClearInfo.clear();
	memset( &m_DungeonClearValue, 0, sizeof(m_DungeonClearValue) );

	m_DungeonClearValue.nClearRound = pPacket->nClearRound;
	m_DungeonClearValue.nClearTime = pPacket->nClearTime;
	m_DungeonClearValue.bClear = pPacket->bClear;

	SDungeonClearInfo dungeonClearInfo;

	for( int i=0; i<pPacket->cCount; i++ )
	{
		dungeonClearInfo.sBaseInfo = pPacket->Info[i];
		m_vecDungeonClearInfo.push_back(dungeonClearInfo);
	}

	CalcDungeonClearInfo();
	GetInterface().OpenDarkLairClearReportDialog( m_pGameTask );

	CDnLocalPlayerActor::StopAllPartyPlayer();
}

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
void CDnDarklairClearImp::OnCalcClearRewardBoxProcess(void* pData)
{
	SCDungeonClearBoxType *pPacket = static_cast<SCDungeonClearBoxType*>(pData);
	for (int i=0; i < pPacket->cCount; i++)
	{
		for( DWORD j=0; j<m_vecDungeonClearInfo.size(); j++ )
		{
			if( m_vecDungeonClearInfo[j].sBaseInfo.nSessionID == pPacket->Info[i].nSessionID )
			{
				memcpy( m_vecDungeonClearInfo[j].cRewardItemType, pPacket->Info[i].cRewardItemType, sizeof(pPacket->Info[i].cRewardItemType) );
				break;
			}
		}
	}
}
#endif

void CDnDarklairClearImp::OnCalcClearRewardItemProcess( void *pData )
{
	SCDungeonClearRewardItem *pPacket = (SCDungeonClearRewardItem *)pData;
	for( int i=0; i<pPacket->cCount; i++ )
	{
		for( DWORD j=0; j<m_vecDungeonClearInfo.size(); j++ ) {
			if( m_vecDungeonClearInfo[j].sBaseInfo.nSessionID == pPacket->Info[i].nSessionID ) {
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
#else
				memcpy( m_vecDungeonClearInfo[j].cRewardItemType, pPacket->Info[i].cRewardItemType, sizeof(pPacket->Info[i].cRewardItemType) );
#endif
				for( int k=0; k<4; k++ ) {
					CDnItem::MakeItemInfo( pPacket->Info[i].nRewardItemID[k], 1, m_vecDungeonClearInfo[j].RewardItem[k] );
					m_vecDungeonClearInfo[j].RewardItem[k].nRandomSeed = pPacket->Info[i].nRewardItemRandomSeed[k];
					m_vecDungeonClearInfo[j].RewardItem[k].cOption = pPacket->Info[i].cRewardItemOption[k];
					m_vecDungeonClearInfo[j].RewardItem[k].wCount = pPacket->Info[i].wRewardItemCount[k];
					m_vecDungeonClearInfo[j].RewardItem[k].bSoulbound = pPacket->Info[i].cRewardItemSoulBound[k] ? true : false;

#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
					m_vecDungeonClearInfo[j].RewardItem[k].cLevel = pPacket->Info[i].cRewardItemLevel[k];
					m_vecDungeonClearInfo[j].RewardItem[k].cPotential = pPacket->Info[i].cRewardItemPotential[k];
#endif
				}
				break;
			}
		}
	}
}

void CDnDarklairClearImp::CalcDungeonClearInfo()
{
	CDnPartyTask *pPartyTask = (CDnPartyTask*)CTaskManager::GetInstance().GetTask( "PartyTask" );
	if( !pPartyTask ) return;

	m_DungeonClearValue.nPartyCount = pPartyTask->GetPartyCount();

	wchar_t wszTemp[64] = {0};
	CDnPartyTask::PartyStruct *pPartyStruct(NULL);

	for( int i=0; i<(int)m_vecDungeonClearInfo.size(); i++ )
	{
		// Note : 기본 정보 셋팅
		//
		{
			pPartyStruct = pPartyTask->GetPartyDataFromSessionID( m_vecDungeonClearInfo[i].sBaseInfo.nSessionID );
			if( !pPartyStruct ) continue;

			m_vecDungeonClearInfo[i].nClassID = pPartyStruct->cClassID;
			m_vecDungeonClearInfo[i].nJobID = pPartyStruct->nVecJobHistoryList[ pPartyStruct->nVecJobHistoryList.size() - 1 ];
			m_vecDungeonClearInfo[i].strName = pPartyStruct->wszCharacterName;
		}
	}

	int nClearTime = m_DungeonClearValue.nClearTime / 1000;
	int nH = nClearTime / (60 * 60);
	int nM = (nClearTime - (nH * 60 * 60)) / 60;
	int nS = (nClearTime - ((nH * 60 * 60) + (nM * 60)));

	swprintf_s( wszTemp, _countof(wszTemp), L"%02d : %02d : %02d", nH, nM, nS );
	m_DungeonClearValue.strTime = wszTemp;
}

CDnDarklairClearImp::SDungeonClearInfo* CDnDarklairClearImp::GetDungeonClearInfo( int nSessionID )
{
	for( int i=0; i<(int)m_vecDungeonClearInfo.size(); i++ )
	{
		if( m_vecDungeonClearInfo[i].sBaseInfo.nSessionID == nSessionID )
		{
			return &m_vecDungeonClearInfo[i];
		}
	}

	return NULL;
}

void CDnDarklairClearImp::OnRecvRoomDLDungeonClearRankInfo( SCDLRankInfo *pPacket )
{
	memcpy( &m_CurScore, &pPacket->CurScore, sizeof(TDLRankHistoryPartyInfo) );
	memcpy( &m_TopScore, &pPacket->TopScore, sizeof(TDLRankHistoryPartyInfo) );
	memcpy( m_HistoryScore, pPacket->HistoryScore, sizeof(TDLRankHistoryPartyInfo) * DarkLair::Rank::SelectTop );

	GetInterface().OpenDarkLairClearRankDialog();
}