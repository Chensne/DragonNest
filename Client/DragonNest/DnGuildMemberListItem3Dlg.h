#pragma once
#include "EtUIDialog.h"

// 컬럼 3개짜리 다이얼로그 아이템
class CDnGuildMemberListItem3Dlg : public CEtUIDialog
{
public:
	CDnGuildMemberListItem3Dlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildMemberListItem3Dlg(void);

protected:
	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticList1;
	CEtUIStatic *m_pStaticList2;

public:
	void SetInfo( const WCHAR *wszName , const WCHAR *wszItemText1, const WCHAR *wszItemText2, bool bEnable );
	LPCWSTR GetName() { return m_pStaticName->GetText(); }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};