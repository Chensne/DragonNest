// EtUIToolDoc.cpp : implementation of the CEtUIToolDoc class
//

#include "stdafx.h"
#include "EtUITool.h"

#include "EtUIToolDoc.h"

#include "LayoutView.h"
#include "EtUIToolView.h"
#include "GlobalValue.h"
#include "DummyView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEtUIToolDoc

IMPLEMENT_DYNCREATE(CEtUIToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CEtUIToolDoc, CDocument)
END_MESSAGE_MAP()


// CEtUIToolDoc construction/destruction

CEtUIToolDoc::CEtUIToolDoc()
{
	// TODO: add one-time construction code here

}

CEtUIToolDoc::~CEtUIToolDoc()
{
}

BOOL CEtUIToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CEtUIToolDoc serialization

void CEtUIToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
		// TODO: 여기에 로딩 코드를 추가합니다.
		CFile *pFile = ar.GetFile();
		CString szFileName = pFile->GetFilePath();
		int nLength = szFileName.GetLength();

		if( nLength > 2 )
		{
			bool bUIFile = false;
			if( szFileName[nLength-2] == 'u' && szFileName[nLength-1] == 'i' ) bUIFile = true;

			if( bUIFile )
			{
				CLayoutView *pLayoutView;
				pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
				pLayoutView->LoadLayoutDlg( szFileName );
				SetPathName( szFileName );
			}
			else
			{
				MessageBox( NULL, "The ui files can only be loaded.", "경고", MB_OK );
			}
		}
	}
}


// CEtUIToolDoc diagnostics

#ifdef _DEBUG
void CEtUIToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEtUIToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEtUIToolDoc commands
