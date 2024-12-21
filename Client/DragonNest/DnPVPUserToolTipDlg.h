#pragma once
#include "EtUIDialog.h"


class CDnPVPUserToolTipDlg : public CEtUIDialog
{
public:
	CDnPVPUserToolTipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPVPUserToolTipDlg(void);
protected:
	CEtUIStatic * m_pPVPLevel;
	CEtUITextureControl * m_pPVPIcon;
	CEtUIStatic * m_pScore;
	CEtUIStatic * m_pKillPoint;
	CEtUIStatic * m_pAssistPoint;
	CEtUIStatic * m_pSupportPoint;
	CEtUIStatic * m_pTotalXP;
public:
	void SetData( byte cPVPLevel , int iTotalScore , int iKillPoint , int iAssistPoint , int iSuppotPoint , int iTotalXP);
	//virtual void Process( float fElapsedTime );  
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};