#pragma once
#include "EtUIDialog.h"
#include "DnPartyEnterDlg.h"

class CDnPartyEnterPasswordDlg : public CDnPartyEnterDlg
{
public:
#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
	CDnPartyEnterPasswordDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
#else
	CDnPartyEnterPasswordDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
#endif 
	virtual ~CDnPartyEnterPasswordDlg(void);

protected:
	CEtUIEditBox* m_pEditBoxPassWord;
	void Process( float fElapsedTime );

public:
#ifdef PRE_PARTY_DB
	virtual int GetPassword();
#else
	virtual LPCWSTR GetPassword();
#endif

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow, ePartyType type );
	virtual void Show( bool bShow );

	void OnPartyJoinFailed();
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};