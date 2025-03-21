#pragma once
#include "afxwin.h"

// CKeyframePropPaneView form view
class CKeyframePropPaneView : public CFormView
{
	DECLARE_DYNCREATE(CKeyframePropPaneView)

protected:
	CKeyframePropPaneView();           // protected constructor used by dynamic creation
	virtual ~CKeyframePropPaneView();

public:
	enum { IDD = IDD_KEYFRAME };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	struct ColorAnchor	{
		float fPos;
		COLORREF color;
		bool bFreeze;
		const bool operator < ( ColorAnchor &rhs ) {
			return fPos < rhs.fPos;
		}
	};
	struct AlphaAnchor {
		float fPos;
		float fValue;
		bool bFreeze;
		const bool operator < ( AlphaAnchor &rhs ) {
			return fPos < rhs.fPos;
		}
	};

	struct ScaleAnchor {
		float fPos;
		float fValue;
		bool bFreeze;
		const bool operator < ( ScaleAnchor &rhs ) {
			return fPos < rhs.fPos;
		}
	};

	std::vector< ColorAnchor >	m_colorAnchors;
	std::vector< AlphaAnchor >	m_alphaAnchors;
	std::vector< ScaleAnchor >	m_scaleAnchors;

	int										m_colorGrabIndex;
	int										m_alphaGrabIndex;
	int										m_scaleGrabIndex;

	CEtBillboardEffectEmitter							*m_pEmitter;
	CString								m_fxString;

	bool									m_bDraw;

	BOOL m_bActivate;
public:
	void AddColorAnchor( float pos , COLORREF color, bool bFreeze = false);
	void AddAlphaAnchor( float pos , float value, bool bFreeze = false);
	void AddScaleAnchor( float pos , float value, bool bFreeze = false);
	bool IsInsideColorTable( CPoint point );
	bool IsInsideAlphaTable( CPoint point );
	bool IsInsideScaleTable( CPoint point );
	float ComputeColorPosition( CPoint point );
	float ComputeAlphaPosition( CPoint point );
	float ComputeScalePosition( CPoint point );
	float ComputeAlphaPositionY( CPoint point );
	float ComputeScalePositionY( CPoint point );
	COLORREF GetTableColor( float t) ;
	void UpdateChangeSetting( CEtBillboardEffectEmitter *pEmitter );
	void OnOtherSelected();
	void OnFXSelected( CString szDesc );
	void OnEmitterSelected(  CEtBillboardEffectEmitter *pEmitter );
	void DrawColorTable(CPaintDC &dc);
	void DrawAlphaTable(CPaintDC &dc);
	void DrawScaleTable(CPaintDC &dc);
	void UpdateSetting();
	void SetDefaultTable();
	void RefreshPointer();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	CString m_szScale;
	afx_msg void OnEnChangeEdit1();
	CEdit m_EditCtrl;
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_ComboBox;
};


