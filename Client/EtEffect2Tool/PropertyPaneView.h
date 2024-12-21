#pragma once

#include "PropertyGridCreator.h"

// CPropertyPaneView form view

class CPropertyPaneView : public CFormView, public CPropertyGridImp
{
	DECLARE_DYNCREATE(CPropertyPaneView)

protected:
	CPropertyPaneView();           // protected constructor used by dynamic creation
	virtual ~CPropertyPaneView();

public:
	enum { IDD = IDD_PROPERTY };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	enum EditMode {
		NONE,
		PTC_PROPERTY,
		PTC_SETTING,
		FX_SETTING,
		FX_GLOBAL_SETTING,
		POSITION,
		ROTATION,
	};
	bool			m_bActivate;
	CEtBillboardEffectEmitter	*m_Emitter;
	bool			*m_pLoop;
	bool			*m_pIterate;
	CString		m_fxString;
	int				m_nMode;

	std::vector<CUnionValueProperty*> m_vecPropertyList;	// property
	std::vector<CUnionValueProperty*> m_vecParticleSetting;	// iterator, loop
	std::vector<CUnionValueProperty*> m_vecFXSetting;	// iterator, loop
	std::vector<CUnionValueProperty*> m_vecGlobalFXSetting;	// iterator, loop
	std::vector<CUnionValueProperty*> m_vecPositionSetting;	// 
	std::vector<CUnionValueProperty*> m_vecRotationSetting;	// 

	std::string	m_backgroundMesh;
public:
	void SetDefaultEmitterParam(SEmitter *Param );
	void UpdateData( SEmitter *Param, bool bSave );
	void ApplyParticleSetting();
	void ApplySetting();
	void ApplyFXSetting(int index);
	void ApplyGlobalFXSetting();
	void ApplyPositionSetting();
	void ApplyRotationSetting();
	void Refresh();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnNotifyGrid( WPARAM wParam, LPARAM lParam );
	void OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex );
	void OnGlobalFXSelected();
	void OnFXSelected( CString str );
	void OnParticleSelected( bool *pLoop, bool *pIterate );
	void OnEmitterSelected(  CEtBillboardEffectEmitter *pEmitter );
	void OnPositionSelected( EtVector3 pos );
	void OnRotationSelected( EtVector3 rot );
	void OnEmpty();
	void RefreshPointer();
	void RefreshProperty();
	void Reset() { m_fxString = "";}
};
