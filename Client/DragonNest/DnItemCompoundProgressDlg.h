#pragma once

#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL

class CDnItemCompoundProgressDlg : public CEtUIDialog
{
public:
	CDnItemCompoundProgressDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnItemCompoundProgressDlg(void);

protected:
	CEtUIButton* m_pCancel;
	CEtUIProgressBar* m_pProgressBar;

	float m_fTime;
	char  m_cFlag;	

protected:

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );


	void	ShowEx( bool bShow, float fTime );

};

#endif 
