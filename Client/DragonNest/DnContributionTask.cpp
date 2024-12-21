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


// ��ŷ��� ����. - ���� ó����.
void CDnContributionTask::RankerListSort()
{	
	int size = (int)m_vListRanker.size();
	for( int i=0; i<size; ++i )
	{
		for( int k=i+1; k<size; ++k )
		{
			// ���� �������� ��ũ�� ��������.
			if( m_vListRanker[ i ]->nGold < m_vListRanker[ k ]->nGold )
			{
				m_vListRanker[ k ]->nRank = m_vListRanker[ i ]->nRank; // ��������.
				_Swap( m_vListRanker[ i ], m_vListRanker[ k ] ); // swap.
			}

			// ����.
			else if( m_vListRanker[ i ]->nGold == m_vListRanker[ k ]->nGold )
			{
				m_vListRanker[ k ]->nRank = m_vListRanker[ i ]->nRank; // ��������.
			}
		}
	}
}

// ��Ŀ��� Sort. ��������.
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

// ��ŷ�������.
void CDnContributionTask::ReleaseRankerList()
{
	// ��η�Ŀ.
	int size = (int)m_vListRanker.size();
	for( int i=0; i<size; ++i )
		SAFE_DELETE( m_vListRanker[ i ] );
	m_vListRanker.clear();  
}

void CDnContributionTask::Finalize()
{
	// ��ŷ�������.
	ReleaseRankerList();

	m_vRewardMouth.clear(); // ����.
	m_vRewardDay.clear();   // �Ϻ�.
}


void CDnContributionTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	if( nMainCmd == eSCMainCmd::SC_DONATION )
	{
		switch( nSubCmd )
		{		
		// ��Ŀ��� ����.
		case eDonation::SC_DONATION_RANKING:
			OnRecvRankListMessage( pData );
			break;

		// ����ϱ� ���.
		case eDonation::SC_DONATE:
			OnRecvContributionResult( pData );
			break;

		// ���â ����.
		case eDonation::SC_DONATION_OPEN:
			OnRecvShowContribution();
			break;
		}
	}
}

 
// ���â ����.
void CDnContributionTask::OnRecvShowContribution()
{
	GetInterface().OpenContributionDialog( true );
}


// ��κ���Dlg �� ó�� ������ ȣ��.
// : �������� �̸��ε��� �ʿ䰡 ��� ����Open�ÿ� �ε��Ѵ�.
void CDnContributionTask::OnFirstOpenRewardDlg()
{
	//// EXT.
	DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TCONTRIBUTION );
	_ASSERT( pTable && "Table [ donationcompensationtable ] �б� ����." );
	DNTableCell * pCell = NULL;

	if( pTable )
	{
		char strField[32]={0,};
		m_vRewardMouth.resize( 11 );
		m_vRewardDay.resize( 11 );

		int size = pTable->GetItemCount();
		_ASSERT( (size<1) && "Table [ donationcompensationtable ] �������." );

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
			// ����.
			if( rowItem.nType == 0 )
				m_vRewardMouth[ rowItem.nRank ] = rowItem;
			
			// �Ϻ�.
			else if( rowItem.nType == 1 )
				m_vRewardDay[ rowItem.nRank ] = rowItem;
		}

	}

}


// ��ŷ��Ͽ��� LocalPlayer�� �ε��� ��ȯ.
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

// ��ŷ��� ���� - ��νÿ� LocalPlayer�� 10���ǿ� ���Ѱ�� ����� �������� ������Ʈ�ѵ� �����Ѵ�.
void CDnContributionTask::SortRankerList( INT64 nContributionGold )
{	
	std::wstring strID = CDnActor::s_hLocalActor->GetName();
	int index = GetRListToLPIndex( strID );
	if( index == -1 )
		return;
	
	// �� ��α� ����( �� ��α��� ��ν� ���� �����ʱ� ������ Ŭ���̾�Ʈ���� �������Ѵ�..
	m_nLPGold += m_nContributionGold; // LocalPlayer.
	m_vListRanker[ index ]->nGold += nContributionGold; // List Ranker.
	
	// ����.
	std::sort( m_vListRanker.begin(), m_vListRanker.end(), RankerSort );
	// RankerListSort(); ����ó���� Sort. ����(���������) �� ���� ó���� �����ʴ� �ٰ� �Ͽ� �ּ�ó��.
		
	//// ������ �� LocalPlayer �� ����. ���Ŀ�.
	//index = GetRListToLPIndex( strID );
	//if( index != -1 )
	//{	
	//	m_nLPRank = m_vListRanker[ index ]->nRank;
	//}


	// Sort ���� List�� Rank �缳��.
	int size = (int)m_vListRanker.size();
	for( int i=0; i<size; ++i )
		m_vListRanker[ i ]->nRank = i + 1;

	// LocalPlayer �� ����.
	index = GetRListToLPIndex( strID );
	if( index != -1 )
		m_nLPRank = m_vListRanker[ index ]->nRank;

}



//-----------------------------------------------
// ��Ŷ���.

// ��ŷ��ϰ��� ���.
void CDnContributionTask::OnRecvRankListMessage( char * pData )
{
	// ��ŷ�������.
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

	// ��η�ŷ��� ����.
	CDnContributionDlg *pDlg = static_cast<CDnContributionDlg *>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CONTRIBUTION_DIALOG ) );
	if( pDlg ) 		
		pDlg->RefreshList();
}

// ����ϱ� ���.
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

		// ��ŷ��� ����.
		SortRankerList( m_nContributionGold );
		m_nContributionGold = 0; // ��ο�û�� �ʱ�ȭ.

		// ���Dialog ����.
		CDnContributionDlg *pDlg = static_cast<CDnContributionDlg *>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CONTRIBUTION_DIALOG ) );
		if( pDlg ) 		
		{
			pDlg->RefreshList(); // ��η�ŷ��� ����.		
			pDlg->RefreshContributionGoldDlg(); // ����ϱ� Dialog ���� - ���������.
		}
	}
	else
	{
		m_nContributionGold = 0; // ��ο�û�� �ʱ�ȭ.

		// ���������� ��Ȱ���� �ʽ��ϴ�.
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory2, pMsg->nResult ) );
	}	
}


//-----------------------------------------------------------------
// ��Ŷ����.

// �����Ŷ ����.
void CDnContributionTask::OnSendMakeDonations( INT64 nCoin )
{
	m_nContributionGold = nCoin; // ��ο�û�� ����.
	SendMakeDonationsMsg( nCoin );
}


// ��ŷ��ϰ�����Ŷ ����.
void CDnContributionTask::OnSendRefreshRankerList()
{
	
	SendRefreshRankerList();
}


#endif


