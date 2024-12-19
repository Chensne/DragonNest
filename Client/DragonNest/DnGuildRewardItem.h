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
	GuildReward::GuildRewardInfo m_GuildRewardInfo;	//��庸�� ����
	bool m_IsAvailableReward;		//Ȱ��ȭ ����
	bool m_IsPurchased;		//���� ����
	bool m_isAppliedRewardBetterThan;	//

	CDnItemSlotButton* m_pItemSlot;	//��� ���� ������ ����.
	CDnItem* m_pTempItem;			//��� ���� ������

	CEtUIStatic *m_pMasterLimit;	//����� ���� ������ ǥ��
	CEtUIStatic *m_pLevelLimit;		//��� ���� ���� ����
	CEtUIStatic *m_pRewardInfo;		//��� ���� �̸�?
	CEtUIStatic *m_pPeriodInfo;		//�Ⱓ ����
	CEtUIStatic *m_pGold;			//���� �ݾ� ���
	CEtUIStatic *m_pSilver;			//���� �ݾ� �ǹ�
	CEtUIStatic *m_pBronze;			//���� �ݾ� ����

	void UpdateInfo();
	void SetRewardItemInfo( const GuildReward::GuildRewardInfo &info );

public:
	void SetGuildRewardInfo(const GuildReward::GuildRewardInfo &info);

	void SetNeedGoldInfo(DWORD dwColor);

	bool IsActivated() { return m_IsAvailableReward; }
	bool IsPurchased() { return m_IsPurchased; }

	const GuildReward::GuildRewardInfo& GetGuildRewardInfo() { return m_GuildRewardInfo; }
};