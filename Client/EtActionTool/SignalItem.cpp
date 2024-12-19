#include "StdAfx.h"
#include "SignalItem.h"
#include "GlobalValue.h"
#include "UnionValueProperty.h"



CSignalItem::CSignalItem( int nUniqueIndex )
{
	m_nUniqueIndex = nUniqueIndex;
	m_nParamUniqueIndex = 0;
	m_bEnableCopyAndPaste = false;
}

CSignalItem::~CSignalItem()
{
	SAFE_DELETE_PVEC( m_pVecList );
}


bool CSignalItem::AddParameter( CUnionValueProperty *pVariable, bool bAllowUnique )
{
	m_pVecList.push_back( pVariable );
	if( bAllowUnique ) {
		if( m_nParamUniqueIndex >= 64 ) {
			m_pVecList.erase( m_pVecList.end() - 1 );
			return false;
		}
		m_nVecUniqueList.push_back( m_nParamUniqueIndex );
		m_nParamUniqueIndex++;
	}
	return true;
}

void CSignalItem::RemoveParameter( CString &szStr )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->GetDescription() == szStr ) {
			SAFE_DELETE( m_pVecList[i] );
			m_pVecList.erase( m_pVecList.begin() + i );
			m_nVecUniqueList.erase( m_nVecUniqueList.begin() + i );
		}
	}
}

bool CSignalItem::IsExistParameter( CString &szStr, CString &szOriginalName )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( strcmp( m_pVecList[i]->GetDescription(), szStr ) == NULL ) {
			if( !szOriginalName.IsEmpty() && strcmp( m_pVecList[i]->GetDescription(), szOriginalName ) == NULL ) continue;
			return true;
		}
	}
	return false;
}

CUnionValueProperty *CSignalItem::GetParameterFromName( CString &szName )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( strcmp( m_pVecList[i]->GetDescription(), szName ) == NULL ) return m_pVecList[i];
	}
	return NULL;
}

bool CSignalItem::Save( FILE *fp )
{
	WriteCString( &m_szName, fp );
	fwrite( &m_nUniqueIndex, sizeof(int), 1, fp );
	fwrite( &m_nParamUniqueIndex, sizeof(int), 1, fp );
	fwrite( &m_nInitialsColor, sizeof(int), 1, fp );
	fwrite( &m_nBackgroundColor, sizeof(int), 1, fp );
	fwrite( &m_bHasLength, sizeof(bool), 1, fp );

	DWORD dwCount = (int)m_pVecList.size();
	fwrite( &dwCount, sizeof(DWORD), 1, fp );
	for( DWORD i=0; i<dwCount; i++ ) {
		int nType = (int)m_pVecList[i]->GetType();
		CString szDesc = m_pVecList[i]->GetDescription();
		CString szSubDesc = m_pVecList[i]->GetSubDescription();

		fwrite( &m_nVecUniqueList[i], sizeof(int), 1, fp );
		fwrite( &nType, sizeof(int), 1, fp );
		WriteCString( &szDesc, fp );
		WriteCString( &szSubDesc, fp );
	}
	return true;
}

bool CSignalItem::Load( FILE *fp )
{
	ReadCString( &m_szName, fp );
	fread( &m_nUniqueIndex, sizeof(int), 1, fp );
	fread( &m_nParamUniqueIndex, sizeof(int), 1, fp );
	fread( &m_nInitialsColor, sizeof(int), 1, fp );
	fread( &m_nBackgroundColor, sizeof(int), 1, fp );
	fread( &m_bHasLength, sizeof(bool), 1, fp );

	DWORD dwCount;
	fread( &dwCount, sizeof(DWORD), 1, fp );
	for( DWORD i=0; i<dwCount; i++ ) {
		int nUniqueIndex;
		int nType;
		CString szDesc;
		CString szSubDesc;

		fread( &nUniqueIndex, sizeof(int), 1, fp );
		fread( &nType, sizeof(int), 1, fp );
		ReadCString( &szDesc, fp );
		ReadCString( &szSubDesc, fp );

		CUnionValueProperty *pVariable = new CUnionValueProperty((CUnionValueProperty::Type)nType );
		pVariable->SetDescription( szDesc );
		pVariable->SetSubDescription( szSubDesc.GetBuffer(), true );

		AddParameter( pVariable, false );
		m_nVecUniqueList.push_back( nUniqueIndex );
	}
	return true;
}

#define _BAN_COPY_CONSTRUCT

bool CSignalItem::ExportHeader( FILE *fp, int& nUsingBuffCount, int& nUsingBuffCount64 )
{
	CString szStructName;

	szStructName.Format( "struct %sStruct : public TBoostMemoryPool< %sStruct > {\n", m_szName, m_szName );
	fprintf( fp, szStructName );

	DWORD dwCurCount = 0;
	int nReserveSize = 256;
	int nReserveSize64 = 512;
	int nInitReserveSize = nReserveSize;
	int nInitReserveSize64 = nReserveSize64;
	int nCurReserveCount = 0;

	CString szVarStr;
	CString szString;
	int nReserveStrCount = 1;

	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_nVecUniqueList[i] != dwCurCount ) {
			nCurReserveCount++;
			i--;
			dwCurCount++;
			continue;
		}
		if( nCurReserveCount != 0 ) {
			szString.Format( "	char cReserved%d[%d];\n", nReserveStrCount, nCurReserveCount * 4 );
			nReserveSize -= nCurReserveCount * 4;
			nReserveSize64 -= nCurReserveCount * 4;
			nReserveStrCount++;
			nCurReserveCount = 0;

			fprintf( fp, szString );
		}
		switch( m_pVecList[i]->GetType() ) {
			case CUnionValueProperty::Integer:
			case CUnionValueProperty::Integer_Combo:
			case CUnionValueProperty::Color:
				szVarStr = "int ";
				nReserveSize64 -= 4;
				break;
			case CUnionValueProperty::Float:
				szVarStr = "float ";
				nReserveSize64 -= 4;
				break;
			case CUnionValueProperty::Vector2:
				szVarStr = "EtVector2 *";
				nReserveSize64 -= 8;
				break;
			case CUnionValueProperty::Vector3:
				szVarStr = "EtVector3 *";
				nReserveSize64 -= 8;
				break;
			case CUnionValueProperty::Vector4:
			case CUnionValueProperty::Vector4Color:
				szVarStr = "EtVector4 *";
				nReserveSize64 -= 8;
				break;
			case CUnionValueProperty::String:
			case CUnionValueProperty::String_FileOpen:
				szVarStr = "char *";
				nReserveSize64 -= 8;
				break;
			case CUnionValueProperty::Boolean:
				szVarStr = "BOOL ";
				nReserveSize64 -= 4;
				break;
			case CUnionValueProperty::Pointer:
				{
					CString szStr = m_pVecList[i]->GetSubDescription();
					const char *szValueData;
					szValueData = _GetSubStrByCount( 1, szStr.GetBuffer(), '|' );
					if( !szValueData ) szValueData = "void *";

					szVarStr = szValueData;
					szVarStr += " ";
					nReserveSize64 -= 8;
				}
				break;
		}

		nReserveSize -= 4;
		szString.Format( "	%s%s;\n", szVarStr, m_pVecList[i]->GetDescription() );
		fprintf( fp, szString );
	
		dwCurCount++;
	}

	nUsingBuffCount = (nInitReserveSize - nReserveSize) / 4;
	nUsingBuffCount64 = (nInitReserveSize64 - nReserveSize64) / 4;
	szString.Format( "	char cReserved%d[%d];\n", nReserveStrCount, nReserveSize );
	fprintf( fp, szString );

#ifdef _BAN_COPY_CONSTRUCT
	szString.Format( "\nprivate:\n\t%sStruct(const %sStruct&);\n\tconst %sStruct& operator=(const %sStruct&);\n\npublic:\n\t%sStruct() {}\n", 
		m_szName, m_szName, m_szName, m_szName, m_szName );
	fprintf( fp, szString );
#endif

	szStructName.Format( "};\n\n", m_szName );
	fprintf( fp, szStructName );

	return true;
}
