#pragma once



// CSequenceView view

class CSequenceView : public CScrollView
{
	DECLARE_DYNCREATE(CSequenceView)

public:
	class CSequenceSignalBase {
	public:
		CSequenceSignalBase();
		virtual ~CSequenceSignalBase();

	public:
		bool bModifyLength;
		bool bFreeze;
		bool bDestroy;

		COLORREF Color;
		COLORREF InitialColor;

		int nStartFrame;
		int nEndFrame;
		int nYOrder;

		CString szString;
		CString szDesc;
	};
protected:
	CSequenceView();           // protected constructor used by dynamic creation
	virtual ~CSequenceView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	bool m_bActivate;
	CBitmap *m_pBitmap;
	CDC *m_pDC;

	CSliderCtrl *m_pSlider;
	CRect m_rcSliderRect;

	int m_nScrollHeight;
	CPoint m_GridSize;
	int m_nGridHeightCount;
	DWORD m_dwAdditionalGridHeightCount;
	bool m_bUpdateAdditionalGridHeightCount;
	bool m_bEnable;
	CPen *m_Pens[2][4];

	bool m_bSelectEmpty;
	CPoint m_SelectEmptyMousePos;

#ifdef _SELECT_MULTIPLE_SIGNAL
	CPoint m_DragEndPoint;
	std::vector<DWORD> m_CurSelectedItems;
#else
	int m_nFocusSignalIndex;
#endif
	std::vector<CSequenceSignalBase *> m_pVecSignalList;
	
	// Mouse Event Variable
	bool m_bButton[2];
	CPoint m_PrevMousePosition;
	char m_cMouseEventFlag;
	char *m_pCursor;

	int m_nCurSingalMaxRange;

	static int s_nMargin;
public:

protected:
	void CreateDC();
	void DeleteDC();

	void DrawSequence();
	void DrawSignals();
	void DrawSignal(int nIndex);

	void CalcSignalRect( int nIndex, CRect &rcRect );
	char CheckSignalRect( int nIndex, const CPoint &point );
	void MousePosToGridPos( CPoint &MousePos, CPoint &ResultPos );
	bool IsMoveSignal(int nStartFrame, int nEndFrame, int nYOrder, bool bCheckFocus = true);

	bool ModifySignalStartFrame( CPoint p, int nCurSelectedSignalIndex );
	bool ModifySignalEndFrame( CPoint p, int nCurSelectedSignalIndex );
	bool ModifySignalMove( CPoint p, int nCurSelectedSignalIndex );

#ifdef _SELECT_MULTIPLE_SIGNAL
	void OnDragEnd(const CPoint& startPoint, const CPoint& endPoint);
	void OnDragging(const CRect& dragArea);

	BOOL IsSignalInDragginArea(DWORD dwIndex, const CRect& dragArea);
	BOOL IsSelectedSignal_Valid(DWORD dwIndex);
	BOOL IsSelectedSignal(DWORD dwIndex);
	BOOL IsSelectedSignal(CPoint& pointInsideItem);

	BOOL ExtendGridHeightCount(int nYOrder);
	void PushBackSelectedSignalIndex(DWORD dwIndex);
	void RemoveSelectedSignal(DWORD dwIndex);
	void ClearSelectedSignals();

	BOOL IsTabKey();
#endif


public:
	std::vector<CSequenceSignalBase *> &GetSignalList() { return m_pVecSignalList; }
	void SetEnable( bool bEnable );

	CSliderCtrl *GetSlider() { return m_pSlider; }
	int GetSliderPos() { return m_pSlider->GetPos(); }
	void SetSliderPos(int nPos) { m_pSlider->SetPos(nPos); Invalidate(); }
	void SetSliderRange(int nMin, int nMax) { m_pSlider->SetRange(nMin, nMax); }

	CSequenceSignalBase *AddSignal( int nStart, int nEnd, int nYOrder, char *szDescription, char *szStr );
	bool CanAddSignal( int nStartFrame, int nEndFrame, int nYOrder );
	bool CanAddSignal( int nX, int nY );
	bool CalcSignalPos( int nX, int nY, int &nPos, int &nYOrder );
	void RemoveSignal( int nIndex );
	void Reset();

	void EnableUpdateAdditionalGridHeightCount( bool bEnable ) { m_bUpdateAdditionalGridHeightCount = bEnable; }
	virtual bool IsValidPropertyPaneState() const;

#ifdef _SELECT_MULTIPLE_SIGNAL
	BOOL IsSignalInArea(int nStart, int nEnd, int nStartYOrder, int nEndYOrder) const;
	BOOL IsSignalInArea(const CRect& area) const;
	int	 IsSignalEndOverRangeMax(const int& nGridEnd);

	int GetLastSignalYOrder(const CRect& area) const;
#endif
	BOOL IsSelectEmpty() { return m_bSelectEmpty; }
	CPoint GetSelectEmptyMousePos() { return m_SelectEmptyMousePos; }

#ifdef _SELECT_MULTIPLE_SIGNAL
	bool IsSelectedSignal() { return (m_CurSelectedItems.empty() == false); }
#else
	bool IsSelectedSignal() { return (m_nFocusSignalIndex != -1); }
#endif
	void RefreshSlider();
	void RefreshScroll();
	void Refresh()
	{
		RefreshScroll();
		RefreshSlider();
		Invalidate();
	}
	void SetFrame( CString str, int nStart, int nEnd );

protected:
	// Sequence Event
	virtual void OnSelectSignal( CSequenceSignalBase *pSignal ) {}
	virtual bool OnRemoveSignal( CSequenceSignalBase *pSignal ) { return true; }
	virtual void OnModifySignal( CSequenceSignalBase *pSignal );

	virtual bool InsertSignal( CSequenceSignalBase *pSignal ) { return true; }
	virtual CSequenceSignalBase *CreateSignal() { CSequenceSignalBase *pSignal = new CSequenceSignalBase(); return pSignal; }
#ifdef _SELECT_MULTIPLE_SIGNAL
	virtual void OnSelectSignalArray( std::vector<CSequenceSignalBase *>& pSignals ) {}
	virtual void OnClearSelectedSignals() {}
#endif


protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
};


