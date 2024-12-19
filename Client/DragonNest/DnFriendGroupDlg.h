#pragma once
#include "EtUIDialog.h"

#define _MIN_NAME_SYLLABLE_COUNT 2

class CDnFriendGroupDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnFriendGroupDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnFriendGroupDlg(void);

protected:
	CEtUIListBox *m_pListBoxGroup;
	CEtUIIMEEditBox *m_pEditBoxGroup;
	CEtUIButton *m_pButtonAdd;
	CEtUIButton *m_pButtonDel;
	CEtUIButton *m_pButtonSave;

protected:
	void SetEditBox();
	bool DoAdd();
	bool IsDuplicatedName(const wchar_t* inputName);
	bool IsFullGroupCount() const;
	bool IsSpecialCharacter(std::wstring& strGroupName) const;
	bool CheckGroupNameValidation(std::wstring& strGroupName);
	void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

public:
	void RefreshFriendGroup();
	void OnAddFriendGroup();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};
