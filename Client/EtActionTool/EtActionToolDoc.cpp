// EtActionToolDoc.cpp : CEtActionToolDoc 클래스의 구현
//

#include "stdafx.h"
#include "EtActionTool.h"

#include "EtActionToolDoc.h"
#include "GlobalValue.h"
#include "SkinPrevDlg.h"
#include "SignalReportDlg.h"

#include "SignalManager.h"
#include "ActionSignal.h"
#include "SignalItem.h"
#include "ActionElement.h"
#include "MainFrm.h"
#include "PaneDefine.h"
#include "ActionPaneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEtActionToolDoc

IMPLEMENT_DYNCREATE(CEtActionToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CEtActionToolDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CEtActionToolDoc::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CEtActionToolDoc::OnFileSave)
	ON_COMMAND(ID_FILE_NEW, &CEtActionToolDoc::OnFileNew)
	ON_COMMAND(ID_SIGNAL_REPORT, &CEtActionToolDoc::OnSignalReport)
	ON_COMMAND(ID_SAVE_SIGNAL_REPORT, &CEtActionToolDoc::OnSaveSignalReport)
	ON_UPDATE_COMMAND_UI(ID_SIGNAL_REPORT, &CEtActionToolDoc::OnUpdateSignalReport)
	ON_UPDATE_COMMAND_UI(ID_SAVE_SIGNAL_REPORT, &CEtActionToolDoc::OnUpdateSaveSignalReport)
	ON_COMMAND(ID_FILE_EXPORT, &CEtActionToolDoc::OnFileExport)
	ON_COMMAND(ID_FILE_IMPORT, &CEtActionToolDoc::OnFileImport)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, &CEtActionToolDoc::OnUpdateOnFileExport)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, &CEtActionToolDoc::OnUpdateOnFileImport)
END_MESSAGE_MAP()


// CEtActionToolDoc 생성/소멸

CEtActionToolDoc::CEtActionToolDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CEtActionToolDoc::~CEtActionToolDoc()
{
	CGlobalValue::GetInstance().CheckIn();
}

BOOL CEtActionToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CEtActionToolDoc serialization

void CEtActionToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
		/*
		DWORD dwAttr = GetFileAttributes( szFileName );
		if( dwAttr & FILE_ATTRIBUTE_READONLY ) {
			CString szStr;
			szStr.Format( "다음 파일들이 읽기전용 속성입니다.\n\n%s", szFileName );
			MessageBox( ((CMainFrame*)AfxGetMainWnd())->m_hWnd, szStr, "경고", MB_OK );
		}
		*/
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
		CFile *pFile = ar.GetFile();
		CString szFileName = pFile->GetFilePath();

		while(1) {
			CString szError;
			szError = CGlobalValue::GetInstance().IsCanCheckOut( szFileName );
			if( !szError.IsEmpty() ) {
				CString szStr;
				szStr.Format( "다른 사람이 현제 파일을 사용중입니다.\n파일 이름 : %s\n현제 사용자 : %s", szFileName, szError );

				int nResult = MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, szStr, "에러", MB_ABORTRETRYIGNORE );
				bool bStop = false;
				switch( nResult ) {
					case IDABORT: return;
					case IDRETRY: break;
					case IDIGNORE: 
						bStop = true;
						CGlobalValue::GetInstance().IgnoreCheckOut( szFileName );
						break;
				}
				if( bStop == true ) break;
			}
			else break;
		}

		CGlobalValue::GetInstance().CheckIn();
		if( CGlobalValue::GetInstance().CheckOut( szFileName ) == false ) {
			MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, "체크아웃 실페!! 파일을 열 수 없습니다.", "에러", MB_OK );
			return;
		}
		if( CGlobalValue::GetInstance().OpenAction( szFileName ) == false ) {
			CGlobalValue::GetInstance().CheckIn();
		}
	}
}


// CEtActionToolDoc 진단

#ifdef _DEBUG
void CEtActionToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEtActionToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEtActionToolDoc 명령

void CEtActionToolDoc::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TCHAR szFilter[] = _T( "Eternity Support File (*.skn;*.act)|*.skn;*.act|Eternity Skin File (*.skn)|*.skn|Eternity Action File (*.act)|*.act|All Files (*.*)|*.*||" );
	CSkinPrevDlg dlg( TRUE, _T("skn;act"), _T("*.skn;*.act"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	dlg.m_ofn.lpstrTitle = "Open Eternity Engine File";

	if( IDOK != dlg.DoModal() ) return;

	while(1) {
		CString szError;
		szError = CGlobalValue::GetInstance().IsCanCheckOut( dlg.GetPathName() );
		if( !szError.IsEmpty() ) {
			CString szStr;
			szStr.Format( "다른 사람이 현제 파일을 사용중입니다.\n파일 이름 : %s\n현제 사용자 : %s", dlg.GetPathName(), szError );

			int nResult = MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, szStr, "에러", MB_ABORTRETRYIGNORE );
			bool bStop = false;
			switch( nResult ) {
				case IDABORT: return;
				case IDRETRY: break;
				case IDIGNORE:
					CGlobalValue::GetInstance().IgnoreCheckOut( dlg.GetPathName() );
					bStop = true;
					break;
			}
			if( bStop == true ) break;
		}
		else break;
	}


	CGlobalValue::GetInstance().CheckIn();
	if( CGlobalValue::GetInstance().CheckOut( dlg.GetPathName() ) == false ) {
		MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, "체크아웃 실페!! 파일을 열 수 없습니다.", "에러", MB_OK );
		return;
	}
	if( CGlobalValue::GetInstance().OpenAction( dlg.GetPathName() ) == false ) {
		CGlobalValue::GetInstance().CheckIn();
	}
}

void CEtActionToolDoc::OnFileSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( CGlobalValue::GetInstance().SaveAction() == false ) {
		MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, "액션 파일 저장 실패", "에러", MB_OK );
	}

}

void CEtActionToolDoc::OnFileNew()
{
	CGlobalValue::GetInstance().CheckIn();
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( CGlobalValue::GetInstance().IsOpenAction() ) {
		CGlobalValue::GetInstance().Reset();
	}
	else {
		TCHAR szFilter[] = _T( "Eternity Action File (*.act)|*.act|All Files (*.*)|*.*||" );
		CSkinPrevDlg dlg( FALSE, _T("act"), _T("*.act"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

		if( dlg.DoModal() != IDOK ) return;
		if( CGlobalValue::GetInstance().CreateAction( dlg.GetPathName() ) == true ) {
			if( CGlobalValue::GetInstance().CheckOut( dlg.GetPathName() ) == false ) {
				MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, "체크아웃 실페!! 파일을 열 수 없습니다.", "에러", MB_OK );
				return;
			}

		}
	}
}

void CEtActionToolDoc::OnSignalReport()
{
	// TODO: Add your command handler code here
	CSignalReportDlg Dlg;

	Dlg.DoModal();
}

void CEtActionToolDoc::OnSaveSignalReport()
{
	// TODO: Add your command handler code here
	TCHAR szFilter[] = _T( "CSV File (*.csv)|*.csv|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("csv"), _T("*.csv"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	if( dlg.DoModal() != IDOK ) return;
	CString szBuffer;
	CGlobalValue::GetInstance().CalcSignalReport( szBuffer );
	FILE *fp;
	fopen_s( &fp, dlg.GetPathName(), "w" );
	fprintf_s( fp, szBuffer );
	fclose(fp);
}

void CEtActionToolDoc::OnUpdateSignalReport(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( CGlobalValue::GetInstance().IsOpenAction() );
}

void CEtActionToolDoc::OnUpdateSaveSignalReport(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( CGlobalValue::GetInstance().IsOpenAction() );
}

void CEtActionToolDoc::OnFileExport()
{
	std::vector<std::string> szVecActionList;
	CWnd *pWnd = GetPaneWnd( ACTION_PANE );
	if( pWnd ) ((CActionPaneView*)pWnd)->GetSelectActionList( szVecActionList );

	if( szVecActionList.empty() ) {
		MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, "익스포트 할 액션을 선택해주세요.", "에러", MB_OK );
		return;
	}

	TCHAR szFilter[] = _T( "actexport File (*.actexport)|*.actexport|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("actexport"), _T("*.actexport"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	if( dlg.DoModal() != IDOK ) return;
	
	CGlobalValue::GetInstance().ExportAction( dlg.GetPathName(), szVecActionList );
}

void CEtActionToolDoc::OnFileImport()
{
	TCHAR szFilter[] = _T( "actexport File (*.actexport)|*.actexport|All Files (*.*)|*.*||" );
	CFileDialog dlg( TRUE, _T("actexport"), _T("*.actexport"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );

	if( IDOK != dlg.DoModal() ) return;

	CGlobalValue::GetInstance().ImportAction( dlg.GetPathName() );
}

void CEtActionToolDoc::OnUpdateOnFileImport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CGlobalValue::GetInstance().IsOpenAction() );
}

void CEtActionToolDoc::OnUpdateOnFileExport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CGlobalValue::GetInstance().IsOpenAction() );
}

void CEtActionToolDoc::OnCheckWalkfront()
{
	int nResult = MessageBox( CGlobalValue::GetInstance().GetView()->m_hWnd, "Monster 폴더 내의 act 파일 중 Walk_Front 액션이 없는 파일들을 체크합니다.\n확인에 다소 시간이 걸릴 수 있습니다.(최장 5분)\n진행하시겠습니까?", "확인", MB_YESNO );
	if (nResult == IDYES)
	{
		CGlobalValue::GetInstance().CheckActionHasWalkFront();
	}
}


