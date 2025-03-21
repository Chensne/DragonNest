#pragma once
#include "EtUIDialog.h"


class CDnPotentialStateItemDlg : public CEtUIDialog
{
public:
	CDnPotentialStateItemDlg ( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPotentialStateItemDlg (void);

protected:
	enum
	{
		E_TEXT_STATS_NAME = 0,
		E_TEXT_STATS_VALUE,
		E_TEXT_CHANGE_STATS,
		
		E_TEXT_MAX,
	};
	CEtUIStatic *m_pStaticText[E_TEXT_MAX];

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	void SetInfo( const std::wstring wszItemText1 , const wchar_t* wszItemText2 , const wchar_t* wszItemText3 , bool bStatsIsUP);
};
