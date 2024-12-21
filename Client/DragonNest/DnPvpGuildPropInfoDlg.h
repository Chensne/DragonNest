#pragma once

#include "EtUIDialog.h"

class CDnPvpGuildPropInfoDlg : public CEtUIDialog
{
public:
	CDnPvpGuildPropInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPvpGuildPropInfoDlg();

	enum{
		SHOW_TIME = 5,	//5√ 
	};

protected:

	CEtUIStatic * m_pBlueIcon;
	CEtUIStatic * m_pRedIcon;
	CEtUIStatic * m_pStaticText;

	float m_fRenderTime;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );

	void ShowInfo( DnActorHandle hActor, int nStringID );
};
