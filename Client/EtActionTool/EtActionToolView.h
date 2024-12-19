// EtActionToolView.h : CEtActionToolView 클래스의 인터페이스
//


#pragma once


class CEtActionToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CEtActionToolView();
	DECLARE_DYNCREATE(CEtActionToolView)

// 특성입니다.
public:
	CEtActionToolDoc* GetDocument() const;

protected:
	bool m_bActivate;
	bool m_bIdle;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);


// 구현입니다.
public:
	virtual ~CEtActionToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	afx_msg void OnPaint();

	bool IsIdle() { return m_bIdle; }
};

#ifndef _DEBUG  // EtActionToolView.cpp의 디버그 버전
inline CEtActionToolDoc* CEtActionToolView::GetDocument() const
   { return reinterpret_cast<CEtActionToolDoc*>(m_pDocument); }
#endif

