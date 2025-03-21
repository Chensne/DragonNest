// XTPShortcutBarPaintManager.cpp : implementation of the CXTPShortcutBarPaintManager class.
//
// This file is a part of the XTREME SHORTCUTBAR MFC class library.
// (c)1998-2008 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"

#include "Common/XTPVC80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPOffice2007Image.h"
#include "Common/XTPSystemHelpers.h"

#include "XTPShortcutBarPaintManager.h"
#include "XTPShortcutBar.h"
#include "XTPShortcutBarPane.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPShortcutBarPaintManager::CXTPShortcutBarPaintManager()
{
	m_bBoldItemCaptionFont = FALSE;
	m_pExpandIcon = NULL;
	m_bClearTypeTextQuality = FALSE;

	CXTPImageManagerIconHandle iconHandle;
	iconHandle.CreateIconFromResource(AfxFindResourceHandle(MAKEINTRESOURCE(XTP_IDI_SHORTCUT_EXPAND_ALPHA),
		RT_GROUP_ICON), MAKEINTRESOURCE(XTP_IDI_SHORTCUT_EXPAND_ALPHA), CSize(16, 16), TRUE);
	ASSERT(!iconHandle.IsEmpty());

	if (!iconHandle.IsEmpty())
	{
		m_pExpandIcon = new CXTPImageManagerIcon(0, 16, 16);
		m_pExpandIcon->SetNormalIcon(iconHandle);
	}

	m_nNormalIconWidth = 16;
	m_nExpandedIconWidth = 24;
	m_bUseStandardFont = TRUE;
	m_nPaneItemHeight = 0;

	m_rcCaptionPadding.SetRect(4, 0, 4, 0);
	m_rcItemCaptionPadding.SetRect(7, 0, 7, 0);
}

CXTPShortcutBarPaintManager::~CXTPShortcutBarPaintManager()
{
	SAFE_DELETE(m_pExpandIcon);
}


void CXTPShortcutBarPaintManager::RefreshMetrics()
{
	RefreshXtremeColors();

	m_fntPaneCaption.DeleteObject();
	m_fntPaneItemCaption.DeleteObject();

	LOGFONT lf;

	VERIFY(::SystemParametersInfo(SPI_GETICONTITLELOGFONT,
		sizeof(LOGFONT), &lf, 0));

	if (m_bClearTypeTextQuality && XTPSystemVersion()->IsClearTypeTextQualitySupported())
	{
		lf.lfQuality = 5;
	}
	BOOL bOfficeFont = !m_strOfficeFont.IsEmpty() && XTPDrawHelpers()->FontExists(m_strOfficeFont);

	if (bOfficeFont)
	{
		STRCPY_S(lf.lfFaceName, LF_FACESIZE, m_strOfficeFont);
	}

	lf.lfWeight = m_bBoldItemCaptionFont ? FW_BOLD : FW_NORMAL;
	m_fntPaneItemCaption.CreateFontIndirect(&lf);

	if (m_bUseStandardFont || !m_fntText.GetSafeHandle())
	{
		lf.lfWeight = FW_BOLD;
		m_fntText.DeleteObject();
		m_fntText.CreateFontIndirect(&lf);
	}

	if (bOfficeFont)
	{
		STRCPY_S(lf.lfFaceName, LF_FACESIZE, m_strOfficeFont);
	}
	else
	{
		STRCPY_S(lf.lfFaceName, LF_FACESIZE, _T("Arial"));
	}
	lf.lfWeight = FW_BOLD;
	lf.lfHeight = 20;
	m_fntPaneCaption.CreateFontIndirect(&lf);

	CWindowDC dc(CWnd::GetDesktopWindow());
	CXTPFontDC font(&dc, &m_fntPaneItemCaption);
	m_nPaneItemHeight = max(19, dc.GetTextExtent(_T(" "), 1).cy + 4);

	m_clrPushedText.SetStandardValue(GetXtremeColor(XPCOLOR_PUSHED_TEXT));
	m_clrSelectedText.SetStandardValue(GetXtremeColor(XPCOLOR_CHECKED_TEXT));
	m_clrHotText.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_TEXT));
	m_clrNormalText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));

}

void CXTPShortcutBarPaintManager::DrawExpandButton(CDC* pDC, CRect rc)
{
	CPoint ptImage (rc.CenterPoint().x - 8, rc.CenterPoint().y - 8);

	if (m_pExpandIcon)
	{
		m_pExpandIcon->Draw(pDC, ptImage);
	}
}

BOOL CXTPShortcutBarPaintManager::IsShortcutItemTruncated(CDC* pDC, CXTPShortcutBarItem* pItem)
{
	if (!pItem->IsExpanded())
		return FALSE;

	int nTexetWidth = pItem->GetItemRect().Width() - 5;

	CXTPImageManagerIcon* pImage = pItem->GetImage(m_nExpandedIconWidth);
	if (pImage)
	{
		nTexetWidth -= m_nExpandedIconWidth + 5;
	}

	CRect rcText(0, 0, 0, 0);
	CXTPFontDC font(pDC, &m_fntText);
	pDC->DrawText(pItem->GetCaption(), rcText, DT_LEFT | DT_SINGLELINE | DT_CALCRECT);

	return rcText.Width() > nTexetWidth;
}


void CXTPShortcutBarPaintManager::DrawShortcutItem(CDC* pDC, CXTPShortcutBarItem* pItem)
{

	CRect rc = pItem->GetItemRect();


	CXTPShortcutBar* pBar = pItem->GetShortcutBar();

	pDC->SetTextColor(m_clrNormalText);
	FillShortcutItemEntry(pDC, pItem);

	CSize szIcon(pItem->IsExpanded() ? m_nExpandedIconWidth : m_nNormalIconWidth, 0);
	CXTPImageManagerIcon* pImage = pItem->GetImage(szIcon.cx);

	BOOL bSelected = pBar->GetHotItem() == pItem && (pBar->GetPressedItem() == NULL || pBar->GetPressedItem() == pItem),
		bPressed = pBar->GetPressedItem() == pItem, bChecked = pItem->IsSelected();


	if (pItem->IsExpanded())
	{
		CRect rcText(rc);
		rcText.left = 5;

		if (pImage)
		{
			CPoint ptImage (5, rc.CenterPoint().y - m_nExpandedIconWidth / 2);
			DrawImage(pDC, ptImage, szIcon, pImage, bSelected, bPressed, bChecked);

			rcText.left = 5 + m_nExpandedIconWidth + 5;

		}

		pDC->SetBkMode(TRANSPARENT);

		CXTPFontDC font(pDC, &m_fntText);
		pDC->DrawText(pItem->GetCaption(), rcText, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);

	}
	else
	{
		if (pItem->IsItemExpandButton())
		{
			DrawExpandButton(pDC, rc);

		}

		else if (pImage)
		{
			CPoint ptImage (rc.CenterPoint().x - m_nNormalIconWidth / 2, rc.CenterPoint().y - m_nNormalIconWidth / 2);
			DrawImage(pDC, ptImage, szIcon, pImage, bSelected, bPressed, bChecked);
		}
	}
}

int CXTPShortcutBarPaintManager::DrawPaneCaption(CDC* pDC, CXTPShortcutBarPane* pPane, BOOL bDraw)
{
	if (bDraw)
	{

		CXTPFontDC font(pDC, &m_fntPaneCaption);

		CXTPClientRect rc(pPane);
		rc.bottom = rc.top + 24;
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_3DSHADOW));

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(GetXtremeColor(COLOR_WINDOW));

		CRect rcText(rc);
		rcText.DeflateRect(m_rcCaptionPadding);
		pDC->DrawText(pPane->GetCaption(), rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
	}
	return 24;
}

int CXTPShortcutBarPaintManager::DrawPaneItemCaption(CDC* pDC, CXTPShortcutBarPaneItem* pItem, BOOL bDraw)
{
	if (bDraw)
	{
		CXTPFontDC font(pDC, &m_fntPaneItemCaption);

		CRect rc = pItem->GetCaptionRect();
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));

		pDC->FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, GetXtremeColor(COLOR_3DSHADOW));

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(GetXtremeColor(COLOR_BTNTEXT));

		CRect rcText(rc);
		rcText.DeflateRect(m_rcItemCaptionPadding);
		pDC->DrawText(pItem->GetCaption(), rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

	}

	return m_nPaneItemHeight;
}

void CXTPShortcutBarPaintManager::Rectangle(CDC* pDC, CRect rc, int nPen, int nBrush)
{
	pDC->FillSolidRect(rc, GetXtremeColor(nBrush));
	pDC->Draw3dRect(rc, GetXtremeColor(nPen), GetXtremeColor(nPen));
}
void CXTPShortcutBarPaintManager::Line(CDC* pDC, int x0, int y0, int x1, int y1, int nPen)
{
	CXTPPenDC pen(*pDC, GetXtremeColor(nPen));
	pDC->MoveTo(x0, y0);
	pDC->LineTo(x1, y1);
}


//////////////////////////////////////////////////////////////////////////
// CXTPShortcutBarOffice2003Theme

CXTPShortcutBarOffice2003Theme::CXTPShortcutBarOffice2003Theme()
{
	m_bShowTopLeftCaptionShadow = FALSE;
}

void CXTPShortcutBarOffice2003Theme::RefreshMetrics()
{
	CXTPShortcutBarPaintManager::RefreshMetrics();

	m_grcSelected.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_CHECKED));
	m_grcHot.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT));
	m_grcPushed.SetStandardValue(GetXtremeColor(XPCOLOR_HIGHLIGHT_PUSHED));
	m_grcShortcutItem.SetStandardValue(GetXtremeColor(COLOR_WINDOW), GetXtremeColor(COLOR_3DFACE));
	m_clrShortcutItemShadow.SetStandardValue(GetXtremeColor(XPCOLOR_FRAME));
	m_grcShortcutBarGripper.SetStandardValue(XTPColorManager()->grcShortcutBarGripper.clrLight,
		XTPColorManager()->grcShortcutBarGripper.clrDark);

	m_grcCaption.SetStandardValue(XTPColorManager()->grcCaption);
	m_clrCaptionText.SetStandardValue(GetXtremeColor(COLOR_WINDOW));

	m_grcItemCaption.SetStandardValue(XTPColorManager()->grcDockBar.clrDark, XTPColorManager()->grcDockBar.clrLight);
	m_grcItemHotCaption.SetStandardValue(m_grcItemCaption);
	m_clrItemCaptionText.SetStandardValue(GetXtremeColor(COLOR_BTNTEXT));

	if (XTPColorManager()->IsLowResolution())
	{
		m_grcShortcutItem.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
		m_clrShortcutItemShadow.SetStandardValue(GetXtremeColor(COLOR_3DSHADOW));
		m_grcShortcutBarGripper.SetStandardValue(GetXtremeColor(COLOR_3DSHADOW));
	}

	if (!XTPColorManager()->IsLunaColorsDisabled())
	{

		XTPCurrentSystemTheme systemTheme = XTPColorManager()->GetCurrentSystemTheme();

		switch (systemTheme)
		{
		case xtpSystemThemeBlue:
		case xtpSystemThemeRoyale:
		case xtpSystemThemeAero:
			m_grcShortcutItem.SetStandardValue(RGB(203, 225, 252), RGB(125, 165, 224));
			break;

		case xtpSystemThemeOlive:
			m_grcShortcutItem.SetStandardValue(RGB(234, 240, 207), RGB(177, 192, 140));
			break;

		case xtpSystemThemeSilver:
			m_grcShortcutItem.SetStandardValue(RGB(225, 226, 236), RGB(149, 147, 177));
			break;
		}

		if (systemTheme != xtpSystemThemeUnknown)
		{
			m_grcSelected.SetStandardValue(RGB(251, 230, 148), RGB(238, 149, 21));
			m_grcPushed.SetStandardValue(RGB(232, 127, 8), RGB(247, 218, 124));
			m_grcHot.SetStandardValue(RGB(255, 255, 220), RGB(247, 192, 91));

			m_clrPushedText.SetStandardValue(0);
		}
	}
	m_clrItemCaptionTop.SetStandardValue(XTPColorManager()->grcDockBar.clrLight);
	m_clrItemCaptionShadow.SetStandardValue(m_clrShortcutItemShadow);

	m_grcItemHotCaption.SetStandardValue(m_grcItemCaption);
}


int CXTPShortcutBarOffice2003Theme::DrawPaneCaption(CDC* pDC, CXTPShortcutBarPane* pPane, BOOL bDraw)
{
	if (bDraw)
	{
		CXTPFontDC font(pDC, &m_fntPaneCaption);

		CXTPClientRect rc(pPane);
		rc.bottom = rc.top + 24;
		XTPDrawHelpers()->GradientFill(pDC, rc, m_grcCaption, FALSE);

		if (m_bShowTopLeftCaptionShadow)
		{
			pDC->FillSolidRect(0, rc.top, rc.Width(), 1, RGB(0xFF, 0xFF, 0xFF));
			pDC->FillSolidRect(0, rc.top, 1, rc.Height(), RGB(0xFF, 0xFF, 0xFF));
		}

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_clrCaptionText);

		CRect rcText(rc);
		rcText.DeflateRect(m_rcCaptionPadding);
		pDC->DrawText(pPane->GetCaption(), rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
	}
	return 24;
}

int CXTPShortcutBarOffice2003Theme::DrawPaneItemCaption(CDC* pDC, CXTPShortcutBarPaneItem* pItem, BOOL bDraw)
{
	if (bDraw)
	{
		CXTPFontDC font(pDC, &m_fntPaneItemCaption);

		CRect rc = pItem->GetCaptionRect();
		XTPDrawHelpers()->GradientFill(pDC, rc, m_grcItemCaption, FALSE);

		pDC->FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, m_clrItemCaptionShadow);
		pDC->FillSolidRect(0, rc.top, rc.Width(), 1, m_clrItemCaptionTop);

		if (m_bShowTopLeftCaptionShadow)
		{
			pDC->FillSolidRect(0, rc.top + 1, rc.Width(), 1, RGB(0xFF, 0xFF, 0xFF));
			pDC->FillSolidRect(0, rc.top + 1, 1, rc.Height() - 2, RGB(0xFF, 0xFF, 0xFF));
		}


		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_clrItemCaptionText);

		CRect rcText(rc);
		rcText.DeflateRect(m_rcItemCaptionPadding);
		pDC->DrawText(pItem->GetCaption(), rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

	}

	return m_nPaneItemHeight;
}


void CXTPShortcutBarOffice2003Theme::FillShortcutBar(CDC* pDC, CXTPShortcutBar* pShortcutBar)
{
	CXTPClientRect rc(pShortcutBar);

	int nItemHeight = pShortcutBar->GetItemSize().cy;

	if (pShortcutBar->IsClientPaneVisible())
	{
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_WINDOW));

		int nTopExpandedLines = pShortcutBar->GetGripperRect().bottom +
			(pShortcutBar->GetExpandedLinesCount() - 1) * nItemHeight;

		XTPDrawHelpers()->GradientFill(pDC, CRect(0, nTopExpandedLines, rc.Width(), rc.bottom),
			m_grcShortcutItem, FALSE);
	}
	else
	{
		XTPDrawHelpers()->GradientFill(pDC, CRect(0, (pShortcutBar->GetExpandedLinesCount() - 1) * nItemHeight - 1, rc.Width(),
			rc.Height()) , m_grcShortcutItem, FALSE);
	}
}

void CXTPShortcutBarOffice2003Theme::DrawShortcutBarFrame(CDC* pDC, CXTPShortcutBar* pShortcutBar)
{
	CXTPWindowRect rc(pShortcutBar);
	rc.OffsetRect(-rc.TopLeft());
	pDC->Draw3dRect(rc, m_clrShortcutItemShadow, m_clrShortcutItemShadow);
}

void CXTPShortcutBarOffice2003Theme::FillShortcutItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem)
{
	CXTPShortcutBar* pShortcutBar = pItem->GetShortcutBar();
	CRect rc = pItem->GetItemRect();
	CXTPClientRect rcShortcutBar(pShortcutBar);

	if (pItem->IsSelected() &&
		(pShortcutBar->GetHotItem() != pItem || (pShortcutBar->GetPressedItem() != NULL && pShortcutBar->GetPressedItem() != pItem)))
	{
		XTPDrawHelpers()->GradientFill(pDC, rc, m_grcSelected, FALSE);
		pDC->SetTextColor(m_clrSelectedText);
	}
	else if (pItem->IsSelected() ||
		(pShortcutBar->GetHotItem() == pItem && pShortcutBar->GetPressedItem() == pItem))
	{
		XTPDrawHelpers()->GradientFill(pDC, rc, m_grcPushed, FALSE);
		pDC->SetTextColor(m_clrPushedText);
	}
	else if ((pShortcutBar->GetHotItem() == pItem && pShortcutBar->GetPressedItem() == NULL) ||
		pShortcutBar->GetPressedItem() == pItem)
	{
		XTPDrawHelpers()->GradientFill(pDC, rc, m_grcHot, FALSE);
		pDC->SetTextColor(m_clrHotText);
	}
	else if (pItem->IsExpanded())
	{
		XTPDrawHelpers()->GradientFill(pDC, rc, m_grcShortcutItem, FALSE);
	}

	if (pItem->IsExpanded() && rc.bottom != rcShortcutBar.bottom)
	{
		pDC->FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, m_clrShortcutItemShadow);
	}
}


int CXTPShortcutBarOffice2003Theme::DrawGripper(CDC* pDC, CXTPShortcutBar* pShortcutBar, BOOL bDraw)
{
	if (bDraw)
	{
		CRect rc = pShortcutBar->GetGripperRect();
		XTPDrawHelpers()->GradientFill(pDC, rc, m_grcShortcutBarGripper, FALSE);

		int nRight = max (2, (rc.Width() - 4 * 9) / 2);
		for (int i = 0; i < 9; i++)
		{
			pDC->FillSolidRect(nRight, rc.top + 1, 2, 2, RGB(40, 50, 71));
			pDC->FillSolidRect(nRight + 1, rc.top + 2, 2, 2, RGB(249, 249, 251));
			pDC->FillSolidRect(nRight + 1, rc.top + 2, 1, 1, RGB(97, 116, 152));

			nRight += 4;
		}
	}
	return 7;
}



//////////////////////////////////////////////////////////////////////////
// CXTPShortcutBarOfficeXPTheme

void CXTPShortcutBarOfficeXPTheme::FillShortcutBar(CDC* pDC, CXTPShortcutBar* pShortcutBar)
{
	CXTPClientRect rc(pShortcutBar);

	pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_TOOLBAR_FACE));

}

void CXTPShortcutBarOfficeXPTheme::DrawShortcutBarFrame(CDC* pDC, CXTPShortcutBar* pShortcutBar)
{
	CXTPWindowRect rc(pShortcutBar);
	rc.OffsetRect(-rc.TopLeft());
	pDC->Draw3dRect(rc, GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHIGHLIGHT));
}

void CXTPShortcutBarOfficeXPTheme::FillShortcutItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem)
{
	CXTPShortcutBar* pShortcutBar = pItem->GetShortcutBar();
	CRect rc = pItem->GetItemRect();

	if (pItem->IsSelected() &&
		(pShortcutBar->GetHotItem() != pItem || (pShortcutBar->GetPressedItem() != NULL && pShortcutBar->GetPressedItem() != pItem)))
	{
		Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_CHECKED);
		pDC->SetTextColor(GetXtremeColor(XPCOLOR_CHECKED_TEXT));
	}
	else if (pItem->IsSelected() ||
		(pShortcutBar->GetHotItem() == pItem && pShortcutBar->GetPressedItem() == pItem))
	{
		Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED);
		pDC->SetTextColor(m_clrPushedText);
	}
	else if ((pShortcutBar->GetHotItem() == pItem && pShortcutBar->GetPressedItem() == NULL) ||
		pShortcutBar->GetPressedItem() == pItem)
	{
		Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT);
		pDC->SetTextColor(GetXtremeColor(XPCOLOR_HIGHLIGHT_TEXT));
	}
}


int CXTPShortcutBarOfficeXPTheme::DrawGripper(CDC* pDC, CXTPShortcutBar* pShortcutBar, BOOL bDraw)
{
	if (bDraw)
	{
		CRect rc = pShortcutBar->GetGripperRect();

		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));


		pDC->FillSolidRect(rc.left, rc.top, rc.Width(), 1, GetXtremeColor(COLOR_3DHIGHLIGHT));
		pDC->FillSolidRect(rc.left, rc.bottom - 1, rc.Width(), 1, GetXtremeColor(COLOR_3DSHADOW));


		Line(pDC, rc.CenterPoint().x - 16, rc.top + 3, rc.CenterPoint().x + 16, rc.top + 3, XPCOLOR_TOOLBAR_GRIPPER);
		Line(pDC, rc.CenterPoint().x - 16, rc.top + 5, rc.CenterPoint().x + 16, rc.top + 5, XPCOLOR_TOOLBAR_GRIPPER);


	}
	return 8;
}


//////////////////////////////////////////////////////////////////////////
// CXTPShortcutBarOffice2000Theme

void CXTPShortcutBarOffice2000Theme::FillShortcutBar(CDC* pDC, CXTPShortcutBar* pShortcutBar)
{
	CXTPClientRect rc(pShortcutBar);

	pDC->FillSolidRect(rc, GetXtremeColor(COLOR_3DSHADOW));

}

void CXTPShortcutBarOffice2000Theme::DrawShortcutBarFrame(CDC* pDC, CXTPShortcutBar* pShortcutBar)
{
	CXTPWindowRect rc(pShortcutBar);
	rc.OffsetRect(-rc.TopLeft());
	pDC->Draw3dRect(rc, GetXtremeColor(COLOR_3DSHADOW), GetXtremeColor(COLOR_3DHIGHLIGHT));
}


void CXTPShortcutBarOffice2000Theme::FillShortcutItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem)
{
	CXTPShortcutBar* pShortcutBar = pItem->GetShortcutBar();
	CRect rc = pItem->GetItemRect();

	if (pItem->IsSelected() &&
		(pShortcutBar->GetHotItem() != pItem || (pShortcutBar->GetPressedItem() != NULL && pShortcutBar->GetPressedItem() != pItem)))
	{
		pDC->Draw3dRect(rc, GetXtremeColor(COLOR_BTNTEXT), GetXtremeColor(COLOR_3DHIGHLIGHT));
	}
	else if (pItem->IsSelected() ||
		(pShortcutBar->GetHotItem() == pItem && pShortcutBar->GetPressedItem() == pItem))
	{
		pDC->Draw3dRect(rc, GetXtremeColor(COLOR_BTNTEXT), GetXtremeColor(COLOR_3DHIGHLIGHT));
	}
	else if ((pShortcutBar->GetHotItem() == pItem && pShortcutBar->GetPressedItem() == NULL) ||
		pShortcutBar->GetPressedItem() == pItem)
	{
		pDC->Draw3dRect(rc, GetXtremeColor(COLOR_3DHIGHLIGHT), GetXtremeColor(COLOR_BTNTEXT));
	}

	pDC->SetTextColor(GetXtremeColor(COLOR_3DHIGHLIGHT));
}


int CXTPShortcutBarOffice2000Theme::DrawGripper(CDC* pDC, CXTPShortcutBar* pShortcutBar, BOOL bDraw)
{
	if (bDraw)
	{
		CRect rc = pShortcutBar->GetGripperRect();
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));
	}
	return 5;
}



void CXTPShortcutBarOffice2000Theme::DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{
	if (bChecked) pImage->Draw(pDC, pt, bChecked && (bSelected || bPressed) ? pImage->GetHotIcon() : pImage->GetCheckedIcon(), sz);
	else if (!bSelected && !bPressed)
	{
		pImage->Draw(pDC, pt, pImage->GetIcon(), sz);
	}
	else if ((bSelected && !bPressed) || (!bSelected && bPressed))
	{
		pImage->Draw(pDC, pt, pImage->GetHotIcon(), sz);
	}
	else if ((bSelected && bPressed))
		pImage->Draw(pDC, CPoint(pt.x + 1, pt.y + 1), pImage->GetHotIcon(), sz);
}


void CXTPShortcutBarOfficeXPTheme::DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{

	if (bChecked) pImage->Draw(pDC, pt, bChecked && (bSelected || bPressed) ? pImage->GetHotIcon() : pImage->GetCheckedIcon(), sz);
	else if (!bSelected && !bPressed)
	{
		pImage->Draw(pDC, pt, pImage->GetFadedIcon(), sz);
	}
	else if ((bSelected && !bPressed) || (!bSelected && bPressed))
	{
		pImage->Draw(pDC, CPoint(pt.x + 1, pt.y + 1), pImage->GetShadowIcon(), sz);
		pImage->Draw(pDC, CPoint(pt.x - 1, pt.y - 1), pImage->GetHotIcon(), sz);
	}
	else if (bSelected && bPressed)
		pImage->Draw(pDC, pt, pImage->GetHotIcon(), sz);
}

void CXTPShortcutBarOffice2003Theme::DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{
	pImage->Draw(pDC, pt, bSelected || bPressed ? pImage->GetHotIcon(): bChecked ? pImage->GetCheckedIcon() : pImage->GetIcon(), sz);
}

//////////////////////////////////////////////////////////////////////////
// CXTPShortcutBarOffice2007Theme
CXTPShortcutBarOffice2007Theme::CXTPShortcutBarOffice2007Theme()
{
	m_bBoldItemCaptionFont = TRUE;
	m_bShowTopLeftCaptionShadow = TRUE;
	m_bClearTypeTextQuality = TRUE;
	m_strOfficeFont = _T("Segoe UI");

	m_rcCaptionPadding.SetRect(8, 0, 8, 0);
	m_rcItemCaptionPadding.SetRect(8, 0, 8, 0);
}

void CXTPShortcutBarOffice2007Theme::RefreshMetrics()
{
	CXTPShortcutBarOffice2003Theme::RefreshMetrics();

	CXTPOffice2007Images* pImages = XTPOffice2007Images();

	CXTPOffice2007Image* pImage = pImages->LoadFile(_T("SHORTCUTBARITEMS"));
	ASSERT(pImage);
	if (!pImage)
		return;

	COLORREF clrText = pImages->GetImageColor(_T("ShortcutBar"), _T("NormalText"));
	m_clrPushedText.SetStandardValue(clrText);
	m_clrNormalText.SetStandardValue(clrText);
	m_clrHotText.SetStandardValue(clrText);
	m_clrSelectedText.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("SelectedText")));

	m_clrShortcutItemShadow.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("FrameBorder")));

	m_grcCaption.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("CaptionLight")),
		pImages->GetImageColor(_T("ShortcutBar"), _T("CaptionDark")));
	m_clrCaptionText.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("CaptionText")));

	m_grcShortcutBarGripper.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("GripperLight")),
		pImages->GetImageColor(_T("ShortcutBar"), _T("GripperDark")));

	m_grcItemCaption.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("ItemNormalCaptionLight")),
		pImages->GetImageColor(_T("ShortcutBar"), _T("ItemNormalCaptionDark")));

	m_grcItemHotCaption.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("ItemHotCaptionLight")),
		pImages->GetImageColor(_T("ShortcutBar"), _T("ItemHotCaptionDark")));

	m_clrItemCaptionText.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("ItemCaptionText")));

	m_clrItemCaptionTop.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("ItemCaptionTop")));
	m_clrItemCaptionShadow.SetStandardValue(pImages->GetImageColor(_T("ShortcutBar"), _T("ItemCaptionShadow")));

	m_grcShortcutItem.SetStandardValue(RGB(227, 239, 255), RGB(192, 219, 255), 0.25f);

}


int CXTPShortcutBarOffice2007Theme::DrawPaneItemCaption(CDC* pDC, CXTPShortcutBarPaneItem* pItem, BOOL bDraw)
{
	if (bDraw)
	{
		CXTPFontDC font(pDC, &m_fntPaneItemCaption);

		CRect rc = pItem->GetCaptionRect();

		COLORREF clrLight = pItem->IsHighlighted() ? m_grcItemHotCaption.clrLight : m_grcItemCaption.clrLight;
		COLORREF clrDark = pItem->IsHighlighted() ? m_grcItemHotCaption.clrDark : m_grcItemCaption.clrDark;

		XTPDrawHelpers()->GradientFill(pDC, CRect(rc.left, rc.top, rc.CenterPoint().x, rc.bottom), clrDark, clrLight, TRUE);
		XTPDrawHelpers()->GradientFill(pDC, CRect(rc.CenterPoint().x, rc.top, rc.right, rc.bottom), clrLight, clrDark, TRUE);

		pDC->FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, m_clrItemCaptionShadow);
		pDC->FillSolidRect(0, rc.top, rc.Width(), 1, m_clrItemCaptionTop);

		if (m_bShowTopLeftCaptionShadow)
		{
			pDC->FillSolidRect(0, rc.top + 1, rc.Width(), 1, RGB(0xFF, 0xFF, 0xFF));
			pDC->FillSolidRect(0, rc.top + 1, 1, rc.Height() - 2, RGB(0xFF, 0xFF, 0xFF));
		}

		CRect rcText(rc);
		rcText.DeflateRect(m_rcItemCaptionPadding);

		if (pItem->IsExpandable())
		{
			CXTPOffice2007Image* pImage = XTPOffice2007Images()->LoadFile(_T("SHORTCUTBARGLYPHS"));
			ASSERT(pImage);
			if (!pImage)
				return m_nPaneItemHeight;

			CRect rcSrc(pImage->GetSource(pItem->IsExpanded() ? 2 : 3, 5));

			CRect rcDest(CPoint(rc.right - 2 - rcSrc.Width(), (rc.top + rc.bottom + 2 - rcSrc.Height()) / 2), rcSrc.Size());
			pImage->DrawImage(pDC, rcDest, rcSrc);

			rcText.right = rcDest.left - 2;
		}

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_clrItemCaptionText);

		pDC->DrawText(pItem->GetCaption(), rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

	}

	return m_nPaneItemHeight;
}

void CXTPShortcutBarOffice2007Theme::FillShortcutBar(CDC* pDC, CXTPShortcutBar* pShortcutBar)
{
	CXTPOffice2007Image* pImage = XTPOffice2007Images()->LoadFile(_T("SHORTCUTBARITEMS"));
	if (!pImage)
	{
		CXTPShortcutBarOffice2003Theme::FillShortcutBar(pDC, pShortcutBar);
		return;
	}


	CXTPClientRect rc(pShortcutBar);

	int nItemHeight = pShortcutBar->GetItemSize().cy;

	if (pShortcutBar->IsClientPaneVisible())
	{
		pDC->FillSolidRect(rc, GetXtremeColor(COLOR_WINDOW));

		int nTopExpandedLines = pShortcutBar->GetGripperRect().bottom +
			(pShortcutBar->GetExpandedLinesCount() - 1) * nItemHeight;

		pImage->DrawImage(pDC, CRect(0, nTopExpandedLines, rc.Width(), rc.bottom), pImage->GetSource(0, 4));
	}
	else
	{
		pImage->DrawImage(pDC, CRect(0, (pShortcutBar->GetExpandedLinesCount() - 1) * nItemHeight - 1, rc.Width(),
			rc.Height()), pImage->GetSource(0, 4));
	}
}

void CXTPShortcutBarOffice2007Theme::FillShortcutItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem)
{
	CXTPOffice2007Image* pImage = XTPOffice2007Images()->LoadFile(_T("SHORTCUTBARITEMS"));

	if (!pImage)
	{
		CXTPShortcutBarOffice2003Theme::FillShortcutItemEntry(pDC, pItem);
		return;
	}

	CXTPShortcutBar* pShortcutBar = pItem->GetShortcutBar();
	CRect rc = pItem->GetItemRect();
	CXTPClientRect rcShortcutBar(pShortcutBar);

	if (pItem->IsSelected() &&
		(pShortcutBar->GetHotItem() != pItem || (pShortcutBar->GetPressedItem() != NULL && pShortcutBar->GetPressedItem() != pItem)))
	{
		pImage->DrawImage(pDC, rc, pImage->GetSource(2, 4));
	}
	else if (pItem->IsSelected() ||
		(pShortcutBar->GetHotItem() == pItem && pShortcutBar->GetPressedItem() == pItem))
	{
		pImage->DrawImage(pDC, rc, pImage->GetSource(3, 4));
		pDC->SetTextColor(m_clrPushedText);
	}
	else if ((pShortcutBar->GetHotItem() == pItem && pShortcutBar->GetPressedItem() == NULL) ||
		pShortcutBar->GetPressedItem() == pItem)
	{
		pImage->DrawImage(pDC, rc, pImage->GetSource(1, 4));
		pDC->SetTextColor(m_clrHotText);
	}
	else if (pItem->IsExpanded())
	{
		pImage->DrawImage(pDC, rc, pImage->GetSource(0, 4));
	}

	if (pItem->IsSelected())
	{
		pDC->SetTextColor(m_clrSelectedText);
	}


	if (pItem->IsExpanded() && rc.bottom != rcShortcutBar.bottom)
	{
		pDC->FillSolidRect(0, rc.bottom - 1, rc.Width(), 1, m_clrShortcutItemShadow);
	}
}

void CXTPShortcutBarOffice2007Theme::DrawExpandButton(CDC* pDC, CRect rc)
{
	CXTPOffice2007Image* pImage = XTPOffice2007Images()->LoadFile(_T("SHORTCUTBARGLYPHS"));
	ASSERT(pImage);
	if (!pImage)
	{
		CXTPShortcutBarOffice2003Theme::DrawExpandButton(pDC, rc);
		return;
	}

	CRect rcSrc(pImage->GetSource(4, 5));
	CRect rcDest(CPoint((rc.left + rc.right - rcSrc.Width()) / 2, (rc.top + rc.bottom - rcSrc.Height()) / 2), rcSrc.Size());

	pImage->DrawImage(pDC, rcDest, rcSrc);
}

int CXTPShortcutBarOffice2007Theme::DrawGripper(CDC* pDC, CXTPShortcutBar* pShortcutBar, BOOL bDraw)
{
	if (bDraw)
	{
		CXTPOffice2007Image* pImage = XTPOffice2007Images()->LoadFile(_T("SHORTCUTBARGRIPPER"));
		ASSERT(pImage);
		if (!pImage)
		{
			return CXTPShortcutBarOffice2003Theme::DrawGripper(pDC, pShortcutBar, bDraw);
		}

		CRect rc = pShortcutBar->GetGripperRect();
		XTPDrawHelpers()->GradientFill(pDC, rc, m_grcShortcutBarGripper, FALSE);

		pDC->FillSolidRect(rc.left, rc.top, rc.Width(), 1, m_clrShortcutItemShadow);
		pDC->FillSolidRect(rc.left, rc.top + 1, rc.Width(), 1, RGB(0xFF, 0xFF, 0xFF));
		pDC->FillSolidRect(rc.left, rc.bottom - 1, rc.Width(), 1, m_clrShortcutItemShadow);

		CRect rcSrc(pImage->GetSource());
		CRect rcDest(CPoint((rc.left + rc.right - rcSrc.Width()) / 2, (rc.top + rc.bottom - rcSrc.Height()) / 2), rcSrc.Size());

		pImage->DrawImage(pDC, rcDest, rcSrc);
	}
	return 9;
}



