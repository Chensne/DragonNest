// EtViewerView.h : CEtViewerView 클래스의 인터페이스
//


#pragma once

#include "FrameSync.h"
#include "InputReceiver.h"
class CEtViewerView : public CView
{
protected: // serialization에서만 만들어집니다.
	CEtViewerView();
	DECLARE_DYNCREATE(CEtViewerView)

// 특성입니다.
public:
	CEtViewerDoc* GetDocument() const;

// 작업입니다.
public:
	void Refresh();
	bool IsIdle() { return m_bIdle; }

protected:
	bool m_bActivate;
	bool m_bIdle;

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
	virtual ~CEtViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPlayPlaypause();
	afx_msg void OnPlayRewind();
	afx_msg void OnPlayForward();
	afx_msg void OnPlayPrev();
	afx_msg void OnPlayNext();
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // EtViewerView.cpp의 디버그 버전
inline CEtViewerDoc* CEtViewerView::GetDocument() const
   { return reinterpret_cast<CEtViewerDoc*>(m_pDocument); }
#endif

