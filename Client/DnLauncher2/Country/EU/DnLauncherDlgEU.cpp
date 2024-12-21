#include "stdafx.h"
#include "DnLauncherDlgEU.h"
#include "../../DnHtmlView.h"


#if defined(_EU)

CDnLauncherDlgEU::CDnLauncherDlgEU( CWnd* pParent )
: CDnLauncherDlg( pParent )
{
	m_nMaxCtrlNum = em_MaxNum;
	m_pRectCtrl = new CRect[em_MaxNum];
}

void CDnLauncherDlgEU::MakeFont()
{
	m_Font.CreateFont( 14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("arial") );
}

#endif // _EU