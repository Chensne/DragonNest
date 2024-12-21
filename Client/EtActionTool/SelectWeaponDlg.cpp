// SelectWeaponDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "SelectWeaponDlg.h"
#include "DnGameWeaponMng.h"


// CSelectWeaponDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSelectWeaponDlg, CDialog)

CSelectWeaponDlg::CSelectWeaponDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectWeaponDlg::IDD, pParent)
	, m_ctrlEdit1(_T(""))
	, m_ctrlEdit2(_T(""))
{

}

CSelectWeaponDlg::~CSelectWeaponDlg()
{
}

void CSelectWeaponDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ctrlEdit1);
	DDX_Text(pDX, IDC_EDIT5, m_ctrlEdit2);
}


BEGIN_MESSAGE_MAP(CSelectWeaponDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSelectWeaponDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelectWeaponDlg �޽��� ó�����Դϴ�.

void CSelectWeaponDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData();
	std::string szWeapon = m_ctrlEdit1.GetBuffer();
	int nWeapon1 = atoi( szWeapon.c_str() );
	szWeapon = m_ctrlEdit2.GetBuffer();
	int nWeapon2 = atoi( szWeapon.c_str() );

	CDnGameWeaponMng::GetInstance().ChangeAttachWeapon( nWeapon1, nWeapon2 );

	OnOK();
}

BOOL CSelectWeaponDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	int nWeapon1 = 0;
	int nWeapon2 = 0;
	CDnGameWeaponMng::GetInstance().GetAttachedWeapon( nWeapon1, nWeapon2 );
	char szTemp[64] = {0,};
	sprintf_s( szTemp, _countof(szTemp), "%d", nWeapon1 );
	m_ctrlEdit1 = szTemp;
	sprintf_s( szTemp, _countof(szTemp), "%d", nWeapon2 );
	m_ctrlEdit2 = szTemp;

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}