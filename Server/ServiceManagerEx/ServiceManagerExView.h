// ServiceManagerExView.h : CServiceManagerExView Ŭ������ �������̽�
//
#pragma once

class CServiceManagerExView : public CView
{
protected: // serialization������ ��������ϴ�.
	CServiceManagerExView();
	DECLARE_DYNCREATE(CServiceManagerExView)

// Ư���Դϴ�.
public:
	CServiceManagerExDoc* GetDocument() const;

// �۾��Դϴ�.
public:

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
	virtual ~CServiceManagerExView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	virtual void OnInitialUpdate();

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ServiceManagerExView.cpp�� ����� ����
inline CServiceManagerExDoc* CServiceManagerExView::GetDocument() const
   { return reinterpret_cast<CServiceManagerExDoc*>(m_pDocument); }
#endif

