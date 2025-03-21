#pragma once
#include "DnCustomDlg.h"
#include "DnInterface.h"
#include "DnSmartMoveCursor.h"

class CDnItemChoiceDlg : public CDnCustomDlg
{
public:
	CDnItemChoiceDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnItemChoiceDlg(void);

	virtual void	InitialUpdate();
	virtual void	Initialize(bool bShow);
	virtual void	InitCustomControl(CEtUIControl *pControl);

	void			Open(const TItem& itemInfo, float progressTotalTime, const DWORD& dropItemUniqueID);
	void			Close();

protected:
	virtual void	Process(float fElapsedTime);
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);

private:
	virtual void	Show(bool bShow);
	void			SetItem(const TItem& itemInfo);
	void			OnSelectCancel();

	CDnItemSlotButton*	m_pItemSlotBtn;
	CEtUIStatic*		m_pItemName;
	CEtUIStatic*		m_pItemLevel;
	CEtUIStatic*		m_pItemClass;
	CEtUIStatic*		m_pWarning;

	CEtUIProgressBar*	m_pProgressBar;
	CEtUIButton*		m_pAcceptBtn;
	CEtUIButton*		m_pGiveUpBtn;

	CDnItem*			m_pItemCache;

	float				m_TotalTime;
	float				m_RemainTime;
	DWORD				m_DropItemUniqueID;
};