#pragma once



// CEventEditView_CreateModifyParameter �� ���Դϴ�.

#include "WizardCommon.h"
#include "afxwin.h"
class CEventEditView_CreateModifyParameter : public CFormView, public CWizardCommon
{
	DECLARE_DYNCREATE(CEventEditView_CreateModifyParameter)

protected:
	CEventEditView_CreateModifyParameter();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CEventEditView_CreateModifyParameter();

	bool m_bActivate;
public:
	enum { IDD = IDD_EVENTEDITVIEW_CREATEMODIFYPARAMETER };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual VIEW_TYPE GetNextView() { return FINISH; }

	virtual bool IsVisivleNextButton() { return true; }
	virtual bool IsVisivlePrevButton() { return true; }
	virtual bool IsVisivleFinishButton() { return true; }
	virtual bool IsFinishOrCancel() { return false; }
	virtual void PreProcess();
	virtual bool PostProcess();

	CDC *m_pmemDC;
	CBitmap *m_pdcBitmap;
	void CreateMemDC();
	void DeleteMemDC();

	void EnableAdditionalControl( int nCurSel );


	CComboBox m_ValueType;
	virtual void OnInitialUpdate();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeCombo1();
	CString m_szParamName;
	CString m_szParamDesc;
	afx_msg void OnEnChangeEdit1();
	float m_fValueMin;
	float m_fValueMax;
	CString m_szValueData;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


