#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#pragma once
#include "DnCustomDlg.h"


class CDnTotalLevelSlotToolTipDlg : public CDnCustomDlg
{
public:
	CDnTotalLevelSlotToolTipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnTotalLevelSlotToolTipDlg();

	void AddText( LPCWSTR szText, const D3DCOLOR TextColor = textcolor::WHITE, DWORD dwFormat = UITEXT_NONE, const D3DCOLOR BgColor = 0 );
	void ClearText();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
protected:
	CEtUITextBox *m_pContents;
};

class CDnTotalLevelSkillActivateListDlg : public CDnCustomDlg
{
public:
	CDnTotalLevelSkillActivateListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnTotalLevelSkillActivateListDlg();

protected:
	std::vector< CDnQuickSlotButton* > m_vecSlotButton;
	CEtUIStatic* m_pCashSlotCover;
	CEtUIStatic* m_pCashButton;

	CDnTotalLevelSlotToolTipDlg* m_pTooltipDlg;

public:
	bool SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton );
	void SetSlot( int nSlotIndex, DnSkillHandle hSkill );
	void ResetSlot(int nSlotIndex);

	void ActivateSkillSlot(int nSlotIndex, bool bActivate);
	void ActivateSkillCashSlot(int nSlotIndex, bool bActivate, __time64_t tExpireDate);

public:
	virtual void Initialize( bool bShow );
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};
#endif // PRE_ADD_TOTAL_LEVEL_SKILL