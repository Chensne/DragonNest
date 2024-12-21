// AddActionDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "AddActionDlg.h"
#include "SignalManager.h"
#include "GlobalValue.h"


// CAddActionDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CAddActionDlg, CDialog)

CAddActionDlg::CAddActionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddActionDlg::IDD, pParent)
	, m_szCustomEdit(_T(""))
{
	m_nType = 0;
}

CAddActionDlg::~CAddActionDlg()
{
}

void CAddActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_Combo1);
	DDX_Control(pDX, IDC_COMBO2, m_Combo2);
	DDX_Text(pDX, IDC_EDIT2, m_szCustomEdit);
}


BEGIN_MESSAGE_MAP(CAddActionDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT2, &CAddActionDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_RADIO1, &CAddActionDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CAddActionDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CAddActionDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDOK, &CAddActionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddActionDlg �޽��� ó�����Դϴ�.

void CAddActionDlg::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� �������ϰ�  ����ũ�� OR �����Ͽ� ������
	// ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ���ؾ߸�
	// �ش� �˸� �޽����� �����ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

BOOL CAddActionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CheckRadioButton( IDC_RADIO1, IDC_RADIO3, IDC_RADIO1 );
	GetDlgItem( IDC_COMBO1 )->EnableWindow( TRUE );
	GetDlgItem( IDC_COMBO2 )->EnableWindow( FALSE );
	GetDlgItem( IDC_EDIT2 )->EnableWindow( FALSE );

	for( DWORD i=0; i<CSignalManager::GetInstance().GetDefineCategoryCount(); i++ ) {
		m_Combo2.AddString( CSignalManager::GetInstance().GetDefineCategoryName(i) );

		for( DWORD j=0; j<CSignalManager::GetInstance().GetDefineStringCount(i); j++ ) {
			if( m_Combo1.FindString( 0, CSignalManager::GetInstance().GetDefineString( i, j ) ) == CB_ERR )
				m_Combo1.AddString( CSignalManager::GetInstance().GetDefineString( i, j ) );
		}
	}
	m_Combo1.SetCurSel(0);
	m_Combo2.SetCurSel(0);
	m_nType = 0;


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CAddActionDlg::OnBnClickedRadio1()
{
	m_nType = 0;
	GetDlgItem( IDC_COMBO1 )->EnableWindow( TRUE );
	GetDlgItem( IDC_COMBO2 )->EnableWindow( FALSE );
	GetDlgItem( IDC_EDIT2 )->EnableWindow( FALSE );
}

void CAddActionDlg::OnBnClickedRadio2()
{
	m_nType = 1;
	GetDlgItem( IDC_COMBO1 )->EnableWindow( FALSE );
	GetDlgItem( IDC_COMBO2 )->EnableWindow( TRUE );
	GetDlgItem( IDC_EDIT2 )->EnableWindow( FALSE );
}

void CAddActionDlg::OnBnClickedRadio3()
{
	m_nType = 2;
	GetDlgItem( IDC_COMBO1 )->EnableWindow( FALSE );
	GetDlgItem( IDC_COMBO2 )->EnableWindow( FALSE );
	GetDlgItem( IDC_EDIT2 )->EnableWindow( TRUE );
}


void CAddActionDlg::CalcResultString()
{
	switch( m_nType ) {
		case 0:
			m_Combo1.GetLBText( m_Combo1.GetCurSel(), m_szResultString );
			break;
		case 1:	
			m_Combo2.GetLBText( m_Combo2.GetCurSel(), m_szResultString );
			break;
				
		case 2:	
			m_szResultString = m_szCustomEdit;
			break;
	}
}

void CAddActionDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData();
	CalcResultString();
	switch( m_nType ) {
		case 0:
		case 2:
			{
				if( m_szResultString.IsEmpty() ) {
					MessageBox( "�׼��� �̸��� ������ �ּ���", "����", MB_OK );
					return;
				}
				if( CGlobalValue::GetInstance().GetObjectFromName( CGlobalValue::GetInstance().GetRootObject(), GetResultString() ) ) {
					MessageBox( "���� �̸��� �׼��� �̹� �����մϴ�.", "����", MB_OK );
					return;
				}
			}
			break;
	}
	OnOK();
}
