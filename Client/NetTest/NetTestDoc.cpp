// NetTestDoc.cpp : CNetTestDoc 클래스의 구현
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


// CNetTestDoc 생성/소멸

CNetTestDoc::CNetTestDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CNetTestDoc::~CNetTestDoc()
{
}

BOOL CNetTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CNetTestDoc serialization

void CNetTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CNetTestDoc 진단

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


// CNetTestDoc 명령
