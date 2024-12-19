// ColorPickerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtEffect2Tool.h"
#include "ColorPickerDlg.h"


// CColorPickerDlg dialog

IMPLEMENT_DYNAMIC(CColorPickerDlg, CDialog)

CColorPickerDlg::CColorPickerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorPickerDlg::IDD, pParent)
	, m_strRed(_T(""))
	, m_strGreen(_T(""))
	, m_strBlue(_T(""))
{		
	m_InitialColor = RGB(128,128,128);
	m_strRed.Format("128");
	m_strGreen.Format("128");
	m_strBlue.Format("128");
}

CColorPickerDlg::~CColorPickerDlg()
{
}

void CColorPickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strRed);
	DDX_Text(pDX, IDC_EDIT2, m_strGreen);
	DDX_Text(pDX, IDC_EDIT3, m_strBlue);
}


BEGIN_MESSAGE_MAP(CColorPickerDlg, CDialog)
	ON_WM_PAINT()
	ON_REGISTERED_MESSAGE(WM_XCOLORPICKER_SELCHANGE, OnSelChange)
	ON_REGISTERED_MESSAGE(WM_XCOLORPICKER_SELENDOK, OnSelendOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CColorPickerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CColorPickerDlg::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT1, &CColorPickerDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CColorPickerDlg::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CColorPickerDlg::OnEnChangeEdit3)
END_MESSAGE_MAP()


// CColorPickerDlg message handlers
static const int startX = 170;
static const int startY = 160;

void CColorPickerDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	RECT rcPickRect;
	rcPickRect.left = startX;
	rcPickRect.top = startY;
	rcPickRect.right = startX + 60;
	rcPickRect.bottom = startY + 60;

	dc.FillSolidRect( &rcPickRect, RGB(128,128,128) );

	rcPickRect.left++;
	rcPickRect.top++;
	rcPickRect.right--;
	rcPickRect.bottom--;

	BYTE red = GetRValue(m_ColorSpectrum.GetRGB());
	BYTE green = GetGValue(m_ColorSpectrum.GetRGB());
	BYTE blue = GetBValue(m_ColorSpectrum.GetRGB());

	dc.FillSolidRect( &rcPickRect, RGB(red,green,blue) );
}

BOOL CColorPickerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	CRect rect;
	GetDlgItem(IDC_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	GetDlgItem(IDC_FRAME)->ShowWindow(SW_HIDE);
	VERIFY(m_ColorSpectrum.Create(AfxGetInstanceHandle(), WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, 
		rect, m_hWnd, 9001, m_InitialColor));//, CXColorSpectrumCtrl::XCOLOR_TOOLTIP_HTML));

	// call SetWindowPos to insert control in proper place in tab order
	::SetWindowPos(m_ColorSpectrum.m_hWnd, ::GetDlgItem(m_hWnd, IDC_FRAME), 
		0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

	::SetFocus(m_ColorSpectrum.m_hWnd);


	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CColorPickerDlg::UpdatePreviewBox()
{
	RECT rcPickRect;
	rcPickRect.left = startX;
	rcPickRect.top = startY;
	rcPickRect.right = startX + 60;
	rcPickRect.bottom = startY + 60;
	InvalidateRect(&rcPickRect, FALSE);
}

// handler for WM_XCOLORPICKER_SELCHANGE
LRESULT CColorPickerDlg::OnSelChange(WPARAM wParam, LPARAM lParam)
{
	m_strRed.Format("%d", GetRValue(wParam) );
	m_strGreen.Format("%d", GetGValue(wParam) );
	m_strBlue.Format("%d", GetBValue(wParam) );

	UpdatePreviewBox();
	UpdateData( FALSE );	

	return 0;
}

// handler for WM_XCOLORPICKER_SELENDOK
LRESULT CColorPickerDlg::OnSelendOk(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("CXColorSpectrumCtrlTestDlg::OnSelendOk:  color=0x%06X  id=%d\n"), wParam, lParam);
	return 0;
}


void CColorPickerDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CColorPickerDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CColorPickerDlg::UpdateColor()
{
	UpdateData( TRUE );
	int red = atoi( m_strRed.GetBuffer() );
	int green = atoi( m_strGreen.GetBuffer() );
	int blue = atoi( m_strBlue.GetBuffer() );
	m_ColorSpectrum.SetRGB( RGB(red, green, blue) );
	UpdatePreviewBox();

	red = __max(0, __min(255, red));
	green = __max(0, __min(255, green));
	blue = __max(0, __min(255, blue));
	m_strRed.Format("%d", red);
	m_strGreen.Format("%d", green);
	m_strBlue.Format("%d", blue);
	UpdateData( FALSE );
}

void CColorPickerDlg::OnEnChangeEdit1()
{
	UpdateColor();
}

void CColorPickerDlg::OnEnChangeEdit2()
{
	UpdateColor();
}

void CColorPickerDlg::OnEnChangeEdit3()
{
	UpdateColor();	
}

COLORREF CColorPickerDlg::GetColor()
{
	COLORREF color = m_ColorSpectrum.GetRGB();
	return color;
}

void CColorPickerDlg::SetColor( COLORREF color )
{
	m_InitialColor = color;
	m_strRed.Format("%d", GetRValue(color));
	m_strGreen.Format("%d", GetGValue(color));
	m_strBlue.Format("%d", GetBValue(color));
}
