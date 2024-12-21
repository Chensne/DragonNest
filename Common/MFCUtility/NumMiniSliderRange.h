#pragma once


// CNumMiniSliderRange

#define UM_NUMMINISLIDER_UPDATE WM_USER + 2000

#include "MiniSliderEx.h"
class CNumMiniSliderRange : public CWnd
{
	DECLARE_DYNAMIC(CNumMiniSliderRange)

public:
	CNumMiniSliderRange();
	virtual ~CNumMiniSliderRange();

	void SetRange( int nMin, int nMax );
	void SetRangeMin( int nValue );
	void SetRangeMax( int nValue );
	int GetRangeMin() { return m_Slider.GetRangeMin(); }
	int GetRangeMax() { return m_Slider.GetRangeMax(); }

	irange GetPos();
	void SetPos( irange range );


protected:
	DECLARE_MESSAGE_MAP()

	CEdit m_Edit;
	CEdit m_Edit2;
	CSpinButtonCtrl m_Spin;
	CSpinButtonCtrl m_Spin2;
	CMiniSliderEx m_Slider;
	CStatic m_Static;
	bool m_bForceUpdate;


	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnUpdatePosSliderEx( WPARAM wParam, LPARAM lParam );
	afx_msg void OnEnUpdateEdit();
	afx_msg void OnEnUpdateEdit2();
	afx_msg void OnEnable(BOOL bEnable);
};


