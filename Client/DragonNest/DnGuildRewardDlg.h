#pragma once

#include "DnCustomDlg.h"
#include "DnTableDB.h"
#include "DnGuildRewardBuyConfirmDlg.h"

class CEtUITextBox;

class CDnGuildRewardTooltipDlg : public CEtUIDialog
{
public:
	CDnGuildRewardTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRewardTooltipDlg(void);

protected:
	//bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CEtUITextBox *m_pContents;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );

	bool SetMarkTooltip(const GuildReward::GuildRewardInfo &info);
	bool AdjustPosition(float orgX, float orgY);
};

class CDnGuildRewardItem;
class CDnGuildRewardDlg : public CDnCustomDlg
{
public:
	CDnGuildRewardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRewardDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

protected:
	typedef list<GuildReward::GuildRewardInfo> GUILDREWARD_LIST;
	GUILDREWARD_LIST m_GuildRewardList;

	CEtUIListBoxEx *m_pListBoxEx;
	CEtUIStatic *m_pGuildLevel;
	CEtUIStatic *m_pStaticPage;
	CEtUIButton *m_pButtonBuy;

	CEtUIComboBox *m_pComboFilter;
	CEtUICheckBox *m_pPurchaseOnly;

	void UpdateTotalPage();
	void UpdateCurrentPage();
	void UpdatePageInfo();
	void UpdatePage(int nCurPage);
	void RefreshPageInfo();


	int m_nTotalPage;
	int m_nCurrentPage;
	int m_nSelectedIndex;

	int m_nElementCountPerPage;

	int m_nGuildLevel;

	CDnGuildRewardItem* GetSelectedRewardItem();
	void UpdateSelectedGuildRewardInfo();

	CDnGuildRewardBuyConfirmDlg* m_pConfirmDlg;

	CDnGuildRewardTooltipDlg* m_pToolTipDlg;

	int GetSortStringNum(int type);
	void AddFilterTypeData();

	int GetFilterType();
	bool GetPurchaseOnly();
public:
	//길드 보상 정보 설정용..
	void InitList();
	void AddGuildRewardInfo(GuildReward::GuildRewardInfo &info, bool bUpdatePage = true);

	//길드 레벨
	void SetGuildLevel(int nLevel);


	void ShowRewardList();
	void CloseBuyGuildRewardConfirmDlg();
};