// SelectWeaponDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "SelectWeaponDlg.h"
#include "DnGameWeaponMng.h"


// CSelectWeaponDlg 대화 상자입니다.

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


// CSelectWeaponDlg 메시지 처리기입니다.

void CSelectWeaponDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}