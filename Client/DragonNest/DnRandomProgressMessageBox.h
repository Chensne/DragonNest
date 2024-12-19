#pragma once

#include "DnCustomDlg.h"
#include <MMSystem.h>
#include "DnCostumeMixDefine.h"

#ifdef PRE_ADD_COSRANDMIX
class CDnItem;
class CDnRandomProgressMessageBox : public CDnCustomDlg
{
public:
	enum eRandomMsgBoxEvent
	{
		RANDOM_MSGBOX_EVENT_END_PROGRESS,
	};

	enum eRandomMsgBoxType
	{
		RANDOM_MSGBOX_NORMAL,
	};

	CDnRandomProgressMessageBox(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnRandomProgressMessageBox(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	template <typename T>
	bool ShowEx(bool bShow, const T& itemListManager, eRandomMsgBoxType type);

protected:
	virtual void Show( bool bShow );

private:
	void OnEndProcess();
	bool IsValidShowItemList() const;
	void MakeRandomMsgBoxType(eRandomMsgBoxType type);
	void HandleError(const std::wstring& msg);
	void ClearItemCache();

	CEtUIButton *m_pCancelButton;
	CEtUIProgressBar *m_pProgressBarTime;
	CDnItemSlotButton* m_pItemSlot;
	CEtUIStatic* m_pItemNameText;

	float m_fMaxTimerSec;
	float m_fTimer;
	float m_fMaxTimerOffsetSec;
	float m_fTimerOffset;
	bool m_bOnlyProgressBar;
	int m_nSoundIndex;
	EtSoundChannelHandle m_hSound;

	std::vector<SRandomProgressData> m_ShowItemList;
#ifdef PRE_ADD_MTRANDOM_CLIENT
	CMtRandomLocalUsable m_Random;
#else
	CMtRandom m_Random;
#endif
	CDnItem* m_pShowItem;
};

template <typename T>
bool CDnRandomProgressMessageBox::ShowEx(bool bShow, const T& itemListManager, CDnRandomProgressMessageBox::eRandomMsgBoxType type)
{
	if (bShow)
	{
		m_ShowItemList.clear();
		itemListManager.MakeShowItemListAndTimer(m_ShowItemList);
		MakeRandomMsgBoxType(type);

		if (IsValidShowItemList() == false)
		{
#ifdef _WORK
			itemListManager.HandleErrorOnMakePreviewItemList();
#else
			m_bOnlyProgressBar = true;
#endif
			Show(false);
			return false;
		}

		m_Random.srand(timeGetTime());
	}

	Show(bShow);
	return true;
}

#endif // PRE_ADD_COSRANDMIX