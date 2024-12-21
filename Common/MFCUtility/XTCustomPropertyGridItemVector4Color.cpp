#include "StdAfx.h"
#include "PropertyGrid/XTPPropertyGridInplaceEdit.h"
#include "PropertyGrid/XTPPropertyGridInplaceButton.h"
#include "PropertyGrid/XTPPropertyGridInplaceList.h"
#include "PropertyGrid/XTPPropertyGridItem.h"
#include "XTCustomPropertyGridItemVector4Color.h"
#include "Controls/XTColorDialog.h"


class CXTCustomPropertyGridItemVector4Color::CXTCustomPropertyGridItemVectorCase : public CXTPPropertyGridItem
{
public:
	CXTCustomPropertyGridItemVectorCase(CString strCaption, int nIndex) : CXTPPropertyGridItem(strCaption)
		, m_nIndex(nIndex)
	{
	}

	virtual void OnValueChanged(CString strValue)
	{
		((CXTCustomPropertyGridItemVector4Color*)m_pParent)->SetVector(m_nIndex, strValue);
	}
protected:
	int m_nIndex;
};

CXTCustomPropertyGridItemVector4Color::CXTCustomPropertyGridItemVector4Color(CString strCaption, EtVector4 Value, EtVector4 *pBindVector)
: CXTPPropertyGridItem(strCaption)
{
	m_nFlags = xtpGridItemHasExpandButton;
	m_vVector = Value;
	m_bNormalized = false;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_vVector );
}
CXTCustomPropertyGridItemVector4Color::CXTCustomPropertyGridItemVector4Color(UINT nID, EtVector4 Value, EtVector4 *pBindVector)
: CXTPPropertyGridItem(nID)
{
	m_nFlags = xtpGridItemHasExpandButton;
	m_vVector = Value;
	m_bNormalized = false;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_vVector );
}

CXTCustomPropertyGridItemVector4Color::~CXTCustomPropertyGridItemVector4Color(void)
{
}

BOOL CXTCustomPropertyGridItemVector4Color::OnDrawItemValue(CDC& dc, CRect rcValue)
{
	COLORREF clr = dc.GetTextColor();
	CRect rcSample(rcValue.left - 2, rcValue.top + 1, rcValue.left + 18, rcValue.bottom - 1);
	CXTPPenDC pen(dc, clr);

	DWORD dwColor = RGB( (DWORD)(m_vVector.x*255.f), (DWORD)(m_vVector.y*255.f), (DWORD)(m_vVector.z*255.f) );
	CXTPBrushDC brush(dc, dwColor );
	dc.Rectangle(rcSample);

	CRect rcText(rcValue);
	rcText.left += 25;

	dc.DrawText(m_strValue, rcText, DT_SINGLELINE | DT_VCENTER);

	return TRUE;
}

CRect CXTCustomPropertyGridItemVector4Color::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	rcValue.left += 25;
	return rcValue;
}


void CXTCustomPropertyGridItemVector4Color::SetValue(CString strValue)
{	
	SetVector( StringToVector(strValue) );
}

void CXTCustomPropertyGridItemVector4Color::SetVector(EtVector4 Value)
{
	m_vVector = Value;

	if(m_pBindVector) *m_pBindVector = m_vVector;

	CXTPPropertyGridItem::SetValue(VectorToString(m_vVector));
	UpdateChilds();
}

void CXTCustomPropertyGridItemVector4Color::BindToVector(EtVector4 *pBindVector)
{
	m_pBindVector = pBindVector;
	if (m_pBindVector) *m_pBindVector = m_vVector;
}

void CXTCustomPropertyGridItemVector4Color::UpdateChilds()
{
	CString szStr;
	for( int i=0; i<4; i++ ) 
	{
		szStr.Format( "%.2f", *((float*)&m_vVector.x + i) );
		m_ItemVector[i]->SetValue( szStr );
		m_ItemVector[i]->SetReadOnly( m_bReadOnly );
	}
}

CString CXTCustomPropertyGridItemVector4Color::VectorToString(EtVector4 Value)
{
	CString str;
	str.Format(_T("%.2f;%.2f;%.2f;%.2f"), Value.x, Value.y,Value.z,Value.w);
	return str;
}

EtVector4 CXTCustomPropertyGridItemVector4Color::StringToVector(CString strValue)
{
	CString szStr[4];
	for( int i=0; i<sizeof(szStr) / sizeof(CString); i++ )
		AfxExtractSubString(szStr[i], strValue, i, ';');

	EtVector4 vVec = EtVector4( (float)atof(szStr[0]), (float)atof(szStr[1]), (float)atof(szStr[2]), (float)atof(szStr[3]) );

	if( m_bNormalized ) D3DXVec4Normalize( &vVec, &vVec );

	return vVec;
}


void CXTCustomPropertyGridItemVector4Color::OnAddChildItem()
{
	char cStr[] = { 'X', 'Y', 'Z', 'W' };
	CString szStr;
	for( int i=0; i<4; i++ )
	{
		szStr.Format( "%c", cStr[i] );
		m_ItemVector[i] = (CXTCustomPropertyGridItemVectorCase*)AddChildItem(new CXTCustomPropertyGridItemVectorCase(szStr, i));
	}

	UpdateChilds();
}

void CXTCustomPropertyGridItemVector4Color::SetX( CString szStr )
{
	OnValueChanged( VectorToString( EtVector4( (float)atof(szStr), m_vVector.y, m_vVector.z, m_vVector.w ) ) );
}

void CXTCustomPropertyGridItemVector4Color::SetY( CString szStr )
{
	OnValueChanged( VectorToString( EtVector4( m_vVector.x, (float)atof(szStr), m_vVector.z, m_vVector.w ) ) );
}

void CXTCustomPropertyGridItemVector4Color::SetZ( CString szStr )
{
	OnValueChanged( VectorToString( EtVector4( m_vVector.x, m_vVector.y, (float)atof(szStr), m_vVector.w ) ) );
}

void CXTCustomPropertyGridItemVector4Color::SetW( CString szStr )
{
	OnValueChanged( VectorToString( EtVector4( m_vVector.x, m_vVector.y, m_vVector.z, (float)atof(szStr) ) ) );
}

void CXTCustomPropertyGridItemVector4Color::SetVector( int nIndex, CString szStr )
{
	switch( nIndex )
	{
		case 0: OnValueChanged( VectorToString( EtVector4( (float)atof(szStr), m_vVector.y, m_vVector.z, m_vVector.w ) ) ); break;
		case 1: OnValueChanged( VectorToString( EtVector4( m_vVector.x, (float)atof(szStr), m_vVector.z, m_vVector.w ) ) ); break;
		case 2: OnValueChanged( VectorToString( EtVector4( m_vVector.x, m_vVector.y, (float)atof(szStr), m_vVector.w ) ) ); break;
		case 3: OnValueChanged( VectorToString( EtVector4( m_vVector.x, m_vVector.y, m_vVector.z, (float)atof(szStr) ) ) ); break;
	}
}

void CXTCustomPropertyGridItemVector4Color::OnBeforeInsert()
{
	if (m_pBindVector && *m_pBindVector != m_vVector )
	{
		SetVector(*m_pBindVector);
	}
}


void CXTCustomPropertyGridItemVector4Color::OnInplaceButtonDown()
{
	if (!OnRequestEdit())
		return;

	if( m_vVector.x >= 1.f ) m_vVector.x = 1.f;
	else if( m_vVector.x < 0.f ) m_vVector.x = 0.f;

	if( m_vVector.y >= 1.f ) m_vVector.y = 1.f;
	else if( m_vVector.y < 0.f ) m_vVector.y = 0.f;

	if( m_vVector.z >= 1.f ) m_vVector.z = 1.f;
	else if( m_vVector.z < 0.f ) m_vVector.z = 0.f;

	DWORD dwColor = RGB( (DWORD)(m_vVector.x*255.f), (DWORD)(m_vVector.y*255.f), (DWORD)(m_vVector.z*255.f) );
#ifdef _XTP_INCLUDE_CONTROLS
	CXTColorDialog dlg(dwColor, dwColor, CPS_XT_SHOW3DSELECTION, m_pGrid);
#else
	CColorDialog dlg(dwColor, 0, m_pGrid);
#endif

	if (dlg.DoModal() == IDOK)
	{
		EtVector4 Value;
		Value.x = 1 / 255.f * GetRValue( dlg.GetColor() );
		Value.y = 1 / 255.f * GetGValue( dlg.GetColor() );
		Value.z = 1 / 255.f * GetBValue( dlg.GetColor() );
		Value.w = m_vVector.w;
		CString strValue = VectorToString(Value);

		if (OnAfterEdit(strValue))
		{
			OnValueChanged(strValue);
			if (m_pGrid && m_pGrid->GetSafeHwnd()) {
				m_pGrid->Invalidate(FALSE);
			}
		}
	}
	else
	{
		OnCancelEdit();
	}
}



