#pragma once
#include "EtUIDialog.h"

class CDnChatRoomMemberListDlg : public CEtUIDialog
{
public:
	CDnChatRoomMemberListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatRoomMemberListDlg(void);

protected:
	CEtUIStatic *m_pStaticLevel;
	CEtUIStatic *m_pStaticJop;
	CEtUIStatic *m_pStaticName;

public:
	void SetInfo( const WCHAR *wszName, const WCHAR *wszJop, int nLevel );
	LPCWSTR GetName() { return m_pStaticName->GetText(); }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};