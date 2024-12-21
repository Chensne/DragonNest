#pragma once


// CFloatMiniSliderRange

#define UM_NUMMINISLIDER_UPDATE WM_USER + 2000

#include "MiniSliderEx.h"
class CFloatMiniSliderRange : public CWnd
{
	DECLARE_DYNAMIC(CFloatMiniSliderRange)

public:
	CFloatMiniSliderRange();
	virtual ~CFloatMiniSliderRange();

	void SetRange( float fMin, float fMax );
	void SetRangeMin( float fValue );
	void SetRangeMax( float fValue );
	float GetRangeMin() { return m_Slider.GetRangeMinFloat(); }
	float GetRangeMax() { return m_Slider.GetRangeMaxFloat(); }

	frange GetPos();
	void SetPos( frange range );


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


