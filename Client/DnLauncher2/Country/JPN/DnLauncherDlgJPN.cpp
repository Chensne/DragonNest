#include "stdafx.h"
#include "DnLauncherDlgJPN.h"
#include "../../DnHtmlView.h"


#if defined(_JPN)

CDnLauncherDlgJPN::CDnLauncherDlgJPN( CWnd* pParent )
: CDnLauncherDlg( pParent )
{
	m_nMaxCtrlNum = em_MaxNum;
	m_pRectCtrl = new CRect[em_MaxNum];
}


BEGIN_MESSAGE_MAP( CDnLauncherDlgJPN, CDnLauncherDlg )
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CDnLauncherDlgJPN::OnDestroy()
{
	SAFE_DELETE_ARRAY( m_pRectCtrl );

	CDnLauncherDlg::OnDestroy();
}

void CDnLauncherDlgJPN::MakeFont()
{
	m_Font.CreateFont( 11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("µ¸¿òÃ¼") );
}

void CDnLauncherDlgJPN::NavigateHtml()
{
	if( m_pDnHtmlView )
	{
		if( DnNHNService::GetInstance().CheckChannelingMS() )
			m_pDnHtmlView->Navigate( L"http://down.hangame.co.jp/jp/purple/dist/j_dnest/Installer_ms.htm" );
		else
			m_pDnHtmlView->Navigate( DNPATCHINFO.GetGuidepageUrl() );
	}
}

#endif // _JPN