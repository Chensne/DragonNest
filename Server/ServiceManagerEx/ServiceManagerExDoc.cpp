// ServiceManagerExDoc.cpp : CServiceManagerExDoc 클래스의 구현
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "ServiceManagerExDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CServiceManagerExDoc

IMPLEMENT_DYNCREATE(CServiceManagerExDoc, CDocument)

BEGIN_MESSAGE_MAP(CServiceManagerExDoc, CDocument)
END_MESSAGE_MAP()


// CServiceManagerExDoc 생성/소멸

CServiceManagerExDoc::CServiceManagerExDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CServiceManagerExDoc::~CServiceManagerExDoc()
{
}

BOOL CServiceManagerExDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}

// CServiceManagerExDoc serialization

void CServiceManagerExDoc::Serialize(CArchive& ar)
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


// CServiceManagerExDoc 진단

#ifdef _DEBUG
void CServiceManagerExDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CServiceManagerExDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG