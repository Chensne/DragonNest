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
	GuildReward::GuildRewardInfo m_GuildRewardInfo;	//��庸�� ����
	
	CEtUIStatic *m_pWarringMsg;		//��� �޽���?
	
	CDnItemSlotButton* m_pItemSlot;	//��� ���� ������ ����.
	CDnItem* m_pTempItem;			//��� ���� ������

	CEtUIStatic *m_pRewardInfo;		//��� ���� �̸�?
	CEtUIStatic *m_pPeriodInfo;		//�Ⱓ ����
	CEtUIStatic *m_pGold;			//���� �ݾ� ���
	CEtUIStatic *m_pSilver;			//���� �ݾ� �ǹ�
	CEtUIStatic *m_pBronze;			//���� �ݾ� ����
	
	void SetRewardItemInfo( const GuildReward::GuildRewardInfo &info );

public:
	void SetGuildRewardInfo(const GuildReward::GuildRewardInfo &info);
	void SetNeedGoldInfo();

};