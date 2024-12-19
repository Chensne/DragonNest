#include "StdAfx.h"
#include "Common/XTPVC80Helpers.h"

#include "PropertyGrid/XTPPropertyGridInplaceEdit.h"
#include "PropertyGrid/XTPPropertyGridInplaceButton.h"
#include "PropertyGrid/XTPPropertyGridInplaceList.h"
#include "PropertyGrid/XTPPropertyGridItem.h"
#include "XTCustomPropertyGridItemNumberSlider.h"

IMPLEMENT_DYNAMIC(CMiniSliderPropertyGrid, CWnd)

CMiniSliderPropertyGrid::CMiniSliderPropertyGrid()
{
	m_bRange = false;
	m_fPos = 25.f;
	m_fEndPos = 25.f;
	m_bLButtonDown = false;
	m_pNumClass = NULL;
	m_pDoubleClass = NULL;
	m_pVectorClass = NULL;
	m_pFloatClass = NULL;
}

CMiniSliderPropertyGrid::~CMiniSliderPropertyGrid()
{
}


BEGIN_MESSAGE_MAP(CMiniSliderPropertyGrid, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE( UM_MINISLIDER_SET_POS, OnSetPos )
	ON_MESSAGE( UM_MINISLIDER_SET_END_POS, OnSetEndPos )
END_MESSAGE_MAP()


LRESULT CMiniSliderPropertyGrid::OnSetPos( WPARAM wParam, LPARAM lParam )
{
	float *pPos = (float *)wParam;
	SetPos( *pPos );
	return S_OK;
}

LRESULT CMiniSliderPropertyGrid::OnSetEndPos( WPARAM wParam, LPARAM lParam )
{
	float *pPos = (float *)wParam;
	SetEndPos( *pPos );
	return S_OK;
}

void CMiniSliderPropertyGrid::SetPos( float fValue )
{
	m_fPos = fValue;
	if( m_fPos <= 0.f ) m_fPos = 0.f;
	if( m_fPos >= 100.f) m_fPos = 100.f;

	CWnd *pWnd = GetParent();
	if( pWnd ) pWnd->SendMessage( UM_MINISLIDER_UPDATE_POS, (WPARAM)&m_fPos );
	Invalidate();
}

void CMiniSliderPropertyGrid::SetEndPos( float fValue )
{
	m_fEndPos = fValue;
	if( m_fEndPos <= 0.f ) m_fEndPos = 0.f;
	if( m_fEndPos >= 100.f) m_fEndPos = 100.f;

	CWnd *pWnd = GetParent();
	if( pWnd ) pWnd->SendMessage( UM_MINISLIDER_UPDATE_END_POS, (WPARAM)&m_fEndPos );
	Invalidate();
}


void CMiniSliderPropertyGrid::OnPaint()
{
	CPaintDC dc(this);

	CRect rcRect;
	GetClientRect( &rcRect );

	dc.Draw3dRect( &rcRect, RGB(0,0,0), RGB(255,255,255) );

	rcRect.DeflateRect( 1, 1, 1, 1 );
	dc.FillSolidRect( &rcRect, RGB( 255, 255, 255 ) );

	rcRect.DeflateRect( 1, 1, 1, 1 );

	float fLength = (float)rcRect.Width();
	if( m_bRange ) {
		rcRect.right = rcRect.left + (int)( ( fLength / 100.f ) * m_fEndPos );
		rcRect.left += (int)( ( fLength / 100.f ) * m_fPos );
	}
	else {
		rcRect.right = rcRect.left + (int)( ( fLength / 100.f ) * m_fPos );
	}

	dc.FillSolidRect( &rcRect, RGB( 80, 80, 80 ) );
}

void CMiniSliderPropertyGrid::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonDown == true ) 
	{
		CRect rcRect;
		GetClientRect( &rcRect );
		ClientToScreen( &rcRect );
		ClipCursor( &rcRect );

		GetClientRect( &rcRect );

		int nPos = point.x;
		int nWidth = rcRect.Width() - 4;
		if( nPos <= 2 ) nPos = 0;
		if( nPos >= rcRect.Width() - 2 ) nPos = rcRect.Width() - 2;

		float fPos = 100.f / (float)nWidth * nPos;
		if( m_bRange ) {
			if( fPos < m_fPos ) {
				SetPos( fPos );
			}
			SetEndPos( fPos );
		}
		else {
			SetPos( fPos );
		}
		if( m_pNumClass || m_pDoubleClass || m_pVectorClass || m_pFloatClass ) {
			if( m_pNumClass ) m_pNumClass->OnChangeSlider();
			else if( m_pDoubleClass ) m_pDoubleClass->OnChangeSlider();
			else if( m_pVectorClass ) m_pVectorClass->OnChangeSlider();
			else if( m_pFloatClass ) m_pFloatClass->OnChangeSlider();
		}
	}

	CWnd::OnMouseMove(nFlags, point);

}

void CMiniSliderPropertyGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = true;

	CRect rcRect;
	GetClientRect( &rcRect );
	ClientToScreen( &rcRect );
	ClipCursor( &rcRect );

	GetClientRect( &rcRect );
	int nPos = point.x;
	int nWidth = rcRect.Width() - 4;
	if( nPos <= 2 ) nPos = 0;
	if( nPos >= rcRect.Width() - 2 ) nPos = rcRect.Width() - 2;

	float fPos = 100.f / (float)nWidth * nPos;
	SetPos( fPos );
	if( m_bRange ) {
		SetEndPos( fPos );
	}
	if( m_pNumClass || m_pDoubleClass || m_pVectorClass || m_pFloatClass ) {
		if( m_pNumClass ) m_pNumClass->OnChangeSlider();
		else if( m_pDoubleClass ) m_pDoubleClass->OnChangeSlider();
		else if( m_pVectorClass ) m_pVectorClass->OnChangeSlider();
		else if( m_pFloatClass ) m_pFloatClass->OnChangeSlider();
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CMiniSliderPropertyGrid::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = false;
	ClipCursor( NULL );

	CWnd::OnLButtonUp(nFlags, point);
}


IMPLEMENT_DYNAMIC(CXTCustomPropertyGridItemNumberSlider, CXTPPropertyGridItem)


CXTCustomPropertyGridItemNumberSlider::CXTCustomPropertyGridItemNumberSlider(const CString& strCaption, long nValue, long* pBindNumber, CSize Range , long nUnit )
	: CXTPPropertyGridItem(strCaption)
{
	m_pBindNumber = pBindNumber;

	m_Range = Range;
	if( m_Range.cx == -1 && m_Range.cy == -1 ) {
		m_Range.cx = -10000000;
		m_Range.cy = 10000000;
	}
	m_nUnit = nUnit;

	SetNumber(nValue);
	m_strDefaultValue = m_strValue;

}
CXTCustomPropertyGridItemNumberSlider::CXTCustomPropertyGridItemNumberSlider(UINT nID, long nValue, long* pBindNumber, CSize Range, long nUnit )
	: CXTPPropertyGridItem(nID)
{
	m_pBindNumber = pBindNumber;

	m_Range = Range;
	if( m_Range.cx == -1 && m_Range.cy == -1 ) {
		m_Range.cx = -10000000;
		m_Range.cy = 10000000;
	}

	m_nUnit = nUnit;

	SetNumber(nValue);
	m_strDefaultValue = m_strValue;
}

CXTCustomPropertyGridItemNumberSlider::~CXTCustomPropertyGridItemNumberSlider()
{
}

void CXTCustomPropertyGridItemNumberSlider::SetValue(CString strValue)
{
	SetNumber(_ttol(strValue));
}

void CXTCustomPropertyGridItemNumberSlider::SetNumber(long nValue)
{
	if( nValue < m_Range.cx ) nValue = m_Range.cx;
	if( nValue > m_Range.cy ) nValue = m_Range.cy;

	m_nValue = nValue;

	if (m_pBindNumber)
	{
		*m_pBindNumber = m_nValue;
	}

	CString strValue;
	strValue.Format(_T("%i"), nValue);
	CXTPPropertyGridItem::SetValue(strValue);


	if( m_Slider ) {
		float fPos = ( 100.f / (float)( m_Range.cy - m_Range.cx ) ) * (float)( m_nValue - m_Range.cx );
		m_Slider.SendMessage( UM_MINISLIDER_SET_POS, (WPARAM)&fPos );
	}
}

void CXTCustomPropertyGridItemNumberSlider::BindToNumber(long* pBindNumber)
{
	m_pBindNumber = pBindNumber;
	if (m_pBindNumber)
	{
		*m_pBindNumber = m_nValue;
	}
}

void CXTCustomPropertyGridItemNumberSlider::OnBeforeInsert()
{
	if (m_pBindNumber && *m_pBindNumber != m_nValue)
	{
		SetNumber(*m_pBindNumber);
	}
}

void CXTCustomPropertyGridItemNumberSlider::OnSelect()
{
	CRect rcRect = CXTPPropertyGridItem::GetValueRect();
	if( rcRect.Width() < 80 ) {}
	else {
		rcRect.left = rcRect.right - 60;
		if( !m_Slider )
			m_Slider.Create( NULL ,NULL, WS_VISIBLE | WS_CHILD | SS_NOTIFY, rcRect, m_pGrid, 0xffff, NULL );
		else m_Slider.MoveWindow( &rcRect );

		float fPos = ( 100.f / (float)( m_Range.cy - m_Range.cx ) ) * (float)( m_nValue - m_Range.cx );
		m_Slider.SetPos( fPos );
		m_Slider.SetClass( this );
		if( GetReadOnly() ) m_Slider.ShowWindow( SW_HIDE );
		else m_Slider.ShowWindow( SW_SHOW );
	}
	CXTPPropertyGridItem::OnSelect();
}

CRect CXTCustomPropertyGridItemNumberSlider::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	if( rcValue.Width() < 80 ) return rcValue;
	rcValue.right -= 60;
	return rcValue;
}

void CXTCustomPropertyGridItemNumberSlider::OnDeselect()
{
	if( m_Slider ) {
		m_Slider.ShowWindow( SW_HIDE );
		m_Slider.SetClass( (CXTCustomPropertyGridItemNumberSlider*)NULL );
		m_Slider.SetClass( (CXTCustomPropertyGridItemDoubleSlider*)NULL );
		m_Slider.SetClass( (CXTCustomPropertyGridItemFloatSlider*)NULL );
	}
	CXTPPropertyGridItem::OnDeselect();
}

void CXTCustomPropertyGridItemNumberSlider::OnChangeSlider()
{
	int nRange = m_Range.cy - m_Range.cx;
	int nValue = m_Range.cx + (int)( ( nRange / 100.f ) * m_Slider.GetPos() );
	SetNumber( nValue );
	m_pGrid->SendNotifyMessage(XTP_PGN_ITEMVALUE_CHANGED, (LPARAM)this);
}

IMPLEMENT_DYNAMIC(CXTCustomPropertyGridItemDoubleSlider, CXTPPropertyGridItem)


CXTCustomPropertyGridItemDoubleSlider::CXTCustomPropertyGridItemDoubleSlider(const CString& strCaption, double fValue, LPCTSTR strFormat, double* pBindDouble, double fRangeMin, double fRangeMax, double fUnit )
	: CXTPPropertyGridItem(strCaption)
{
	m_pBindDouble = pBindDouble;
	m_strFormat = strFormat;
	m_bUseSystemDecimalSymbol = m_strFormat.IsEmpty();

	m_fRangeMin = fRangeMin;
	m_fRangeMax = fRangeMax;
	m_fUnit = fUnit;

	SetDouble(fValue);
	m_strDefaultValue = m_strValue;


	EnableAutomation();
}

CXTCustomPropertyGridItemDoubleSlider::CXTCustomPropertyGridItemDoubleSlider(UINT nID, double fValue, LPCTSTR strFormat, double* pBindDouble, double fRangeMin, double fRangeMax, double fUnit )
	: CXTPPropertyGridItem(nID)
{
	m_pBindDouble = pBindDouble;
	m_strFormat = strFormat;
	m_bUseSystemDecimalSymbol = m_strFormat.IsEmpty();

	m_fRangeMin = fRangeMin;
	m_fRangeMax = fRangeMax;
	m_fUnit = fUnit;

	SetDouble(fValue);
	m_strDefaultValue = m_strValue;


	EnableAutomation();
}

CXTCustomPropertyGridItemDoubleSlider::~CXTCustomPropertyGridItemDoubleSlider()
{
}

void CXTCustomPropertyGridItemDoubleSlider::SetValue(CString strValue)
{
	SetDouble(StringToDouble(strValue));
}

double CXTCustomPropertyGridItemDoubleSlider::StringToDouble(LPCTSTR strValue)
{
	if (m_bUseSystemDecimalSymbol)
	{
		TRY
		{
			COleVariant oleString(strValue);
			oleString.ChangeType(VT_R8);
			return oleString.dblVal;
		}
		CATCH(COleException, e)
		{
		}
		END_CATCH
	}

#ifdef _UNICODE
	char astring[20];
	WideCharToMultiByte (CP_ACP, 0, strValue, -1, astring, 20, NULL, NULL);
	return (double)atof(astring);
#else
	return (double)atof(strValue);
#endif
}

CString CXTCustomPropertyGridItemDoubleSlider::DoubleToString(double dValue)
{
	if (m_bUseSystemDecimalSymbol)
	{
		TRY
		{
			COleVariant oleString(dValue);
			oleString.ChangeType(VT_BSTR);
			return CString(oleString.bstrVal);
		}
		CATCH(COleException, e)
		{
		}
		END_CATCH
	}

	CString strFormat(m_strFormat);
	if (strFormat.IsEmpty())
		strFormat = _T("%0.2f");

	CString strValue;
	strValue.Format(m_strFormat, dValue);
	return strValue;
}

void CXTCustomPropertyGridItemDoubleSlider::SetDouble(double fValue)
{
	if( fValue < m_fRangeMin ) fValue = m_fRangeMin;
	if( fValue > m_fRangeMax ) fValue = m_fRangeMax;

	m_fValue = fValue;

	if (m_pBindDouble)
	{
		*m_pBindDouble = m_fValue;
	}

	CXTPPropertyGridItem::SetValue(DoubleToString(fValue));
	if( m_Slider ) {
		float fPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_fValue - m_fRangeMin );
		m_Slider.SendMessage( UM_MINISLIDER_SET_POS, (WPARAM)&fPos );
	}
}

void CXTCustomPropertyGridItemDoubleSlider::BindToDouble(double* pBindNumber)
{
	m_pBindDouble = pBindNumber;
	if (m_pBindDouble)
	{
		*m_pBindDouble = m_fValue;
	}
}

void CXTCustomPropertyGridItemDoubleSlider::OnBeforeInsert()
{
	if (m_pBindDouble && *m_pBindDouble != m_fValue)
	{
		SetDouble(*m_pBindDouble);
	}
}

void CXTCustomPropertyGridItemDoubleSlider::OnSelect()
{
	CRect rcRect = CXTPPropertyGridItem::GetValueRect();
	if( rcRect.Width() < 80 ) {}
	else {
		rcRect.left = rcRect.right - 60;
		if( !m_Slider )
			m_Slider.Create( NULL ,NULL, WS_VISIBLE | WS_CHILD | SS_NOTIFY, rcRect, m_pGrid, 0xffff, NULL );
		else m_Slider.MoveWindow( &rcRect );

		float fPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_fValue - m_fRangeMin );
		m_Slider.SetPos( fPos );
		m_Slider.SetClass( this );
		if( GetReadOnly() ) m_Slider.ShowWindow( SW_HIDE );
		else m_Slider.ShowWindow( SW_SHOW );
	}
	CXTPPropertyGridItem::OnSelect();
}

CRect CXTCustomPropertyGridItemDoubleSlider::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	if( rcValue.Width() < 80 ) return rcValue;
	rcValue.right -= 60;
	return rcValue;
}

void CXTCustomPropertyGridItemDoubleSlider::OnDeselect()
{
	if( m_Slider ) {
		m_Slider.ShowWindow( SW_HIDE );
		m_Slider.SetClass( (CXTCustomPropertyGridItemNumberSlider*)NULL );
		m_Slider.SetClass( (CXTCustomPropertyGridItemDoubleSlider*)NULL );
		m_Slider.SetClass( (CXTCustomPropertyGridItemFloatSlider*)NULL );
	}
	CXTPPropertyGridItem::OnDeselect();
}

void CXTCustomPropertyGridItemDoubleSlider::OnChangeSlider()
{
	double fRange = m_fRangeMax - m_fRangeMin;
	double fValue = m_fRangeMin + ( ( fRange / 100.f ) * m_Slider.GetPos() );
	SetDouble( fValue );
	m_pGrid->SendNotifyMessage(XTP_PGN_ITEMVALUE_CHANGED, (LPARAM)this);
}


IMPLEMENT_DYNAMIC(CXTCustomPropertyGridItemFloatSlider, CXTPPropertyGridItem)


CXTCustomPropertyGridItemFloatSlider::CXTCustomPropertyGridItemFloatSlider(const CString& strCaption, float fValue, LPCTSTR strFormat, float* pBindFloat, float fRangeMin, float fRangeMax, float fUnit )
: CXTPPropertyGridItem(strCaption)
{
	m_pBindFloat = pBindFloat;
	m_strFormat = strFormat;
	m_bUseSystemDecimalSymbol = m_strFormat.IsEmpty();

	m_fRangeMin = fRangeMin;
	m_fRangeMax = fRangeMax;
	m_fUnit = fUnit;

	SetFloat(fValue);
	m_strDefaultValue = m_strValue;


	EnableAutomation();
}

CXTCustomPropertyGridItemFloatSlider::CXTCustomPropertyGridItemFloatSlider(UINT nID, float fValue, LPCTSTR strFormat, float* pBindFloat, float fRangeMin, float fRangeMax, float fUnit )
: CXTPPropertyGridItem(nID)
{
	m_pBindFloat = pBindFloat;
	m_strFormat = strFormat;
	m_bUseSystemDecimalSymbol = m_strFormat.IsEmpty();

	m_fRangeMin = fRangeMin;
	m_fRangeMax = fRangeMax;
	m_fUnit = fUnit;

	SetFloat(fValue);
	m_strDefaultValue = m_strValue;


	EnableAutomation();
}

CXTCustomPropertyGridItemFloatSlider::~CXTCustomPropertyGridItemFloatSlider()
{
}

/////////////////////////////////////////////////////////////////////////////
//

void CXTCustomPropertyGridItemFloatSlider::SetValue(CString strValue)
{
	SetFloat(StringToFloat(strValue));
}

float CXTCustomPropertyGridItemFloatSlider::StringToFloat(LPCTSTR strValue)
{
	if (m_bUseSystemDecimalSymbol)
	{
		TRY
		{
			COleVariant oleString(strValue);
			oleString.ChangeType(VT_R8);
			return oleString.fltVal;
		}
		CATCH(COleException, e)
		{
		}
		END_CATCH
	}

#ifdef _UNICODE
	char astring[20];
	WideCharToMultiByte (CP_ACP, 0, strValue, -1, astring, 20, NULL, NULL);
	return (float)atof(astring);
#else
	return (float)atof(strValue);
#endif
}

CString CXTCustomPropertyGridItemFloatSlider::FloatToString(float dValue)
{
	if (m_bUseSystemDecimalSymbol)
	{
		TRY
		{
			COleVariant oleString(dValue);
			oleString.ChangeType(VT_BSTR);
			return CString(oleString.bstrVal);
		}
		CATCH(COleException, e)
		{
		}
		END_CATCH
	}

	CString strFormat(m_strFormat);
	if (strFormat.IsEmpty())
		strFormat = _T("%0.2f");

	CString strValue;
	strValue.Format(m_strFormat, dValue);
	return strValue;
}

void CXTCustomPropertyGridItemFloatSlider::SetFloat(float fValue)
{
	if( fValue < m_fRangeMin ) fValue = m_fRangeMin;
	if( fValue > m_fRangeMax ) fValue = m_fRangeMax;

	m_fValue = fValue;

	if (m_pBindFloat)
	{
		*m_pBindFloat = m_fValue;
	}

	CXTPPropertyGridItem::SetValue(FloatToString(fValue));
	if( m_Slider ) {
		float fPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_fValue - m_fRangeMin );
		m_Slider.SendMessage( UM_MINISLIDER_SET_POS, (WPARAM)&fPos );
	}
}

void CXTCustomPropertyGridItemFloatSlider::BindToFloat(float* pBindNumber)
{
	m_pBindFloat = pBindNumber;
	if (m_pBindFloat)
	{
		*m_pBindFloat = m_fValue;
	}
}

void CXTCustomPropertyGridItemFloatSlider::OnBeforeInsert()
{
	if (m_pBindFloat && *m_pBindFloat != m_fValue)
	{
		SetFloat(*m_pBindFloat);
	}
}

void CXTCustomPropertyGridItemFloatSlider::OnSelect()
{
	CRect rcRect = CXTPPropertyGridItem::GetValueRect();
	if( rcRect.Width() < 80 ) {}
	else {
		rcRect.left = rcRect.right - 60;
		if( !m_Slider )
			m_Slider.Create( NULL ,NULL, WS_VISIBLE | WS_CHILD | SS_NOTIFY, rcRect, m_pGrid, 0xffff, NULL );
		else m_Slider.MoveWindow( &rcRect );

		float fPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_fValue - m_fRangeMin );
		m_Slider.SetPos( fPos );
		m_Slider.SetClass( this );
		if( GetReadOnly() ) m_Slider.ShowWindow( SW_HIDE );
		else m_Slider.ShowWindow( SW_SHOW );
	}
	CXTPPropertyGridItem::OnSelect();
}

CRect CXTCustomPropertyGridItemFloatSlider::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	if( rcValue.Width() < 80 ) return rcValue;
	rcValue.right -= 60;
	return rcValue;
}

void CXTCustomPropertyGridItemFloatSlider::OnDeselect()
{
	if( m_Slider ) {
		m_Slider.ShowWindow( SW_HIDE );
		m_Slider.SetClass( (CXTCustomPropertyGridItemNumberSlider*)NULL );
		m_Slider.SetClass( (CXTCustomPropertyGridItemFloatSlider*)NULL );
		m_Slider.SetClass( (CXTCustomPropertyGridItemFloatSlider*)NULL );
	}
	CXTPPropertyGridItem::OnDeselect();
}

void CXTCustomPropertyGridItemFloatSlider::OnChangeSlider()
{
	float fRange = m_fRangeMax - m_fRangeMin;
	float fValue = m_fRangeMin + ( ( fRange / 100.f ) * m_Slider.GetPos() );
	SetFloat( fValue );
	m_pGrid->SendNotifyMessage(XTP_PGN_ITEMVALUE_CHANGED, (LPARAM)this);
}




///////////////////////test
class CXTCustomPropertyGridItemVector2DRange::CXTCustomPropertyGridItemVectorCase : public CXTPPropertyGridItem
{
public:
	CXTCustomPropertyGridItemVectorCase(CString strCaption, int nIndex)
		: CXTPPropertyGridItem(strCaption) { m_nIndex = nIndex; }

	virtual void OnValueChanged(CString strValue)
	{
		switch( m_nIndex ) {
case 0:	((CXTCustomPropertyGridItemVector2DRange*)m_pParent)->SetX(strValue); break;
case 1:	((CXTCustomPropertyGridItemVector2DRange*)m_pParent)->SetY(strValue); break;
		}
	}
protected:
	int m_nIndex;
};

CXTCustomPropertyGridItemVector2DRange::CXTCustomPropertyGridItemVector2DRange(CString strCaption, D3DXVECTOR2 Value, D3DXVECTOR2 *pBindVector, float fRangeMin, float fRangeMax, float fUnit )
: CXTPPropertyGridItem(strCaption)
{
	m_bAutoNormalize = FALSE;
	m_Value = Value;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_Value );

	m_fRangeMin = fRangeMin;
	m_fRangeMax = fRangeMax;
	m_fUnit = fUnit;
	m_Slider.SetRangeMode( true );

	if( fabs( (float)( (int)fUnit - fUnit ) ) == 0.f )
		m_bInteger = true;
	else m_bInteger = false;

}
CXTCustomPropertyGridItemVector2DRange::CXTCustomPropertyGridItemVector2DRange(UINT nID, D3DXVECTOR2 Value, D3DXVECTOR2 *pBindVector, float fRangeMin, float fRangeMax, float fUnit )
: CXTPPropertyGridItem(nID)
{
	m_bAutoNormalize = FALSE;
	m_Value = Value;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_Value );

	m_fRangeMin = fRangeMin;
	m_fRangeMax = fRangeMax;
	m_fUnit = fUnit;

	if( fabs( (float)( (int)fUnit - fUnit ) ) == 0.f )
		m_bInteger = true;
	else m_bInteger = false;
}

CXTCustomPropertyGridItemVector2DRange::~CXTCustomPropertyGridItemVector2DRange(void)
{
}


void CXTCustomPropertyGridItemVector2DRange::SetValue(CString strValue)
{	
	SetVector( StringToVector(strValue) );
}

void CXTCustomPropertyGridItemVector2DRange::SetVector(D3DXVECTOR2 Value)
{
	if( Value.x < m_fRangeMin ) Value.x = m_fRangeMin;
	if( Value.x > m_fRangeMax ) Value.x = m_fRangeMax;
	if( Value.y < m_fRangeMin ) Value.y = m_fRangeMin;
	if( Value.y > m_fRangeMax ) Value.y = m_fRangeMax;

	m_Value = Value;

	if(m_pBindVector)
	{
		*m_pBindVector = m_Value;
	}

	CXTPPropertyGridItem::SetValue(VectorToString(m_Value));
	UpdateChilds();

	if( m_Slider ) {
		float fPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_Value.x - m_fRangeMin );
		m_Slider.SendMessage( UM_MINISLIDER_SET_POS, (WPARAM)&fPos );

		fPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_Value.y - m_fRangeMin );
		m_Slider.SendMessage( UM_MINISLIDER_SET_END_POS, (WPARAM)&fPos );
	}
}

void CXTCustomPropertyGridItemVector2DRange::BindToVector(D3DXVECTOR2 *pBindVector)
{
	m_pBindVector = pBindVector;
	if (m_pBindVector)
	{
		*m_pBindVector = m_Value;
	}
}

void CXTCustomPropertyGridItemVector2DRange::UpdateChilds()
{
	CString szX, szY, szZ;
	if( m_bInteger ) {
		szX.Format( "%d", (int)m_Value.x );
		szY.Format( "%d", (int)m_Value.y );
	}
	else {
		szX.Format( "%.2f", m_Value.x );
		szY.Format( "%.2f", m_Value.y );
	}

	m_itemX->SetValue( szX );
	m_itemY->SetValue( szY );
	m_itemX->SetReadOnly( m_bReadOnly );
	m_itemY->SetReadOnly( m_bReadOnly );
}

CString CXTCustomPropertyGridItemVector2DRange::VectorToString(D3DXVECTOR2 Value)
{
	CString str;
	if( m_bInteger ) {
		str.Format(_T("%d; %d;"), (int)Value.x, (int)Value.y);
	}
	else {
		str.Format(_T("%.2f; %.2f;"), Value.x, Value.y);
	}
	return str;
}

D3DXVECTOR2 CXTCustomPropertyGridItemVector2DRange::StringToVector(CString strValue)
{
	CString szX, szY;
	AfxExtractSubString(szX, strValue, 0, ';');
	AfxExtractSubString(szY, strValue, 1, ';');

	if( m_bAutoNormalize == TRUE ) {
		D3DXVECTOR2 vVec = D3DXVECTOR2( (float)atof(szX), (float)atof(szY) );
		float fLength = sqrtf( ( vVec.x * vVec.x ) + ( vVec.y * vVec.y ) );
		vVec /= fLength;
		return vVec;
	}
	else return D3DXVECTOR2( (float)atof(szX), (float)atof(szY) );
}


void CXTCustomPropertyGridItemVector2DRange::OnAddChildItem()
{
	m_itemX = (CXTCustomPropertyGridItemVectorCase*)AddChildItem(new CXTCustomPropertyGridItemVectorCase(_T("Min"), 0));
	m_itemY = (CXTCustomPropertyGridItemVectorCase*)AddChildItem(new CXTCustomPropertyGridItemVectorCase(_T("Max"), 1));
	UpdateChilds();
}

void CXTCustomPropertyGridItemVector2DRange::SetX( CString szStr )
{
	OnValueChanged( VectorToString( D3DXVECTOR2( (float)atof(szStr), m_Value.y ) ) );

	/*
	if( m_Slider ) {
		float fPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_Value.x - m_fRangeMin );
		m_Slider.SendMessage( UM_MINISLIDER_SET_POS, (WPARAM)&m_Value.x );
	}
	*/

}

void CXTCustomPropertyGridItemVector2DRange::SetY( CString szStr )
{
	OnValueChanged( VectorToString( D3DXVECTOR2( m_Value.x, (float)atof(szStr) ) ) );

	/*
	if( m_Slider ) {
		float fPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_Value.y - m_fRangeMin );
		m_Slider.SendMessage( UM_MINISLIDER_SET_END_POS, (WPARAM)&m_Value.y );
	}
	*/
}



////////////////////////////////////////////////////////////////////////////////////////////////////


void CXTCustomPropertyGridItemVector2DRange::OnSelect()
{
	CRect rcRect = CXTPPropertyGridItem::GetValueRect();
	if( rcRect.Width() < 80 ) {}
	else {
		rcRect.left = rcRect.right - 60;
		if( !m_Slider )
			m_Slider.Create( NULL ,NULL, WS_VISIBLE | WS_CHILD | SS_NOTIFY, rcRect, m_pGrid, 0xffff, NULL );
		else m_Slider.MoveWindow( &rcRect );

		float fPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_Value.x - m_fRangeMin );
		float fEndPos = ( 100.f / (float)( m_fRangeMax - m_fRangeMin ) ) * (float)( m_Value.y - m_fRangeMin );
		m_Slider.SetPos( fPos );
		m_Slider.SetEndPos( fEndPos );
		m_Slider.SetClass( this );
		if( GetReadOnly() ) m_Slider.ShowWindow( SW_HIDE );
		else m_Slider.ShowWindow( SW_SHOW );
	}
	CXTPPropertyGridItem::OnSelect();
}

void CXTCustomPropertyGridItemVector2DRange::OnDeselect()
{
	if( m_Slider ) {
		m_Slider.ShowWindow( SW_HIDE );
		m_Slider.SetClass( (CXTCustomPropertyGridItemNumberSlider*)NULL );
		m_Slider.SetClass( (CXTCustomPropertyGridItemDoubleSlider*)NULL );
	}
	CXTPPropertyGridItem::OnDeselect();
}

CRect CXTCustomPropertyGridItemVector2DRange::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	if( rcValue.Width() < 80 ) return rcValue;
	rcValue.right -= 60;
	return rcValue;
}

void CXTCustomPropertyGridItemVector2DRange::OnChangeSlider()
{
	double fRange = m_fRangeMax - m_fRangeMin;
	double fValue = m_fRangeMin + ( ( fRange / 100.f ) * m_Slider.GetPos() );
	double fMaxValue = m_fRangeMin + ( ( fRange / 100.f ) * m_Slider.GetEndPos() );
	SetVector( D3DXVECTOR2( (float)fValue, (float)fMaxValue ) );
	m_pGrid->SendNotifyMessage(XTP_PGN_ITEMVALUE_CHANGED, (LPARAM)this);
}


IMPLEMENT_DYNAMIC(CXTPPropertyGridItemFloat, CXTPPropertyGridItem)


CXTPPropertyGridItemFloat::CXTPPropertyGridItemFloat(const CString& strCaption, float fValue, LPCTSTR strFormat, float* pBindFloat)
: CXTPPropertyGridItem(strCaption)
{
	m_pBindFloat = pBindFloat;
	m_strFormat = strFormat;
	m_bUseSystemDecimalSymbol = m_strFormat.IsEmpty();

	SetFloat(fValue);
	m_strDefaultValue = m_strValue;

	EnableAutomation();
}

CXTPPropertyGridItemFloat::CXTPPropertyGridItemFloat(UINT nID, float fValue, LPCTSTR strFormat, float* pBindFloat)
: CXTPPropertyGridItem(nID)
{
	m_pBindFloat = pBindFloat;
	m_strFormat = strFormat;
	m_bUseSystemDecimalSymbol = m_strFormat.IsEmpty();

	SetFloat(fValue);
	m_strDefaultValue = m_strValue;

	EnableAutomation();
}

CXTPPropertyGridItemFloat::~CXTPPropertyGridItemFloat()
{
}

/////////////////////////////////////////////////////////////////////////////
//

void CXTPPropertyGridItemFloat::SetValue(CString strValue)
{
	SetFloat(StringToFloat(strValue));
}

float CXTPPropertyGridItemFloat::StringToFloat(LPCTSTR strValue)
{
	if (m_bUseSystemDecimalSymbol)
	{
		TRY
		{
			COleVariant oleString(strValue);
			oleString.ChangeType(VT_R8);
			return oleString.fltVal;
		}
		CATCH(COleException, e)
		{
		}
		END_CATCH
	}

#ifdef _UNICODE
	char astring[20];
	WideCharToMultiByte (CP_ACP, 0, strValue, -1, astring, 20, NULL, NULL);
	return (float)atof(astring);
#else
	return (float)atof(strValue);
#endif
}

CString CXTPPropertyGridItemFloat::FloatToString(float dValue)
{
	if (m_bUseSystemDecimalSymbol)
	{
		TRY
		{
			COleVariant oleString(dValue);
			oleString.ChangeType(VT_BSTR);
			return CString(oleString.bstrVal);
		}
		CATCH(COleException, e)
		{
		}
		END_CATCH
	}

	CString strFormat(m_strFormat);
	if (strFormat.IsEmpty())
		strFormat = _T("%0.2f");

	CString strValue;
	strValue.Format(m_strFormat, dValue);
	return strValue;
}

void CXTPPropertyGridItemFloat::SetFloat(float fValue)
{
	m_fValue = fValue;

	if (m_pBindFloat)
	{
		*m_pBindFloat = m_fValue;
	}

	CXTPPropertyGridItem::SetValue(FloatToString(fValue));
}

void CXTPPropertyGridItemFloat::BindToFloat(float* pBindNumber)
{
	m_pBindFloat = pBindNumber;
	if (m_pBindFloat)
	{
		*m_pBindFloat = m_fValue;
	}
}

void CXTPPropertyGridItemFloat::OnBeforeInsert()
{
	if (m_pBindFloat && *m_pBindFloat != m_fValue)
	{
		SetFloat(*m_pBindFloat);
	}
}


void CXTPPropertyGridItemFloat::OnInplaceButtonDown()
{
	m_pGrid->GetParent()->GetParent()->SendMessage( UM_PROPERTYGRID_ONCUSTOM_DIALOG, (WPARAM)0, (LPARAM)this );
	return;
}