#pragma once

#include "DnCustomDlg.h"
#include "DnTableDB.h"

class CDnItem;
class CDnGuildRewardItem : public CDnCustomDlg
{
public:
	CDnGuildRewardItem( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRewardItem(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual void Process( float fElapsedTime );

protected:
	GuildReward::GuildRewardInfo m_GuildRewardInfo;	//길드보상 정보
	bool m_IsAvailableReward;		//활성화 여부
	bool m_IsPurchased;		//구입 여부
	bool m_isAppliedRewardBetterThan;	//

	CDnItemSlotButton* m_pItemSlot;	//길드 보상 아이템 슬롯.
	CDnItem* m_pTempItem;			//길드 보상 아이템

	CEtUIStatic *m_pMasterLimit;	//길드장 제한 아이콘 표시
	CEtUIStatic *m_pLevelLimit;		//길드 레벨 제한 정보
	CEtUIStatic *m_pRewardInfo;		//길드 보상 이름?
	CEtUIStatic *m_pPeriodInfo;		//기간 정보
	CEtUIStatic *m_pGold;			//구매 금액 골드
	CEtUIStatic *m_pSilver;			//구매 금액 실버
	CEtUIStatic *m_pBronze;			//구매 금액 구리

	void UpdateInfo();
	void SetRewardItemInfo( const GuildReward::GuildRewardInfo &info );

public:
	void SetGuildRewardInfo(const GuildReward::GuildRewardInfo &info);

	void SetNeedGoldInfo(DWORD dwColor);

	bool IsActivated() { return m_IsAvailableReward; }
	bool IsPurchased() { return m_IsPurchased; }

	const GuildReward::GuildRewardInfo& GetGuildRewardInfo() { return m_GuildRewardInfo; }
};