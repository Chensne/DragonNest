// NetTestDoc.cpp : CNetTestDoc Ŭ������ ����
//

#include "stdafx.h"
#include "NetTest.h"

#include "NetTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNetTestDoc

IMPLEMENT_DYNCREATE(CNetTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CNetTestDoc, CDocument)
END_MESSAGE_MAP()


// CNetTestDoc ����/�Ҹ�

CNetTestDoc::CNetTestDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CNetTestDoc::~CNetTestDoc()
{
}

BOOL CNetTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CNetTestDoc serialization

void CNetTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CNetTestDoc ����

#ifdef _DEBUG
void CNetTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNetTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNetTestDoc ���
