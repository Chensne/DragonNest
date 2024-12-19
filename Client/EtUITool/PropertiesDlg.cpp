// PropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtUITool.h"
#include "PropertiesDlg.h"
#include "RenderBase.h"


// CPropertiesDlg dialog

IMPLEMENT_DYNAMIC(CPropertiesDlg, CDialog)

CPropertiesDlg::CPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertiesDlg::IDD, pParent)
	, m_szShaderFolder(_T(""))
	, m_szResourceFolder(_T(""))
	, m_szUITemplateFolder(_T(""))
{
	m_bDisableCancel = false;
}

CPropertiesDlg::~CPropertiesDlg()
{
}

void CPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_EDIT1, m_szShaderFolder);
	DDX_Text(pDX, IDC_EDIT4, m_szResourceFolder);
	DDX_Text(pDX, IDC_EDIT5, m_szUITemplateFolder);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertiesDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPropertiesDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropertiesDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CPropertiesDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPropertiesDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPropertiesDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CPropertiesDlg message handlers

BOOL CPropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData( FALSE );

	if( m_bDisableCancel == true )
		GetDlgItem( IDCANCEL )->EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPropertiesDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	if( m_szShaderFolder.IsEmpty() || m_szResourceFolder.IsEmpty() || m_szUITemplateFolder.IsEmpty() ) {
		MessageBox( "폴더를 설정해 주세요", "에러", MB_OK );
		return;
	}

	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ShaderFolder", m_szShaderFolder.GetBuffer() );
	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ResourceFolder", m_szResourceFolder.GetBuffer() );
	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "UITemplateFolder", m_szUITemplateFolder.GetBuffer() );

	OnOK();
}

void CPropertiesDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_bDisableCancel == true ) return;
	OnCancel();
}

void CPropertiesDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CXTBrowseDialog Dlg;
	UpdateData(TRUE);
	Dlg.SetTitle(_T("Select Shader Directory"));
	if( !m_szShaderFolder.IsEmpty() ) {
		TCHAR path[_MAX_PATH];
		STRCPY_S(path, _MAX_PATH, m_szShaderFolder);
		Dlg.SetSelPath(path);
	}
	if( Dlg.DoModal() == IDOK ) {
		m_szShaderFolder = Dlg.GetSelPath();
		UpdateData(FALSE);
	}
}

void CPropertiesDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CXTBrowseDialog Dlg;
	UpdateData(TRUE);
	Dlg.SetTitle(_T("Select Resource Directory"));
	if( !m_szShaderFolder.IsEmpty() ) {
		TCHAR path[_MAX_PATH];
		STRCPY_S(path, _MAX_PATH, m_szResourceFolder);
		Dlg.SetSelPath(path);
	}
	if( Dlg.DoModal() == IDOK ) {
		m_szResourceFolder = Dlg.GetSelPath();
		UpdateData(FALSE);
	}
}
void CPropertiesDlg::OnBnClickedButton3()
{
	CXTBrowseDialog Dlg;
	UpdateData(TRUE);
	Dlg.SetTitle(_T("Select UI Template Directory"));
	if( !m_szUITemplateFolder.IsEmpty() ) {
		TCHAR path[_MAX_PATH];
		STRCPY_S(path, _MAX_PATH, m_szUITemplateFolder);
		Dlg.SetSelPath(path);
	}
	if( Dlg.DoModal() == IDOK ) {
		m_szUITemplateFolder = Dlg.GetSelPath();
		UpdateData(FALSE);
	}
}
