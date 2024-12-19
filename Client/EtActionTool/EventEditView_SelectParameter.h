#pragma once



// CEventEditView_SelectParameter 폼 뷰입니다.

#include "WizardCommon.h"
#include "afxcmn.h"
#include "ColorListCtrl.h"
class CEventEditView_SelectParameter : public CFormView, public CWizardCommon
{
	DECLARE_DYNCREATE(CEventEditView_SelectParameter)

protected:
	CEventEditView_SelectParameter();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CEventEditView_SelectParameter();

	bool m_bActivate;
public:
	enum { IDD = IDD_EVENTEDITVIEW_SELECTPARAMETER };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual VIEW_TYPE GetNextView();

	virtual bool IsVisivleNextButton() { return true; }
	virtual bool IsVisivlePrevButton() { return true; }
	virtual bool IsVisivleFinishButton() { return true; }
	virtual bool IsFinishOrCancel() { return false; }
	virtual void PreProcess();
	virtual bool PostProcess();
	virtual void OnInitialUpdate();
	CColorListCtrl m_ListCtrl;
};


