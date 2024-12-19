#pragma once
#include "EtUIDialog.h"

class CDnOccupationModeNoticeDlg : public CEtUIDialog
{
public:
	CDnOccupationModeNoticeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnOccupationModeNoticeDlg(void);

protected:

	enum{
		eNOTICE_BLIND = 0,
		eNOTICE_TIME = 3,
	};

	CEtUIStatic * m_pStaticBlueNotice;
	CEtUIStatic * m_pStaticRedNotice;

	bool  m_bNotice;
	float m_fNoticeTime;

public:

	void ShowNotice( int eTeam, int nStringID = 0 );

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
};
