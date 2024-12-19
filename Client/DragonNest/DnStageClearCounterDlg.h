#pragma once
#include "DnCountDlg.h"

class CDnStageClearCounterDlg : public CDnCountDlg
{
public:
	CDnStageClearCounterDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnStageClearCounterDlg(void);

protected:
	COLOR_TYPE	m_ColorType;
	EtVector2	m_AxisPos;
	bool		m_bWithSign;

public:
	void			Set(int val, COLOR_TYPE color);
	void			ShowPlusSign(bool bEnable); // todo : Minus Sign

	virtual void	InitialUpdate();
	virtual void	Initialize( bool bShow );
	virtual void	Process( float fElapsedTime );
	virtual void	Render( float fElapsedTime );
	virtual void	Hide();

private:
	void			CalcAndSetPos();
};
