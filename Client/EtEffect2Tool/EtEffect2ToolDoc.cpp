// EtEffect2ToolDoc.cpp : CEtEffect2ToolDoc Ŭ������ ����
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


// CEtEffect2ToolDoc ����/�Ҹ�

CEtEffect2ToolDoc::CEtEffect2ToolDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CEtEffect2ToolDoc::~CEtEffect2ToolDoc()
{
}

BOOL CEtEffect2ToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CEtEffect2ToolDoc serialization

void CEtEffect2ToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
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
				MessageBox( NULL, "eff���ϸ� �ε� �����մϴ�.", "���", MB_OK );
			}
		}
	}
}


// CEtEffect2ToolDoc ����

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
// CEtEffect2ToolDoc ���
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
		AfxMessageBox( "��ƼŬ ������   \n���� �Ǿ����ϴ�.   \n");
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