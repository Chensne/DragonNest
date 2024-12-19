#include "StdAfx.h"
#include "XTCustomPropertyGridItemColor.h"
#include "PropertyGrid/XTPPropertyGridInplaceEdit.h"
#include "PropertyGrid/XTPPropertyGridInplaceButton.h"
#include "PropertyGrid/XTPPropertyGridInplaceList.h"
#include "PropertyGrid/XTPPropertyGridItem.h"
#include "Controls/XTColorDialog.h"

#ifdef _XTP_INCLUDE_CONTROLS
#include "Controls/XTColorDialog.h"
#endif


CXTCustomPropertyGridItemColor::CXTCustomPropertyGridItemColor( const CString& strCaption, COLORREF clr, COLORREF* pBindColor )
: CXTPPropertyGridItem(strCaption)
{
	m_pBindColor = pBindColor;
	SetColor(clr);

	m_strDefaultValue = m_strValue;
	m_nFlags = xtpGridItemHasEdit | xtpGridItemHasExpandButton;
}

CXTCustomPropertyGridItemColor::CXTCustomPropertyGridItemColor( UINT nID, COLORREF clr, COLORREF* pBindColor )
: CXTPPropertyGridItem(nID)
{
	m_pBindColor = pBindColor;
	SetColor(clr);

	m_nFlags = xtpGridItemHasEdit | xtpGridItemHasExpandButton;
	m_strDefaultValue = m_strValue;
}

CXTCustomPropertyGridItemColor::~CXTCustomPropertyGridItemColor()
{
}

BOOL CXTCustomPropertyGridItemColor::OnDrawItemValue( CDC& dc, CRect rcValue )
{
	COLORREF clr = dc.GetTextColor();
	CRect rcSample(rcValue.left - 2, rcValue.top + 1, rcValue.left + 18, rcValue.bottom - 1);
	CXTPPenDC pen(dc, clr);
	CXTPBrushDC brush(dc, m_clrValue);

	dc.Rectangle(rcSample);

	CRect rcText(rcValue);
	rcText.left += 25;

	dc.DrawText(m_strValue, rcText, DT_SINGLELINE | DT_VCENTER);

	return TRUE;
}

CRect CXTCustomPropertyGridItemColor::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	rcValue.left += 25;
	return rcValue;
}

COLORREF AFX_CDECL CXTCustomPropertyGridItemColor::StringToRGB(LPCTSTR str)
{
	CString strRed, strGreen, strBlue;

	AfxExtractSubString( strRed, str, 0, ';' );
	AfxExtractSubString( strGreen, str, 1, ';' );
	AfxExtractSubString( strBlue, str, 2, ';' );

	return RGB(__min(_ttoi(strRed), 255), __min(_ttoi(strGreen), 255), __min(_ttoi(strBlue), 255));
}

CString AFX_CDECL CXTCustomPropertyGridItemColor::RGBToString(COLORREF clr)
{
	CString str;
	str.Format(_T("%i; %i; %i"), GetRValue(clr), GetGValue(clr), GetBValue(clr));

	return str;
}

void CXTCustomPropertyGridItemColor::SetValue(CString strValue)
{
	SetColor( StringToRGB(strValue) );
}

void CXTCustomPropertyGridItemColor::SetColor(COLORREF clr)
{
	m_clrValue = clr;

	if (m_pBindColor)
	{
		*m_pBindColor = clr;
	}

	CXTPPropertyGridItem::SetValue(RGBToString(clr));
}

void CXTCustomPropertyGridItemColor::BindToColor(COLORREF* pBindColor)
{
	m_pBindColor = pBindColor;
	if (m_pBindColor) *m_pBindColor = m_clrValue;
}


void CXTCustomPropertyGridItemColor::OnBeforeInsert()
{
	if (m_pBindColor && *m_pBindColor != m_clrValue) SetColor(*m_pBindColor);
}

void CXTCustomPropertyGridItemColor::OnInplaceButtonDown()
{
	if (!OnRequestEdit())
		return;

#ifdef _XTP_INCLUDE_CONTROLS
	CXTColorDialog dlg(m_clrValue, m_clrValue, CPS_XT_SHOW3DSELECTION, m_pGrid);
#else
	CColorDialog dlg(m_clrValue, 0, m_pGrid);
#endif

	if (dlg.DoModal() == IDOK)
	{
		CString strValue = RGBToString(dlg.GetColor());

		if (OnAfterEdit(strValue))
		{
			OnValueChanged(strValue);
			m_pGrid->Invalidate(FALSE);
		}
	}
	else
	{
		OnCancelEdit();
	}
}