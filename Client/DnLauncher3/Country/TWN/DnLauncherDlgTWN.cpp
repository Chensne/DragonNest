#include "stdafx.h"
#include "DnLauncherDlgTWN.h"

#if defined(_TWN)


CDnLauncherDlgTWN::CDnLauncherDlgTWN( CWnd* pParent )
: CDnLauncherDlg( pParent )
, m_bUseAgreement( FALSE )
, m_UseAgreementLinkClicked( FALSE )
{
	m_nMaxCtrlNum = em_MaxNumEx;
	m_pRectCtrl = new CRect[em_MaxNumEx];
}

void CDnLauncherDlgTWN::DoDataExchange( CDataExchange* pDX )
{
	CDnLauncherDlg::DoDataExchange( pDX );

	//DDX_Control( pDX, IDC_CHECK_USE_AGREEMENT, m_BtnCheckUseAgreement );
	DDX_Control( pDX, IDC_USE_AGREEMENT, m_StaticUseAgreement );
	DDX_Control( pDX, IDC_USE_AGREEMENT_LINK, m_StaticUseAgreementLink );
}


BEGIN_MESSAGE_MAP( CDnLauncherDlgTWN, CDnLauncherDlg )
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	//ON_BN_CLICKED( IDC_CHECK_USE_AGREEMENT, &CDnLauncherDlgTWN::OnClickedCheckUseAgreement )
	ON_BN_CLICKED( IDC_USE_AGREEMENT_LINK, &CDnLauncherDlgTWN::OnClickedUseAgreementLink )
END_MESSAGE_MAP()


void CDnLauncherDlgTWN::OnDestroy()
{
	SAFE_DELETE_ARRAY( m_pRectCtrl );
	m_FontLink.DeleteObject();

	CDnLauncherDlg::OnDestroy();
}

void CDnLauncherDlgTWN::InitControl()
{
	CDnLauncherDlg::InitControl();

	m_BtnCheckDirectGameStart.ShowWindow( SW_HIDE );
	//m_BtnCheckUseAgreement.MoveWindow( m_pRectCtrl[em_CheckBox_UseAgreement] );

	m_BtnStartGame.SetSkinJpg( IDR_JPG_GAMESTART, IDR_JPG_GAMESTART_CLICK, IDR_JPG_GAMESTART_ON, IDR_JPG_GAMESTART_DISABLED );

	m_StaticDirectGameStart.ShowWindow( SW_HIDE );

	m_StaticUseAgreement.SetTextColor( WHITE );
	m_StaticUseAgreement.SetFont( &m_Font );
	m_StaticUseAgreement.MoveWindow( m_pRectCtrl[em_Text_UseAgreement] );
	m_StaticUseAgreement.SetWindowText( _S( STR_USE_AGREEMENT ) );

	m_StaticUseAgreementLink.SetTextColor( RGB( 0, 0, 255 ) );
	m_StaticUseAgreementLink.SetFont( &m_FontLink );
	m_StaticUseAgreementLink.MoveWindow( m_pRectCtrl[em_Text_UseAgreementLink] );
	m_StaticUseAgreementLink.SetWindowText( _S( STR_USE_AGREEMENT_LINK ) );
}

void CDnLauncherDlgTWN::MakeFont()
{
	m_Font.CreateFont( 11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("mingliu") );
	m_FontLink.CreateFont( 11, 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("mingliu") );
}

HBRUSH CDnLauncherDlgTWN::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
	RECT rect;

	switch(nCtlColor)
	{
	case CTLCOLOR_STATIC:
		//	case CTLCOLOR_EDIT:
		//	case CTLCOLOR_LISTBOX:
		//	case CTLCOLOR_SCROLLBAR:
		{
			if( pWnd->GetDlgCtrlID() == IDC_USE_AGREEMENT_LINK )
			{
				if( m_UseAgreementLinkClicked == FALSE )
					pDC->SetTextColor( RGB( 0, 0, 255 ) );
				else
					pDC->SetTextColor( RGB( 255, 0, 0 ) );
			}
			pDC->SetBkMode(TRANSPARENT);
			pWnd->GetWindowRect(&rect);
			RedrawWindow(&rect);
			return (HBRUSH)GetStockObject(NULL_BRUSH);;
		}
		break;
	}

	return hbr;
}

BOOL CDnLauncherDlgTWN::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	CPoint pt;
	CRect rc;

	GetCursorPos( &pt );
	GetDlgItem( IDC_USE_AGREEMENT_LINK )->GetWindowRect( rc );

	if( rc.PtInRect( pt ) )
	{
		SetCursor(AfxGetApp()->LoadStandardCursor( MAKEINTRESOURCE( IDC_HAND ) ) );
		return TRUE;
	}

	return CDialog::OnSetCursor( pWnd, nHitTest, message );
}

void CDnLauncherDlgTWN::OnClickedCheckUseAgreement()
{
	m_bUseAgreement = m_BtnCheckUseAgreement.GetCheck();
	if( m_bUseAgreement && m_emDownloadPatchState == PATCH_COMPLETE )
		m_BtnStartGame.EnableWindow( TRUE );
	else
		m_BtnStartGame.EnableWindow( FALSE );
}

void CDnLauncherDlgTWN::OnClickedUseAgreementLink()
{
	m_UseAgreementLinkClicked = TRUE;
	GetDlgItem( IDC_USE_AGREEMENT_LINK )->Invalidate();
	ShellExecute( m_hWnd, L"open", L"http://dn.51newsx.com", NULL, NULL, SW_SHOW );
}

void CDnLauncherDlgTWN::EnableStartButton()
{
	CDnLauncherDlg::EnableStartButton();

	//if( !m_bUseAgreement )
		m_BtnStartGame.EnableWindow( true );
}

#endif // _TWN