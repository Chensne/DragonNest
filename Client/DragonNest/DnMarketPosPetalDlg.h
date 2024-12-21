#pragma once

class CDnMarketPosPetal : public CEtUIDialog
{
public :
	CDnMarketPosPetal( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMarketPosPetal();

protected:

	CEtUIStatic * m_pStaticPetal;
	int	m_nPetalBalance;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void SetPetalBalance( int nPetalBalance );
	int  GetPetalBalance()	{ return m_nPetalBalance; }
};