#pragma once


// CMiniSliderEx
#define UM_MINISLIDER_UPDATE_POS WM_USER + 4000
#define UM_MINISLIDER_SET_POS WM_USER + 4001

class CMiniSliderEx : public CWnd
{
	DECLARE_DYNAMIC(CMiniSliderEx)

public:
	enum SLIDER_TYPE {
		ST_NORMAL,
		ST_RANGE,
	};
	enum UNIT_TYPE {
		UT_INT,
		UT_FLOAT,
	};

	CMiniSliderEx( SLIDER_TYPE Type = ST_NORMAL, UNIT_TYPE UnitType = UT_INT );
	virtual ~CMiniSliderEx();

	void SetSliderType( SLIDER_TYPE Type ) { m_Type = Type; }
	void SetUnitType( UNIT_TYPE Type ) { m_UnitType = Type; }

	void SetRange( int nMin, int nMax ) { m_nRangeMin = nMin; m_nRangeMax = nMax; }
	void SetRange( float fMin, float fMax ) { m_fRangeMin = fMin; m_fRangeMax = fMax; }

	void SetRangeMin( int nValue ) { m_nRangeMin = nValue; }
	void SetRangeMax( int nValue ) { m_nRangeMax = nValue; }
	void SetRangeMin( float fValue ) { m_fRangeMin = fValue; }
	void SetRangeMax( float fValue ) { m_fRangeMax = fValue; }

	int GetRangeMin() { return m_nRangeMin; }
	int GetRangeMax() { return m_nRangeMax; }
	float GetRangeMinFloat() { return m_fRangeMin; }
	float GetRangeMaxFloat() { return m_fRangeMax; }

	int GetPos() { return m_nPos; }
	int GetEndPos() { return m_nEndPos; }
	float GetPosFloat() { return m_fPos; }
	float GetEndPosFloat() { return m_fEndPos; }

	void SetPos( int nValue );
	void SetEndPos( int nValue );

	void SetPos( float fValue );
	void SetEndPos( float fValue );
	
	int GetTicPos( int nIndex );

	void SetSendRootParent( int nCount ) { m_nSendRootParent = nCount; }

protected:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSetPos( WPARAM wParam, LPARAM lParam );

	SLIDER_TYPE m_Type;
	UNIT_TYPE m_UnitType;
	union {
		int m_nRangeMin;
		float m_fRangeMin;
	};
	union {
		int m_nRangeMax;
		float m_fRangeMax;
	};
	union {
		int m_nPos;
		float m_fPos;
	};
	union {
		int m_nEndPos;
		float m_fEndPos;
	};

	bool m_bLButtonDown;
	int m_nSendRootParent;
};


