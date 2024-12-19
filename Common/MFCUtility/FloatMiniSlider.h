#pragma once


// CFloatMiniSlider

#define UM_NUMMINISLIDER_UPDATE WM_USER + 2000

#include "MiniSliderEx.h"
class CFloatMiniSlider : public CWnd
{
	DECLARE_DYNAMIC(CFloatMiniSlider)

public:
	CFloatMiniSlider();
	virtual ~CFloatMiniSlider();

	void SetRange( float fMin, float fMax );
	void SetRangeMin( float fValue );
	void SetRangeMax( float fValue );
	float GetRangeMin() { return m_Slider.GetRangeMinFloat(); }
	float GetRangeMax() { return m_Slider.GetRangeMaxFloat(); }
	float GetPos() { return m_Slider.GetPosFloat(); }
	void SetPos( float fValue );


protected:
	DECLARE_MESSAGE_MAP()

	CEdit m_Edit;
	CSpinButtonCtrl m_Spin;
	CMiniSliderEx m_Slider;
	CStatic m_Static;
	bool m_bForceUpdate;


	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnUpdatePosSliderEx( WPARAM wParam, LPARAM lParam );
	afx_msg void OnEnUpdateEdit();
	afx_msg void OnEnable(BOOL bEnable);
};


