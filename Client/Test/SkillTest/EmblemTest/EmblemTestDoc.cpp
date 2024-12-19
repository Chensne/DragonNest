// EmblemTestDoc.cpp : implementation of the CEmblemTestDoc class
//

#include "stdafx.h"
#include "EmblemTest.h"

#include "EmblemTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEmblemTestDoc

IMPLEMENT_DYNCREATE(CEmblemTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CEmblemTestDoc, CDocument)
END_MESSAGE_MAP()


// CEmblemTestDoc construction/destruction

CEmblemTestDoc::CEmblemTestDoc()
{
	// TODO: add one-time construction code here

}

CEmblemTestDoc::~CEmblemTestDoc()
{
}

BOOL CEmblemTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CEmblemTestDoc serialization

void CEmblemTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CEmblemTestDoc diagnostics

#ifdef _DEBUG
void CEmblemTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEmblemTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEmblemTestDoc commands
