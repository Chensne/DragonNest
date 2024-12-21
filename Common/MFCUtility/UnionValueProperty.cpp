#include "StdAfx.h"
#include "UnionValueProperty.h"

CUnionValueProperty::CUnionValueProperty( Type Type )
{
	Create( Type );
	m_bDefaultEnable = TRUE;
	m_bCustomDialog = false;
}

CUnionValueProperty::~CUnionValueProperty()
{
	Destroy();
}

void CUnionValueProperty::Create( Type Type )
{
	m_Type = Type;
	m_szDescription = NULL;
	m_pPtr = NULL;
	m_szCategory = NULL;
	m_pXTPropItem = NULL;
	m_bAllocSubDesc = false;

	switch( Type ) {
		case Vector2:
			m_pVec2Value = new EtVector2( 0.f, 0.f );
			break;
		case Vector3:
			m_pVec3Value = new EtVector3( 0.f, 0.f, 0.f );
			break;
		case Vector4:
		case Vector4Color:
			m_pVec4Value = new EtVector4( 0.f, 0.f, 0.f, 0.f );
			break;

	}
	m_pSubDescription = NULL;
}

void CUnionValueProperty::Destroy()
{
	SAFE_DELETEA( m_szCategory );
	SAFE_DELETEA( m_szDescription );
	if( m_bAllocSubDesc ) SAFE_DELETEA( m_pSubDescription );

	switch( m_Type ) 
	{
		case String:
		case String_FileOpen: SAFE_DELETEA( m_szValue ); break;
		case Vector2: SAFE_DELETE( m_pVec2Value ); break;
		case Vector3: SAFE_DELETE( m_pVec3Value ); break;
		case Vector4:
		case Vector4Color: SAFE_DELETE( m_pVec4Value ); break;

	}
}

void CUnionValueProperty::SetDescription( const char *szDescription )
{
	SAFE_DELETEA( m_szDescription );
	m_szDescription = _CopyString( szDescription );
}

void CUnionValueProperty::SetSubDescription( char *szDescription, bool bAlloc )
{
	if( m_bAllocSubDesc == true ) SAFE_DELETEA( m_pSubDescription );

	m_bAllocSubDesc = bAlloc;
	if( bAlloc == true ) {
		int nSize = (int)strlen(szDescription) + 1;
		m_pSubDescription = new char[nSize];
		sprintf_s( m_pSubDescription, nSize, szDescription );
	}
	else m_pSubDescription = szDescription;
}

void CUnionValueProperty::SetCategory( const char *szStr )
{
	SAFE_DELETEA( m_szCategory );
	m_szCategory = _CopyString( szStr );
}

void CUnionValueProperty::SetVariable( char *szValue )
{
	SAFE_DELETEA( m_szValue );
	m_szValue = _CopyString( szValue );
}

void CUnionValueProperty::SetVariable( int nValue, int nMin, int nMax )
{
	m_nRangeValue[0] = nValue;
	m_nRangeValue[1] = nMin;
	m_nRangeValue[2] = nMax;
}

void CUnionValueProperty::SetVariable( float fValue, float fMin, float fMax )
{
	m_fRangeValue[0] = fValue;
	m_fRangeValue[1] = fMin;
	m_fRangeValue[2] = fMax;
}

CString *CUnionValueProperty::GetBindStr()
{
	switch( m_Type ) 
	{
		case Integer_Combo:
			{
				const char *szString = _GetSubStrByCount( GetVariableInt() + 1, (char*)GetSubDescription(), '|' );
				if( szString == NULL ) m_szBindStr.Format( "%d", GetVariableInt() );
				else m_szBindStr = szString;
			}
			break;
		case Color:
		case Integer:	m_szBindStr.Format( "%d", GetVariableInt() );	break;
		case Float:	m_szBindStr.Format( "%f", GetVariableFloat() );	break;
		case Char:	m_szBindStr.Format( "%c", GetVariableChar() );	break;
		case Boolean:	m_szBindStr.Format( "%s", GetVariableBool() == true ? "True" : "False" );	break;
		case String:	
		case String_FileOpen:
			m_szBindStr.Format( "%s", GetVariableString() );	break;
			break;
		case Vector2:
			m_szBindStr.Format( "%f; %f", m_pVec2Value->x, m_pVec2Value->y );
			break;
		case Vector3:
			m_szBindStr.Format( "%f; %f; %f", m_pVec3Value->x, m_pVec3Value->y, m_pVec3Value->z );
			break;
		case Vector4:
		case Vector4Color:
			m_szBindStr.Format( "%f; %f; %f; %f", m_pVec4Value->x, m_pVec4Value->y, m_pVec4Value->z, m_pVec4Value->w );
			break;
	}

	return &m_szBindStr;
}

const CString& CUnionValueProperty::GetBindStr() const
{
	return m_szBindStr;
}

CUnionValueProperty &CUnionValueProperty::operator = ( CUnionValueProperty  &e )
{
	Destroy();
	Create( e.m_Type );
	SetDescription( e.m_szDescription );
	SetCategory( e.m_szCategory );
	SetSubDescription( e.m_pSubDescription, e.m_bAllocSubDesc );
	SetDefaultEnable( e.m_bDefaultEnable );
	m_pXTPropItem = e.m_pXTPropItem;

	switch( m_Type ) 
	{
		case Boolean: SetVariable( e.GetVariableBool() );	break;	
		case Char: SetVariable( e.GetVariableChar() );	break;
		case Integer: SetVariable( e.GetVariableInt() );	break;
		case Float: SetVariable( e.GetVariableFloat() );	break;
		case String: SetVariable( e.GetVariableString() );	break;
		case Pointer: SetVariable( e.GetVariablePtr() );	break;
		case Vector2: SetVariable( e.GetVariableVector2() );	break;
		case Vector3: SetVariable( e.GetVariableVector3() );	break;
		case Vector4: SetVariable( e.GetVariableVector4() );	break;
		case Color:	SetVariable( e.GetVariableInt() );	break;
		case Integer_Range:	SetVariable( e.GetVariableIntRange() );break;	
		case Integer_Combo:	SetVariable( e.GetVariableInt() );break;
		case String_FileOpen: SetVariable( e.GetVariableString() );	break;
		case Vector4Color: SetVariable( e.GetVariableVector4() );	break;
		case Float_Range: SetVariable( e.GetVariableFloatRange() );	break;

	}
	return *this; 
}

void CUnionValueProperty::ChangeType( Type Type )
{
	if( m_Type == Type ) return;
	Destroy();
	Create( Type );
}