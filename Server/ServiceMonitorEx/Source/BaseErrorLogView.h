#pragma once


#include "BaseView.h"
#include "OXLayoutManager.h"
#include "OXGridList.h"
#include "OXGridEdit.h"
#include <sstream>

// CBaseErrorLogView 폼 뷰입니다.

class CBaseErrorLogView : public CBaseView
{
	DECLARE_DYNCREATE(CBaseErrorLogView)

public:
	enum EF_ERRORLOGTYPE	// 에러 로그 타입
	{
		EV_ELT_ITE,			// Service Monitor Internal Error Log
		EV_ELT_ECP,			// Service Exception Log
		EV_ELT_CNT,
	};
	enum EF_UI_SIZE			// UI 크기 정의
	{
		EV_UIS_ERRORLOGVIEW_PAD_A			= 10,

		EV_UIS_ERRORLOGITEVIEW_PAD_Y		= 50,
		EV_UIS_ERRORLOGITEVIEW_HGT			= 200,

		EV_UIS_ERRORLOGECPVIEW_PAD_Y		= 50,

		EV_UIS_ERRORLOGCLEARBUTTON_GAP		= 10,
		EV_UIS_ERRORLOGCLEARBUTTON_WTH		= 80,
		EV_UIS_ERRORLOGCLEARBUTTON_HGT		= 20,

		EV_UIS_TITLELINE_PAD_X				= 10,		// 제목 외부 간격 X
		EV_UIS_TITLELINE_PAD_Y				= 34,		// 제목 외부 간격 Y
		EV_UIS_TITLELINE_HGT				= 18,		// 제목 높이

		EV_UIS_TITLETEXT_PAD_X				= 10,		// 제목 외부 간격 X
	};

public:
	CBaseErrorLogView();

protected:
//	CBaseErrorLogView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CBaseErrorLogView();

public:
	enum { IDD = IDD_BASEERLFRM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	void DrawTitle(CDC* pDC);

	void ClearGridList(EF_ERRORLOGTYPE pType) { ASSERT(0 <= pType && EV_ELT_CNT > pType); m_GridList[pType].DeleteAllItems(); }

	void SetAlert(BOOL pDoAlert) { m_DoAlert = pDoAlert; }

public:
 	void AddGridList(INT pErrorCode, LPCTSTR pMsg, BOOL pDoAlert);
 	void AddGridList(INT pServerID, CHAR pServerType, INT nWorldID[WORLDCOUNTMAX], EF_SERVERSTATE pServerState, INT pExceptionCode, LPCTSTR pDetail, BOOL pDoAlert);
	void ShowOperatingResult (INT nCmd, INT nRet);

	BOOL IsAlert() const { return m_DoAlert; }

	void SetPartitionView(CBasePartitionView* pView);
	CBasePartitionView* GetPartitionView();

	void RecvReportData(const wchar_t* szReport);
	void SaveToClipboard();

private:
	BOOL m_IsInitialized;	// CView 에는 다이얼로그의 WM_INITDIALOG 메시지가 오지 않으므로 OnUpdate() 에서 관리

	CImageList m_ImageList;
	COXLayoutManager m_LayoutManager;
 	CListCtrl	m_GridList[EV_ELT_CNT];


	WNDPROC m_OldGridListSubProc[EV_ELT_CNT];

	int m_TabIconTick;
	BOOL m_DoAlert;

	CBasePartitionView* m_pPartitionView;

	BOOL m_IsClose;

	std::wstringstream m_Report;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedErrIteListClear();
	afx_msg void OnBnClickedErrEcpListClear();
	afx_msg void OnPaint();
	afx_msg void OnCustomDrawErrIteList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDrawErrEcpList(NMHDR* pNMHDR, LRESULT* pResult);
};

