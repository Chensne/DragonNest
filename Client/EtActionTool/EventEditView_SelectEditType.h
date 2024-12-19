#pragma once



// CEventEditView_SelectEditType �� ���Դϴ�.
#include "WizardCommon.h"

class CEventEditView_SelectEditType : public CFormView, public CWizardCommon
{
	DECLARE_DYNCREATE(CEventEditView_SelectEditType)

public:
	CEventEditView_SelectEditType();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CEventEditView_SelectEditType();

public:
	int m_nEditType;

public:
	enum { IDD = IDD_EVENTEDIT_SELECTEDITTYPE };
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
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio6();

	virtual VIEW_TYPE GetNextView();

	virtual bool IsVisivleNextButton();
	virtual bool IsVisivlePrevButton() { return false; }
	virtual bool IsVisivleFinishButton() { return true; }
	virtual bool IsFinishOrCancel() { return false; }
	virtual void PreProcess();
	virtual bool PostProcess();

	virtual void OnInitialUpdate();


};


