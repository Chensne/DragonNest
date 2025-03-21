#include "StdAfx.h"
#include "PropertyGrid/XTPPropertyGridInplaceEdit.h"
#include "PropertyGrid/XTPPropertyGridInplaceButton.h"
#include "PropertyGrid/XTPPropertyGridInplaceList.h"
#include "PropertyGrid/XTPPropertyGridItem.h"
#include "XTCustomPropertyGridItemVector.h"


class CXTCustomPropertyGridItemVector3::CXTCustomPropertyGridItemVector3Case : public CXTPPropertyGridItem
{
public:
	CXTCustomPropertyGridItemVector3Case(CString strCaption, int nIndex) : CXTPPropertyGridItem(strCaption) 
		, m_nIndex(nIndex)
	{
	}

	virtual void OnValueChanged(CString strValue)
	{
		((CXTCustomPropertyGridItemVector3*)m_pParent)->SetVector( m_nIndex, strValue );
	}
protected:
	int m_nIndex;
};

CXTCustomPropertyGridItemVector3::CXTCustomPropertyGridItemVector3(CString strCaption, EtVector3 Value, EtVector3 *pBindVector)
: CXTPPropertyGridItem(strCaption)
{
	m_vVector = Value;
	m_bNormalized = false;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_vVector );
}
CXTCustomPropertyGridItemVector3::CXTCustomPropertyGridItemVector3(UINT nID, EtVector3 Value, EtVector3 *pBindVector)
: CXTPPropertyGridItem(nID)
{
	m_vVector = Value;
	m_bNormalized = false;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_vVector );
}

CXTCustomPropertyGridItemVector3::~CXTCustomPropertyGridItemVector3()
{
}


void CXTCustomPropertyGridItemVector3::SetValue(CString strValue)
{	
	SetVector( StringToVector(strValue) );
}

void CXTCustomPropertyGridItemVector3::SetVector(EtVector3 vVec)
{
	m_vVector = vVec;
	if(m_pBindVector) *m_pBindVector = m_vVector;

	CXTPPropertyGridItem::SetValue(VectorToString(m_vVector));

	UpdateChilds();
}

void CXTCustomPropertyGridItemVector3::BindToVector(EtVector3 *pBindVector)
{
	m_pBindVector = pBindVector;
	if (m_pBindVector) *m_pBindVector = m_vVector;
}

void CXTCustomPropertyGridItemVector3::UpdateChilds()
{
	CString szStr;
	for( int i=0; i<3; i++ ) 
	{
		szStr.Format( "%.2f", *((float*)&m_vVector.x + i) );
		m_ItemVector[i]->SetValue( szStr );
	}
}

CString CXTCustomPropertyGridItemVector3::VectorToString(EtVector3 Value)
{
	CString str;
	str.Format(_T("%.2f; %.2f; %.2f"), Value.x, Value.y, Value.z);
	return str;
}

EtVector3 CXTCustomPropertyGridItemVector3::StringToVector(CString strValue)
{
	CString szStr[3];
	for( int i=0; i<sizeof(szStr) / sizeof(CString); i++ )
		AfxExtractSubString(szStr[i], strValue, i, ';');

	EtVector3 vVec = EtVector3( (float)atof(szStr[0]), (float)atof(szStr[1]), (float)atof(szStr[2]) );

	if( m_bNormalized ) D3DXVec3Normalize( &vVec, &vVec );

	return vVec;
}


void CXTCustomPropertyGridItemVector3::OnAddChildItem()
{
	char cStr = 'X';
	CString szStr;
	for( int i=0; i<3; i++ )
	{
		szStr.Format( "%c", cStr + i );
		m_ItemVector[i] = (CXTCustomPropertyGridItemVector3Case*)AddChildItem(new CXTCustomPropertyGridItemVector3Case(szStr, i));
	}

	UpdateChilds();
}

void CXTCustomPropertyGridItemVector3::SetX( CString szStr )
{
	OnValueChanged( VectorToString( EtVector3( (float)atof(szStr), m_vVector.y, m_vVector.z ) ) );
}

void CXTCustomPropertyGridItemVector3::SetY( CString szStr )
{
	OnValueChanged( VectorToString( EtVector3( m_vVector.x, (float)atof(szStr), m_vVector.z ) ) );
}

void CXTCustomPropertyGridItemVector3::SetZ( CString szStr )
{
	OnValueChanged( VectorToString( EtVector3( m_vVector.x, m_vVector.y, (float)atof(szStr) ) ) );
}

void CXTCustomPropertyGridItemVector3::SetVector( int nIndex, CString szStr )
{
	switch( nIndex )
	{
		case 0: OnValueChanged( VectorToString( EtVector3( (float)atof(szStr), m_vVector.y, m_vVector.z ) ) ); break;
		case 1: OnValueChanged( VectorToString( EtVector3( m_vVector.x, (float)atof(szStr), m_vVector.z ) ) ); break;
		case 2: OnValueChanged( VectorToString( EtVector3( m_vVector.x, m_vVector.y, (float)atof(szStr) ) ) ); break;
	}
}

//////////////////////////////////////////////////////
class CXTCustomPropertyGridItemVector2::CXTCustomPropertyGridItemVector2Case : public CXTPPropertyGridItem
{
public:
	CXTCustomPropertyGridItemVector2Case(CString strCaption, int nIndex) : CXTPPropertyGridItem(strCaption) 
		, m_nIndex(nIndex)
	{
	}

	virtual void OnValueChanged(CString strValue)
	{
		((CXTCustomPropertyGridItemVector2*)m_pParent)->SetVector( m_nIndex, strValue );
	}
protected:
	int m_nIndex;
};

CXTCustomPropertyGridItemVector2::CXTCustomPropertyGridItemVector2(CString strCaption, EtVector2 Value, EtVector2 *pBindVector)
: CXTPPropertyGridItem(strCaption)
{
	m_vVector = Value;
	m_bNormalized = false;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_vVector );
}
CXTCustomPropertyGridItemVector2::CXTCustomPropertyGridItemVector2(UINT nID, EtVector2 Value, EtVector2 *pBindVector)
: CXTPPropertyGridItem(nID)
{
	m_vVector = Value;
	m_bNormalized = false;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_vVector );
}

CXTCustomPropertyGridItemVector2::~CXTCustomPropertyGridItemVector2()
{
}


void CXTCustomPropertyGridItemVector2::SetValue(CString strValue)
{	
	SetVector( StringToVector(strValue) );
}

void CXTCustomPropertyGridItemVector2::SetVector(EtVector2 vVec)
{
	m_vVector = vVec;
	if(m_pBindVector) *m_pBindVector = m_vVector;

	CXTPPropertyGridItem::SetValue(VectorToString(m_vVector));

	UpdateChilds();
}

void CXTCustomPropertyGridItemVector2::BindToVector(EtVector2 *pBindVector)
{
	m_pBindVector = pBindVector;
	if (m_pBindVector) *m_pBindVector = m_vVector;
}

void CXTCustomPropertyGridItemVector2::UpdateChilds()
{
	CString szStr;
	for( int i=0; i<2; i++ ) 
	{
		szStr.Format( "%.2f", *((float*)&m_vVector.x + i) );
		m_ItemVector[i]->SetValue( szStr );
	}
}

CString CXTCustomPropertyGridItemVector2::VectorToString(EtVector2 Value)
{
	CString str;
	str.Format(_T("%.2f; %.2f"), Value.x, Value.y);
	return str;
}

EtVector2 CXTCustomPropertyGridItemVector2::StringToVector(CString strValue)
{
	CString szStr[2];
	for( int i=0; i<sizeof(szStr) / sizeof(CString); i++ )
		AfxExtractSubString(szStr[i], strValue, i, ';');

	EtVector2 vVec = EtVector2( (float)atof(szStr[0]), (float)atof(szStr[1]) );

	if( m_bNormalized ) D3DXVec2Normalize( &vVec, &vVec );

	return vVec;
}


void CXTCustomPropertyGridItemVector2::OnAddChildItem()
{
	char cStr = 'X';
	CString szStr;
	for( int i=0; i<2; i++ )
	{
		szStr.Format( "%c", cStr + i );
		m_ItemVector[i] = (CXTCustomPropertyGridItemVector2Case*)AddChildItem(new CXTCustomPropertyGridItemVector2Case(szStr, i));
	}

	UpdateChilds();
}

void CXTCustomPropertyGridItemVector2::SetX( CString szStr )
{
	OnValueChanged( VectorToString( EtVector2( (float)atof(szStr), m_vVector.y ) ) );
}

void CXTCustomPropertyGridItemVector2::SetY( CString szStr )
{
	OnValueChanged( VectorToString( EtVector2( m_vVector.x, (float)atof(szStr) ) ) );
}

void CXTCustomPropertyGridItemVector2::SetVector( int nIndex, CString szStr )
{
	switch( nIndex )
	{
		case 0: OnValueChanged( VectorToString( EtVector2( (float)atof(szStr), m_vVector.y ) ) ); break;
		case 1: OnValueChanged( VectorToString( EtVector2( m_vVector.x, (float)atof(szStr) ) ) ); break;
	}
}


///////////////////////////////////////////////
class CXTCustomPropertyGridItemVector4::CXTCustomPropertyGridItemVector4Case : public CXTPPropertyGridItem
{
public:
	CXTCustomPropertyGridItemVector4Case(CString strCaption, int nIndex) : CXTPPropertyGridItem(strCaption) 
		, m_nIndex(nIndex)
	{
	}

	virtual void OnValueChanged(CString strValue)
	{
		((CXTCustomPropertyGridItemVector4*)m_pParent)->SetVector( m_nIndex, strValue );
	}
protected:
	int m_nIndex;
};

CXTCustomPropertyGridItemVector4::CXTCustomPropertyGridItemVector4(CString strCaption, EtVector4 Value, EtVector4 *pBindVector)
: CXTPPropertyGridItem(strCaption)
{
	m_vVector = Value;
	m_bNormalized = false;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_vVector );
}
CXTCustomPropertyGridItemVector4::CXTCustomPropertyGridItemVector4(UINT nID, EtVector4 Value, EtVector4 *pBindVector)
: CXTPPropertyGridItem(nID)
{
	m_vVector = Value;
	m_bNormalized = false;
	BindToVector( pBindVector );
	m_strValue = VectorToString( m_vVector );
}

CXTCustomPropertyGridItemVector4::~CXTCustomPropertyGridItemVector4()
{
}


void CXTCustomPropertyGridItemVector4::SetValue(CString strValue)
{	
	SetVector( StringToVector(strValue) );
}

void CXTCustomPropertyGridItemVector4::SetVector(EtVector4 vVec)
{
	m_vVector = vVec;
	if(m_pBindVector) *m_pBindVector = m_vVector;

	CXTPPropertyGridItem::SetValue(VectorToString(m_vVector));

	UpdateChilds();
}

void CXTCustomPropertyGridItemVector4::BindToVector(EtVector4 *pBindVector)
{
	m_pBindVector = pBindVector;
	if (m_pBindVector) *m_pBindVector = m_vVector;
}

void CXTCustomPropertyGridItemVector4::UpdateChilds()
{
	CString szStr;
	for( int i=0; i<4; i++ ) 
	{
		szStr.Format( "%.2f", *((float*)&m_vVector.x + i) );
		m_ItemVector[i]->SetValue( szStr );
	}
}

CString CXTCustomPropertyGridItemVector4::VectorToString(EtVector4 Value)
{
	CString str;
	str.Format(_T("%.2f; %.2f; %.2f; %.2f"), Value.x, Value.y, Value.z, Value.w);
	return str;
}

EtVector4 CXTCustomPropertyGridItemVector4::StringToVector(CString strValue)
{
	CString szStr[4];
	for( int i=0; i<sizeof(szStr) / sizeof(CString); i++ )
		AfxExtractSubString(szStr[i], strValue, i, ';');

	EtVector4 vVec = EtVector4( (float)atof(szStr[0]), (float)atof(szStr[1]), (float)atof(szStr[2]), (float)atof(szStr[3]) );

	if( m_bNormalized ) D3DXVec4Normalize( &vVec, &vVec );

	return vVec;
}


void CXTCustomPropertyGridItemVector4::OnAddChildItem()
{
	char cStr[] = { 'X', 'Y', 'Z', 'W' };
	CString szStr;
	for( int i=0; i<4; i++ )
	{
		szStr.Format( "%c", cStr[i] );
		m_ItemVector[i] = (CXTCustomPropertyGridItemVector4Case*)AddChildItem(new CXTCustomPropertyGridItemVector4Case(szStr, i));
	}

	UpdateChilds();
}

void CXTCustomPropertyGridItemVector4::SetX( CString szStr )
{
	OnValueChanged( VectorToString( EtVector4( (float)atof(szStr), m_vVector.y, m_vVector.z, m_vVector.w ) ) );
}

void CXTCustomPropertyGridItemVector4::SetY( CString szStr )
{
	OnValueChanged( VectorToString( EtVector4( m_vVector.x, (float)atof(szStr), m_vVector.z, m_vVector.w ) ) );
}

void CXTCustomPropertyGridItemVector4::SetZ( CString szStr )
{
	OnValueChanged( VectorToString( EtVector4( m_vVector.x, m_vVector.y, (float)atof(szStr), m_vVector.w ) ) );
}

void CXTCustomPropertyGridItemVector4::SetW( CString szStr )
{
	OnValueChanged( VectorToString( EtVector4( m_vVector.x, m_vVector.y, m_vVector.z, (float)atof(szStr) ) ) );
}

void CXTCustomPropertyGridItemVector4::SetVector( int nIndex, CString szStr )
{
	switch( nIndex )
	{
		case 0: OnValueChanged( VectorToString( EtVector4( (float)atof(szStr), m_vVector.y, m_vVector.z, m_vVector.w ) ) ); break;
		case 1: OnValueChanged( VectorToString( EtVector4( m_vVector.x, (float)atof(szStr), m_vVector.z, m_vVector.w ) ) ); break;
		case 2: OnValueChanged( VectorToString( EtVector4( m_vVector.x, m_vVector.y, (float)atof(szStr), m_vVector.w ) ) ); break;
		case 3: OnValueChanged( VectorToString( EtVector4( m_vVector.x, m_vVector.y, m_vVector.w, (float)atof(szStr) ) ) ); break;
	}
}
