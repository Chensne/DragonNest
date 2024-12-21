// DnTextModifyDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DnResourceEditer.h"
#include "DnTextModifyDlg.h"
#include "DnControlManager.h"


// CDnTextModifyDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDnTextModifyDlg, CDialog)

CDnTextModifyDlg::CDnTextModifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnTextModifyDlg::IDD, pParent)
{
}

CDnTextModifyDlg::~CDnTextModifyDlg()
{
}

void CDnTextModifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_editText);
}


BEGIN_MESSAGE_MAP(CDnTextModifyDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_TEXT, &CDnTextModifyDlg::OnEnChangeEditText)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDnTextModifyDlg 메시지 처리기입니다.

void CDnTextModifyDlg::OnEnChangeEditText()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.
}

void CDnTextModifyDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_editText)
	{
		CString szStr;
		m_editText.GetWindowText(szStr);
		DNCTRLMANAGER.SetEditString(szStr);
	}
}

BOOL CDnTextModifyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_editText.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
