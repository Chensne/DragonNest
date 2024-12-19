#pragma once

#ifdef PRE_ADD_GUILD_CONTRIBUTION
#include "DnCustomDlg.h"

class CDnGuildContributionListDlg : public CDnCustomDlg
{
public:
	CDnGuildContributionListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnGuildContributionListDlg();
protected:

	CEtUIStatic* m_pStaticRank;
	CEtUIStatic* m_pStaticName;
	CEtUIStatic* m_pStaticPoint;
	CDnJobIconStatic* m_pStaticJobIcon;
	
public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );

	void SetInfo( int nRank, int nJobIcon, WCHAR* wszName, int nPoint ); 
};

#endif