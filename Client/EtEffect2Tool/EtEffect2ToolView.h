// EtEffect2ToolView.h : CEtEffect2ToolView Ŭ������ �������̽�
//


#pragma once


class CEtEffect2ToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CEtEffect2ToolView();
	DECLARE_DYNCREATE(CEtEffect2ToolView)

// Ư���Դϴ�.
public:
	CEtEffect2ToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:
	bool			m_bActivate;

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CEtEffect2ToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};

#ifndef _DEBUG  // EtEffect2ToolView.cpp�� ����� ����
inline CEtEffect2ToolDoc* CEtEffect2ToolView::GetDocument() const
   { return reinterpret_cast<CEtEffect2ToolDoc*>(m_pDocument); }
#endif

