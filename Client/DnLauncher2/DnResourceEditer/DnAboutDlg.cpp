// DnAboutDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DnResourceEditer.h"
#include "DnAboutDlg.h"


// CDnAboutDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDnAboutDlg, CDialog)

CDnAboutDlg::CDnAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnAboutDlg::IDD, pParent)
{

}

CDnAboutDlg::~CDnAboutDlg()
{
}

void CDnAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDnAboutDlg, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDnAboutDlg 메시지 처리기입니다.

BOOL CDnAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	


	return TRUE;  // return TRUE unless you set the focus to a control
}

int CDnAboutDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}
