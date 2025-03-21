#include "StdAfx.h"
#include "ActionBase.h"

#include "MainFrm.h"
#include "resource.h"
#include "UserMessage.h"
#include "PaneDefine.h"

#include "ActionObject.h"
#include "ActionSignal.h"
#include "ActionElement.h"
#include "ObjectLightFolder.h"
#include "ObjectLightDir.h"
#include "ObjectLightSpot.h"
#include "ObjectLightPoint.h"


CActionBase::CActionBase()
{
	m_Type = CActionBase::UNKNOWN;
	m_pParent = NULL;
	m_ItemID = NULL;
	m_pTreeCtrl = NULL;
	m_bFocus = false;
	m_bShow = true;
}

CActionBase::~CActionBase()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );
	RemoveChild( (CActionBase*)NULL );
}

CActionBase *CActionBase::GetChildFromName( CString &szName )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( m_pVecChild[i]->m_szName == szName ) return m_pVecChild[i];
	}
	return NULL;
}


void CActionBase::AddChild( CActionBase *pObject )
{
	m_pVecChild.push_back( pObject );
}

void CActionBase::RemoveChild( CActionBase *pObject )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( pObject == NULL ) {
			m_pVecChild[i]->RemoveChild( (CActionBase*)NULL );
			SAFE_DELETE( m_pVecChild[i] );
			m_pVecChild.erase( m_pVecChild.begin() + i );
			i--;
		}
		else {
			if( m_pVecChild[i] == pObject ) {
				m_pVecChild[i]->RemoveChild( (CActionBase*)NULL );
				SAFE_DELETE( m_pVecChild[i] );
				m_pVecChild.erase( m_pVecChild.begin() + i );
				return;
			}
		}
	}
}

void CActionBase::RemoveChild( CString &szName )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( szName.IsEmpty() ) {
			m_pVecChild[i]->RemoveChild( (CActionBase *)NULL );
			SAFE_DELETE( m_pVecChild[i] );
			m_pVecChild.erase( m_pVecChild.begin() + i );
			i--;
		}
		else {
			if( m_pVecChild[i]->m_szName == szName ) {
				m_pVecChild[i]->RemoveChild( (CActionBase*)NULL );
				SAFE_DELETE( m_pVecChild[i] );
				m_pVecChild.erase( m_pVecChild.begin() + i );
				return;
			}
		}
	}
}

void CActionBase::Process( LOCAL_TIME LocalTime )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		m_pVecChild[i]->Process( LocalTime );
	}
}

void CActionBase::AddPropertyInfo( PropertyGridBaseDefine Define[] )
{
	CUnionValueProperty *pVariable;
	for( DWORD i=0; ; i++ ) {
		if( Define[i].szCategory == NULL ) break;

		pVariable = new CUnionValueProperty( Define[i].Type );
		pVariable->SetCategory( Define[i].szCategory );
		pVariable->SetDescription( Define[i].szStr );
		pVariable->SetSubDescription( Define[i].szDescription, true );
		pVariable->SetDefaultEnable( Define[i].bEnable );

		m_pVecPropertyList.push_back( pVariable );
	}
}

void CActionBase::SetModify( bool bModify, CString szStr )
{
	m_bModify = bModify;

	if( m_bModify == false ) {
		m_szModify.Empty();
	}
	else m_szModify += szStr;
}

void CActionBase::GetChildModifyString( CString &szBuf, int nTabCount )
{
	if( !m_szModify.IsEmpty() ) {
		CString szTemp;
		szTemp.Format( "%s - %s", m_szName, GetClassName() );
		szBuf += szTemp + "\n";
		szBuf += m_szModify;
	}
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		m_pVecChild[i]->GetChildModifyString( szBuf, nTabCount );
	}
}

bool CActionBase::IsChildModify()
{
	if( m_bModify == true ) return true;
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( m_pVecChild[i]->IsChildModify() == true ) return true;
	}
	return false;
}

bool CActionBase::ExportObject( FILE *fp, int &nCount )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( m_pVecChild[i]->ExportObject( fp, nCount ) == false ) return false;
	}
	return true;
}

bool CActionBase::ImportObject( FILE *fp )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( m_pVecChild[i]->ImportObject( fp ) == false ) return false;
	}
	return true;
}

CActionBase &CActionBase::operator = ( CActionBase &e )
{
	m_Type = e.m_Type;

	CActionBase *pBase;
	CActionBase *pCopy;
	for( DWORD i=0; i<e.m_pVecChild.size(); i++ ) {
		pBase = e.m_pVecChild[i];
		switch( pBase->GetType() ) {
			case OBJECT: 
				pCopy = new CActionObject; 
				*(CActionObject*)pCopy = *(CActionObject*)pBase;
				break;
			case ELEMENT: 
				pCopy = new CActionElement; 
				*(CActionElement*)pCopy = *(CActionElement*)pBase;
				break;
			case SIGNAL: 
				pCopy = new CActionSignal; 
				*(CActionSignal*)pCopy = *(CActionSignal*)pBase;
				break;
			case LIGHTDIR: 
				pCopy = new CObjectLightDir; 
				*(CObjectLightDir*)pCopy = *(CObjectLightDir*)pBase;
				break;
			case LIGHTPOINT: 
				pCopy = new CObjectLightPoint; 
				*(CObjectLightPoint*)pCopy = *(CObjectLightPoint*)pBase;
				break;
			case LIGHTSPOT: 
				pCopy = new CObjectLightSpot; 
				*(CObjectLightSpot*)pCopy = *(CObjectLightSpot*)pBase;
				break;
			case LIGHTFOLDER: 
				pCopy = new CObjectLightFolder; 
				*(CObjectLightFolder*)pCopy = *(CObjectLightFolder*)pBase;
				break;
		}
		pCopy->SetParent( this );
		m_pVecChild.push_back( pCopy );
	}

	CUnionValueProperty *pVariable;
	CUnionValueProperty *pCopyVar;
	for( DWORD i=0; i<e.m_pVecPropertyList.size(); i++ ) {
		pVariable = e.m_pVecPropertyList[i];
		pCopyVar = new CUnionValueProperty( pVariable->GetType() );
		*pCopyVar = *pVariable;
		m_pVecPropertyList.push_back( pCopyVar );
	}

	m_pTreeCtrl = e.m_pTreeCtrl;
	m_bModify = e.m_bModify;
	m_bFocus = e.m_bFocus;
	m_bShow = e.m_bShow;
	Activate();

	return *this;
}