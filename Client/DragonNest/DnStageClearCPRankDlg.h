#pragma once
#include "EtUIDialog.h"

class CDnStageClearCPRankDlg : public CEtUIDialog
{
public:
	enum{
		E_NAME,
		E_RANK,
		E_CP_POINT,
		E_MAX
	};

public:
	CDnStageClearCPRankDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnStageClearCPRankDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );

public:
	void SetLegendData( const WCHAR * wszName, const char cRank, int nCP_Point, bool bNew );
	void SetAbyssData( const WCHAR * wszName, const char cRank, int nCP_Point, bool bNew );
	void SetMyData( const WCHAR * wszName, const char cRank, int nCP_Point, bool bNew );

protected:
	void SetData( CEtUIStatic ** pStatic, const WCHAR * wszName, const char cRank, int nCP_Point, bool bNew );

protected:
	CEtUIStatic * m_pLegendStatic[E_MAX];
	CEtUIStatic * m_pAbyssStatic[E_MAX];
	CEtUIStatic * m_pMyStatic[E_MAX];
};