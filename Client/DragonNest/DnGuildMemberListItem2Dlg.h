#pragma once
#include "EtUIDialog.h"

// 컬럼 2개짜리 다이얼로그 아이템
class CDnGuildMemberListItem2Dlg : public CEtUIDialog
{
public:
	CDnGuildMemberListItem2Dlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildMemberListItem2Dlg(void);

protected:
	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticList1;

public:
	void SetInfo( const WCHAR *wszName , const WCHAR *wszItemText, bool bEnable );
	LPCWSTR GetName() { return m_pStaticName->GetText(); }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};