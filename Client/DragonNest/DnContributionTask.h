#pragma once

#ifdef PRE_ADD_DONATION

// ��νý��� Task.

#include "Task.h"
#include "MessageListener.h"

// ��η�Ŀ Data.
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


// ��κ��������.
struct SContributionRewardItems
{
	UINT nType : 2; // ����,�Ϻ� ����( ����2�������� ).
	UINT nRank : 4; // ����.

	// ������ 5��.
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

	// ��η�Ŀ Data List.
	std::vector< SContributionRanker * > m_vListRanker;

	// LocalPlayer.
	int m_nLPRank;
	INT64 m_nLPGold;
	INT64 m_nContributionGold; // ��ο�û�ѱݾ� - ��μ����ÿ� LPGold�� �߰��Ѵ�.


	// ��κ���. - ����������.
	std::vector< SContributionRewardItems > m_vRewardMouth; // ����.
	std::vector< SContributionRewardItems > m_vRewardDay;   // �Ϻ�.


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

	void OnRecvRankListMessage( char * pData ); // ��ŷ��ϰ��Ű����Ŷ.
	
	// ��ŷ��Ͽ��� LocalPlayer�� �ε��� ��ȯ.
	int GetRListToLPIndex( std::wstring & strID );

	// ��ŷ��� ����.
	void RankerListSort();

	// ��ŷ�������.
	void ReleaseRankerList();

public:

	// ��ŷ��ϰ�����Ŷ ����.
	void OnSendRefreshRankerList();

	// �����Ŷ ����.
	void OnSendMakeDonations( INT64 nCoin );

	// ��κ���Dlg �� ó�� ������ ȣ��.
	// : �������� �̸��ε��� �ʿ䰡 ��� ����Open�ÿ� �ε��Ѵ�.
	void OnFirstOpenRewardDlg();

	// ����ϱ� ���.
	void OnRecvContributionResult( char * pData );

	// ���â ����.
	void OnRecvShowContribution();

	// ��ŷ��� ���� - ��νÿ� LocalPlayer�� 10���ǿ� ���Ѱ�� ����� �������� ������Ʈ�ѵ� �����Ѵ�.
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

	// ��κ��� �����۸��.
	const std::vector< SContributionRewardItems > & GetRewardItemsMonth(){ 
		return m_vRewardMouth; // ����. 
	}
	const std::vector< SContributionRewardItems > & GetRewardItemsDay(){
		return m_vRewardDay;   // �Ϻ�.
	}

};

#endif