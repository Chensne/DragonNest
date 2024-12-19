#pragma once
#include "EtUIDialog.h"

class CDnGuildWantedListPopupDlg : public CEtUIDialog
{
public:
	CDnGuildWantedListPopupDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWantedListPopupDlg(void);

protected:
	CEtUITextBox *m_pTextBox;
	CEtUIStatic *m_pStaticLine;
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CEtUIStatic *m_pStaticLine2;
#endif

public:
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	void SetInfo( const WCHAR *wszName, int nLevel, int nCount, int nCountMax, const WCHAR *wszNotice, const WCHAR *wszHomepage, const WCHAR *wszGuildMasterName );
#else
	void SetInfo( const WCHAR *wszName, int nLevel, int nCount, int nCountMax, const WCHAR *wszNotice );
#endif

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};