// EtViewerDoc.cpp : CEtViewerDoc 클래스의 구현
//

#include "stdafx.h"
#include "EtViewer.h"

#include "EtViewerDoc.h"
#include "GlobalValue.h"
#include "RenderBase.h"
#include "MainFrm.h"
#include "PaneDefine.h"
#include "UserMessage.h"
#include "SkinPrevDlg.h"
#include "EtResourceMng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEtViewerDoc

IMPLEMENT_DYNCREATE(CEtViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CEtViewerDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CEtViewerDoc::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CEtViewerDoc::OnFileSave)
	ON_COMMAND(ID_FILE_NEW, &CEtViewerDoc::OnFileNew)
	ON_COMMAND(ID_FILE_EXPORTSCENE, &CEtViewerDoc::OnFileExportscene)
END_MESSAGE_MAP()


// CEtViewerDoc 생성/소멸

CEtViewerDoc::CEtViewerDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CEtViewerDoc::~CEtViewerDoc()
{
}

BOOL CEtViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CEtViewerDoc serialization

void CEtViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
		CFile *pFile = ar.GetFile();
		CString szFileName = pFile->GetFilePath();

		char szTemp[512]={0,}, szExt[32] = {0,}, szPath[512] = {0,};
		_GetExt( szExt, _countof(szExt), szFileName );
		_GetFullFileName( szTemp, _countof(szTemp), szFileName );
		_GetPath( szPath, _countof(szTemp), szFileName );

		SetCurrentDirectory( szPath );
		CEtResourceMng::GetInstance().AddResourcePath( szPath );

		if( _stricmp( szExt, "skn" ) == NULL ) {
			if( CGlobalValue::GetInstance().GetView() == NULL ) {
				CGlobalValue::GetInstance().m_szVecInitLoadFile = szTemp;
				return;
			}

//			CGlobalValue::GetInstance().ResetObject();
			CObjectBase *pParent = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
			if( pParent == NULL ) return;

			CObjectBase *pBase = new CObjectSkin;
			pBase->SetName( CString(szTemp) );
			((CObjectSkin*)pBase)->LoadSkin( szFileName, true );

			CGlobalValue::GetInstance().AddObject( pParent->GetName(), pBase );

			DWORD dwAttr = GetFileAttributes( szFileName );
			if( dwAttr & FILE_ATTRIBUTE_READONLY ) {
				CString szStr;
				szStr.Format( "다음 파일들이 읽기전용 속성입니다.\n\n%s", szFileName );
				MessageBox( ((CMainFrame*)AfxGetMainWnd())->m_hWnd, szStr, "경고", MB_OK );
			}
		}
		else if( _stricmp( szExt, "ani" ) == NULL ) {
			int nCount = CGlobalValue::GetInstance().GetObjectCount( CObjectBase::SKIN );
			if( nCount != 1 ) return;
			CObjectBase *pParent = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SKIN );

			CObjectBase *pBase = new CObjectAnimation;
			if( ((CObjectAnimation*)pBase)->LoadAnimation( szFileName ) == false ) {
				SAFE_DELETE( pBase );
			}
			else 
				CGlobalValue::GetInstance().AddObject( pParent->GetName(), pBase );
		}
		else if( _stricmp( szExt, "scn" ) == NULL ) {
			CGlobalValue::GetInstance().ImportScene( CString(szTemp) );
		}
	}
}


// CEtViewerDoc 진단

#ifdef _DEBUG
void CEtViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEtViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEtViewerDoc 명령

void CEtViewerDoc::OnFileOpen()
{
	CString szFileList;
	TCHAR szFilter[] = _T( "Eternity Support File (*.skn;*.ani;*.scn;*.txt)|*.skn;*.ani;*.scn;*.txt|Eternity Skin File (*.skn)|*.skn|Eternity Animation File (*.ani)|*.ani|Eternity Scene File (*.scn)|*.scn|Eternity Simulation File (*.txt)|*.txt|All Files (*.*)|*.*||" );
	CSkinPrevDlg dlg( TRUE, _T("skn;ani;scn;txt"), _T("*.skn;*.ani;*.scn;*.txt"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT| OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );
	dlg.GetOFN().lpstrFile = szFileList.GetBuffer(100000);
	dlg.GetOFN().nMaxFile = 100000;

	dlg.m_ofn.lpstrTitle = "Open Eternity Engine File";

	if( IDOK != dlg.DoModal() ) {
		szFileList.ReleaseBuffer();
		return;
	}

	// 파일 오픈시 규칙
	// Scene 이 비어있을 경우 
	//   skn만 골랐을 경우 skn 로드
	//   skn하나 & ani 한개 이상 골랐을 경우 로드
	//   skn여러게에 ani 한개 이상 골랐을 경우 실패
	//   ani만 골랐을 경우 실패
	// Scene 에 skn하나 있을 경우
	//   ani 만 골랐을 경우 로드
	//   skn 골랐을 경우 Scene reset 후 로드
	//   skn하나 & ani 한개 이상 골랐을 Scene Reset 후 로드

	std::vector<CString> szVecSkinList;
	std::vector<CString> szVecAniList;
	std::vector<CString> szVecSceneList;
	std::vector<CString> szVecSimList;
	CGlobalValue::GetInstance().ParseFileList( szFileList, "skn", szVecSkinList );
	CGlobalValue::GetInstance().ParseFileList( szFileList, "ani", szVecAniList );
	CGlobalValue::GetInstance().ParseFileList( szFileList, "scn", szVecSceneList );
	CGlobalValue::GetInstance().ParseFileList( szFileList, "txt", szVecSimList );
	szFileList.ReleaseBuffer();

	bool bResetScene = true;
	if( szVecSceneList.size() > 0 ) {
		bResetScene = false;
		CGlobalValue::GetInstance().ImportScene( szVecSceneList[0] );

	}
	if( szVecSkinList.size() == 0 ) {
		if( szVecSimList.size() > 0)
		{
			CObjectSkin *pBase = (CObjectSkin*)CGlobalValue::GetInstance().GetOneObject( CObjectBase::SKIN );
			if( pBase ) {
				char szTemp[512];
				strcpy_s( szTemp, szVecSimList[0].GetBuffer() );
				pBase->LoadSimulation( szTemp );
			}
		}
		if( szVecAniList.size() > 0 ) {
			if( CGlobalValue::GetInstance().GetObjectCount( CObjectBase::SKIN ) != 1 ) return;

			CObjectBase *pBase = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SKIN );

			float fPer = 100 / (float)szVecAniList.size() + 1.f;
			CGlobalValue::GetInstance().UpdateProgress( 0 );

			// 에뉘 로딩
			for( DWORD i=0; i<szVecAniList.size(); i++ ) {
				char szTemp[512];
				_GetFullFileName( szTemp, _countof(szTemp), szVecAniList[i] );
				CObjectBase *pBaseAni = new CObjectAnimation;

				if( ((CObjectAnimation*)pBaseAni)->LoadAnimation( szVecAniList[i] ) == false ) {
					SAFE_DELETE( pBaseAni );
					continue;
				}

				CGlobalValue::GetInstance().AddObject( pBase->GetTreeItemID(), pBaseAni );

				CGlobalValue::GetInstance().UpdateProgress( (int)(fPer*(i+1)) );
			}
			CGlobalValue::GetInstance().UpdateProgress(0);
		}
	}
	else if( szVecSkinList.size() == 1 ) {
		if( bResetScene ) CGlobalValue::GetInstance().ResetObject();

		float fPer = 100 / (float)szVecAniList.size() + 1.f;
		CGlobalValue::GetInstance().UpdateProgress( 0 );

		char szTemp[512];
		_GetFullFileName( szTemp, _countof(szTemp), szVecSkinList[0] );
		CObjectBase *pBase = new CObjectSkin;
		pBase->SetName( CString(szTemp) );
		((CObjectSkin*)pBase)->LoadSkin( szVecSkinList[0], true );

		DWORD dwAttr = GetFileAttributes( szVecSkinList[0] );
		if( dwAttr & FILE_ATTRIBUTE_READONLY ) {
			CString szStr;
			szStr.Format( "다음 파일들이 읽기전용 속성입니다.\n\n%s", szVecSkinList[0] );
			MessageBox( ((CMainFrame*)AfxGetMainWnd())->m_hWnd, szStr, "경고", MB_OK );
		}

		CObjectBase *pParent = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
		CGlobalValue::GetInstance().AddObject( pParent, pBase );

		CGlobalValue::GetInstance().UpdateProgress( (int)(fPer*1.f) );
		// 에뉘 로딩
		for( DWORD i=0; i<szVecAniList.size(); i++ ) {
			char szTemp[512];
			_GetFullFileName( szTemp, _countof(szTemp), szVecAniList[i] );
			CObjectBase *pBaseAni = new CObjectAnimation;

			if( ((CObjectAnimation*)pBaseAni)->LoadAnimation( szVecAniList[i] ) == false ) {
				SAFE_DELETE( pBaseAni );
				continue;
			}

			CGlobalValue::GetInstance().AddObject( pBase->GetTreeItemID(), pBaseAni );

			CGlobalValue::GetInstance().UpdateProgress( (int)(fPer*(i+2)) );
		}
		CGlobalValue::GetInstance().UpdateProgress( 0 );
	}
	else if( szVecSkinList.size() > 1 ) {
		if( szVecAniList.size() > 0 ) return;

		if( bResetScene ) CGlobalValue::GetInstance().ResetObject();

		float fPer = 100 / (float)szVecSkinList.size() + 1.f;
		CGlobalValue::GetInstance().UpdateProgress( 0 );
		for( DWORD i=0; i<szVecSkinList.size(); i++ ) {
			char szTemp[512];
			_GetFullFileName( szTemp, _countof(szTemp), szVecSkinList[i] );
			CObjectBase *pBase = new CObjectSkin;
			pBase->SetName( CString(szTemp) );
			((CObjectSkin*)pBase)->LoadSkin( szVecSkinList[i], true );

			CObjectBase *pParent = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
			CGlobalValue::GetInstance().AddObject( pParent, pBase );

			CGlobalValue::GetInstance().UpdateProgress( (int)(fPer*(i+1)) );
		}
		CGlobalValue::GetInstance().UpdateProgress( 0 );
	}
}

void CEtViewerDoc::OnFileSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CObjectBase *pBase = CGlobalValue::GetInstance().GetRootObject();
	if( pBase->IsChildModify() == true ) {
		CGlobalValue::GetInstance().SaveSkinObject( pBase );
	}
}

void CEtViewerDoc::OnFileNew()
{
	CDocument::OnNewDocument();
	CGlobalValue::GetInstance().ResetObject();
}

void CEtViewerDoc::OnFileExportscene()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFileList;
	TCHAR szFilter[] = _T( "Eternity Scene File (*.scn)|*.scn|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("scn"), _T("*.scn"), OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	dlg.m_ofn.lpstrTitle = "Save Eternity Scene File";

	if( IDOK != dlg.DoModal() ) return;

	CGlobalValue::GetInstance().ExportScene( dlg.GetPathName() );
}

