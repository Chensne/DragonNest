#pragma once
#include "DnCustomDlg.h"
#include "DnCountCtl.h"

class CDnCountDlg : public CDnCustomDlg
{
public:
	CDnCountDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCountDlg(void);

protected:
	CDnCountCtl *m_pCountCtl;
	float		m_fDelayTime;
	bool		m_bAlways;
	bool		m_bHide;

protected:
	virtual void UpdateCount() {}

public:
	float			GetDelayTime() { return m_fDelayTime; }
	void			SetDelayTime( float fDelayTime ) { m_fDelayTime = fDelayTime; }

public:
	void			ShowCount( bool bShow, bool bAlways = false );
	bool			IsShowCount();
	virtual void	Hide();

public:
	virtual void Process( float fElapsedTime );
};
