// LogFindDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "LogFindDlg.h"

IMPLEMENT_DYNAMIC(CLogFindDlg, CDialog)

CLogFindDlg::CLogFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogFindDlg::IDD, pParent), m_OptionFlags(0)
{

}

CLogFindDlg::~CLogFindDlg()
{
}

void CLogFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLogFindDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLogFindDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_LOG_FIND, &CLogFindDlg::OnEnChangeEditLogFind)
END_MESSAGE_MAP()

BOOL CLogFindDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem(IDOK);
	pButton->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLogFindDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString what;
	GetDlgItemText(IDC_EDIT_LOG_FIND, what);
	ASSERT(!(what.IsEmpty()));
	m_FindWhat = what.GetBuffer();

	m_OptionFlags = 0x00;

	if (IsDlgButtonChecked(IDC_CHECK_LOG_CASE) != 0)
		m_OptionFlags |= MATCH_CASE;

	if (IsDlgButtonChecked(IDC_CHECK_LOG_WHOLE_WORD) != 0)
		m_OptionFlags |= MATCH_WHOLE_WORD;

	OnOK();
}

void CLogFindDlg::OnEnChangeEditLogFind()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString text;
	GetDlgItemText(IDC_EDIT_LOG_FIND, text);

	CButton* pButton = (CButton*)GetDlgItem(IDOK);
	pButton->EnableWindow(!(text.IsEmpty()));
}
