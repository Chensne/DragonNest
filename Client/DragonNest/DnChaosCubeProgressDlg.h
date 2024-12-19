#pragma once

#ifdef PRE_ADD_CHAOSCUBE

#include "EtUIDialog.h"

class CDnChaosCubeProgressDlg : public CEtUIDialog
{
public:

	enum ECUBETYPE
	{
		ECUBETYPE_NONE,
		ECUBETYPE_CHAOS,
		ECUBETYPE_CHOICE
	};

protected:

	ECUBETYPE m_cubeType;

	CEtUIStatic * m_pText;
	CEtUIButton *m_pCancelButton;
	CEtUIProgressBar *m_pProgressBarTime;

	float m_fTimer;

	struct SCChaosCubeRequest * m_pRequestData;


public:
	CDnChaosCubeProgressDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnChaosCubeProgressDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

public:

	void SetRequestData( struct SCChaosCubeRequest * pData, ECUBETYPE type );


};

#endif