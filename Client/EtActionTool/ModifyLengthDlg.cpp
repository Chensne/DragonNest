// ModifyLengthDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "ModifyLengthDlg.h"


// CModifyLengthDlg 대화 상자입니다.

int CModifyLengthDlg::s_nType = 1;
bool CModifyLengthDlg::s_bYesAll = false;

IMPLEMENT_DYNAMIC(CModifyLengthDlg, CDialog)

CModifyLengthDlg::CModifyLengthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyLengthDlg::IDD, pParent)
	, m_szActionName(_T(""))
{

}

CModifyLengthDlg::~CModifyLengthDlg()
{
}

void CModifyLengthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_szActionName);
}


BEGIN_MESSAGE_MAP(CModifyLengthDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CModifyLengthDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CModifyLengthDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOKALL, &CModifyLengthDlg::OnBnClickedOkall)
	ON_BN_CLICKED(IDC_RADIO1, &CModifyLengthDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CModifyLengthDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CModifyLengthDlg::OnBnClickedRadio3)
END_MESSAGE_MAP()


// CModifyLengthDlg 메시지 처리기입니다.

void CModifyLengthDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CModifyLengthDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	s_bYesAll = false;
	OnOK();
}

void CModifyLengthDlg::OnBnClickedOkall()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	s_bYesAll = true;
	OnOK();
}

BOOL CModifyLengthDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	int nDefaultButton = 0;
	switch( s_nType ) {
		case 0:	nDefaultButton = IDC_RADIO1;	break;
		case 1: nDefaultButton = IDC_RADIO2;	break;
		case 2: nDefaultButton = IDC_RADIO3;	break;
	}
	CheckRadioButton( IDC_RADIO1, IDC_RADIO3, nDefaultButton );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CModifyLengthDlg::OnBnClickedRadio1()
{
	s_nType = 0;
}

void CModifyLengthDlg::OnBnClickedRadio2()
{
	s_nType = 1;
}

void CModifyLengthDlg::OnBnClickedRadio3()
{
	s_nType = 2;
}
