#pragma once
#include "EtUIDialog.h"


class CDnPVPPopupDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnPVPPopupDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPPopupDlg(void);

protected:
	
	CEtUIButton *m_pButtonPrivate;
	CEtUIButton *m_pButtonFriend;
	std::wstring m_wstTargetName; // 대상이름

protected:
	void InitControl();


public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
	void SetTargetName(std::wstring wstName){m_wstTargetName = wstName;}

private:
	bool CheckFriendCondition(const WCHAR* name);
	bool IsFriend( const WCHAR *name );
	// 길드메뉴 중 일부는 마을에서만 사용가능하다고 한다.
	bool CheckValidCommand();
};
