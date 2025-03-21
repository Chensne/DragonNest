#pragma once
#include "EtUIDialog.h"


class CDnPVPDetailedInfoDlg : public CEtUIDialog
{
public:
	CDnPVPDetailedInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPVPDetailedInfoDlg(void);
protected:
	CEtUIStatic * m_pGrade;
	CEtUITextureControl * m_pGradeIcon;

	CEtUIStatic * m_pXP;
	CEtUIStatic * m_pLevelUpArrow;
	CEtUIStatic * m_pNextLevleUp;
	CEtUIStatic * m_pKillPoint;
	CEtUIStatic * m_pSupportPoint;
	CEtUIStatic * m_pAssistPoint;
	
	CEtUIStatic * m_pDefeatedW;
	CEtUIStatic * m_pDefeatedA;
	CEtUIStatic * m_pDefeatedS;
	CEtUIStatic * m_pDefeatedC;

	CEtUIStatic * m_pKillW;
	CEtUIStatic * m_pKillA;
	CEtUIStatic * m_pKillS;
	CEtUIStatic * m_pKillC;

public:
	void SetKillRecord( int KillW , int KillA , int KillS, int KillC ,int W , int A , int S, int C );
	void SetRecorfInfo( byte cGrade ,UINT uiTotalXP , UINT uiKillXP,UINT uiAssistXP,UINT uiSupportXP  );

	//virtual void Process( float fElapsedTime );  
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};