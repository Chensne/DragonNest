#include "stdafx.h"
#include "DnLauncherDlgSG.h"

#if defined(_SG)


CDnLauncherDlgSG::CDnLauncherDlgSG( CWnd* pParent )
: CDnLauncherDlg( pParent )
{
	m_nMaxCtrlNum = em_MaxNum;
	m_pRectCtrl = new CRect[em_MaxNum];
}


BEGIN_MESSAGE_MAP( CDnLauncherDlgSG, CDnLauncherDlg )
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CDnLauncherDlgSG::OnDestroy()
{
	SAFE_DELETE_ARRAY( m_pRectCtrl );

	CDnLauncherDlg::OnDestroy();
}

void CDnLauncherDlgSG::MakeFont()
{
	m_Font.CreateFont( 14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, _T(" Lucida Sans Unicode") );
}

#endif // _SG