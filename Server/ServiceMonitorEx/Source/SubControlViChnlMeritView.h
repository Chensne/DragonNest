#pragma once

#include "Define.h"
#include "ServiceInfo.h"
#include "SubControlBaseView.h"
#include "OXGridList.h"
#include "OXGridEdit.h"
#include <map>
#include <vector>


// CSubControlViChnlMeritView ��ȭ �����Դϴ�.

class CSubControlViChnlMeritView : public CSubControlBaseView
{
	DECLARE_DYNAMIC(CSubControlViChnlMeritView)

public:
	CSubControlViChnlMeritView(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSubControlViChnlMeritView();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CTRLFRM_VICHNLMERIT };

public:
	void UpdateData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
