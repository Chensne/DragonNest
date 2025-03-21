// PropertiesDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "PropertiesDlg.h"
#include "SignalManager.h"
#include "RenderBase.h"
#include "GlobalValue.h"


// CPropertiesDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropertiesDlg, CDialog)

CPropertiesDlg::CPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertiesDlg::IDD, pParent)
	, m_szWorkingFolder(_T(""))
	, m_szShaderFolder(_T(""))
	, m_szResourceFolder(_T(""))
{
	m_bDisableCancel = false;
}

CPropertiesDlg::~CPropertiesDlg()
{
}

void CPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szWorkingFolder);
	DDX_Text(pDX, IDC_EDIT6, m_szShaderFolder);
	DDX_Text(pDX, IDC_EDIT7, m_szResourceFolder);
}


BEGIN_MESSAGE_MAP(CPropertiesDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPropertiesDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropertiesDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CPropertiesDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CPropertiesDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CPropertiesDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CPropertiesDlg 메시지 처리기입니다.

BOOL CPropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( CGlobalValue::GetInstance().IsOpenAction() ) {
		MessageBox( "파일이 열려있으면 설정을 변경할 수 없습니다. 닫아줘", "에러", MB_OK );
		OnOK();
		return TRUE;
	}
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_szWorkingFolder = CSignalManager::GetInstance().GetWorkingFolder();
	m_szShaderFolder = CRenderBase::GetInstance().GetShaderFolder();
	m_szResourceFolder = CRenderBase::GetInstance().GetResourceFolder();

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
	if( m_szWorkingFolder.IsEmpty() || m_szShaderFolder.IsEmpty() || m_szResourceFolder.IsEmpty() ) {
		MessageBox( "폴더를 설정해 주세요", "에러", MB_OK );
		return;
	}

	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "WorkingFolder", m_szWorkingFolder.GetBuffer() );
	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ShaderFolder", m_szShaderFolder.GetBuffer() );
	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ResourceFolder", m_szResourceFolder.GetBuffer() );

	if( CRenderBase::GetInstance().GetShaderFolder() != m_szShaderFolder )
		CRenderBase::GetInstance().ChangeShaderFolder( m_szShaderFolder );
	if( CSignalManager::GetInstance().GetWorkingFolder() != m_szWorkingFolder )
		CSignalManager::GetInstance().ChangeWorkingFolder( m_szWorkingFolder );
	if( CRenderBase::GetInstance().GetResourceFolder() != m_szResourceFolder )
		CRenderBase::GetInstance().ChangeResourceFolder( m_szResourceFolder );

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
	Dlg.SetTitle(_T("Select Working Directory"));
	if( !m_szWorkingFolder.IsEmpty() ) {
		TCHAR path[_MAX_PATH];
		STRCPY_S(path, _MAX_PATH, m_szWorkingFolder);
		Dlg.SetSelPath(path);
	}
	if( Dlg.DoModal() == IDOK ) {
		m_szWorkingFolder = Dlg.GetSelPath();
		UpdateData(FALSE);
	}
}

void CPropertiesDlg::OnBnClickedButton3()
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

void CPropertiesDlg::OnBnClickedButton4()
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