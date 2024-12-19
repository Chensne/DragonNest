// MainDialogBar.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "MainDialogBar.h"


// CMainDialogBar

IMPLEMENT_DYNAMIC(CMainDialogBar, CDialogBar)

CMainDialogBar::CMainDialogBar()
{

}

CMainDialogBar::~CMainDialogBar()
{
}


BEGIN_MESSAGE_MAP(CMainDialogBar, CDialogBar)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CMainDialogBar �޽��� ó�����Դϴ�.



void CMainDialogBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialogBar::OnPaint()��(��) ȣ������ ���ʽÿ�.

	Graphics aGraphics(dc);
	aGraphics.SetSmoothingMode(SmoothingModeHighQuality);

	CRect aRect;
	GetClientRect(&aRect);

	// Draw Title
	{
		// Gradient Line
		{
			LinearGradientBrush aBrush(
				Rect(
					EV_UIS_TITLELINE_PAD_X, 
					EV_UIS_TITLELINE_PAD_Y, 
					aRect.right - (EV_UIS_TITLELINE_PAD_X * 2), 
					EV_UIS_TITLELINE_HGT
				),
				Color(100, 221, 221, 221),
				Color(255, 120, 120, 120),
				180.0f,
				FALSE
				);

			aGraphics.FillRectangle(
				&aBrush, 
				EV_UIS_TITLELINE_PAD_X, 
				EV_UIS_TITLELINE_PAD_Y, 
				aRect.right - (EV_UIS_TITLELINE_PAD_X * 2 + 1), 
				EV_UIS_TITLELINE_HGT
				);
		}

		// Draw Text
		{
			LOGFONT aLogFont;
			::memset(&aLogFont, 0x00, sizeof(aLogFont));
			_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
			aLogFont.lfCharSet = DEFAULT_CHARSET;
			aLogFont.lfHeight = 20;
			aLogFont.lfWeight = FW_EXTRABOLD;

			CFont aNewFont;
			VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
			CFont* aOldFont = dc.SelectObject(&aNewFont);

			COLORREF aOldTxColor = dc.SetTextColor(RGB(255, 255, 255));
			int aOldBkMode = dc.SetBkMode(TRANSPARENT);

			CRect aRect(
				EV_UIS_TITLELINE_PAD_X + EV_UIS_TITLETEXT_PAD_X, 
				EV_UIS_TITLELINE_PAD_Y, 
				aRect.right - EV_UIS_TITLELINE_PAD_X - EV_UIS_TITLETEXT_PAD_X, 
				aRect.top + EV_UIS_TITLELINE_HGT + EV_UIS_TITLELINE_PAD_Y
				);

			dc.DrawText(_T("Total Service Status"), &aRect, DT_LEFT | DT_VCENTER);

			dc.SetTextColor(aOldTxColor);
			dc.SetBkMode(aOldBkMode);

			dc.SelectObject(aOldFont);
			aNewFont.DeleteObject();
		}


	}


}
