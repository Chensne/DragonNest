#include "StdAfx.h"
#include "PropertyGridCreator.h"
#include "XTCustomPropertyGridItemFile.h"
#include "XTCustomPropertyGridItemVector.h"
#include "XTCustomPropertyGridItemVector4Color.h"
#include "XTCustomPropertyGridItemNumberSlider.h"
#include "XTCustomPropertyGridItemColor.h"
#include "XTCustomPropertyGridItemNumber.h"
#include "XTCustomPropertyGridItem.h"
#include "XTCustomPropertyGridItemNumber.h"


CPropertyGridImp::CPropertyGridImp()
{
	m_pDefineList = NULL;
	m_bSelfAlloc = false;
	m_ppVecVariable = NULL;
	m_bUpdateItem = false;
}

CPropertyGridImp::CPropertyGridImp( PropertyGridBaseDefine *pDefineList )
{
	m_pDefineList = pDefineList;
	m_ppVecVariable = NULL;
	m_bSelfAlloc = true;
	m_bUpdateItem = false;
}

CPropertyGridImp::CPropertyGridImp( std::vector<CUnionValueProperty *> *pVecList )
{
	m_pDefineList = NULL;
	m_ppVecVariable = pVecList;
	m_bSelfAlloc = false;
	m_bUpdateItem = false;
}


CPropertyGridImp::~CPropertyGridImp()
{
	if( m_bSelfAlloc == true ) {
		SAFE_DELETE_PVEC( m_pVecVariable );
	}
	else {
		SAFE_DELETE_VEC( m_pVecVariable );
	}
}

CXTPPropertyGridItem *CPropertyGridImp::CreateItem( CUnionValueProperty *pVariable )
{
	CXTPPropertyGridItem *pSubItem = NULL;
	switch( pVariable->GetType() ) {
		case CUnionValueProperty::Integer:
			{
				std::vector<CString> szVecList;
				CString szStr;
				for( int k=0; ; k++ ) {
					const char *pPtr = _GetSubStrByCount( k, (char*)pVariable->GetSubDescription(), '|' );
					if( pPtr == NULL ) break;
					szStr.Format( "%s", pPtr );
					szVecList.push_back( szStr );
				}

				if( szVecList.size() == 4 )
					pSubItem = new CXTCustomPropertyGridItemNumberSlider( pVariable->GetDescription(), pVariable->m_nValue, (long*)&pVariable->m_nValue, CSize( atoi(szVecList[1]), atoi(szVecList[2]) ), atoi(szVecList[3]) );	
				else 
					pSubItem = new CXTCustomPropertyGridItemNumber( pVariable->GetDescription(), pVariable->m_nValue, (long*)&pVariable->m_nValue );
			}
			break;
		case CUnionValueProperty::Float:
			{
				std::vector<CString> szVecList;
				CString szStr;
				for( int k=0; ; k++ ) {
					const char *pPtr = _GetSubStrByCount( k, (char*)pVariable->GetSubDescription(), '|' );
					if( pPtr == NULL ) break;
					szStr.Format( "%s", pPtr );
					szVecList.push_back( szStr );
				}

				if( szVecList.size() == 4 )
					pSubItem = new CXTCustomPropertyGridItemFloatSlider( pVariable->GetDescription(), pVariable->m_fValue, CString("%0.3f"), (float*)&pVariable->m_fValue, (float)atof(szVecList[1]),(float)atof(szVecList[2]), (float)atof(szVecList[3]) );
				else
					pSubItem = new CXTPPropertyGridItemFloat( pVariable->GetDescription(), pVariable->m_fValue, CString("%0.3f"), (float*)&pVariable->m_fValue );
			}
			break;
		case CUnionValueProperty::Char:
		case CUnionValueProperty::String:
			pSubItem = new CXTCustomPropertyGridItem( pVariable->GetDescription(), *pVariable->GetBindStr(), pVariable->GetBindStr() );
			break;
		case CUnionValueProperty::Vector2:
			{
				std::vector<CString> szVecList;
				CString szStr;
				for( int k=0; ; k++ ) {
					const char *pPtr = _GetSubStrByCount( k, (char*)pVariable->GetSubDescription(), '|' );
					if( pPtr == NULL ) break;
					szStr.Format( "%s", pPtr );
					szVecList.push_back( szStr );
				}

				if( szVecList.size() == 4 )
					pSubItem = new CXTCustomPropertyGridItemVector2DRange( pVariable->GetDescription(), *pVariable->m_pVec2Value, pVariable->m_pVec2Value, (float)atof(szVecList[1]), (float)atof(szVecList[2]), (float)atof(szVecList[3]) );
				else pSubItem = new CXTCustomPropertyGridItemVector2( pVariable->GetDescription(), *pVariable->m_pVec2Value, pVariable->m_pVec2Value );
			}
			break;
		case CUnionValueProperty::Vector3:
			pSubItem = new CXTCustomPropertyGridItemVector3( pVariable->GetDescription(), *pVariable->m_pVec3Value, pVariable->m_pVec3Value );
			if( strstr( pVariable->GetSubDescription(), "!" ) != NULL ) {
				((CXTCustomPropertyGridItemVector3*)pSubItem)->SetNormalized( true );
			}
			break;
		case CUnionValueProperty::Vector4:
			pSubItem = new CXTCustomPropertyGridItemVector4( pVariable->GetDescription(), *pVariable->m_pVec4Value, pVariable->m_pVec4Value );
			break;
		case CUnionValueProperty::Vector4Color:
			pSubItem = new CXTCustomPropertyGridItemVector4Color( pVariable->GetDescription(), *pVariable->m_pVec4Value, pVariable->m_pVec4Value );
			break;
		case CUnionValueProperty::Boolean:
			pSubItem = new CXTPPropertyGridItemBool( pVariable->GetDescription(), (BOOL)pVariable->m_bValue, (BOOL*)&pVariable->m_bValue );
			break;
		case CUnionValueProperty::Color:
			pSubItem = new CXTCustomPropertyGridItemColor( pVariable->GetDescription(), (COLORREF)pVariable->m_nValue, (COLORREF*)&pVariable->m_nValue );
			break;
		case CUnionValueProperty::Integer_Combo:
			{
				const char *szString = _GetSubStrByCount( 1 + pVariable->m_nValue, (char*)pVariable->GetSubDescription() );
				pSubItem = new CXTPPropertyGridItem( pVariable->GetDescription(), szString, pVariable->GetBindStr() );
				CXTPPropertyGridItemConstraints* pList = pSubItem->GetConstraints();

				for( int k=0; ; k++ ) {
					const char *szString = _GetSubStrByCount( k, (char*)pVariable->GetSubDescription(), '|' );
					if( szString == NULL ) break;
					if( k == 0 ) continue;
					pList->AddConstraint( szString );
				}
				pList->SetCurrent( pVariable->m_nValue );

				pSubItem->SetFlags( xtpGridItemHasComboButton );
			}
			break;
		case CUnionValueProperty::String_FileOpen:
			{
				CString szDesc, szExt, szFileDesc;
				for( int k=0; ; k++ ) {
					const char *szString = _GetSubStrByCount( k, (char*)pVariable->GetSubDescription(), '|' );
					if( szString == NULL ) break;
					switch( k ) {
						case 0: szDesc = szString;	break;
						case 1: szFileDesc = szString; break;
						case 2: szExt = szString;	break;
					}
				}

				pSubItem = new CXTCustomPropertyGridItemFile( pVariable->GetDescription(), *pVariable->GetBindStr(), pVariable->GetBindStr() );
				((CXTCustomPropertyGridItemFile*)pSubItem)->SetExt( szExt, szFileDesc );
				((CXTCustomPropertyGridItemFile*)pSubItem)->SetInitDirectory( m_szInitDirectory );
			}
			break;
	}
	pVariable->SetXTPropItem( pSubItem );
	return pSubItem;
}

CUnionValueProperty *CPropertyGridImp::CreateVariable( PropertyGridBaseDefine *pDefine )
{
	CUnionValueProperty *pVariable = NULL;

	pVariable = new CUnionValueProperty( pDefine->Type );
	pVariable->SetDescription( pDefine->szStr );
	pVariable->SetSubDescription( pDefine->szDescription );
	pVariable->SetDefaultEnable( pDefine->bEnable );

	return pVariable;
}

void CPropertyGridImp::ResetPropertyGrid()
{
	m_pVecItem.clear();
	m_PropertyGrid.ResetContent();
	m_PropertyGrid.ShowHelp( FALSE );
	m_PropertyGrid.ShowHelp( TRUE );

	if( m_bSelfAlloc == true ) {
		SAFE_DELETE_PVEC( m_pVecVariable );
	}
	else {
		SAFE_DELETE_VEC( m_pVecVariable );
	}
}

bool CPropertyGridImp::RefreshPropertyGrid( PropertyGridBaseDefine *pDefineList, int nCount )
{
	ResetPropertyGrid();

	m_bSelfAlloc = true;
	if( pDefineList ) {
		m_pDefineList = pDefineList;
	}

	std::vector<CategoryStruct> vecCategory;

	if( nCount == -1 ) nCount = 65535;
	for( int i=0; i<nCount; i++ ) {
		if( m_pDefineList[i].szCategory == NULL ) break;

		// 일단 카테고리를 찾구..
		bool bExist = false;
		CXTPPropertyGridItem *pCategory = NULL;
		for( DWORD j=0; j<vecCategory.size(); j++ ) {
			if( strcmp( vecCategory[j].szStr.GetBuffer(), m_pDefineList[i].szCategory ) == NULL ) {
				pCategory = vecCategory[j].pItem;
				bExist = true;
				break;
			}
		}
		if( bExist == false ) {
			pCategory = m_PropertyGrid.AddCategory( m_pDefineList[i].szCategory );
			CategoryStruct Struct;
			Struct.szStr = m_pDefineList[i].szCategory;
			Struct.pItem = pCategory;
			vecCategory.push_back( Struct );
		}
		// 아템을 인서트 시켜준다.
		CUnionValueProperty *pVariable = CreateVariable( &m_pDefineList[i] );
		OnSetValue( pVariable, i );
		CXTPPropertyGridItem *pSubItem = CreateItem( pVariable );

		if( pSubItem ) {
			const char *pStr = _GetSubStrByCount( 0, (char*)pVariable->GetSubDescription(), '|' );
			pSubItem->SetReadOnly( !pVariable->IsDefaultEnable() );
			pSubItem->SetDescription( pStr );
			pCategory->AddChildItem( pSubItem );
			m_pVecItem.push_back( pSubItem );
			m_pVecVariable.push_back( pVariable );

			if( m_pDefineList[i].bCustomDialog ) {
				pSubItem->SetFlags( pSubItem->GetFlags() | xtpGridItemHasExpandButton );
			}
		}
		OnSetValue( pVariable, i );
	}
	for( DWORD i=0; i<vecCategory.size(); i++ ) {
		vecCategory[i].pItem->Expand();
	}

	return true;
}

bool CPropertyGridImp::RefreshPropertyGrid( std::vector<CUnionValueProperty *> *pVecList )
{
	ResetPropertyGrid();

	m_bSelfAlloc = false;
	if( pVecList ) {
		m_ppVecVariable = pVecList;
	}

	std::vector<CategoryStruct> vecCategory;

	for( DWORD i=0; i<m_ppVecVariable->size(); i++ ) {
		CUnionValueProperty *pVariable = (*m_ppVecVariable)[i];

		// 일단 카테고리를 찾구..
		bool bExist = false;
		CXTPPropertyGridItem *pCategory = NULL;
		for( DWORD j=0; j<vecCategory.size(); j++ ) {
			if( strcmp( vecCategory[j].szStr.GetBuffer(), pVariable->GetCategory() ) == NULL ) {
				pCategory = vecCategory[j].pItem;
				bExist = true;
				break;
			}
		}
		if( bExist == false ) {
			pCategory = m_PropertyGrid.AddCategory( pVariable->GetCategory() );
			CategoryStruct Struct;
			Struct.szStr = pVariable->GetCategory();
			Struct.pItem = pCategory;
			vecCategory.push_back( Struct );
		}
		// 아템을 인서트 시켜준다.
		OnSetValue( pVariable, i );
		CXTPPropertyGridItem *pSubItem = CreateItem( pVariable );

		if( pSubItem ) {
			pSubItem->SetReadOnly( !pVariable->IsDefaultEnable() );
			if( pVariable->GetSubDescription() ) {
				const char *pStr = _GetSubStrByCount( 0, (char*)pVariable->GetSubDescription(), '|' );
				pSubItem->SetDescription( pStr );
			}
			pCategory->AddChildItem( pSubItem );
			m_pVecItem.push_back( pSubItem );
			m_pVecVariable.push_back( pVariable );

			if( pVariable->IsCustomDialog() ) {
				pSubItem->SetFlags( pSubItem->GetFlags() | xtpGridItemHasExpandButton );
			}
		}
	}
	for( DWORD i=0; i<vecCategory.size(); i++ ) {
		vecCategory[i].pItem->Expand();
	}

	return true;
}

LRESULT CPropertyGridImp::ProcessNotifyGrid( WPARAM wParam, LPARAM lParam )
{
	int nGridAction = (int)wParam;
	CXTPPropertyGridItem *pItem = (CXTPPropertyGridItem *)lParam;
	switch( nGridAction ) {
		case XTP_PGN_ITEMVALUE_CHANGED:
			{
				std::vector<CUnionValueProperty *> *pVecList;
				if( m_bSelfAlloc == true ) pVecList = &m_pVecVariable;
				else pVecList = m_ppVecVariable;

				for( DWORD i=0; i<pVecList->size(); i++ ) {
					CUnionValueProperty *pVariable = (*pVecList)[i];
					if( pVariable->GetXTPropItem() == pItem ) {
						m_bUpdateItem = false;
						CString szStr = pItem->GetValue();
						switch( pVariable->GetType() ) {
							case CUnionValueProperty::Float:
								pVariable->SetVariable( (float)atof(szStr) );
								break;
							case CUnionValueProperty::Char:
								pVariable->SetVariable( (char*)*szStr.GetBuffer() );
								break;
							case CUnionValueProperty::String:
								pVariable->SetVariable( szStr.GetBuffer() );
								break;
							case CUnionValueProperty::Integer_Combo:
								{
									for( int j=1; ; j++ ) {
										const char *szString = _GetSubStrByCount( j, (char*)pVariable->GetSubDescription() );
										if( strcmp( szStr, szString ) == 0 ) {
											pVariable->SetVariable( j - 1 );
											break;
										}
									}
								}
								break;
							case CUnionValueProperty::String_FileOpen:
								{
									pVariable->SetVariable( szStr.GetBuffer() );
								}
								break;
						}
						OnChangeValue( pVariable, i );
						if( pVariable->GetType() == CUnionValueProperty::String_FileOpen || m_bUpdateItem == true )
							pItem->SetValue( *pVariable->GetBindStr() );
						break;
					}
				}

			}
			break;
		case XTP_PGN_SELECTION_CHANGED:
			{
				std::vector<CUnionValueProperty *> *pVecList;
				if( m_bSelfAlloc == true ) pVecList = &m_pVecVariable;
				else pVecList = m_ppVecVariable;
				if( pVecList == NULL ) break;

				for( DWORD i=0; i<pVecList->size(); i++ ) {
					CUnionValueProperty *pVariable = (*pVecList)[i];
					if( pVariable->GetDescription() == pItem->GetCaption() ) {
						OnSelectChangeValue( pVariable, i );
						if( pVariable->GetType() == CUnionValueProperty::String_FileOpen || m_bUpdateItem == true ) 
							pItem->SetValue( *pVariable->GetBindStr() );
						break;
					}
				}
			}
			break;
	}
	return S_OK;
}

LRESULT CPropertyGridImp::ProcessCustomDialogGrid( WPARAM wParam, LPARAM lParam )
{
	CXTPPropertyGridItem *pItem = (CXTPPropertyGridItem *)lParam;

	std::vector<CUnionValueProperty *> *pVecList;
	if( m_bSelfAlloc == true ) pVecList = &m_pVecVariable;
	else pVecList = m_ppVecVariable;
	if( pVecList == NULL ) return S_OK;

	for( DWORD i=0; i<pVecList->size(); i++ ) {
		CUnionValueProperty *pVariable = (*pVecList)[i];
		if( pVariable->GetDescription() == pItem->GetCaption() ) {
			OnCustomDialogValue( pVariable, i );
			break;
		}
	}

	return S_OK;
}

void CPropertyGridImp::ModifyItem( WPARAM wParam, LPARAM lParam )
{
	int nItemIndex = (int)wParam;
	CUnionValueProperty *pVariable = (CUnionValueProperty *)lParam;

	if( nItemIndex >= (int)m_pVecItem.size() )
	{
		ASSERT( 0 && _T("Index Overflow") );
		return;
	}

	m_pVecItem[ nItemIndex ]->SetValue( *pVariable->GetBindStr() );
}

void CPropertyGridImp::SetReadOnly( DWORD dwIndex, bool bReadonly )
{
	if( dwIndex < 0 || dwIndex >= m_pVecItem.size() ) return;
	m_pVecItem[dwIndex]->SetReadOnly( bReadonly );
}

void CPropertyGridImp::SetReadOnly( CUnionValueProperty *pVariable, bool bReadonly )
{
	pVariable->SetDefaultEnable( !bReadonly );
}

void CPropertyGridImp::RefreshPropertyGridVariable()
{
	std::vector<CUnionValueProperty *> *pVecList;
	if( m_bSelfAlloc == true ) pVecList = &m_pVecVariable;
	else pVecList = m_ppVecVariable;

	for( DWORD i=0; i<pVecList->size(); i++ ) {
		CUnionValueProperty *pVariable = (*pVecList)[i];
		OnSetValue( pVariable, i );
		pVariable->GetXTPropItem()->SetValue( *pVariable->GetBindStr() );
	}
}

void CPropertyGridImp::UpdateItem()
{
	m_bUpdateItem = true;
}

void CPropertyGridImp::SetInitDirectory( const char * szInitDirectory )
{
	m_szInitDirectory = std::string( szInitDirectory );
}