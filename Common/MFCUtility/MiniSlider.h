#pragma once


// CMiniSlider
#define UM_MINISLIDER_UPDATE_POS WM_USER + 4000
#define UM_MINISLIDER_SET_POS WM_USER + 4001

class CMiniSlider : public CWnd
{
	DECLARE_DYNAMIC(CMiniSlider)

public:
	CMiniSlider();
	virtual ~CMiniSlider();

	void SetPos( float fValue );
	float GetPos() { return m_fPos; }

protected:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSetPos( WPARAM wParam, LPARAM lParam );

	float m_fPos;
	bool m_bLButtonDown;
};


