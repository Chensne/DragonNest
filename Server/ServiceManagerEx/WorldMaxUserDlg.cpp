// WorldMaxUserDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "WorldMaxUserDlg.h"

// CWorldMaxUserDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWorldMaxUserDlg, CDialog)

CWorldMaxUserDlg::CWorldMaxUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldMaxUserDlg::IDD, pParent), m_MaxUser(0)
{

}

CWorldMaxUserDlg::~CWorldMaxUserDlg()
{
}

void CWorldMaxUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWorldMaxUserDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CWorldMaxUserDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CWorldMaxUserDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_MaxUser = GetDlgItemInt(IDC_EDIT_WORLD_MAX_USER);

	wchar_t msg[256];
	::wsprintf(msg, L"Are you sure to set world max user? [users : %d]", m_MaxUser);

	if (IDYES != ::AfxMessageBox(msg, MB_YESNO))
		return;

	OnOK();
}
