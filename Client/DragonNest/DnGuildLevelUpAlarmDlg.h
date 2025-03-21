#pragma once
#include "EtUIDialog.h"

class CDnGuildLevelUpAlarmDlg : public CEtUIDialog
{
public:
	CDnGuildLevelUpAlarmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildLevelUpAlarmDlg(void);

protected:
	CEtUIStatic *m_pStaticLevel;
	float m_fShowTime;

public:
	void SetInfo( int nLevel, float fFadeTime );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
};