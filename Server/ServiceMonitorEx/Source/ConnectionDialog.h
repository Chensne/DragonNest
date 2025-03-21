#pragma once

#include "afxcmn.h"
#include "afxwin.h"


// CConnectionDialog 대화 상자입니다.

class CConnectionDialog : public CDialog
{
	DECLARE_DYNAMIC(CConnectionDialog)

public:
	enum EF_UI_SIZE			// UI 크기 정의
	{
		EV_UIS_TITLELINE_1_PAD_LFT	= 110,		// 제목 1 외부 간격 좌측
		EV_UIS_TITLELINE_1_PAD_TOP	= 53,		// 제목 1 외부 간격 상단
		EV_UIS_TITLELINE_1_WTH		= 170,		// 제목 1 너비
		EV_UIS_TITLELINE_1_HGT		= 18,		// 제목 1 높이

		EV_UIS_TITLETEXT_1_PAD_X	= 10,		// 제목 1 외부 간격 X

		EV_UIS_TITLELINE_2_PAD_LFT	= 110,		// 제목 2 외부 간격 좌측
		EV_UIS_TITLELINE_2_PAD_TOP	= 77,		// 제목 2 외부 간격 상단
		EV_UIS_TITLELINE_2_WTH		= 170,		// 제목 2 너비
		EV_UIS_TITLELINE_2_HGT		= 18,		// 제목 2 높이

		EV_UIS_TITLETEXT_2_PAD_X	= 10,		// 제목 2 외부 간격 X
	};

public:
	CConnectionDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CConnectionDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CNNTDLG };

public:
	void EnableButton(BOOL pIsConnect);

	CString GetID() const;
	CString GetPass() const;

	void SaveConfig();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedCnndlgCancel();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

private:
	CIPAddressCtrl m_IpAddress;
	CEdit m_PortNo;
	CEdit m_ID;
	CEdit m_Pass;
public:
	afx_msg void OnClose();
};
