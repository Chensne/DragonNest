#pragma once

#include "DnCustomDlg.h"
#include "DnTableDB.h"

class CDnItem;
class CDnItemSlotButton;
class CDnGuildRewardBuyConfirmDlg : public CDnCustomDlg
{
public:
	CDnGuildRewardBuyConfirmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRewardBuyConfirmDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

protected:
	GuildReward::GuildRewardInfo m_GuildRewardInfo;	//길드보상 정보
	
	CEtUIStatic *m_pWarringMsg;		//경고 메시지?
	
	CDnItemSlotButton* m_pItemSlot;	//길드 보상 아이템 슬롯.
	CDnItem* m_pTempItem;			//길드 보상 아이템

	CEtUIStatic *m_pRewardInfo;		//길드 보상 이름?
	CEtUIStatic *m_pPeriodInfo;		//기간 정보
	CEtUIStatic *m_pGold;			//구매 금액 골드
	CEtUIStatic *m_pSilver;			//구매 금액 실버
	CEtUIStatic *m_pBronze;			//구매 금액 구리
	
	void SetRewardItemInfo( const GuildReward::GuildRewardInfo &info );

public:
	void SetGuildRewardInfo(const GuildReward::GuildRewardInfo &info);
	void SetNeedGoldInfo();

};