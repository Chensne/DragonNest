// SubControlViChnlMeritView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "SubControlViChnlMeritView.h"
#include "ControlDialog.h"
#include "NetConnection.h"


// CSubControlViChnlMeritView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSubControlViChnlMeritView, CSubControlBaseView)

CSubControlViChnlMeritView::CSubControlViChnlMeritView(CWnd* pParent /*=NULL*/)
	: CSubControlBaseView(EV_CTT_VICHNLMERIT, CSubControlViChnlMeritView::IDD, pParent)
{

}

CSubControlViChnlMeritView::~CSubControlViChnlMeritView()
{
}

void CSubControlViChnlMeritView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSubControlViChnlMeritView, CSubControlBaseView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CSubControlViChnlMeritView::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

//	CDialog::OnOK();
}

void CSubControlViChnlMeritView::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

//	CDialog::OnCancel();
}

void CSubControlViChnlMeritView::UpdateData()
{
	CControlDialog* aControlDialog = static_cast<CControlDialog*>(GetParent()->GetParent());
	if (!aControlDialog) return;

	aControlDialog;


}

// CSubControlViChnlMeritView 메시지 처리기입니다.

BOOL CSubControlViChnlMeritView::OnInitDialog()
{
	CSubControlBaseView::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CSubControlViChnlMeritView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	switch(pMsg->message) {
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam) {
			case VK_ESCAPE:
				GetParent()->GetParent()->ShowWindow(SW_HIDE);
				return TRUE;	// ESC 키로 부모 탭 컨트롤의 부모 대화상자를 숨김
//				break;
			}
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSubControlViChnlMeritView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CSubControlBaseView::OnPaint()을(를) 호출하지 마십시오.

	{
		LOGFONT aLogFont;
		::memset(&aLogFont, 0x00, sizeof(aLogFont));
		_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
		aLogFont.lfCharSet = DEFAULT_CHARSET;
		aLogFont.lfHeight = 25;
		aLogFont.lfWeight = FW_ULTRABOLD;

		CFont aNewFont;
		VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
		CFont* aOldFont = dc.SelectObject(&aNewFont);

		CRect aRect;
		GetClientRect(&aRect);
		
		dc.DrawText(_T("Not Supported Yet"), &aRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		dc.SelectObject(aOldFont);
		aNewFont.DeleteObject();
	}

	
}

void CSubControlViChnlMeritView::OnSize(UINT nType, int cx, int cy)
{
	CSubControlBaseView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

//	m_LayoutManager.OnSize(cx,cy);
}

void CSubControlViChnlMeritView::OnDestroy()
{
	CSubControlBaseView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
