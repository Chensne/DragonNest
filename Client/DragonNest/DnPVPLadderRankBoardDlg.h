#pragma once

#include "DnCustomDlg.h"
#include "DnBlindDlg.h"

class CDnPVPLadderRankBoardDlg : public CDnCustomDlg, public CDnBlindCallBack
{
public:
	CDnPVPLadderRankBoardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPVPLadderRankBoardDlg();

	enum ePVPLadderRankCommonValue
	{
		TopPlayerNumber = 3, // 1등 2등 3등 합산하여 3명은 , 레더아이템 다이얼로그가 아닌 직접 스태틱으로 그려진다.
	};

	struct sPVPLadderRankInfoUI
	{
		CEtUIStatic *pStatic_Name;
		CDnJobIconStatic *pStatic_JobIcon;
		CEtUIStatic *pStatic_Rank;
		CEtUIStatic *pStatic_WinCount;
		CEtUIStatic *pStatic_LadderCount;
		CEtUIStatic *pStatic_WinPercent;
		
		sPVPLadderRankInfoUI()
		{
			pStatic_Name = NULL;
			pStatic_JobIcon = NULL;
			pStatic_Rank = NULL;
			pStatic_WinCount = NULL;
			pStatic_LadderCount = NULL;
			pStatic_WinPercent = NULL;
		}
	
	};

protected:
	
	bool m_bRankInfoReqeust;
	sPVPLadderRankInfoUI m_sMyPVPLadderRankInfo;
	sPVPLadderRankInfoUI m_sTopPlayerPVPLadderRankInfo[TopPlayerNumber];
	CEtUIStatic *m_pStatic_RankTitle;
	CEtUIListBoxEx *m_pListBoxEX_UserList;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void InitializeRankInfo(sPVPLadderRankInfoUI &pRankInfo);
	void InitializeRankInfoByCount(int nUserNumber,sPVPLadderRankInfoUI &pRankInfo);

	void SetUILadderInfo(TPvPLadderRanking *pInfo,sPVPLadderRankInfoUI &RankInfo);
	void SetPVPLadderRankInfo(SCPvPLadderRankBoard* pData);
	void SetPVPLadderRankMatchType(int nMatchType);

	void SetRankInfoRequest( bool bRequest ) { m_bRankInfoReqeust = bRequest; }
	bool IsRankInfoRequest() { return m_bRankInfoReqeust; }

public:

};
