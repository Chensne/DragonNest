#pragma once

#include "DnStageClearImp.h"

class CDnDarklairClearImp : public CDnStageClearImp
{
public:
	CDnDarklairClearImp( CDnGameTask *pTask );
	virtual ~CDnDarklairClearImp();

	struct SDungeonClearInfo
	{
		TDLDungeonClearInfo sBaseInfo;
		char cRewardItemType[4];
		TItem RewardItem[4];
		/*
		int nRewardItemID[4];
		int nRewardItemRandomSeed[4];
		char cRewardItemOption[4];
		*/

		int nClassID;
		int nJobID;
		std::wstring strName;

		SDungeonClearInfo()
			: nClassID(0)
			, nJobID(0)
		{
			SecureZeroMemory( &sBaseInfo, sizeof(TDLDungeonClearInfo) );
			memset( cRewardItemType, 0, sizeof(cRewardItemType) );
//			memset( &nRewardItemID, 0, sizeof(nRewardItemID) );
			memset( RewardItem, 0, sizeof(RewardItem) );
		}
	};
	struct SDungeonClearValue
	{
		int nClearRound;
		int nClearTime;
		int nPartyCount;
		bool bClear;
		std::wstring strTime;
	};

protected:
	SDungeonClearValue m_DungeonClearValue;
	std::vector<SDungeonClearInfo> m_vecDungeonClearInfo;

	TDLRankHistoryPartyInfo m_CurScore;
	TDLRankHistoryPartyInfo m_TopScore;
	TDLRankHistoryPartyInfo m_HistoryScore[DarkLair::Rank::SelectTop];

protected:
	void CalcDungeonClearInfo();

public:
	virtual void OnCalcClearProcess( void *pData );
	virtual void OnCalcClearRewardItemProcess( void *pData );
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	virtual void OnCalcClearRewardBoxProcess( void *pData );
#endif

	void OnRecvRoomDLDungeonClearRankInfo( SCDLRankInfo *pPacket );

	std::vector<SDungeonClearInfo>& GetDungeonClearInfoList() { return m_vecDungeonClearInfo; }
	SDungeonClearInfo* GetDungeonClearInfo( int nSessionID );
	SDungeonClearValue& GetDungeonClearValue() { return m_DungeonClearValue; }

	TDLRankHistoryPartyInfo *GetCurrentScoreInfo() { return &m_CurScore; }
	TDLRankHistoryPartyInfo *GetUserTopScoreInfo() { return &m_TopScore; }
	TDLRankHistoryPartyInfo *GetTopHistoryInfo( int nIndex ) { return &m_HistoryScore[nIndex]; }
};