#pragma once
#include "DnCustomDlg.h"
#include "DnPVPLadderRankBoardDlg.h"

class CDnPVPLadderBoardListItemDlg : public CDnCustomDlg
{
public:
	CDnPVPLadderBoardListItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPLadderBoardListItemDlg();

protected:
	CDnPVPLadderRankBoardDlg::sPVPLadderRankInfoUI m_sUILadderInfo;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	void SetUILadderInfo(TPvPLadderRanking *pInfo);
	void InitializeRankInfo(CDnPVPLadderRankBoardDlg::sPVPLadderRankInfoUI &pRankInfo);
};
