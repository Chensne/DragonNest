// PropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EtViewer.h"
#include "PropertiesDlg.h"
#include "RenderBase.h"


// CPropertiesDlg dialog

IMPLEMENT_DYNAMIC(CPropertiesDlg, CDialog)

CPropertiesDlg::CPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertiesDlg::IDD, pParent)
	, m_szShaderFolder(_T(""))
{
	m_bDisableCancel = false;
}

CPropertiesDlg::~CPropertiesDlg()
{
}

void CPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szShaderFolder);
}


BEGIN_MESSAGE_MAP(CPropertiesDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPropertiesDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPropertiesDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CPropertiesDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPropertiesDlg message handlers

BOOL CPropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_szShaderFolder = CRenderBase::GetInstance().GetShaderFolder();

	UpdateData( FALSE );

	if( m_bDisableCancel == true )
		GetDlgItem( IDCANCEL )->EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CPropertiesDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData();
	if( m_szShaderFolder.IsEmpty() ) {
		MessageBox( "������ ������ �ּ���", "����", MB_OK );
		return;
	}

	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ShaderFolder", m_szShaderFolder.GetBuffer() );

	if( CRenderBase::GetInstance().GetShaderFolder() != m_szShaderFolder )
		CRenderBase::GetInstance().ChangeShaderFolder( m_szShaderFolder );

	OnOK();
}

void CPropertiesDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_bDisableCancel == true ) return;
	OnCancel();
}

void CPropertiesDlg::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
