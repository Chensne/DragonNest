// EtEffect2ToolView.h : CEtEffect2ToolView 클래스의 인터페이스
//


#pragma once


class CEtEffect2ToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CEtEffect2ToolView();
	DECLARE_DYNCREATE(CEtEffect2ToolView)

// 특성입니다.
public:
	CEtEffect2ToolDoc* GetDocument() const;

// 작업입니다.
public:
	bool			m_bActivate;

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CEtEffect2ToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};

#ifndef _DEBUG  // EtEffect2ToolView.cpp의 디버그 버전
inline CEtEffect2ToolDoc* CEtEffect2ToolView::GetDocument() const
   { return reinterpret_cast<CEtEffect2ToolDoc*>(m_pDocument); }
#endif

