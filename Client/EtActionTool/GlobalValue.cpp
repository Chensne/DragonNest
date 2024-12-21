#include "StdAfx.h"
#include "GlobalValue.h"
#include "ActionBase.h"
#include "ActionObject.h"
#include "ActionElement.h"
#include "ActionSignal.h"
#include "MainFrm.h"
#include "resource.h"
#include "PaneDefine.h"
#include "UserMessage.h"
#include "ObjectLightFolder.h"
#include "SignalCustomRender.h"
#include "SignalManager.h"
#include "SignalItem.h"
#include "ActionGlobal.h"
#include "DnGameWeaponMng.h"
#ifdef _CHECK_WALKFRONT
#include "RenderBase.h"
#endif

CGlobalValue g_GlobalValue;

CGlobalValue::CGlobalValue()
{
	m_pActiveView = NULL;
	m_pRootAction = NULL;
	m_pControlObject = NULL;
	m_pPlayObject = NULL;
	m_pSignalCustonRenderObject = NULL;
	m_nFPS = 60;

	m_pRootLight = new CObjectLightFolder;
	m_pRootLight->SetName( CString( "Lights" ) );
	m_pRootLight->Activate();

	m_pGlobalOption = new CActionGlobal;
	m_pGlobalOption->SetName( CString( "Global" ) );
	m_pGlobalOption->Activate();
}

CGlobalValue::~CGlobalValue()
{
	Finalize();
}

void CGlobalValue::Finalize()
{
	SAFE_DELETE( m_pGlobalOption );
	if( m_pRootLight )
		((CObjectLightFolder*)m_pRootLight)->SaveLightSetting( CString("") );

	SAFE_DELETE( m_pRootLight );
	SAFE_DELETE( m_pRootAction );
}

void CGlobalValue::SetView( CView *pView )
{
	m_pActiveView = pView;
}

CView *CGlobalValue::GetView() const
{
	return m_pActiveView;
}

void CGlobalValue::Reset()
{
	SAFE_DELETE( m_pRootAction );
	SetControlObject( NULL );
	m_pSignalCustonRenderObject = NULL;

	CWnd *pWnd = GetPaneWnd( ACTION_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );

	pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );

	pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );

	if( !m_szInitLoadFile.IsEmpty() ) {
		OpenAction( m_szInitLoadFile );
		m_szInitLoadFile.Empty();
	}
}

bool CGlobalValue::IsExistFile( CString szFileName )
{
	FILE *fp = NULL;
	fopen_s( &fp, szFileName, "rb" );
	if( fp == NULL ) return false;
	fclose(fp);
	return true;
}

bool CGlobalValue::CreateAction( CString szName )
{
	char szPath[512] = { 0, };
	char szFileName[512] = { 0, };
	char szExt[32] = { 0, };
	CString szActName;


	_GetExt( szExt, _countof(szExt), szName );
	_GetFileName( szFileName, _countof(szFileName), szName );
	_GetPath( szPath, _countof(szPath), szName );

	SAFE_DELETE( m_pRootAction );
	m_pRootAction = new CActionObject;

	m_pRootAction->SetName( (CString)( szFileName ) );
	((CActionObject*)m_pRootAction)->SetPath( (CString)( szPath ) );

	((CActionObject*)m_pRootAction)->SaveAction();
	szActName.Format( "%s%s.act", szPath, szFileName );
	((CActionObject*)m_pRootAction)->LoadAction( szActName );

	CWnd *pWnd = GetPaneWnd( ACTION_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, (WPARAM)m_pRootAction );

	m_pRootAction->Activate();

	pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) pWnd->SendMessage( UM_EVENT_PANE_RESETCOPYCUT );

	pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)m_pRootAction );

	return true;
}

bool CGlobalValue::OpenAction( CString szName )
{
	m_pSignalCustonRenderObject = NULL;
	if( GetView() == NULL ) {
		m_szInitLoadFile = szName;
		return false;
	}

	char szPath[512] = { 0, };
	char szFileName[512] = { 0, };
	char szExt[32] = { 0, };

	SAFE_DELETE( m_pRootAction );

	_GetExt( szExt, _countof(szExt), szName );
	_GetFileName( szFileName, _countof(szFileName), szName );
	_GetPath( szPath, _countof(szPath), szName );

	if( _stricmp( szExt, "skn" ) != NULL &&
		_stricmp( szExt, "act" ) != NULL ) return false;

	CString szSkinName, szAniName, szActName;

	szSkinName.Format( "%s%s.skn", szPath, szFileName );
	szAniName.Format( "%s%s.ani", szPath, szFileName );
	szActName.Format( "%s%s.act", szPath, szFileName );

	bool bExistSknFile = IsExistFile( szSkinName );
	bool bExistAniFile = IsExistFile( szAniName );
	bool bExistActFile = IsExistFile( szActName );

//	if( bExistSknFile == false ) return false;

	m_pRootAction = new CActionObject;

	if( bExistSknFile && bExistAniFile ) ((CActionObject*)m_pRootAction)->LoadAni( szAniName );

	if( bExistSknFile )
		((CActionObject*)m_pRootAction)->LoadSkin( szSkinName );
	else {
		// �׼������� �ְ� ��Ų�� ������쿡 �ٸ� ��Ų�� �� �� �ֵ��� â�� �ѹ� ����ش�.
		TCHAR szFilter[] = _T( "EternityEngine Skin File (*.skn)|*.ani|All Files (*.*)|*.*||" );
		CFileDialog dlg( TRUE, _T("skn"), _T("*.skn"), OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, NULL );
		if( dlg.DoModal() == IDOK ) {
			szSkinName = dlg.GetPathName();
			((CActionObject*)m_pRootAction)->LoadSkin( szSkinName );
			if( bExistAniFile ) {
				((CActionObject*)m_pRootAction)->LoadAni( szAniName );
			}
		}
	}

	if( bExistActFile ) {
		if( !bExistAniFile ) { // �׼������� �ֱ� LoadAction �ϱ� ���� ���ϸ��̼��� ���� �־�� �ϴ� ������ ��쿡 �켱 �����ϰ� ���ش�.
			if( ((CActionObject*)m_pRootAction)->CheckExistLinkAniAction( szActName ) ) {
				TCHAR szFilter[] = _T( "EternityEngine Animation File (*.ani)|*.ani|All Files (*.*)|*.*||" );
				CFileDialog dlg( TRUE, _T("ani"), _T("*.ani"), OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, NULL );
				if( dlg.DoModal() == IDOK ) {
					((CActionObject*)m_pRootAction)->LoadAni( dlg.GetPathName() );
//					((CActionObject*)m_pRootAction)->LoadSkin( szSkinName );
				}
			}
		}
		if( ((CActionObject*)m_pRootAction)->LoadAction( szActName ) == false ) {
			MessageBox( GetView()->m_hWnd, "���������� ��������!!", "����", MB_OK );
			SAFE_DELETE( m_pRootAction );
			return false;
		}
		else {
			DWORD dwAttr = GetFileAttributes( szActName );
			if( dwAttr & FILE_ATTRIBUTE_READONLY ) {
				CString szStr;
				szStr.Format( "���� ���ϵ��� �б����� �Ӽ��Դϴ�.\n\n%s", szActName );
				MessageBox( ((CMainFrame*)AfxGetMainWnd())->m_hWnd, szStr, "���", MB_OK );
			}
		}
	}

	CDnGameWeaponMng::GetInstance().OnLoadAction( szActName );

	m_pRootAction->SetName( (CString)( szFileName ) );
	((CActionObject*)m_pRootAction)->SetPath( (CString)( szPath ) );

	CWnd *pWnd = GetPaneWnd( ACTION_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, (WPARAM)m_pRootAction );

	m_pRootAction->Activate();

	pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) pWnd->SendMessage( UM_EVENT_PANE_RESETCOPYCUT );

	pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)m_pRootAction );

	return true;
}

bool CGlobalValue::SaveAction()
{
	if( !m_pRootAction ) return true;

	return ((CActionObject*)m_pRootAction)->SaveAction();
}

bool CGlobalValue::IsOpenAction()
{
	if( m_pRootAction ) return true;
	return false;
}

bool CGlobalValue::AddElement( CString szString )
{
	if( !m_pRootAction ) return false;

	if( GetObjectFromName( m_pRootAction, szString ) ) return false;
	CActionElement *pElement = new CActionElement;
	pElement->SetName( szString );
	pElement->SetParent( m_pRootAction );

	m_pRootAction->AddChild( pElement );

	CWnd *pWnd = GetPaneWnd( ACTION_PANE );
	if( pWnd ) pWnd->SendMessage( UM_ACTION_PANE_ADD, (WPARAM)pElement );

	pElement->Activate();

	pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE, 0, (LPARAM)pElement );


	return true;
}

bool CGlobalValue::RemoveElement( HTREEITEM hti )
{
	if( !m_pRootAction ) return false;
	CActionBase *pBase = GetObjectFromTreeID( m_pRootAction, hti );

	if( pBase == NULL ) return false;
	if( pBase == m_pControlObject ) m_pControlObject = NULL;
	if( pBase == m_pPlayObject ) {
		((CActionElement*)pBase)->Stop();
		m_pPlayObject = NULL;
	}

	pBase->GetParent()->RemoveChild( pBase );
	return true;
}

CActionBase *CGlobalValue::GetObjectFromTreeID( CActionBase *pRoot, HTREEITEM hti )
{
	if( pRoot == NULL ) return NULL;
	CActionBase *pBase = FindObject( pRoot, hti );
	return pBase;
}

CActionBase *CGlobalValue::GetObjectFromName( CActionBase *pRoot, CString szName )
{
	if( pRoot == NULL ) return NULL;
	CActionBase *pBase = FindObject( pRoot, szName );
	return pBase;
}

CActionBase *CGlobalValue::FindObject( CActionBase *pObject, CString &szDescription )
{
	if( pObject->GetName() == szDescription ) return pObject;
	for( DWORD i=0; i<pObject->GetChildCount(); i++ ) {
		CActionBase *pChild = FindObject( pObject->GetChild(i), szDescription );
		if( pChild != NULL ) return pChild;
	}
	return NULL;
}

CActionBase *CGlobalValue::FindObject( CActionBase *pObject, HTREEITEM hti )
{
	if( pObject->GetTreeItemID() == hti ) return pObject;
	for( DWORD i=0; i<pObject->GetChildCount(); i++ ) {
		CActionBase *pChild = FindObject( pObject->GetChild(i), hti );
		if( pChild != NULL ) return pChild;
	}
	return NULL;
}

CActionBase *CGlobalValue::GetRootObject()
{
	return m_pRootAction;
}

bool CGlobalValue::IsCanPlayControl()
{
	if( m_pPlayObject ) return true;
	return false;
}

void CGlobalValue::SetControlObject( CActionBase *pBase ) 
{ 
	m_pPlayObject = NULL;

	m_pControlObject = pBase; 
	if( m_pControlObject == NULL ) return;

	switch( m_pControlObject->GetType() ){
		case CActionBase::OBJECT:
			break;
		case CActionBase::ELEMENT:
			{
				CActionElement *pElement = (CActionElement *)m_pControlObject;
				if( pElement->IsCanPlay() ) m_pPlayObject = m_pControlObject;
			}
			break;
	}
}
void WriteCString( CString *pStr, FILE *fp )
{
	static char szStr[4096];
	sprintf_s( szStr, pStr->GetBuffer() );

	int nLength = pStr->GetLength() + 1;
	fwrite( &nLength, sizeof(int), 1, fp );
	fwrite( szStr, nLength, 1, fp );
}

void ReadCString( CString *pStr, FILE *fp )
{
	static char szStr[4096];
	int nLength;

	fread( &nLength, sizeof(int), 1, fp );
	fread( szStr, nLength, 1, fp );

	*pStr = szStr;
}

CActionSignal *CGlobalValue::AddSignal( CString szElementName, int nSignalIndex, int nStartFrame, int nEndFrame, int nYOrder )
{
	CActionElement *pElement = (CActionElement *)GetObjectFromName( m_pRootAction, szElementName );
	CActionSignal *pSignal = new CActionSignal;
	pSignal->SetParent( pElement );
	pElement->AddChild( pSignal );

	pSignal->SetSignalIndex( nSignalIndex );
	pSignal->SetStartFrame( nStartFrame );
	pSignal->SetEndFrame( nEndFrame );
	pSignal->SetYOrder( nYOrder );

	pSignal->Activate();

	return pSignal;
}


CActionBase *CGlobalValue::GetRootLightObject()
{
	return m_pRootLight;
}

void CGlobalValue::RemoveLightFromTreeID( HTREEITEM hti )
{
	CActionBase *pBase = FindObject( m_pRootLight, hti );
	if( pBase == NULL ) return;
	if( pBase->GetParent() ) {
		/*
		CWnd *pWnd = GetPaneWnd( WORKSPACE_PANE );
		pWnd->SendMessage( UM_WORKSPACEPANE_REMOVE, (WPARAM)pBase );

		*/
		pBase->GetParent()->RemoveChild( pBase );
	}
}


void CGlobalValue::AddLight( CActionBase *pBase )
{
	if( !m_pRootLight ) return;

	m_pRootLight->AddChild( pBase );
	pBase->SetParent( m_pRootLight );

	CWnd *pWnd = GetPaneWnd( LIGHT_PANE );
	if( pWnd ) pWnd->SendMessage( UM_LIGHT_PANE_ADD, (WPARAM)pBase );

	pBase->Activate();
}

CString CGlobalValue::GetUniqueName( CActionBase::OBJECT_TYPE Type )
{
	char szStr[512];
	int nCount = 0;
	switch( Type ) {
		case CActionBase::ELEMENT:
			while(1) {
				sprintf_s( szStr, "Action #%02d", nCount );
				if( IsSameName( m_pRootAction, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CActionBase::LIGHTDIR:
			while(1) {
				sprintf_s( szStr, "Directional #%02d", nCount );
				if( IsSameName( m_pRootLight, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CActionBase::LIGHTPOINT:
			while(1) {
				sprintf_s( szStr, "Point #%02d", nCount );
				if( IsSameName( m_pRootLight, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CActionBase::LIGHTSPOT:
			while(1) {
				sprintf_s( szStr, "Spot #%02d", nCount );
				if( IsSameName( m_pRootLight, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
	}
	return CString(szStr);
}

BOOL CGlobalValue::IsSameName( CActionBase *pRoot, CString &szUniqueName )
{
	CActionBase *pBase = FindObject( pRoot, szUniqueName );
	if( pBase == NULL ) return FALSE;
	return TRUE;
}


CString CGlobalValue::IsCanCheckOut( const char *szFileName )
{
#ifdef NO_LOCK
	return CString("");
#endif //NO_LOCK
	CString szErrorMsg;

	if( ( ( szFileName[0] >= 'A' && szFileName[0] <= 'Z' ) || ( szFileName[0] >= 'a' && szFileName[0] <= 'a' ) ) && szFileName[1] == ':' ) {
		return szErrorMsg;
	}
	char szName[256] = { 0, };
	char szPath[512] = { 0, };
	CString szCheckoutName;
	_GetFileName( szName, _countof(szName), szFileName );
	_GetPath( szPath, _countof(szPath), szFileName );
	szCheckoutName.Format( "%s\\%s.chk", CSignalManager::GetInstance().GetWorkingFolder(), szName );
	FILE *fp;
	fopen_s( &fp, szCheckoutName, "rb" );
	if( fp == NULL ) return szErrorMsg;

	CString szFullPathName, szHostName, szIP;

	ReadCString( &szFullPathName, fp );
	ReadCString( &szHostName, fp );
	ReadCString( &szIP, fp );

	CString szTemp;
	szTemp.Format( "%s%s", szPath, szName );
	szTemp.MakeLower();
	if( !strcmp( szTemp, szFullPathName ) ) {
		szErrorMsg.Format( "%s ( %s )", szHostName, szIP );
	}

	fclose(fp);
	return szErrorMsg;
}

bool CGlobalValue::CheckOut( const char *szFileName )
{
#ifdef NO_LOCK
	return true;
#endif //NO_LOCK
	CString szError = IsCanCheckOut( szFileName );
	if( !szError.IsEmpty() ) return false;

	if( ( ( szFileName[0] >= 'A' && szFileName[0] <= 'Z' ) || ( szFileName[0] >= 'a' && szFileName[0] <= 'a' ) ) && szFileName[1] == ':' ) {
		return true;
	}

	CString szIP, szHostName;
	CSignalManager::GetInstance().GetLocalIP( szIP );
	CSignalManager::GetInstance().GetHostName( szHostName );

	CString szCheckoutName;
	char szName[256] = { 0, };
	char szPath[512] = { 0, };
	_GetFileName( szName, _countof(szName), szFileName );
	_GetPath( szPath, _countof(szPath), szFileName );
	szCheckoutName.Format( "%s\\%s.chk", CSignalManager::GetInstance().GetWorkingFolder(), szName );

	FILE *fp;
	fopen_s( &fp, szCheckoutName, "wb" );
	if( fp == NULL ) return false;

	CString szTemp;
	szTemp.Format( "%s%s", szPath, szName );
	szTemp.MakeLower();
	WriteCString( &szTemp, fp );
	WriteCString( &szHostName, fp );
	WriteCString( &szIP, fp );

	m_szCheckOutFile = szCheckoutName;

	fclose(fp);
	return true;
}

bool CGlobalValue::CheckIn()
{
#ifdef NO_LOCK
	return true;
#endif //NO_LOCK
	if( !m_szCheckOutFile.IsEmpty() ) {
		if( DeleteFile( m_szCheckOutFile ) == FALSE ) return false;
	}

	m_szCheckOutFile.Empty();
	return true;
}

void CGlobalValue::IgnoreCheckOut( const char *szFileName )
{
	CString szCheckoutName;
	char szName[256] = { 0, };
	_GetFileName( szName, _countof(szName), szFileName );
	szCheckoutName.Format( "%s\\%s.chk", CSignalManager::GetInstance().GetWorkingFolder(), szName );

	DeleteFile( szCheckoutName );
}

void CGlobalValue::CalcSignalReport( CString &szBuffer )
{
	if( GetRootObject() == NULL ) return;
	CString szStr;

	CActionBase *pRoot = GetRootObject();
	CString szName = pRoot->GetName();
	int nActionCount = pRoot->GetChildCount();
	szStr.Format( "FileName,%s.act,,Action Count,%d\n\n", szName, nActionCount );
	szBuffer += szStr;
	std::vector<std::string> szVecSort;
	for( DWORD i=0; i<pRoot->GetChildCount(); i++ ) {
		szVecSort.push_back( pRoot->GetChild(i)->GetName().GetBuffer() );
	}
	std::sort( szVecSort.begin(), szVecSort.end() );

	CActionElement *pChild;
	for( DWORD i=0; i<szVecSort.size(); i++ ) {
		pChild = (CActionElement *)pRoot->GetChildFromName( (CString)szVecSort[i].c_str() );

		szStr.Format( "%s,LinkAni:%s,Length:%d,NextAction:%s,BlendFrame:%d,StartFrame:%d,Count:%d,,\n", 
			pChild->GetName(), 
			pChild->GetLinkAniName(), 
			pChild->GetLength(), 
			pChild->GetNextActionName(),
			pChild->GetBlendFrame(),
			pChild->GetNextActionFrame(),
			pChild->GetChildCount() );
		szBuffer += szStr;

		CActionSignal *pSignal;
		std::map<std::string, std::vector<int>> szMapList;
		std::map<std::string, std::vector<int>>::iterator it;
		std::vector<int> szTemp;
		CString szSignalName;
		CSignalItem *pItem;
		for( DWORD j=0; j<pChild->GetChildCount(); j++ ) {
			pSignal = (CActionSignal*)pChild->GetChild(j);
			pItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( pSignal->GetSignalIndex() );
			szSignalName = pItem->GetName();
			it = szMapList.find( szSignalName.GetBuffer() );
			if( it != szMapList.end() ) {
				it->second.push_back(j);
			}
			else {
				szTemp.clear();
				szTemp.push_back(j);
				szMapList.insert( make_pair( szSignalName, szTemp ) );
			}
		}
		DWORD j=0;
		szStr.Format( ",," );
		szBuffer += szStr;
		for( it = szMapList.begin(); it != szMapList.end(); it++ ){
			szStr.Format( "%s:%d,", it->first.c_str(), it->second.size() );
			szBuffer += szStr;
		}
		szBuffer += "\n";

		std::vector<std::vector<std::string>> szVecSignal;
		std::vector<std::string> szVecStr;
		int nMaxLine = 0;

		for( it = szMapList.begin(); it != szMapList.end(); it++ ){
			szVecStr.clear();
			for( DWORD k=0; k<it->second.size(); k++ ) {
				pSignal = (CActionSignal*)pChild->GetChild(it->second[k]);
				pItem = CSignalManager::GetInstance().GetSignalItemFromUniqueIndex( pSignal->GetSignalIndex() );

				szStr.Format( "%s-%d (%d~%d)", pItem->GetName(), k+1, pSignal->GetStartFrame(), pSignal->GetEndFrame() );
				szVecStr.push_back( szStr.GetBuffer() );
				std::string szTemp;
				for( DWORD h=0; h<pItem->GetParameterCount(); h++ ) {
					/*
					CUnionValueProperty *pVariable = pSignal->GetProperty(h);
					if( !pVariable ) szTemp = "NULL";
					else {
						if( pVariable->GetType() == CUnionValueProperty::Integer_Combo ) {
						}
						else szTemp = pVariable->GetBindStr();
					}
					*/
					szStr.Format( "%s:%s", pItem->GetParameter(h)->GetDescription(), ( pSignal->GetProperty(h) ) ? *pSignal->GetProperty(h)->GetBindStr() : "NULL" );
					szVecStr.push_back( szStr.GetBuffer() );
				}
				szVecStr.push_back("");

				if( (int)szVecStr.size() > nMaxLine ) nMaxLine = (int)szVecStr.size();
			}
			szVecSignal.push_back( szVecStr );
		}
		for( int j=0; j<nMaxLine; j++ ) {
			szBuffer += ",,";
			for( DWORD k=0; k<szVecSignal.size(); k++ ) {
				if( j < (int)szVecSignal[k].size() ) 
					szBuffer += szVecSignal[k][j].c_str();
				szBuffer += ",";
			}
			szBuffer += "\n";
		}


		szBuffer += "\n\n";
	}
}

void CGlobalValue::ExportAction( const char *szFileName, std::vector<std::string> &szVecActionList )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "wb" );
	if( fp == NULL ) return;

	int nCount = 0;
	int nCount2 = 0;
	int nOffset = 0;
	CString szHeader = "Eternity Engine Action Export File 1.0";

	WriteCString( &szHeader, fp );
	nOffset = ftell(fp);
	fwrite( &nCount, sizeof(int), 1, fp );

	for( DWORD i=0; i<szVecActionList.size(); i++ ) {
		CActionBase *pAction = GetObjectFromName( m_pRootAction, (CString)szVecActionList[i].c_str() );
		if( !pAction || pAction->GetType() != CActionBase::ELEMENT ) continue;

		pAction->ExportObject( fp, nCount2 );
		nCount++;
	}
	fseek( fp, nOffset, SEEK_SET );
	fwrite( &nCount, sizeof(int), 1, fp );

	fclose( fp );
}

void CGlobalValue::ImportAction( const char *szFileName )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "rb" );
	if( fp == NULL ) return;

	int nCount;
	CString szHeader;
	ReadCString( &szHeader, fp );
	fread( &nCount, sizeof(int), 1, fp );
	if( szHeader != "Eternity Engine Action Export File 1.0" ) return;
	if( nCount < 1 ) return;


	CWnd *pWnd = GetPaneWnd( ACTION_PANE );
	for( int i=0; i<nCount; i++ ) {
		CActionBase *pAction = new CActionElement;
		pAction->SetParent( m_pRootAction );
		pAction->ImportObject( fp );

		int nNameCount = 0;
		CString szOrigName = pAction->GetName();
		CString szTempName = szOrigName;
		while( IsSameName( m_pRootAction, szTempName ) ) {
			szTempName.Format( "%s #%02d", szOrigName, nNameCount );
			pAction->SetName( szTempName );
		}
		m_pRootAction->AddChild( pAction );

		if( pWnd ) pWnd->SendMessage( UM_ACTION_PANE_ADD, (WPARAM)pAction );
		pAction->Activate();
	}
	fclose( fp );

	pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );

	pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );
}



#ifdef _CHECK_WALKFRONT
void CGlobalValue::GetActFileListFromFolder(const std::string& folderName, std::vector<std::string>& fileList) const
{
	HANDLE hFile;
	WIN32_FIND_DATA fd;
	CHAR str[256];

	wsprintf(str, "%s*.*", folderName.c_str());
	hFile = FindFirstFile(str, &fd);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	while (FindNextFile(hFile, &fd))
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0)
		{
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				if (strcmp(fd.cFileName, ".."))
				{
					wsprintf(str, "%s%s\\", folderName.c_str(), fd.cFileName);

					GetActFileListFromFolder(str, fileList);
				}
			}
			else
			{
				std::string fileNameWrapper(fd.cFileName);
				std::string ext;
				std::string::size_type offSet;
				offSet = fileNameWrapper.find_last_of(".");
				if (offSet != std::string::npos)
					ext = fileNameWrapper.substr(offSet, std::string::npos);
				if (ext.compare(".act") == 0)
				{
					CHAR fileNameWithPath[MAX_PATH];
					wsprintf(fileNameWithPath, "%s%s", folderName.c_str(), fd.cFileName);
					fileList.push_back(fileNameWithPath);
				}
			}
		}
	}

	FindClose(hFile);
}

bool CGlobalValue::CheckActionHasWalkFront() const
{
	clock_t before;
	double result;
	before = clock();

	time_t cur;
	struct tm timeData;
	char buf[32] = {0};

	cur = time(NULL);
	localtime_s(&timeData, &cur);

	strftime(buf, sizeof(buf), "%y%m%d_%H%M%S", &timeData);

	std::vector<std::string> fileList, noWalkFrontActionNames;
	std::string resourceFolderWrapper(CRenderBase::GetInstance().GetResourceFolder()), folderNameTemp;

	std::vector<std::string> checkFolderList;
	checkFolderList.push_back("Monster");

	std::vector<std::string>::const_iterator checkFolderIter = checkFolderList.begin();
	for (; checkFolderIter != checkFolderList.end(); ++checkFolderIter)
	{
		const std::string& curFolderName = (*checkFolderIter);
		folderNameTemp = FormatA("%s\\Char\\%s\\", resourceFolderWrapper.c_str(), curFolderName.c_str());
		GetActFileListFromFolder(folderNameTemp.c_str(), fileList);
	}

	std::vector<std::string>::const_iterator iter = fileList.begin();
	for (; iter != fileList.end(); ++iter)
	{
		const std::string& fileName = (*iter);
		CActionObject testObj;
		if (testObj.CheckActionExistWithLoadFile(fileName.c_str(), "Walk_Front") == CActionObject::eERROR_NOACTION &&
			testObj.CheckActionExistWithLoadFile(fileName.c_str(), "Stand") == CActionObject::eERROR_NOACTION)
			noWalkFrontActionNames.push_back(fileName);
	}

	FILE *fp;
	std::string resultFileName;
	resultFileName = FormatA("d:\\CheckWalkFrontAction_%s.txt", buf);
	fopen_s(&fp, resultFileName.c_str(), "wt");
	std::vector<std::string>::const_iterator noWalkIter = noWalkFrontActionNames.begin();
	for (; noWalkIter != noWalkFrontActionNames.end(); ++noWalkIter)
	{
		const std::string& noWalkString = (*noWalkIter);
		fprintf(fp, "%s\n", noWalkString.c_str());
	}
	fclose(fp);

	result = (double)(clock() - before) / CLOCKS_PER_SEC;

	std::string checkResult = FormatA("üũ �Ϸ�.\n%s���Ͽ� ����� ����Ǿ����ϴ�.\n�ɸ��ð�:%d��", resultFileName.c_str(), (int)result);
	MessageBox( GetView()->m_hWnd, checkResult.c_str(), "�Ϸ�", MB_OK );

	return true;
}
#endif
