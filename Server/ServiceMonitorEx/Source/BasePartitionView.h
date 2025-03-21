
#pragma once


#include "BaseView.h"
#include "NetConnection.h"
#include "PartitionDialog.h"

#include "MainTaskbarIcon.h"
#include "ControlDialog.h"

#include "ConfigMngr.h"
#include "OXToolTipCtrl.h"

class CBaseMonitorView;
class CBaseInformationView;
class CBaseErrorLogView;

class CBasePartitionView : public CScrollView
{
	DECLARE_DYNCREATE(CBasePartitionView)	
public:
	enum EF_IMAGELISTTYPE	// 이미지 리스트 타입
	{
		EV_ILT_SMALL,		// SMALL
		EV_ILT_LARGE,		// LARGE
		EV_ILT_CNT,
	};

	enum EF_STATUSBUTTONTYPE	// 상태 버튼 타입
	{
		EV_SBT_CONNECT,		// CONNECT
		EV_SBT_ERRORLOG,	// ERRORLOG
		EV_SBT_CNT,
	};

	enum PARTITION_DLG_SIZE		// 파티션 다이얼로그 사이즈
	{
		PDS_BOTTOM = 80,
	};
public:
	CBasePartitionView();

protected:
	virtual ~CBasePartitionView();
public:
	enum { IDD = IDD_BASEPATFRM };

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif



public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CRect GetFrameViewRect();
	void Close ();
	void Start (CNetConnection* pConnection);
	CNetConnection* GetNetConnection ();

	void SetConnectionID (ULONG nID);
	ULONG GetConnectionID ();

	CBaseMonitorView* GetMoniterView ();
	CBaseInformationView* GetInformationView ();
	CBaseErrorLogView* GetErrorLogView ();

	void SetPageImageIndex(int pPageIndex, int pImageIndex);
	int GetActivePageIndex() const;

	CServiceInfo* GetServiceInfo() {return m_NetConnection->GetSession()->GetServiceInfo();}

	void SetHideControlDialog(BOOL pDoHide) { m_DoHideControlDialog = pDoHide; }

	void SetActiveErrorLog ();

	CPartitionDlg* GetPartitionDlg () {return m_pPartitionDlg;}

	void ForceAllTabViewActivate();

	void Refreshworldinfo();

	void Set(INT nPageIndex) { m_nPageIndex = nPageIndex; }
	INT GetPageIndex() const { return m_nPageIndex; }

	void ShowReportStatus(int nPage, int nMaxPage);
	
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	
	
private:
	void InitUpdate ();
	void CreateAllView ();
	DWORD GetTickTerm(DWORD dwOldTick, DWORD dwCurTick);
	void OnRequestServiceInfo (CNetSession* pSession);
	void OnRefreshServiceStatus (CNetSession* pSession);


private:
	CNetConnection*			m_NetConnection;
	COX3DTabViewContainer	m_MainTabView;
	COXToolTipCtrl			m_ToolTip;
	CImageList				m_ImageList[EV_ILT_CNT];
	int						m_TabIconTick[EV_SBT_CNT];
	CPartitionDlg*			m_pPartitionDlg;

	CBaseMonitorView*		m_pMonitorView;
	CBaseInformationView*	m_pInformationView;
	CBaseErrorLogView*		m_pErrorLogView;
	BOOL					m_DoAlert;
	BOOL					m_IsInitialized;

	ULONG					m_ConnectionID;

	BOOL					m_DoHideControlDialog;
	BOOL					m_IsSized;
	DWORD					m_RefreshWorldInfoTick;
	CCreateContext*			m_pCreateContext;

	INT						m_nPageIndex;	
};

