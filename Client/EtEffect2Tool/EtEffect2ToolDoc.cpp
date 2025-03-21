// EtEffect2ToolDoc.cpp : CEtEffect2ToolDoc 클래스의 구현
//

#include "stdafx.h"
#include "EtEffect2Tool.h"

#include "EtEffect2ToolDoc.h"
#include "GlobalValue.h"
#include "PropertyPaneView.h"
#include "ParticlePaneView.h"
#include "MainFrm.h"
#include "RenderBase.h"
#include "KeyframePropPaneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEtEffect2ToolDoc

IMPLEMENT_DYNCREATE(CEtEffect2ToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CEtEffect2ToolDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CEtEffect2ToolDoc::OnFileOpen)
	ON_COMMAND(ID_PLAY_PLAY, &CEtEffect2ToolDoc::OnPlay)
	ON_COMMAND(ID_PLAY_PREV, &CEtEffect2ToolDoc::OnPrev)
	ON_COMMAND(ID_PLAY_NEXT, &CEtEffect2ToolDoc::OnNext)
	ON_COMMAND(ID_FILE_SAVE, &CEtEffect2ToolDoc::OnFileSave)
	ON_COMMAND(ID_PARTICLE_SAVE, &CEtEffect2ToolDoc::OnParticleFileSave)
	ON_COMMAND(ID_FILE_NEW, &CEtEffect2ToolDoc::OnFileNew)
	ON_COMMAND(ID_GRIDTOGGLE, &CEtEffect2ToolDoc::OnGridToggle)
	ON_COMMAND(ID_PLAY_PLAYLOOP, &CEtEffect2ToolDoc::OnPlayPlayLoop)
	ON_COMMAND(ID_OPTION_UP, &CEtEffect2ToolDoc::OnOptionUp)
	ON_COMMAND(ID_OPTION_DOWN, &CEtEffect2ToolDoc::OnOptionDown)
END_MESSAGE_MAP()


// CEtEffect2ToolDoc 생성/소멸

CEtEffect2ToolDoc::CEtEffect2ToolDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CEtEffect2ToolDoc::~CEtEffect2ToolDoc()
{
}

BOOL CEtEffect2ToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CEtEffect2ToolDoc serialization

void CEtEffect2ToolDoc::Serialize(CArchive& ar)
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
		int nLength = szFileName.GetLength();

		if( nLength > 3 )
		{
			bool bEffFile = false;
			if( szFileName[nLength-3] == 'e' && szFileName[nLength-2] == 'f' && szFileName[nLength-1] == 'f' ) bEffFile = true;

			if( bEffFile )
			{
				CGlobalValue::GetInstance().Reset();
				CGlobalValue::GetInstance().Load( szFileName.GetBuffer() );
				CGlobalValue::GetInstance().GetPropertyPaneView()->Refresh();
				((CMainFrame*)AfxGetMainWnd())->ShowPane( 2 );

				CString szStr;
				szStr.Format( "%s - EtEffectTool", szFileName.GetBuffer() );
				CMainFrame *pFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
				::SetWindowText( pFrame->m_hWnd, szStr );
				m_currentFile = szFileName;
			}
			else
			{
				MessageBox( NULL, "eff파일만 로드 가능합니다.", "경고", MB_OK );
			}
		}
	}
}


// CEtEffect2ToolDoc 진단

#ifdef _DEBUG
void CEtEffect2ToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEtEffect2ToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CEtEffect2ToolDoc::OnFileNew()
{
	CGlobalValue::GetInstance().GetKeyframePropPaneView()->RefreshPointer();
	CGlobalValue::GetInstance().GetPropertyPaneView()->RefreshPointer();
	CGlobalValue::GetInstance().Reset();
}
// CEtEffect2ToolDoc 명령
void CEtEffect2ToolDoc::OnFileOpen()
{
	TCHAR szFilter[] = _T( "Eternity Effect File(*.eff)|*.eff|All Files (*.*)|*.*||" );
	CFileDialog Dlg(TRUE, _T("eff"), _T("*.eff"), OFN_FILEMUSTEXIST| OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ENABLESIZING, szFilter );

	Dlg.m_ofn.lpstrTitle = _T("Open Eternity Effect File");

	CString szParticleLocation;
	if( Dlg.DoModal() == IDOK ) {
		szParticleLocation = Dlg.GetPathName();
		char *szFileName = szParticleLocation.GetBuffer();
		CGlobalValue::GetInstance().Reset();
		CGlobalValue::GetInstance().Load( szFileName );
		CGlobalValue::GetInstance().GetPropertyPaneView()->Refresh();
		((CMainFrame*)AfxGetMainWnd())->ShowPane( 2 );
		
		CString szStr;
		szStr.Format( "%s - EtEffectTool", Dlg.GetPathName() );
		CMainFrame *pFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
		::SetWindowText( pFrame->m_hWnd, szStr );
		m_currentFile = Dlg.GetFileName();
	}
}

void CEtEffect2ToolDoc::OnFileSave()
{
	TCHAR szFilter[] = _T( "Eternity Effect File (*.eff)|*.eff|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("eff"), m_currentFile.GetBuffer(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter );

	dlg.m_ofn.lpstrTitle = "Save Eternity Engine Effect File";

	if( IDOK != dlg.DoModal() )
		return;

	CGlobalValue::GetInstance().Save( dlg.GetPathName().GetBuffer() );

	CString szStr;
	szStr.Format( "%s - EtEffectTool", dlg.GetPathName() );
	CMainFrame *pFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
	::SetWindowText( pFrame->m_hWnd, szStr );
}

void CEtEffect2ToolDoc::OnParticleFileSave()
{
	if( CGlobalValue::GetInstance().GetParticlePaneView()->GetCurrentDepth() == 1 ) {
		CGlobalValue::GetInstance().GetParticlePaneView()->OnSaveParticle();	
		AfxMessageBox( "파티클 파일이   \n저장 되었습니다.   \n");
	}
}

void CEtEffect2ToolDoc::OnPlay()
{
	CGlobalValue::GetInstance().SetLoopPlay(false);
	CGlobalValue::GetInstance().PlayFXObject();
}

void CEtEffect2ToolDoc::OnPrev()
{
	CGlobalValue::GetInstance().PrevFXObject();
}

void CEtEffect2ToolDoc::OnNext()
{
	CGlobalValue::GetInstance().NextFXObject();
}


void CEtEffect2ToolDoc::OnGridToggle()
{
	// TODO: Add your command handler code here
	CRenderBase::GetInstance().SetShowGrid( !CRenderBase::GetInstance().IsShowGrid() );
}

void CEtEffect2ToolDoc::OnPlayPlayLoop()
{
	CGlobalValue::GetInstance().SetLoopPlay(true);
	CGlobalValue::GetInstance().PlayFXObject();
	// TODO: Add your command handler code here
}

void CEtEffect2ToolDoc::OnOptionUp()
{
	// TODO: Add your command handler code here
	int nOption = CEtBillboardEffect::GetEffectCountOption();
	if( nOption == 1 ) nOption = 0;
	else if( nOption == 2 ) nOption = 1;
	CEtBillboardEffect::SetEffectCountOption( nOption );
}

void CEtEffect2ToolDoc::OnOptionDown()
{
	// TODO: Add your command handler code here
	int nOption = CEtBillboardEffect::GetEffectCountOption();
	if( nOption == 0 ) nOption = 1;
	else if( nOption == 1 ) nOption = 2;
	CEtBillboardEffect::SetEffectCountOption( nOption );
}