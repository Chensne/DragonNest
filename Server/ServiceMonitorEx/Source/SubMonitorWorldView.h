#pragma once

#include "Define.h"
#include "ServiceInfo.h"
#include "CriticalSection.hpp"
#include "ControlDialog.h"
#include "BasePartitionView.h"
#include <map>
#include <vector>

class CSubMonitorFrameView;

// SERVERDRAW

typedef struct SERVERDRAW : public SERVERINFO
{

public:
	typedef	SERVERINFO	TP_SUPER;

public:
	SERVERDRAW(const SERVERINFO& pServerInfo);

	VOID Reset(BOOL pIsAll = TRUE);

public:
	CRect m_Area;		// 위치 영역
	BOOL m_IsPick;		// 선택 여부
	EF_SERVERSTATE m_OldServerState;	// 서버 상태

} *LPSERVERDRAW, * const LPCSERVERDRAW;


// SERVERTYPEPICK

typedef struct SERVERTYPEPICK
{

public:
	SERVERTYPEPICK();

	VOID Reset();

public:
	CRect m_Area;		// 위치 영역
	BOOL m_IsPick;		// 선택 여부

} *LPSERVERTYPEPICK, * const LPCSERVERTYPEPICK;


// CSubMonitorWorldView 뷰입니다.

class CSubMonitorWorldView : public CScrollView
{
	DECLARE_DYNCREATE(CSubMonitorWorldView)

public:
	enum EF_UI_SIZE		// UI 크기 정의
	{
		EV_UIS_NOSERVERLIST_HGT		= 60,		// '서버 목록 없음' 표시 너비
	};

public:

	// P.S.> 화면 표시와 정보 관리의 기준은 서버임 (TP_LISTSVTP)

	// WORLD
	typedef	std::map<INT, WORLDINFO>			TP_LISTWRLD;
	typedef	TP_LISTWRLD::iterator				TP_LISTWRLD_ITR;
	typedef	TP_LISTWRLD::const_iterator			TP_LISTWRLD_CTR;
	typedef	std::vector<WORLDINFO>				TP_LISTWRLDAUTO;
	typedef	TP_LISTWRLDAUTO::iterator			TP_LISTWRLDAUTO_ITR;
	typedef	TP_LISTWRLDAUTO::const_iterator		TP_LISTWRLDAUTO_CTR;

	// SERVER
	typedef	std::map<INT, SERVERDRAW>			TP_LISTSERV;
	typedef	TP_LISTSERV::iterator				TP_LISTSERV_ITR;
	typedef	TP_LISTSERV::const_iterator			TP_LISTSERV_CTR;
	typedef std::vector<SERVERDRAW>				TP_LISTSERVAUTO;
	typedef	TP_LISTSERVAUTO::iterator			TP_LISTSERVAUTO_ITR;
	typedef	TP_LISTSERVAUTO::const_iterator		TP_LISTSERVAUTO_CTR;

	typedef std::vector<EF_SERVERTYPE>			TP_LISTSVTP;
	typedef	TP_LISTSVTP::iterator				TP_LISTSVTP_ITR;
	typedef	TP_LISTSVTP::const_iterator			TP_LISTSVTP_CTR;

	// CHANNEL (VI)
	typedef	std::map<DWORD64, CHANNELINFO>		TP_LISTCHNL;
	typedef	TP_LISTCHNL::iterator				TP_LISTCHNL_ITR;
	typedef	TP_LISTCHNL::const_iterator			TP_LISTCHNL_CTR;
	typedef	std::vector<CHANNELINFO>			TP_LISTCHNLAUTO;
	typedef	TP_LISTCHNLAUTO::iterator			TP_LISTCHNLAUTO_ITR;
	typedef	TP_LISTCHNLAUTO::const_iterator		TP_LISTCHNLAUTO_CTR;

protected:
	CSubMonitorWorldView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CSubMonitorWorldView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	void Set(INT nWorldID, INT pPageIndex);

	BOOL IsAlert() const { return m_DoAlert; }

	INT GetWorldID() const { return m_WorldID; }
	INT GetPageIndex() const { return m_PageIndex; }
	LONG64 GetServiceInfoUpdateNo() const { return m_ServiceInfoUpdateNo; }
	void SetServiceInfoUpdateNo(LONG64 pServiceInfoUpdateNo) { m_ServiceInfoUpdateNo = pServiceInfoUpdateNo; }
	INT GetWorldUserCount() const { return m_WorldUserCount; }

	// WORLD
	WORLDINFO* AtWorld(INT nWorldID);
	const WORLDINFO* AtWorld(INT nWorldID) const;
	void GetWorldList(TP_LISTWRLDAUTO& pListWrld);
	BOOL IsWorldEmpty() const { return(m_ListWrld.empty()); }

	// SERVER
	SERVERDRAW* AtServer(INT pServerID);
	const SERVERDRAW* AtServer(INT pServerID) const;
	void GetServerList(TP_LISTSERVAUTO& pListServ, EF_SERVERTYPE pServerType /*= EV_SVT_NONE*/, BOOL pIsPick /*= FALSE*/);
	BOOL IsServerEmpty() const { return(m_ListServ.empty()); }

	// CHANNEL (VI)
	CHANNELINFO* AtChannel(INT pServerID, INT pChannelID);
	const CHANNELINFO* AtChannel(INT pServerID, INT pChannelID) const;
	void GetChannelList(TP_LISTCHNLAUTO& pListChnl, INT pServerID /*= EV_SERVERID_DEF*/);
	BOOL IsChannelEmpty() const { return(m_ListChnl.empty()); }

	void PickServerAll(BOOL pDoPick);

	void SetBaseInfo (CBasePartitionView* pMainView, CSubMonitorFrameView* pSubView, CServiceInfo* pInfo);

private:
	void RebuildServiceInfo();
	void RefreshServiceInfo();

	// WORLD
	void AddWorldInfo(const WORLDINFO* pWorldInfo);
	void SetWorldInfo(const WORLDINFO* pWorldInfo);
	void ClearWorldInfoList();

	// SERVER
	void AddServerInfo(const SERVERINFO* pServerInfo);
	void SetServerInfo(const SERVERINFO* pServerInfo);
	void ClearServerInfoList();

	// CHANNEL (VI)
	void AddChannelInfo(const CHANNELINFO* pChannelInfo);
	void SetChannelInfo(const CHANNELINFO* pChannelInfo);
	void ClearChannelInfoList();

	void DrawView(CDC* pDC, const CRect& pRect, CPoint& pStartPoint, int* pHeight = NULL);

	void DoPopupMenu();

	void DisplayServerState(INT pServerID, BOOL pIsNew = FALSE);

	void UpdateControlDialog(class CControlDialog* pControlDialog);

private:
	BOOL m_IsInitialized;	// CView 에는 다이얼로그의 WM_INITDIALOG 메시지가 오지 않으므로 OnUpdate() 에서 관리

	INT m_WorldID;
	LONG64 m_ServiceInfoUpdateNo;
	INT m_WorldUserCount;		// 월드 내의 현재 전체 동시 접속자 수

	INT m_LastPickServerID;		// 사용자가 가장 최근 선택한 서버 ID

	class CServerDrawBaseFilter* m_ServerDrawFilter[EV_SVT_CNT];

	TP_LISTWRLD m_ListWrld;
	TP_LISTSERV m_ListServ;
	TP_LISTCHNL m_ListChnl;
//	mutable TP_LOCK m_Lock;				// 동기화 불필요 ?

	SERVERTYPEPICK m_ServerTypePick[EV_SVT_CNT];

	INT m_PageIndex;

	int m_TabIconTick;
	BOOL m_DoAlert;
//	BOOL m_PlayAlert;

	CServiceInfo* m_pServiceInfo;
	CBasePartitionView* m_pPartitionView;
	CSubMonitorFrameView* m_pSubMoniterFrameView;

	COX3DTabViewContainer m_TestTabView;


protected:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.

	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWorldViewSelectAll();
	afx_msg void OnPopupProcOnOff();
	afx_msg void OnPopupWorldUserLimit();
	afx_msg void OnPopupViChnlShowHide();
	afx_msg void OnPopupViChnlMerit();
	afx_msg void OnPopupGaShowHide();
	afx_msg void OnPopupGaMerit();
	afx_msg void OnPopupProcMakeDump();
};


