#pragma once
#include "EtUIDialog.h"

class CDnMasterJoinDlg;
class CDnMasterJoinListDlg : public CEtUIDialog
{
public:
	CDnMasterJoinListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMasterJoinListDlg(void);

protected:
	CEtUIStatic *	m_pStaticName;
	CEtUIStatic *	m_pStaticLevel;
	CEtUIStatic *	m_pStaticClass;

public:
	void SetInfo( const WCHAR * wszName, BYTE cLevel, BYTE cClass );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
};
