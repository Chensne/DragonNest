#pragma once

#include "Define.h"
#include "ServiceInfo.h"
#include "SubControlBaseView.h"
#include "OXGridList.h"
#include "OXGridEdit.h"
#include <map>
#include <vector>


// CSubControlWorldUserLimitView 대화 상자입니다.

class CSubControlWorldUserLimitView : public CSubControlBaseView
{
	DECLARE_DYNAMIC(CSubControlWorldUserLimitView)

public:
	CSubControlWorldUserLimitView(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSubControlWorldUserLimitView();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CTRLFRM_WORLDUSERLIMIT };

public:
	void UpdateData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
