// XTPMarkupBorder.h: interface for the CXTPMarkupBorder class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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

#if !defined(__XTPMARKUPBORDER_H__)
#define __XTPMARKUPBORDER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPMarkupDecorator.h"

class CXTPMarkupBrush;
class CXTPMarkupBuilder;

class _XTP_EXT_CLASS CXTPMarkupBorder : public CXTPMarkupDecorator
{
	DECLARE_MARKUPCLASS(CXTPMarkupBorder);

protected:
	CXTPMarkupBorder();
	virtual ~CXTPMarkupBorder();

public:
	void SetPadding(int nLeft, int nTop, int nRight, int nBottom);
	void SetPadding(int nPadding);
	CXTPMarkupThickness* GetPadding() const;

	void SetBorderThickness(int nLeft, int nTop, int nRight, int nBottom);
	void SetBorderThickness(int nBorderThickness);
	CXTPMarkupThickness* GetBorderThickness() const;

	void SetBackground(CXTPMarkupBrush* brush);
	CXTPMarkupBrush* GetBackground() const;

	void SetBorderBrush(CXTPMarkupBrush* brush);
	CXTPMarkupBrush* GetBorderBrush() const;

protected:
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize ArrangeOverride(CSize szFinalSize);
	virtual void OnRender(CXTPMarkupDrawingContext* drawingContext);
	virtual CXTPMarkupInputElement* InputHitTestOverride(CPoint point) const;

public:
	static CXTPMarkupDependencyProperty* m_pBackgroundProperty;
	static CXTPMarkupDependencyProperty* m_pBorderThicknessProperty;
	static CXTPMarkupDependencyProperty* m_pPaddingProperty;
	static CXTPMarkupDependencyProperty* m_pBorderBrushProperty;
};

AFX_INLINE void CXTPMarkupBorder::SetPadding(int nLeft, int nTop, int nRight, int nBottom) {
	SetValue(m_pPaddingProperty, new CXTPMarkupThickness(nLeft, nTop, nRight, nBottom));
}
AFX_INLINE void CXTPMarkupBorder::SetPadding(int padding) {
	SetValue(m_pPaddingProperty, new CXTPMarkupThickness(padding));
}
AFX_INLINE CXTPMarkupThickness* CXTPMarkupBorder::GetPadding() const {
	return  MARKUP_STATICCAST(CXTPMarkupThickness, GetValue(m_pPaddingProperty));
}
AFX_INLINE void CXTPMarkupBorder::SetBorderThickness(int nLeft, int nTop, int nRight, int nBottom) {
	SetValue(m_pBorderThicknessProperty, new CXTPMarkupThickness(nLeft, nTop, nRight, nBottom));
}
AFX_INLINE void CXTPMarkupBorder::SetBorderThickness(int padding) {
	SetValue(m_pBorderThicknessProperty, new CXTPMarkupThickness(padding));
}
AFX_INLINE CXTPMarkupThickness* CXTPMarkupBorder::GetBorderThickness() const {
	return  MARKUP_STATICCAST(CXTPMarkupThickness, GetValue(m_pBorderThicknessProperty));
}


#endif // !defined(__XTPMARKUPBORDER_H__)
