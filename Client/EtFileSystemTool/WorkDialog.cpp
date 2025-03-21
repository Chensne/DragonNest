// WorkDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NxFileSystemTool.h"
#include "WorkDialog.h"
#include "FileIOThread.h"

CWorkDialog*		g_pWorking;

// CWorkDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWorkDialog, CDialog)
CWorkDialog::CWorkDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkDialog::IDD, pParent)
{
}

CWorkDialog::~CWorkDialog()
{
}

void 
CWorkDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}


BEGIN_MESSAGE_MAP(CWorkDialog, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_NFS_PROGRESS, OnImportProgress)
END_MESSAGE_MAP()



BOOL
CWorkDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch(pMsg->wParam)
	{
	case VK_ESCAPE:
		pMsg->wParam=NULL; // ESC 는 무시한다.
		break;
	case VK_RETURN:
		pMsg->wParam=NULL; // 엔터도 무시한다.
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void 
CWorkDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	::PostMessage(this->GetSafeHwnd(),WM_NCLBUTTONDOWN,HTCAPTION,0);

	CDialog::OnLButtonDown(nFlags, point);
}


LRESULT 
CWorkDialog::OnImportProgress(WPARAM wParam, LPARAM lParam)
{
	m_Progress.SetRange32((int)0, (int)lParam);
	m_Progress.SetPos((int)wParam);

	return 0;

}