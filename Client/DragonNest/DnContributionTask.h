#pragma once

#ifdef PRE_ADD_DONATION

// 기부시스템 Task.

#include "Task.h"
#include "MessageListener.h"

// 기부랭커 Data.
struct SContributionRanker
{
	int nRank;
	int nJob;
	std::wstring strGuildName;
	std::wstring strID;
	INT64 nGold;

	SContributionRanker(){}
	SContributionRanker( int rank, int job, wchar_t *guidName, wchar_t *id, INT64 gold )
		: nRank(rank), nJob(job), strGuildName(guidName), strID(id), nGold(gold)
	{}	
};


// 기부보상아이템.
struct SContributionRewardItems
{
	UINT nType : 2; // 월별,일별 구분( 현재2가지구분 ).
	UINT nRank : 4; // 순위.

	// 보상템 5개.
	int nItemID[5];
	WORD nItemCnt[5];	

	SContributionRewardItems()
		: nType(0), nRank(0)
	{
		memset( nItemID, 0, sizeof(int) * 5 );
		memset( nItemCnt, 0, sizeof( WORD ) * 5 );
	}	
	//SContributionRewardItems( SContributionRewardItems & value ){
	//	nType = value.nType; nRank = value.nRank;
	//	memcpy( nItemID, value.nItemID, sizeof(int) );
	//	memcpy( nItemCnt, value.nItemCnt, sizeof(WORD) );
	//}
};

class CDnContributionTask : public CTask, public CTaskListener, public CSingleton<CDnContributionTask>
{

private:

	// 기부랭커 Data List.
	std::vector< SContributionRanker * > m_vListRanker;

	// LocalPlayer.
	int m_nLPRank;
	INT64 m_nLPGold;
	INT64 m_nContributionGold; // 기부요청한금액 - 기부성공시에 LPGold에 추가한다.


	// 기부보상. - 순위별순서.
	std::vector< SContributionRewardItems > m_vRewardMouth; // 월별.
	std::vector< SContributionRewardItems > m_vRewardDay;   // 일별.


public:

	CDnContributionTask();
	~CDnContributionTask(){
		Finalize();
	}

	bool Initialize();
	void Finalize();


	// Override - CTask ------------//
	//virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}
	

	// Override - CTaskListener ----//
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );


private:

	void OnRecvRankListMessage( char * pData ); // 랭킹목록갱신결과패킷.
	
	// 랭킹목록에서 LocalPlayer의 인덱스 반환.
	int GetRListToLPIndex( std::wstring & strID );

	// 랭킹목록 정렬.
	void RankerListSort();

	// 랭킹목록제거.
	void ReleaseRankerList();

public:

	// 랭킹목록갱신패킷 전송.
	void OnSendRefreshRankerList();

	// 기부패킷 전송.
	void OnSendMakeDonations( INT64 nCoin );

	// 기부보상Dlg 가 처음 열릴때 호출.
	// : 보상템을 미리로드할 필요가 없어서 최초Open시에 로드한다.
	void OnFirstOpenRewardDlg();

	// 기부하기 결과.
	void OnRecvContributionResult( char * pData );

	// 기부창 열기.
	void OnRecvShowContribution();

	// 랭킹목록 정렬 - 기부시에 LocalPlayer가 10위권에 속한경우 내목록 소지금을 업데이트한뒤 정렬한다.
	void SortRankerList( INT64 nContributionGold );


	// Ranker List.
	const std::vector< SContributionRanker * > & GetListRanker(){
		return m_vListRanker;
	}
	
	// LocalPlayer.
	int GetLPRank(){
		return m_nLPRank;
	}
	INT64 GetLPGold(){
		return m_nLPGold;
	}	

	// 기부보상 아이템목록.
	const std::vector< SContributionRewardItems > & GetRewardItemsMonth(){ 
		return m_vRewardMouth; // 월별. 
	}
	const std::vector< SContributionRewardItems > & GetRewardItemsDay(){
		return m_vRewardDay;   // 일별.
	}

};

#endif