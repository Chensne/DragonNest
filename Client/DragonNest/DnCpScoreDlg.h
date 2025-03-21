
#pragma once

#include "EtUIDialog.h"

class CDnCpScoreDlg : public CEtUIDialog
{
public:
	CDnCpScoreDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_BOTTOM, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCpScoreDlg(void)	{}

	void ShowDlg(const SUICoord& standard);

	virtual void	InitialUpdate();
	virtual void	Initialize( bool bShow );
	virtual void	Process( float fElapsedTime );
	virtual void	Render( float fElapsedTime );
	SUICoord		GetBaseBGCoord() const;

protected:
	CEtUIStatic*	m_pBg;
	SUICoord		m_BaseUICoord;
};
