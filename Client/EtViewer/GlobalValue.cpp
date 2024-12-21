#include "StdAfx.h"
#include "GlobalValue.h"

#include "MainFrm.h"
#include "resource.h"
#include "EtViewerDoc.h"
#include "EtViewerView.h"
#include "PaneDefine.h"
#include "UserMessage.h"

CGlobalValue g_GlobalValue;


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

CGlobalValue::CGlobalValue()
{
	m_pActiveView = NULL;
	m_pObjectRoot = new CObjectBase;
	m_pControlObject = NULL;
	m_bFullScreen = false;
}

CGlobalValue::~CGlobalValue()
{
	Finalize();
}

void CGlobalValue::Finalize()
{
	ResetObject();
	SAFE_DELETE( m_pObjectRoot );
}

CString CGlobalValue::GetUniqueString( CObjectBase::OBJECT_TYPE Type )
{
	char szStr[512];
	int nCount = 0;
	switch( Type ) {
		case CObjectBase::UNKNOWN:
			while(1) {
				sprintf_s( szStr, "Unknown #%02d", nCount );
				if( IsSameName( Type, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CObjectBase::SCENE:
			while(1) {
				sprintf_s( szStr, "Scene #%02d", nCount );
				if( IsSameName( Type, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CObjectBase::SKIN:
			while(1) {
				sprintf_s( szStr, "RenderObject #%02d", nCount );
				if( IsSameName( Type, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CObjectBase::SUBMESH:
			while(1) {
				sprintf_s( szStr, "Geometry #%02d", nCount );
				if( IsSameName( Type, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CObjectBase::ANIMATION:
			while(1) {
				sprintf_s( szStr, "Animation #%02d", nCount );
				if( IsSameName( Type, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CObjectBase::LIGHTDIR:
			while(1) {
				sprintf_s( szStr, "Directional #%02d", nCount );
				if( IsSameName( Type, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CObjectBase::LIGHTPOINT:
			while(1) {
				sprintf_s( szStr, "Point #%02d", nCount );
				if( IsSameName( Type, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CObjectBase::LIGHTSPOT:
			while(1) {
				sprintf_s( szStr, "Spot #%02d", nCount );
				if( IsSameName( Type, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			break;
		case CObjectBase::LIGHTFOLDER:
			sprintf_s( szStr, "Light Folder" );
			/*
			while(1) {
				sprintf_s( szStr, "Light #%02d", nCount );
				if( IsSameName( Type, CString(szStr) ) == FALSE ) break;
				else nCount++;
			}
			*/
			break;
	}
	return CString(szStr);
}

BOOL CGlobalValue::IsSameName( CObjectBase::OBJECT_TYPE Type, CString &szUniqueName )
{
	CObjectBase *pBase = FindObject( m_pObjectRoot, szUniqueName );
	if( pBase == NULL ) return FALSE;
	return TRUE;
}



void CGlobalValue::ResetObject( bool bCreateDefaultObject )
{
	if( m_pObjectRoot == NULL ) return;
	CWnd *pWnd = GetPaneWnd( ANI_PANE );
	if( pWnd ) pWnd->SendMessage( UM_ANICONTROL_REFRESH );
	pWnd = GetPaneWnd( PROP_SHADER_PANE );
	if( pWnd ) pWnd->SendMessage( UM_SHADERPROP_REFRESH );

	SaveLightSetting( CString("") );
	for( DWORD i=0; i<m_pObjectRoot->GetChildCount(); i++ ) {
		CGlobalValue::GetInstance().RemoveObjectFromTreeID( m_pObjectRoot->GetChild(i)->GetTreeItemID() );
		i--;
	}

	if( bCreateDefaultObject ) {
		// Default - Scene 1���� Light Folder & Directional Light 1����!
		CObjectBase *pBase = new CObjectScene;
		AddObject( CString(), pBase );
		CString szParentName = pBase->GetName();

		pBase = new CObjectLightFolder;
		AddObject( szParentName, pBase );

		LoadLightSetting( CString("") );

		if( !m_szVecInitLoadFile.IsEmpty() ) {
			CObjectBase *pParent = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
			if( pParent == NULL ) return;

			CObjectBase *pBase = new CObjectSkin;
			pBase->SetName( m_szVecInitLoadFile );
			((CObjectSkin*)pBase)->LoadSkin( m_szVecInitLoadFile, true );

			CGlobalValue::GetInstance().AddObject( pParent->GetName(), pBase );

			m_szVecInitLoadFile.Empty();
		}
		m_pControlObject = NULL;
	}

	CString szStr;
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::SKIN, szStr );
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::ANIMATION, szStr );
	GetView()->GetDocument()->SetTitle( szStr );
}

void CGlobalValue::RemoveObject( CString &szDescription )
{
	CObjectBase *pBase = FindObject( m_pObjectRoot, szDescription );
	if( pBase == NULL ) return;
	if( pBase->GetParent() ) {
		CWnd *pWnd = GetPaneWnd( WORKSPACE_PANE );
		pWnd->SendMessage( UM_WORKSPACEPANE_REMOVE, (WPARAM)pBase );

		pBase->GetParent()->RemoveChild( szDescription );
	}

	CString szStr;
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::SKIN, szStr );
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::ANIMATION, szStr );
	GetView()->GetDocument()->SetTitle( szStr );
}

void CGlobalValue::RemoveObjectFromTreeID( HTREEITEM hti )
{
	CObjectBase *pBase = FindObject( m_pObjectRoot, hti );
	if( pBase == NULL ) return;
	if( pBase->GetParent() ) {
		CWnd *pWnd = GetPaneWnd( WORKSPACE_PANE );
		pWnd->SendMessage( UM_WORKSPACEPANE_REMOVE, (WPARAM)pBase );

		pBase->GetParent()->RemoveChild( pBase );
	}

	CString szStr;
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::SKIN, szStr );
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::ANIMATION, szStr );
	GetView()->GetDocument()->SetTitle( szStr );

}

CObjectBase *CGlobalValue::GetObjectFromDescription( CString  &szDescription )
{
	CObjectBase *pBase = FindObject( m_pObjectRoot, szDescription );
	return pBase;
}

CObjectBase *CGlobalValue::GetObjectFromTreeID( HTREEITEM hti )
{
	CObjectBase *pBase = FindObject( m_pObjectRoot, hti );
	return pBase;
}

CObjectBase::OBJECT_TYPE CGlobalValue::GetObjectTypeFromDescription( CString &szDescription )
{
	CObjectBase *pBase = GetObjectFromDescription( szDescription );
	if( pBase == NULL ) return CObjectBase::UNKNOWN;

	return pBase->GetType();
}

CObjectBase *CGlobalValue::FindObject( CObjectBase *pObject, CString &szDescription )
{
	if( pObject->GetName() == szDescription ) return pObject;
	for( DWORD i=0; i<pObject->GetChildCount(); i++ ) {
		CObjectBase *pChild = FindObject( pObject->GetChild(i), szDescription );
		if( pChild != NULL ) return pChild;
	}
	return NULL;
}

CObjectBase *CGlobalValue::FindObject( CObjectBase *pObject, HTREEITEM hti )
{
	if( pObject->GetTreeItemID() == hti ) return pObject;
	for( DWORD i=0; i<pObject->GetChildCount(); i++ ) {
		CObjectBase *pChild = FindObject( pObject->GetChild(i), hti );
		if( pChild != NULL ) return pChild;
	}
	return NULL;
}

CObjectBase *CGlobalValue::FindObject( CObjectBase *pObject, CObjectBase::OBJECT_TYPE Type )
{
	if( pObject->GetType() == Type ) return pObject;
	for( DWORD i=0; i<pObject->GetChildCount(); i++ ) {
		CObjectBase *pChild = FindObject( pObject->GetChild(i), Type );
		if( pChild != NULL ) return pChild;
	}
	return NULL;
}

BOOL CGlobalValue::AddObject( CObjectBase *pParent, CObjectBase *pObject )
{
	if( pObject->GetName().IsEmpty() ) {
		pObject->SetName( GetUniqueString( pObject->GetType() ) );
	}
	if( pParent == NULL ) {
		m_pObjectRoot->AddChild( pObject );
		pObject->SetParent( m_pObjectRoot );
	}
	else {
		CObjectBase *pBase = FindObject( m_pObjectRoot, pParent->GetTreeItemID() );
		if( pBase == NULL ) return FALSE;

		pBase->AddChild( pObject );
		pObject->SetParent( pBase );
	}

	CWnd *pWnd = GetPaneWnd( WORKSPACE_PANE );
	pWnd->SendMessage( UM_WORKSPACEPANE_ADD, (WPARAM)pObject );

	pObject->Activate();

	CString szStr;
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::SKIN, szStr );
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::ANIMATION, szStr );
	GetView()->GetDocument()->SetTitle( szStr );

	return TRUE;
}

BOOL CGlobalValue::AddObject( HTREEITEM hParent, CObjectBase *pObject )
{
	if( pObject->GetName().IsEmpty() ) {
		pObject->SetName( GetUniqueString( pObject->GetType() ) );
	}
	if( hParent == NULL ) {
		m_pObjectRoot->AddChild( pObject );
		pObject->SetParent( m_pObjectRoot );
	}
	else {
		CObjectBase *pBase = FindObject( m_pObjectRoot, hParent );
		if( pBase == NULL ) return FALSE;

		pBase->AddChild( pObject );
		pObject->SetParent( pBase );
	}

	CWnd *pWnd = GetPaneWnd( WORKSPACE_PANE );
	pWnd->SendMessage( UM_WORKSPACEPANE_ADD, (WPARAM)pObject );

	pObject->Activate();

	CString szStr;
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::SKIN, szStr );
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::ANIMATION, szStr );
	GetView()->GetDocument()->SetTitle( szStr );

	return TRUE;
}

BOOL CGlobalValue::AddObject( CString &szParentName, CObjectBase *pObject )
{
	if( pObject->GetName().IsEmpty() ) {
		pObject->SetName( GetUniqueString( pObject->GetType() ) );
	}
	if( szParentName.IsEmpty() ) {
		m_pObjectRoot->AddChild( pObject );
		pObject->SetParent( m_pObjectRoot );
	}
	else {
		CObjectBase *pBase = FindObject( m_pObjectRoot, szParentName );
		if( pBase == NULL ) return FALSE;

		pBase->AddChild( pObject );
		pObject->SetParent( pBase );
	}

	CWnd *pWnd = GetPaneWnd( WORKSPACE_PANE );
	pWnd->SendMessage( UM_WORKSPACEPANE_ADD, (WPARAM)pObject );

	pObject->Activate();

	CString szStr;
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::SKIN, szStr );
	CGlobalValue::GetInstance().GetObjectName( CObjectBase::ANIMATION, szStr );
	GetView()->GetDocument()->SetTitle( szStr );

	return TRUE;
}

void CGlobalValue::ParseFileList( CString &szFileBuffer, char *szImportExt, std::vector<CString> &szVecList )
{
	char szBuffer[1024], szExt[256];
	CString szFolder;
	std::vector<CString> szVecAniList;


	int nBeginPos = 0;
	int nEndPos = 0;
	char cPrevChar, cCurChar;
	bool bFirstFlag = false;
	bool bSecondFlag = false;

	for( DWORD i=1;; i++ ) {
		nEndPos = i;
		cCurChar = szFileBuffer.GetBuffer()[i];
		cPrevChar = szFileBuffer.GetBuffer()[i-1];
		if( cCurChar == 0 && cPrevChar != 0 ) {
			strncpy_s( szBuffer, szFileBuffer.GetBuffer() + nBeginPos, nEndPos - nBeginPos );
			nBeginPos = i+1;

			if( bFirstFlag == false ) {	// ù��°�� ����
				bFirstFlag = true;
				szFolder = szBuffer;
			}
			else { // ������ ���ϸ���Ʈ
				bSecondFlag = true;
				_GetExt( szExt, _countof(szExt), szBuffer );
				if( _stricmp( szExt, szImportExt ) == NULL ) {
					szVecList.push_back( szFolder + "\\" + szBuffer );
				}
			}
		}
		else if( cCurChar == 0 && cPrevChar == 0 ) break;
	}

	if( bSecondFlag == false ) {
		_GetExt( szExt, _countof(szExt), szFolder );
		if( _stricmp( szExt, szImportExt ) == NULL ) {
			szVecList.push_back( szFolder );
		}
	}
}


int CGlobalValue::GetObjectCount( CObjectBase::OBJECT_TYPE Type )
{
	int nCount = 0;
	CountObject( Type, m_pObjectRoot, nCount );
	return nCount;
}

CObjectBase *CGlobalValue::GetOneObject( CObjectBase::OBJECT_TYPE Type )
{
	int nCount = 0;
	CountObject( Type, m_pObjectRoot, nCount );
	if( nCount != 1 ) return NULL;

	return FindObject( m_pObjectRoot, Type );
}

void CGlobalValue::GetObjectName( CObjectBase::OBJECT_TYPE Type, CString &szStr, CObjectBase *pBase )
{
	if( pBase == NULL ) pBase = m_pObjectRoot;
	if( pBase->GetType() == Type ) {
		if( Type == CObjectBase::ANIMATION ) {
			char szTemp[512] = { 0, };
			sscanf_s( pBase->GetName(), "%s",  szTemp, sizeof(szTemp) );
			szStr += szTemp;
		}
		else szStr += pBase->GetName();

		szStr += ", ";
	}
	for( DWORD i=0; i<pBase->GetChildCount(); i++ ) {
		GetObjectName( Type, szStr, pBase->GetChild(i) );
	}
}

void CGlobalValue::CountObject( CObjectBase::OBJECT_TYPE Type, CObjectBase *pObject, int &nCount )
{
	if( pObject->GetType() == Type ) nCount++;
	for( DWORD i=0; i<pObject->GetChildCount(); i++ ) {
		CountObject( Type, pObject->GetChild(i), nCount );
	}
}

void CGlobalValue::UpdateProgress( int nPer )
{
	((CMainFrame*)AfxGetMainWnd())->UpdateProgress( nPer );
}

void CGlobalValue::SaveSkinObject( CObjectBase *pObject, const char *szNewFileName )
{
	if( pObject->GetType() == CObjectBase::SKIN ) {
		((CObjectSkin*)pObject)->SaveSkin( ( szNewFileName == NULL ) ? CString("") : szNewFileName );
	}
	for( DWORD i=0; i<pObject->GetChildCount(); i++ ) {
		SaveSkinObject( pObject->GetChild(i), szNewFileName );
	}
}

void CGlobalValue::SaveLightSetting( CString szFileName )
{
	CObjectLightFolder *pFolder = (CObjectLightFolder *)GetOneObject( CObjectBase::LIGHTFOLDER );
	if( pFolder == NULL ) return;
	pFolder->SaveLightSetting( szFileName );
}

void CGlobalValue::LoadLightSetting( CString szFileName )
{
	CObjectLightFolder *pFolder = (CObjectLightFolder *)GetOneObject( CObjectBase::LIGHTFOLDER );
	if( pFolder == NULL ) return;
	pFolder->LoadLightSetting( szFileName );
}

bool CGlobalValue::ExportScene( CString szFileName )
{
	if( m_pObjectRoot == NULL ) return false;
	FILE *fp;
	fopen_s( &fp, szFileName.GetBuffer(), "wb" );
	if( fp == NULL ) return false;
	SceneHeader Header;

	fwrite( &Header, sizeof(SceneHeader), 1, fp );

	int nObjectCount = 0;

	m_pObjectRoot->ExportObject( fp, nObjectCount );

	// ��ũ���� �߰� ����.
	int nLinkCount = 0;
	std::vector< CString > vecString;
	CString szLinkObject, szParentObject, szBoneName;
	char szTemp[256];
	CObjectBase *pRoot = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
	for( DWORD i=0; i<pRoot->GetChildCount(); i++ )
	{
		CObjectSkin *pSkin = dynamic_cast<CObjectSkin *>(pRoot->GetChild(i));
		if( !pSkin ) continue;

		EtAniObjectHandle hHandle = pSkin->GetObjectHandle();
		if( !hHandle ) continue;
		EtAniObjectHandle hParentHandle = hHandle->GetParent();
		if( !hParentHandle ) continue;
		if( !hParentHandle->GetAniHandle() ) continue;
		if( hParentHandle->GetAniHandle()->GetBoneCount() < 1 ) continue;

		_GetFileName( szTemp, _countof(szTemp), pSkin->GetFileName() );
		szLinkObject = szTemp;
		_GetFileName( szTemp, _countof(szTemp), hHandle->GetParent()->GetSkinFileName() );
		szParentObject = szTemp;

		szBoneName = "";
		if( hHandle->GetLinkBoneIndex() != -1 )
		{
			if( pSkin->GetSkinType() == CObjectSkin::SKIN_EARRING || pSkin->GetSkinType() == CObjectSkin::SKIN_WING || pSkin->GetSkinType() == CObjectSkin::SKIN_TAIL )
			{
				switch( pSkin->GetSkinType() )
				{
				case CObjectSkin::SKIN_EARRING: szBoneName = "#Cash_EarRing"; break;
				case CObjectSkin::SKIN_WING: szBoneName = "#Cash_Wing"; break;
				case CObjectSkin::SKIN_TAIL: szBoneName = "#Cash_Tail"; break;
				}
			}
			else
			{
				CEtBone *pBone = hParentHandle->GetAniHandle()->GetBone( hHandle->GetLinkBoneIndex() );
				if( pBone )
					szBoneName = pBone->GetName();
			}
		}

		vecString.push_back( szLinkObject );
		vecString.push_back( szParentObject );
		vecString.push_back( szBoneName );
		++nLinkCount;
	}
	fwrite( &nLinkCount, sizeof(int), 1, fp );
	for( int i = 0; i < nLinkCount; ++i )
	{
		WriteCString( &vecString[i*3+0], fp );
		WriteCString( &vecString[i*3+1], fp );
		WriteCString( &vecString[i*3+2], fp );
	}

	Header.nObjectCount = nObjectCount;
	fseek( fp, 0, SEEK_SET );
	fwrite( &Header, sizeof(SceneHeader), 1, fp );

	fclose(fp);
	return true;
}

bool CGlobalValue::ImportScene( CString szFileName )
{
	FILE *fp;
	fopen_s( &fp, szFileName.GetBuffer(), "rb" );
	if( fp == NULL ) return false;

	SceneHeader Header;
	fread( &Header, sizeof(SceneHeader), 1, fp );
	if( strcmp( Header.szStr, "TdScene" ) != NULL ) {
		fclose(fp);
		return false;
	}
	if( Header.nVersion != 1 ) return false;

	CGlobalValue::GetInstance().ResetObject( false );

	CObjectScene *pLastScene = NULL;
	CObjectLightFolder *pLastLightFolder = NULL;
	CObjectSkin *pLastSkin = NULL;
	for( int i=0; i<Header.nObjectCount; i++ ) {
		int nType;
		fread( &nType, sizeof(int), 1, fp );
		switch( nType ) {
			case CObjectBase::SCENE:
				{
					CObjectScene *pScene = new CObjectScene;
					AddObject( CString(""), pScene );
					pScene->ImportObject( fp );
					pLastScene = pScene;
				}
				break;
			case CObjectBase::LIGHTFOLDER:
				{
					CObjectLightFolder *pFolder = new CObjectLightFolder;
					AddObject( pLastScene->GetTreeItemID(), pFolder );

					pFolder->ImportObject( fp );
					pLastLightFolder = pFolder;
				}
				break;
			case CObjectBase::LIGHTDIR:
				{
					CObjectLightDir *pLight = new CObjectLightDir;
					AddObject( pLastLightFolder->GetTreeItemID(), pLight );

					pLight->ImportObject( fp );
				}
				break;
			case CObjectBase::LIGHTPOINT:
				{
					CObjectLightPoint *pLight = new CObjectLightPoint;
					AddObject( pLastLightFolder->GetTreeItemID(), pLight );

					pLight->ImportObject( fp );
				}
				break;
			case CObjectBase::LIGHTSPOT:
				{
					CObjectLightSpot *pLight = new CObjectLightSpot;
					AddObject( pLastLightFolder->GetTreeItemID(), pLight );

					pLight->ImportObject( fp );
				}
				break;
			case CObjectBase::SKIN:
				{
					CObjectSkin *pSkin = new CObjectSkin;

					pSkin->ImportObject( fp );
					char szTemp[512];
					_GetFullFileName( szTemp, _countof(szTemp), pSkin->GetFileName() );
					pSkin->SetName( CString(szTemp) );

					AddObject( pLastScene->GetTreeItemID(), pSkin );
					pSkin->Activate();
					pLastSkin = pSkin;
				}
				break;
			case CObjectBase::ANIMATION:
				{
					CObjectAnimation *pAni = new CObjectAnimation;

					pAni->ImportObject( fp );
					bool bParentSkin = pAni->IsParentTypeSkin();
					if( bParentSkin && pLastSkin )
						AddObject( pLastSkin->GetTreeItemID(), pAni );
					else if( bParentSkin == false && pLastScene )
						AddObject( pLastScene->GetTreeItemID(), pAni );
//					pAni->Activate();

					pAni->Stop();
					pAni->Resume();
					pAni->Play( pAni->GetLoadedCurAniIndex() );
					pAni->Pause();
					pAni->SetCurFrame( pAni->GetLoadedCurFrame() );
				}
				break;
		}
	}

	// ��ũ���� �߰� �ε�
	int nLinkCount = 0;
	fread( &nLinkCount, sizeof(int), 1, fp );
	CString szLinkObject, szParentObject, szBoneName;
	char szTemp[256];

	CObjectBase *pRoot = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
	CObjectSkin *m_pSelectSkin;
	CObjectSkin *m_pTargetSkin;
	for( int j = 0; j < nLinkCount; ++j )
	{
		ReadCString( &szLinkObject, fp );
		ReadCString( &szParentObject, fp );
		ReadCString( &szBoneName, fp );
		m_pSelectSkin = m_pTargetSkin = NULL;

		for( DWORD i=0; i<pRoot->GetChildCount(); i++ )
		{
			CObjectSkin *pSkin = dynamic_cast<CObjectSkin *>(pRoot->GetChild(i));
			if( !pSkin ) continue;
			EtAniObjectHandle hHandle = pSkin->GetObjectHandle();
			if( !hHandle ) continue;

			_GetFileName( szTemp, _countof(szTemp), pSkin->GetFileName() );
			std::string szTempLower = szTemp;
			ToLowerA( szTempLower );

			if( strcmp( szLinkObject.GetBuffer(), szTemp ) == NULL )
				m_pSelectSkin = pSkin;

			if( strcmp( szParentObject.GetBuffer(), szTempLower.c_str() ) == NULL )
				m_pTargetSkin = pSkin;
		}

		if( m_pSelectSkin && m_pTargetSkin )
		{
			EtAniObjectHandle hParentHandle = m_pTargetSkin->GetObjectHandle();
			if( !hParentHandle ) continue;
			if( !hParentHandle->GetAniHandle() ) continue;
			if( hParentHandle->GetAniHandle()->GetBoneCount() < 1 ) continue;

			if( m_pSelectSkin->GetSkinType() == CObjectSkin::SKIN_EARRING || m_pSelectSkin->GetSkinType() == CObjectSkin::SKIN_WING || m_pSelectSkin->GetSkinType() == CObjectSkin::SKIN_TAIL )
			{
				m_pSelectSkin->GetObjectHandle()->SetParent( m_pTargetSkin->GetObjectHandle(), szBoneName );
			}
			else
			{
				int nBoneIndex = -1;
				if( szBoneName.GetLength() > 0 ) {
					nBoneIndex = hParentHandle->GetAniHandle()->GetBoneIndex( szBoneName.GetBuffer() );
				}
				m_pSelectSkin->GetObjectHandle()->SetParent( m_pTargetSkin->GetObjectHandle(), nBoneIndex );
			}
		}
	}

	fclose(fp);

	return true;
}
