#pragma once

class CPropertyGrid;

class CDefaultStateView : public CView
{
	DECLARE_DYNCREATE(CDefaultStateView)

protected:
	CDefaultStateView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CDefaultStateView();

public:
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	void RefreshView();

private:
	CPropertyGrid* m_pProperty;
};


