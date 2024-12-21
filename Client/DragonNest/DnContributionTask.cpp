#include "Stdafx.h"

#ifdef PRE_ADD_DONATION

#include <algorithm>

#include "VillageSendPacket.h"
#include "DnActor.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnContributionDlg.h"
#include "DnContributionTask.h"


// 랭킹목록 정렬. - 동점 처리용.
void CDnContributionTask::RankerListSort()
{	
	int size = (int)m_vListRanker.size();
	for( int i=0; i<size; ++i )
	{
		for( int k=i+1; k<size; ++k )
		{
			// 다음 아이템이 더크다 순서변경.
			if( m_vListRanker[ i ]->nGold < m_vListRanker[ k ]->nGold )
			{
				m_vListRanker[ k ]->nRank = m_vListRanker[ i ]->nRank; // 순위변경.
				_Swap( m_vListRanker[ i ], m_vListRanker[ k ] ); // swap.
			}

			// 동률.
			else if( m_vListRanker[ i ]->nGold == m_vListRanker[ k ]->nGold )
			{
				m_vListRanker[ k ]->nRank = m_vListRanker[ i ]->nRank; // 순위변경.
			}
		}
	}
}

// 랭커목록 Sort. 내림차순.
bool RankerSort( SContributionRanker * lVal, SContributionRanker * rVal ){	
	return rVal->nGold < lVal->nGold;
}

CDnContributionTask::CDnContributionTask()
: CTaskListener( false )
, m_nContributionGold( 0 )
, m_nLPGold( 0 )
, m_nLPRank( 0 )
{
}

bool CDnContributionTask::Initialize()
{
	return true;
}

// 랭킹목록제거.
void CDnContributionTask::ReleaseRankerList()
{
	// 기부랭커.
	int size = (int)m_vListRanker.size();
	for( int i=0; i<size; ++i )
		SAFE_DELETE( m_vListRanker[ i ] );
	m_vListRanker.clear();  
}

void CDnContributionTask::Finalize()
{
	// 랭킹목록제거.
	ReleaseRankerList();

	m_vRewardMouth.clear(); // 월별.
	m_vRewardDay.clear();   // 일별.
}


void CDnContributionTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	if( nMainCmd == eSCMainCmd::SC_DONATION )
	{
		switch( nSubCmd )
		{		
		// 랭커목록 갱신.
		case eDonation::SC_DONATION_RANKING:
			OnRecvRankListMessage( pData );
			break;

		// 기부하기 결과.
		case eDonation::SC_DONATE:
			OnRecvContributionResult( pData );
			break;

		// 기부창 열기.
		case eDonation::SC_DONATION_OPEN:
			OnRecvShowContribution();
			break;
		}
	}
}

 
// 기부창 열기.
void CDnContributionTask::OnRecvShowContribution()
{
	GetInterface().OpenContributionDialog( true );
}


// 기부보상Dlg 가 처음 열릴때 호출.
// : 보상템을 미리로드할 필요가 없어서 최초Open시에 로드한다.
void CDnContributionTask::OnFirstOpenRewardDlg()
{
	//// EXT.
	DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TCONTRIBUTION );
	_ASSERT( pTable && "Table [ donationcompensationtable ] 읽기 실패." );
	DNTableCell * pCell = NULL;

	if( pTable )
	{
		char strField[32]={0,};
		m_vRewardMouth.resize( 11 );
		m_vRewardDay.resize( 11 );

		int size = pTable->GetItemCount();
		_ASSERT( (size<1) && "Table [ donationcompensationtable ] 비어있음." );

		for( int i=0; i<size; ++i )
		{
			SContributionRewardItems rowItem;

			int itemID = pTable->GetItemID( i );
			rowItem.nRank = pTable->GetFieldFromLablePtr( itemID, "_Grade" )->GetInteger();			 
			rowItem.nType = pTable->GetFieldFromLablePtr( itemID, "_Type" )->GetInteger();			

			for( int k=0; k<5; ++k )
			{
				sprintf_s( strField, 32, "_Compensation%d", k + 1 );
				pCell = pTable->GetFieldFromLablePtr( itemID, strField );
				if( pCell )
					rowItem.nItemID[ k ] = pCell->GetInteger();

				sprintf_s( strField, 32, "_CompensationValue%d", k + 1 );
				pCell = pTable->GetFieldFromLablePtr( itemID, strField );
				if( pCell )
					rowItem.nItemCnt[ k ] = pCell->GetInteger();				
			}
			// 월별.
			if( rowItem.nType == 0 )
				m_vRewardMouth[ rowItem.nRank ] = rowItem;
			
			// 일별.
			else if( rowItem.nType == 1 )
				m_vRewardDay[ rowItem.nRank ] = rowItem;
		}

	}

}


// 랭킹목록에서 LocalPlayer의 인덱스 반환.
int CDnContributionTask::GetRListToLPIndex( std::wstring & strID )
{	
	int size = (int)m_vListRanker.size();
	for( int i=0; i<size; ++i )	
	{
		if( m_vListRanker[i]->strID == strID )
		{					
			return i;
		}		
	}	
	return -1;
}

// 랭킹목록 정렬 - 기부시에 LocalPlayer가 10위권에 속한경우 내목록 소지금을 업데이트한뒤 정렬한다.
void CDnContributionTask::SortRankerList( INT64 nContributionGold )
{	
	std::wstring strID = CDnActor::s_hLocalActor->GetName();
	int index = GetRListToLPIndex( strID );
	if( index == -1 )
		return;
	
	// 내 기부금 변경( 내 기부금은 기부시 마다 오지않기 때문에 클라이언트에서 저장계산한다..
	m_nLPGold += m_nContributionGold; // LocalPlayer.
	m_vListRanker[ index ]->nGold += nContributionGold; // List Ranker.
	
	// 정렬.
	std::sort( m_vListRanker.begin(), m_vListRanker.end(), RankerSort );
	// RankerListSort(); 동점처리용 Sort. 동점(같은골드기부) 에 대한 처리를 하지않는 다고 하여 주석처리.
		
	//// 정렬한 뒤 LocalPlayer 의 순위. 정렬용.
	//index = GetRListToLPIndex( strID );
	//if( index != -1 )
	//{	
	//	m_nLPRank = m_vListRanker[ index ]->nRank;
	//}


	// Sort 이후 List의 Rank 재설정.
	int size = (int)m_vListRanker.size();
	for( int i=0; i<size; ++i )
		m_vListRanker[ i ]->nRank = i + 1;

	// LocalPlayer 의 순위.
	index = GetRListToLPIndex( strID );
	if( index != -1 )
		m_nLPRank = m_vListRanker[ index ]->nRank;

}



//-----------------------------------------------
// 패킷결과.

// 랭킹목록갱신 결과.
void CDnContributionTask::OnRecvRankListMessage( char * pData )
{
	// 랭킹목록제거.
	ReleaseRankerList();


	SCDonationRanking * aRankList = ( SCDonationRanking * )pData;
		
	// LocalPlayer.
	m_nLPRank = aRankList->nMyRanking;
	/*if( m_nLPRank == -1 )
		m_nLPRank = 10000;
	else if( m_nLPRank < -1 )
		m_nLPRank = 0;*/
	if( m_nLPRank < 0 )
		m_nLPRank = -1;


	m_nLPGold = aRankList->nMyCoin;	

	// Ranker List.		
	for( int i=0; i<10; ++i )
	{	
		const Donation::Ranking & aRanker = aRankList->Ranking[ i ];	
		if( aRanker.biCharacterID == 0 || _wcsicmp( aRanker.wszCharacterName, L"" ) == 0 )
			break;

		m_vListRanker.push_back( new SContributionRanker( i+1, aRanker.btJobCode, (wchar_t*)(aRanker.wszGuildName), (wchar_t*)(aRanker.wszCharacterName), aRanker.nCoin ) );
	}

	// 기부랭킹목록 갱신.
	CDnContributionDlg *pDlg = static_cast<CDnContributionDlg *>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CONTRIBUTION_DIALOG ) );
	if( pDlg ) 		
		pDlg->RefreshList();
}

// 기부하기 결과.
void CDnContributionTask::OnRecvContributionResult( char * pData )
{
	SCDonate * pMsg = ( SCDonate * )pData;

	if( pMsg->nResult == 0 )
	{
		if( m_nContributionGold >= 10000 )
		{
			wchar_t buf[1024]={0,};
			swprintf_s( buf, 1024, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7332 ), CDnActor::s_hLocalActor->GetName(), m_nContributionGold/10000 );		
			GetInterface().MessageBox( buf );
		}

		// 랭킹목록 정렬.
		SortRankerList( m_nContributionGold );
		m_nContributionGold = 0; // 기부요청금 초기화.

		// 기부Dialog 갱신.
		CDnContributionDlg *pDlg = static_cast<CDnContributionDlg *>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CONTRIBUTION_DIALOG ) );
		if( pDlg ) 		
		{
			pDlg->RefreshList(); // 기부랭킹목록 갱신.		
			pDlg->RefreshContributionGoldDlg(); // 기부하기 Dialog 갱신 - 현재소지금.
		}
	}
	else
	{
		m_nContributionGold = 0; // 기부요청금 초기화.

		// 서버연결이 원활하지 않습니다.
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory2, pMsg->nResult ) );
	}	
}


//-----------------------------------------------------------------
// 패킷전송.

// 기부패킷 전송.
void CDnContributionTask::OnSendMakeDonations( INT64 nCoin )
{
	m_nContributionGold = nCoin; // 기부요청금 저장.
	SendMakeDonationsMsg( nCoin );
}


// 랭킹목록갱신패킷 전송.
void CDnContributionTask::OnSendRefreshRankerList()
{
	
	SendRefreshRankerList();
}


#endif


