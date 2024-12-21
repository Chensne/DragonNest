#include "StdAfx.h"
#include "DnCountDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCountDlg::CDnCountDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_pCountCtl(NULL)
	, m_fDelayTime(0.0f)
	, m_bAlways(false)
	, m_bHide(false)
{
}

CDnCountDlg::~CDnCountDlg(void)
{
}

void CDnCountDlg::Process( float fElapsedTime )
{
	if( m_fDelayTime > 0.0f )
	{
		if (m_fDelayTime > 0.f && m_bAlways == false)
			m_fDelayTime -= fElapsedTime;
		UpdateCount();
	}
}

void CDnCountDlg::ShowCount( bool bShow, bool bAlways )
{
	m_pCountCtl->Show( bShow );

	if (bAlways)
		m_fDelayTime = 1.f;
	m_bAlways = bAlways;
}

bool CDnCountDlg::IsShowCount()
{
	return m_pCountCtl->IsShow();
//	return ( m_pCountCtl->GetAlpha() != 0 && m_fDelayTime > 0.f ) ? true : false;
}

void CDnCountDlg::Hide()
{
	m_pCountCtl->SetAlpha(0);
	m_bHide = true;
}