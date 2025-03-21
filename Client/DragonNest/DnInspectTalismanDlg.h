#pragma once

#if defined(PRE_ADD_TALISMAN_SYSTEM)
#include "DnCustomDlg.h"

class CDnInspectTalismanSlotDlg;
class CDnInspectTalismanInfoDlg;
class CDnInspectTalismanDlg : public CDnCustomDlg
{
public:
	CDnInspectTalismanDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnInspectTalismanDlg();

	CDnInspectTalismanSlotDlg* m_pSlotDlg;
	CDnInspectTalismanInfoDlg* m_pDetailInfoDlg;

protected:
	CEtUIButton* m_pDetailInfoButton;
	CEtUIScrollBar* m_pScrollBar;
	CEtUIListBoxEx* m_pListBoxEx;

	float m_fOriginalPosX;
	float m_fOriginalPosY;
	int   m_nSlotMaxCount;
	void SetSlotOffset();

public:
	void RefreshTalismanEquipData();
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual void Render( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DnCharTalismanListDlg.h"
// 슬롯 클래스
class CDnInspectTalismanSlotDlg : public CDnCharTalismanListDlg
{
public:
	CDnInspectTalismanSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnInspectTalismanSlotDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DnCharTalismanInfoDlg.h"
// 상세정보보기 클래스
class CDnInspectTalismanInfoDlg : public CDnCharTalismanInfoDlg
{
public:
	CDnInspectTalismanInfoDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnInspectTalismanInfoDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};
#endif // PRE_ADD_TALISMAN_SYSTEM