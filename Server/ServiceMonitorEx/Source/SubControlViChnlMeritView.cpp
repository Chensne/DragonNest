// SubControlViChnlMeritView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "SubControlViChnlMeritView.h"
#include "ControlDialog.h"
#include "NetConnection.h"


// CSubControlViChnlMeritView ��ȭ �����Դϴ�.

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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnOK();
}

void CSubControlViChnlMeritView::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnCancel();
}

void CSubControlViChnlMeritView::UpdateData()
{
	CControlDialog* aControlDialog = static_cast<CControlDialog*>(GetParent()->GetParent());
	if (!aControlDialog) return;

	aControlDialog;


}

// CSubControlViChnlMeritView �޽��� ó�����Դϴ�.

BOOL CSubControlViChnlMeritView::OnInitDialog()
{
	CSubControlBaseView::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CSubControlViChnlMeritView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	switch(pMsg->message) {
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam) {
			case VK_ESCAPE:
				GetParent()->GetParent()->ShowWindow(SW_HIDE);
				return TRUE;	// ESC Ű�� �θ� �� ��Ʈ���� �θ� ��ȭ���ڸ� ����
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CSubControlBaseView::OnPaint()��(��) ȣ������ ���ʽÿ�.

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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

//	m_LayoutManager.OnSize(cx,cy);
}

void CSubControlViChnlMeritView::OnDestroy()
{
	CSubControlBaseView::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
