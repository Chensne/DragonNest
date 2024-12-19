#include "StdAfx.h"
#include "TaskManager.h"
#include "DnPVPLadderRankBoardDlg.h"
#include "DnPVPLadderRankBoardItemDlg.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPLadderRankBoardDlg::CDnPVPLadderRankBoardDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pListBoxEX_UserList = NULL;
	m_pStatic_RankTitle = NULL;
	m_bRankInfoReqeust = false;
}

CDnPVPLadderRankBoardDlg::~CDnPVPLadderRankBoardDlg()
{

}

void CDnPVPLadderRankBoardDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();
	m_pListBoxEX_UserList = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_RANK");

 	InitializeRankInfo(m_sMyPVPLadderRankInfo);
 	for(int i=0; i<TopPlayerNumber ; i++)
 		InitializeRankInfoByCount(i,m_sTopPlayerPVPLadderRankInfo[i]);

	m_pStatic_RankTitle = GetControl<CEtUIStatic>("ID_TEXT_RANKTITLE");
}

void CDnPVPLadderRankBoardDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpLadderRankBoardDlg.ui" ).c_str(), bShow );
}

void CDnPVPLadderRankBoardDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
	DnActorHandle hNpc = CDnActor::FindActorFromUniqueID( nUID );

	if( hNpc )
	{
		if( bShow )
			hNpc->SetActionQueue( "Open" );
		else
		{
			hNpc->SetActionQueue( "Close" );
			GetInterface().CloseBlind();
		}
	}

	if( !bShow )
	{
		m_pListBoxEX_UserList->RemoveAllItems();
		GetInterface().CloseBlind();
		//GetInterface().CloseNpcTalkReturnDlg();
	}

	CEtUIDialog::Show( bShow );
}

void CDnPVPLadderRankBoardDlg::SetUILadderInfo(TPvPLadderRanking *pInfo,sPVPLadderRankInfoUI &RankInfo)
{
	WCHAR wszStr[128];

	RankInfo.pStatic_JobIcon->SetIconID(pInfo->cJobCode);
	RankInfo.pStatic_LadderCount->SetIntToText(pInfo->iPvPLadderGradePoint);
	RankInfo.pStatic_Name->SetText(pInfo->wszCharName);

	wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 809 ),pInfo->iRank );
	RankInfo.pStatic_Rank->SetText(wszStr);
		
	wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126039 ),pInfo->iWin , pInfo->iLose ,pInfo->iDraw );
	RankInfo.pStatic_WinCount->SetText(wszStr);

	int TotalCount = (pInfo->iWin + pInfo->iLose + pInfo->iDraw) ? (pInfo->iWin + pInfo->iLose + pInfo->iDraw) : 1;
	wsprintf(wszStr,L"%d%%", (int)((float)pInfo->iWin/(float)TotalCount * 100) );
	RankInfo.pStatic_WinPercent->SetText(wszStr);
}

void CDnPVPLadderRankBoardDlg::SetPVPLadderRankInfo(SCPvPLadderRankBoard* pData)
{
	SetUILadderInfo( &pData->MyRanking ,m_sMyPVPLadderRankInfo); // 내랭크 
	for(int i=0; i<TopPlayerNumber ;i++)
		SetUILadderInfo( &pData->Top[i] ,m_sTopPlayerPVPLadderRankInfo[i]);

	for(int i=TopPlayerNumber; i<LadderSystem::Common::RankingTop ; i++)
	{
		CDnPVPLadderBoardListItemDlg *pItem = m_pListBoxEX_UserList->InsertItem<CDnPVPLadderBoardListItemDlg>(i - TopPlayerNumber);
		pItem->SetUILadderInfo(&pData->Top[i]);
	}
	m_bRankInfoReqeust = false;
}

void CDnPVPLadderRankBoardDlg::SetPVPLadderRankMatchType(int nMatchType)
{
	switch(nMatchType)
	{
	case LadderSystem::MatchType::_1vs1:
		m_pStatic_RankTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126178) );
		break;
	case LadderSystem::MatchType::_2vs2:
		m_pStatic_RankTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126179) );
		break;
	case LadderSystem::MatchType::_3vs3:
		m_pStatic_RankTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126180) );
		break;
	case LadderSystem::MatchType::_4vs4:
		m_pStatic_RankTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126181) );
		break;
	default:
		m_pStatic_RankTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126178) );
		break;
	}
}

bool CDnPVPLadderRankBoardDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	// 종료버튼이 없으므로 이렇게 MsgProc에서 처리하겠다.
	// 그래도 안보이는 버튼으로 처리하는게 낫긴 날테니, 담에는 이런 식으로 하지 않는게 좋을거 같다.
	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show( false );
			return true;
		}
	}

	return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnPVPLadderRankBoardDlg::InitializeRankInfoByCount(int nUserNumber,sPVPLadderRankInfoUI &pRankInfo)
{
	char wszUIName[256];

	sprintf(wszUIName, "ID_TEXT_RANK_NAME%d", nUserNumber);
	pRankInfo.pStatic_Name = GetControl<CEtUIStatic>(wszUIName);       // 이름

	sprintf(wszUIName, "ID_RANK_CLASS%d", nUserNumber);
	pRankInfo.pStatic_JobIcon = GetControl<CDnJobIconStatic>(wszUIName);    // 직업

	sprintf(wszUIName, "ID_TEXT_RANK%d", nUserNumber);
	pRankInfo.pStatic_Rank = GetControl<CEtUIStatic>(wszUIName);       // 랭크

	sprintf(wszUIName, "ID_TEXT_RANK_WINCOUNT%d", nUserNumber);
	pRankInfo.pStatic_WinCount = GetControl<CEtUIStatic>(wszUIName);   // 승무패

	sprintf(wszUIName, "ID_TEXT_RANK_COUNT%d", nUserNumber);
	pRankInfo.pStatic_LadderCount =GetControl<CEtUIStatic>(wszUIName); // 레더 평점

	sprintf(wszUIName, "ID_TEXT_RANK_COUNTPERCENT%d", nUserNumber);         
	pRankInfo.pStatic_WinPercent = GetControl<CEtUIStatic>(wszUIName); // 승률
}

void CDnPVPLadderRankBoardDlg::InitializeRankInfo(sPVPLadderRankInfoUI &pRankInfo)
{
	pRankInfo.pStatic_Name = GetControl<CEtUIStatic>("ID_TEXT_RANK_NAME");               // 이름
	pRankInfo.pStatic_JobIcon = GetControl<CDnJobIconStatic>("ID_RANK_CLASS");                // 직업
	pRankInfo.pStatic_Rank = GetControl<CEtUIStatic>("ID_TEXT_RANK");                         // 랭크
	pRankInfo.pStatic_WinCount = GetControl<CEtUIStatic>("ID_TEXT_RANK_WINCOUNT");       // 승무패
	pRankInfo.pStatic_LadderCount =GetControl<CEtUIStatic>("ID_TEXT_RANK_COUNT");        // 레더 평점
	pRankInfo.pStatic_WinPercent = GetControl<CEtUIStatic>("ID_TEXT_RANK_COUNTPERCENT"); // 승률
}
