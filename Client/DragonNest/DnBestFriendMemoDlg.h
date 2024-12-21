#pragma once


// 메모작성 Dlg. //

#ifdef PRE_ADD_BESTFRIEND

#include "EtUIDialog.h"


class CDnBestFriendMemoDlg : public CEtUIDialog
{

private:

	CEtUIIMEEditBox * m_IMEmemo;

public:

	CDnBestFriendMemoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnBestFriendMemoDlg(){
		ReleaseDlg();
	}

	void ReleaseDlg();


	// Override - CEtUIDialog //		
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();	
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

private:

	void InputMemo(); // 메모입력.

};



#endif