#include "StdAfx.h"
#include "ObjectBase.h"

#include "MainFrm.h"
#include "resource.h"
#include "EtViewerDoc.h"
#include "EtViewerView.h"
#include "UserMessage.h"
#include "PaneDefine.h"
#include "GlobalValue.h"



CObjectBase::CObjectBase()
{
	m_Type = CObjectBase::UNKNOWN;
	m_pParent = NULL;
	m_ItemID = NULL;
	m_bShow = true;
	m_pTreeCtrl = NULL;
	m_bFocus = false;
	m_bModify = false;
}

CObjectBase::~CObjectBase()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );
	RemoveChild( (CObjectBase*)NULL );
}

CObjectBase *CObjectBase::GetChildFromName( CString &szName )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( m_pVecChild[i]->m_szName == szName ) return m_pVecChild[i];
	}
	return NULL;
}


void CObjectBase::AddChild( CObjectBase *pObject )
{
	m_pVecChild.push_back( pObject );
}

void CObjectBase::RemoveChild( CObjectBase *pObject )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( pObject == NULL ) {
			m_pVecChild[i]->RemoveChild( (CObjectBase*)NULL );
			SAFE_DELETE( m_pVecChild[i] );
			m_pVecChild.erase( m_pVecChild.begin() + i );
			i--;
		}
		else {
			if( m_pVecChild[i] == pObject ) {
				m_pVecChild[i]->RemoveChild( (CObjectBase*)NULL );
				SAFE_DELETE( m_pVecChild[i] );
				m_pVecChild.erase( m_pVecChild.begin() + i );
				return;
			}
		}
	}
}

void CObjectBase::RemoveChild( CString &szName )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( szName.IsEmpty() ) {
			m_pVecChild[i]->RemoveChild( (CObjectBase *)NULL );
			SAFE_DELETE( m_pVecChild[i] );
			m_pVecChild.erase( m_pVecChild.begin() + i );
			i--;
		}
		else {
			if( m_pVecChild[i]->m_szName == szName ) {
				m_pVecChild[i]->RemoveChild( (CObjectBase*)NULL );
				SAFE_DELETE( m_pVecChild[i] );
				m_pVecChild.erase( m_pVecChild.begin() + i );
				return;
			}
		}
	}
}

void CObjectBase::Process( LOCAL_TIME LocalTime )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		m_pVecChild[i]->Process( LocalTime );
	}
}

void CObjectBase::AddPropertyInfo( PropertyGridBaseDefine Define[] )
{
	CUnionValueProperty *pVariable;
	for( DWORD i=0; ; i++ ) {
		if( Define[i].szCategory == NULL ) break;

		pVariable = new CUnionValueProperty( Define[i].Type );
		pVariable->SetCategory( Define[i].szCategory );
		pVariable->SetDescription( Define[i].szStr );
		pVariable->SetSubDescription( Define[i].szDescription, true );
		pVariable->SetDefaultEnable( Define[i].bEnable );
		pVariable->SetCustomDialog( Define[i].bCustomDialog );

		m_pVecPropertyList.push_back( pVariable );
	}
}

void CObjectBase::SetModify( bool bModify, CString szStr )
{
	m_bModify = bModify;

	if( m_bModify == false ) {
		m_szModify.Empty();
	}
	else m_szModify += szStr;
}

void CObjectBase::GetChildModifyString( CString &szBuf, int nTabCount )
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

void CObjectBase::Show( bool bShow ) 
{ 
	m_bShow = bShow; 

	if( m_Type == SUBMESH ) {
		CObjectSkin *pSkin = dynamic_cast<CObjectSkin*>(m_pParent);
		if( pSkin ) {
			int i;
			for( i = 0; i < (int)pSkin->GetChildCount(); i++) {
				if( pSkin->GetChild( i ) == this ) break;
			}
			if( i != pSkin->GetChildCount() ) {
				pSkin->GetObjectHandle()->ShowSubmesh( i, bShow);
			}
		}
	}

	CWnd *pWnd = GetPaneWnd( WORKSPACE_PANE );
	if( pWnd && m_ItemID )
		pWnd->SendMessage( UM_WORKSPACEPANE_SETSHOWFLAG, (WPARAM)m_ItemID, (LPARAM)m_bShow );
}


bool CObjectBase::IsChildModify()
{
	if( m_bModify == true ) return true;
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		if( m_pVecChild[i]->IsChildModify() == true ) return true;
	}
	return false;
}

void CObjectBase::ExportObject( FILE *fp, int &nCount )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		m_pVecChild[i]->ExportObject( fp, nCount );
	}
}

void CObjectBase::ImportObject( FILE *fp )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		m_pVecChild[i]->ImportObject( fp );
	}
}

void CObjectBase::GetChildFileName( std::vector<CString> &szVecResult )
{
	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		m_pVecChild[i]->GetChildFileName( szVecResult );
	}
}