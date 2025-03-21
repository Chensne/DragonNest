#pragma once


#include "BaseView.h"
#include "OXShortcutBar.h"
#include "OXLayoutManager.h"
#include "PropertyGrid.h"
#include "OXGridList.h"
#include "OXGridEdit.h"

class CSubMonitorFrameView;

#define	DF_PROPERTYGRID_HITEM_DEFAULT	(static_cast<HITEM>(-1))


// CBaseMonitorView 폼 뷰입니다.

class CBaseMonitorView : public CBaseView
{
	DECLARE_DYNCREATE(CBaseMonitorView)

public:
	enum EF_IMAGELISTTYPE	// 이미지 리스트 타입
	{
		EV_ILT_SMALL,		// SMALL
		EV_ILT_LARGE,		// LARGE
		EV_ILT_CNT,
	};
	enum EF_UI_SIZE			// UI 크기 정의
	{
		EV_UIS_MONITORFRAME_PAD_Y		= 50,
		EV_UIS_MONITORFRAME_PAD_A		= 20,

		EV_UIS_SERVERSTATEGRID_WTH		= 280,
		EV_UIS_SERVERSTATEGRID_HGT		= 260,

		EV_UIS_CHANNELSTATELIST_PAD_Y	= 10,
		EV_UIS_CHANNELSTATELIST_WTH		= 280,
	};
	enum EF_SERVERSTATEITEMTYPE		// 서버 상태 그리드 아이템 타입		(주의 !!!> 서버 상태 속성 그리드의 아이템 추가 순서와 반드시 일치해야 함)
	{
		// Basic Information
		EV_SSI_SERVERTYPE,			// 서버 타입
		EV_SSI_SERVERID,			// 서버 ID
		EV_SSI_NETLUNCHERID,		// 넷런처 ID
		EV_SSI_RSCSVRRVS,			// 리소스 SVN 리비전
		EV_SSI_EXEVERSION,			// 실행파일 버전
		EV_SSI_WORLDID,				// 월드 ID (목록)
		EV_SSI_PUBLICIP,			// Public
		EV_SSI_IPADDRESS,			// IP 주소

		// Current State
		EV_SSI_SERVERSTATE,			// 서버 상태

		// Specific State
		EV_SSI_VISIBILITY,			// 개방 여부 (GA)
		EV_SSI_USERCOUNT,			// 인원 수 (현재/최대) (GA)
		EV_SSI_ROOMCOUNT,			// 방 개수 (현재) (GA)
		EV_SSI_MAXROOMCOUNT,		// 방 개수 (최대) (GA)
		EV_SSI_MAXFARMCOUNT,		// 농장 방 개수 (최대) (GA)
		EV_SSI_TOTALFARMUSERCOUNT,	// 농장 유저수 (최대) (GA)
		EV_SSI_CNT,
	};
	enum EF_CHANNELSTATEITEMTYPE	// 채널 상태 리스트 아이템 타입		(주의 !!!> 채널 상태 그리드 리스트에 컬럼 추가 순서와 반드시 일치해야 함)
	{
		EV_CSI_CHANNELID,			// Channel ID
		EV_CSI_MAPINDEX,			// Map Index
		EV_CSI_USERCOUNT,			// User Count (Cur/Max)
		EV_CSI_ATTRIBUTE,			// Attribute
		EV_CSI_MERITID,				// Merit ID
		EV_CSI_VISIBILITY,			// Visibility
		EV_CSI_LIMITLEVEL,			// Limit Level
		EV_CSI_ISMIDSHOW,			// Middleware Show
		EV_CSI_MIDSID,				// Middleware SID
		EV_CSI_MIDTHREADID,			// Middleware ThreadID
		EV_CSI_CNT,
	};

public:
	CBaseMonitorView();

protected:
//	CBaseMonitorView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CBaseMonitorView();

public:
	enum { IDD = IDD_BASEMNTFRM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	BOOL m_IsInitialized;	// CView 에는 다이얼로그의 WM_INITDIALOG 메시지가 오지 않으므로 OnUpdate() 에서 관리

	COXLayoutManager m_LayoutManager;
	CPropertyGrid m_ServerStateGrid;
	COXGridList	m_ChannelStateList;

	
	CSubMonitorFrameView* m_SubMonitorFrameView;

	HITEM m_ServerStateItem[EV_SSI_CNT];

	int m_LastDispServerID;
	CBasePartitionView* m_pPartitionView;


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	CRect GetFrameViewRect();
	void Clear();

	void SetServerStateGrid(const struct SERVERINFO* pServerInfo);
	void ClearServerStateGrid();

	void AddChannelStateList(const struct CHANNELINFO* pChannelInfo);
	void SetChannelStateList(const struct CHANNELINFO* pChannelInfo);
	void ClearChannelStateList();

	int GetLastDispServerID() const { return m_LastDispServerID; }
	void SetLastDispServerID(int pServerID) { m_LastDispServerID = pServerID; }

	void SetPartitionView(CBasePartitionView* pView);
	CBasePartitionView* GetPartitionView();

	void AddFarmStateList(UINT nFarmDBID, int nCurUserCount, bool Activate);
	void SetFarmStateList(UINT nFarmDBID, int nCurUserCount, bool Activate);
	void SetChannelStateColum(int nServerType);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

