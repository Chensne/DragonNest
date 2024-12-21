#pragma once
#include "EtUIDialog.h"

class CDnChatRoomCreateDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnChatRoomCreateDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatRoomCreateDlg(void);

protected:
	CEtUIIMEEditBox *m_pEditBoxName;
	CEtUIButton *m_pButtonOK;
	bool m_bShowedJustNow;	// 방금전에 보여짐. 핫키로 보여질때만 처리해서 문자 잘못 찍히는거 해결한다.

	CEtUIComboBox *m_pComboRoomType;
	CEtUIComboBox *m_pComboCondition;
	CEtUIIMEEditBox *m_pEditBoxPR0;
	CEtUIIMEEditBox *m_pEditBoxPR1;
	CEtUIIMEEditBox *m_pEditBoxPR2;
	CEtUICheckBox *m_pCheckBoxSecret;
	CEtUIEditBox *m_pEditBoxPassword;

public:
	void SetChatRoomFlag( bool bShowByHotKey ) { m_bShowedJustNow = bShowByHotKey; }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};