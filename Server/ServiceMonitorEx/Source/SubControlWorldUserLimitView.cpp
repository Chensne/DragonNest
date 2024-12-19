// SubControlWorldUserLimitView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "SubControlWorldUserLimitView.h"
#include "ControlDialog.h"
#include "NetConnection.h"


// CSubControlWorldUserLimitView ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSubControlWorldUserLimitView, CSubControlBaseView)

CSubControlWorldUserLimitView::CSubControlWorldUserLimitView(CWnd* pParent /*=NULL*/)
	: CSubControlBaseView(EV_CTT_WORLDUSERLIMIT, CSubControlWorldUserLimitView::IDD, pParent)
{

}

CSubControlWorldUserLimitView::~CSubControlWorldUserLimitView()
{
}

void CSubControlWorldUserLimitView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSubControlWorldUserLimitView, CSubControlBaseView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CSubControlWorldUserLimitView::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnOK();
}

void CSubControlWorldUserLimitView::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnCancel();
}

void CSubControlWorldUserLimitView::UpdateData()
{
	CControlDialog* aControlDialog = static_cast<CControlDialog*>(GetParent()->GetParent());
	if (!aControlDialog) return;

	aControlDialog;


}

// CSubControlWorldUserLimitView �޽��� ó�����Դϴ�.

BOOL CSubControlWorldUserLimitView::OnInitDialog()
{
	CSubControlBaseView::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CSubControlWorldUserLimitView::PreTranslateMessage(MSG* pMsg)
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

void CSubControlWorldUserLimitView::OnPaint()
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

void CSubControlWorldUserLimitView::OnSize(UINT nType, int cx, int cy)
{
	CSubControlBaseView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

//	m_LayoutManager.OnSize(cx,cy);
}

void CSubControlWorldUserLimitView::OnDestroy()
{
	CSubControlBaseView::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
