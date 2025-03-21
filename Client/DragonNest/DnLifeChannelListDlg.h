#pragma once

#include "EtUIDialog.h"

class CDnLifeChannelListDlg : public CEtUIDialog
{
public:
	CDnLifeChannelListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeChannelListDlg(void);

protected:
	CEtUIStatic *	m_pStaticName;
	CEtUIStatic *	m_pStaticLevel;
	CEtUIStatic *	m_pStaticClass;

	int m_nVectorID;

public:
	void SetInfo( const WCHAR * wszName, bool bHarvest, int nTime, int nID);
	void SetEmpty();

	int GetID()	{ return m_nVectorID; }

public:
	virtual void Initialize( bool bShow );
};