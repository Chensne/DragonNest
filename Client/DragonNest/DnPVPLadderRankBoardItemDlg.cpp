#include "StdAfx.h"
#include "DnPVPLadderRankBoardItemDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPLadderBoardListItemDlg::CDnPVPLadderBoardListItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
}

CDnPVPLadderBoardListItemDlg::~CDnPVPLadderBoardListItemDlg()
{
}

void CDnPVPLadderBoardListItemDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpLadderRankBoardListDlg.ui" ).c_str(), bShow );
}

void CDnPVPLadderBoardListItemDlg::InitialUpdate()
{
	CDnCustomDlg::InitialUpdate();
	InitializeRankInfo(m_sUILadderInfo);
}

void CDnPVPLadderBoardListItemDlg::SetUILadderInfo(TPvPLadderRanking *pInfo)
{
	WCHAR wszStr[128];

	m_sUILadderInfo.pStatic_JobIcon->SetIconID(pInfo->cJobCode);
	m_sUILadderInfo.pStatic_LadderCount->SetIntToText(pInfo->iPvPLadderGradePoint);
	m_sUILadderInfo.pStatic_Name->SetText(pInfo->wszCharName);

	wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 809 ),pInfo->iRank );
	m_sUILadderInfo.pStatic_Rank->SetText(wszStr);

	wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126039 ),pInfo->iWin , pInfo->iLose ,pInfo->iDraw );
	m_sUILadderInfo.pStatic_WinCount->SetText(wszStr);

	int TotalCount = (pInfo->iWin + pInfo->iLose + pInfo->iDraw) ? (pInfo->iWin + pInfo->iLose + pInfo->iDraw) : 1;
	wsprintf(wszStr,L"%d%%", (int)((float)pInfo->iWin/(float)TotalCount * 100) );
	m_sUILadderInfo.pStatic_WinPercent->SetText(wszStr);
}

void CDnPVPLadderBoardListItemDlg::InitializeRankInfo(CDnPVPLadderRankBoardDlg::sPVPLadderRankInfoUI &pRankInfo)
{
	pRankInfo.pStatic_Name = GetControl<CEtUIStatic>("ID_TEXT_RANK_NAME");               // ÀÌ¸§
	pRankInfo.pStatic_JobIcon = GetControl<CDnJobIconStatic>("ID_RANK_CLASS");                // Á÷¾÷
	pRankInfo.pStatic_Rank = GetControl<CEtUIStatic>("ID_TEXT_RANK");                         // ·©Å©
	pRankInfo.pStatic_WinCount = GetControl<CEtUIStatic>("ID_TEXT_RANK_WINCOUNT");       // ½Â¹«ÆÐ
	pRankInfo.pStatic_LadderCount =GetControl<CEtUIStatic>("ID_TEXT_RANK_COUNT");        // ·¹´õ ÆòÁ¡
	pRankInfo.pStatic_WinPercent = GetControl<CEtUIStatic>("ID_TEXT_RANK_COUNTPERCENT"); // ½Â·ü
}
