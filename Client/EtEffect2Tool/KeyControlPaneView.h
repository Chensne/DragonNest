#pragma once

// CKeyControlPaneView form view
class CAxisRenderObject;
class CKeyControlPaneView : public CScrollView
{
	DECLARE_DYNCREATE(CKeyControlPaneView)

protected:
	CKeyControlPaneView();           // protected constructor used by dynamic creation
	virtual ~CKeyControlPaneView();

public:
	enum { IDD = IDD_KEYCONTROL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	CSliderCtrl	*m_pSliderCtrl;
	DWORD		m_dwGridWidth;
	DWORD		m_dwGridHeight;
	DWORD		m_dwGridHeightCount;
	DWORD		m_dwScrollHeight;
	CDC			*m_pMemDC;
	CBitmap		*m_pdcBitmap;
	CPoint		m_mousePoint;
	bool			m_bActivate;
	int				m_overIndex;

	CEtEffectDataContainer **m_ppTable;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	void CreateSlider();
	void RefreshSlider();
	void RefreshScroll();
	void CreateMemDC();
	void DeleteMemDC();
	void DrawGrid();
	void DrawItems();
	int GetMouseOverIndex();
	RECT GetKeyframeRect( int index );	

public:
	void Reset();
	virtual void OnInitialUpdate();
	void SetRangeMax( int nMax );
	CSliderCtrl* GetSliderCtrl() { return m_pSliderCtrl; }
	afx_msg void OnPaint();
	void OnFXSelected( CString str );
protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};


