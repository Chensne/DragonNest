// EtViewerView.h : CEtViewerView Ŭ������ �������̽�
//


#pragma once

#include "FrameSync.h"
#include "InputReceiver.h"
class CEtViewerView : public CView
{
protected: // serialization������ ��������ϴ�.
	CEtViewerView();
	DECLARE_DYNCREATE(CEtViewerView)

// Ư���Դϴ�.
public:
	CEtViewerDoc* GetDocument() const;

// �۾��Դϴ�.
public:
	void Refresh();
	bool IsIdle() { return m_bIdle; }

protected:
	bool m_bActivate;
	bool m_bIdle;

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CEtViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
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

#ifndef _DEBUG  // EtViewerView.cpp�� ����� ����
inline CEtViewerDoc* CEtViewerView::GetDocument() const
   { return reinterpret_cast<CEtViewerDoc*>(m_pDocument); }
#endif

