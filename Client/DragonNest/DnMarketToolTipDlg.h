#pragma once

class CDnMarketToolTipDlg : public CEtUIDialog
{
public:
	CDnMarketToolTipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMarketToolTipDlg();

private:
	void UpdateTooltipDlg();

private:
	typedef CEtUIDialog	 BaseClass;

public:
	void SetMoney(int nOnePrice );
	void SetPetal(int nOnePrice );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};
