#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_GUILD_EASYSYSTEM

class CDnGuildDlg;
class CDnGuildWantedListTabDlg;

class CDnGuildBaseDlg : public CEtUIDialog
{
public:
	CDnGuildBaseDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildBaseDlg(void);

protected:
	CDnGuildDlg *m_pGuildDlg;
	CDnGuildWantedListTabDlg *m_pGuildWantedListTabDlg;

public:
	CDnGuildDlg *GetGuildDialog() { return m_pGuildDlg; }
	CDnGuildWantedListTabDlg *GetGuildWantedListTabDialog() { return m_pGuildWantedListTabDlg; }

public:
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};

#else

class CDnGuildDlg;
class CDnGuildWantedListDlg;

class CDnGuildBaseDlg : public CEtUIDialog
{
public:
	CDnGuildBaseDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildBaseDlg(void);

protected:
	CDnGuildDlg *m_pGuildDlg;
	CDnGuildWantedListDlg *m_pGuildWantedListDlg;

public:
	CDnGuildDlg *GetGuildDialog() { return m_pGuildDlg; }
	CDnGuildWantedListDlg *GetGuildWantedListDialog() { return m_pGuildWantedListDlg; }

public:
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};

#endif