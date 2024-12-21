// BaseMonitorView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "BaseMonitorView.h"
#include "SubMonitorFrameView.h"
#include "ServiceInfo.h"
#include "BasePartitionView.h"


// CBaseMonitorView

IMPLEMENT_DYNCREATE(CBaseMonitorView, CBaseView)

CBaseMonitorView::CBaseMonitorView()
	: CBaseView(CBaseMonitorView::IDD)
{
	m_IsInitialized = FALSE;
	m_SubMonitorFrameView = NULL;
	m_LastDispServerID = EV_SERVERID_DEF;
	m_pPartitionView = NULL;

	for (int aIndex = 0 ; EV_SSI_CNT > aIndex ; ++aIndex) {
		m_ServerStateItem[aIndex] = DF_PROPERTYGRID_HITEM_DEFAULT;
	}
}

CBaseMonitorView::~CBaseMonitorView()
{

}

void CBaseMonitorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVERSTATE_GRID, m_ServerStateGrid);
	DDX_Control(pDX, IDC_CHANNELSTATE_LIST, m_ChannelStateList);
}

BEGIN_MESSAGE_MAP(CBaseMonitorView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CBaseMonitorView 진단입니다.

#ifdef _DEBUG
void CBaseMonitorView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaseMonitorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG




// CBaseMonitorView 메시지 처리기입니다.


int CBaseMonitorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	

	return 0;
}

void CBaseMonitorView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (m_IsInitialized) {
		m_LayoutManager.OnSize(cx,cy);

		if (m_SubMonitorFrameView &&::IsWindow(m_SubMonitorFrameView->GetSafeHwnd()))
			m_SubMonitorFrameView->MoveWindow(GetFrameViewRect(), TRUE);

		CRect aRect(0, 0, cx, cy);
	
		m_ServerStateGrid.MoveWindow(
			aRect.right - EV_UIS_MONITORFRAME_PAD_A - EV_UIS_SERVERSTATEGRID_WTH,
			aRect.top + EV_UIS_MONITORFRAME_PAD_Y,
			EV_UIS_SERVERSTATEGRID_WTH,
			EV_UIS_SERVERSTATEGRID_HGT,
			TRUE
			);

		m_ChannelStateList.MoveWindow(
			aRect.right - EV_UIS_MONITORFRAME_PAD_A - EV_UIS_CHANNELSTATELIST_WTH,
			aRect.top + EV_UIS_MONITORFRAME_PAD_Y + EV_UIS_SERVERSTATEGRID_HGT + EV_UIS_CHANNELSTATELIST_PAD_Y,
			EV_UIS_CHANNELSTATELIST_WTH,
			aRect.bottom - (aRect.top + EV_UIS_MONITORFRAME_PAD_Y + EV_UIS_MONITORFRAME_PAD_A + EV_UIS_SERVERSTATEGRID_HGT + EV_UIS_CHANNELSTATELIST_PAD_Y),
			TRUE
			);
	}
}


CRect CBaseMonitorView::GetFrameViewRect()
{
	CRect aRect;
	GetClientRect(&aRect);

	return CRect(
		aRect.left + EV_UIS_MONITORFRAME_PAD_A,
		aRect.top + EV_UIS_MONITORFRAME_PAD_Y,
		aRect.right - (EV_UIS_MONITORFRAME_PAD_A * 2) - EV_UIS_SERVERSTATEGRID_WTH,
		aRect.bottom - EV_UIS_MONITORFRAME_PAD_A
		);
}

BOOL CBaseMonitorView::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT_VALID(this);

	switch(pMsg->message) {
	case WM_KEYDOWN:
		{
			
		}
		break;
	}

	return CFormView::PreTranslateMessage(pMsg);
}

void CBaseMonitorView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CRect aRect;
	GetClientRect(&aRect);

	if (!m_IsInitialized) {
		m_IsInitialized = TRUE;

		// Monitor Frame View
		{
			static CCreateContext aContext;
			::memset(&aContext, 0, sizeof(aContext));

			aContext.m_pNewViewClass = RUNTIME_CLASS(CSubMonitorFrameView);

			m_SubMonitorFrameView = reinterpret_cast<CSubMonitorFrameView*>(reinterpret_cast<CFrameWnd*>(this)->CreateView(&aContext));
			if (!m_SubMonitorFrameView)
				return;

			m_SubMonitorFrameView->OnInitialUpdate();
			m_SubMonitorFrameView->ShowWindow(SW_NORMAL);
			m_SubMonitorFrameView->MoveWindow(GetFrameViewRect());
		}

		// Server State Property Grid
		{
			int aServerStateItemType = 0;

			// Basic Information
			{
				HSECTION aSection = m_ServerStateGrid.AddSection(_T("Basic Information"));
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Server Type"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Server ID"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("NetLuncher ID"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Resource Revision"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("EXE. Version"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("World ID(s)"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Public IP"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Private IP"), _T("N/A"), false);
			}

			// Current State
			{
				HSECTION aSection = m_ServerStateGrid.AddSection(_T("Current State"));
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Server State"), _T("N/A"), false);
			}

			// Specific State
			{
				HSECTION aSection = m_ServerStateGrid.AddSection(_T("Specific State"));
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Visibility"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("User Count"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Room Count"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Max Room Count"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Max Farm Count"), _T("N/A"), false);
				m_ServerStateItem[aServerStateItemType++] = m_ServerStateGrid.AddStringItem(aSection, _T("Total FarmUserCount"), _T("N/A"), false);
			}

			m_ServerStateGrid.MoveWindow(
				aRect.right - EV_UIS_MONITORFRAME_PAD_A - EV_UIS_SERVERSTATEGRID_WTH,
				aRect.top + EV_UIS_MONITORFRAME_PAD_Y,
				EV_UIS_SERVERSTATEGRID_WTH,
				EV_UIS_SERVERSTATEGRID_HGT,
				TRUE
				);

			m_ServerStateGrid.SetGutterWidth(135);
		}

		// Channel State List
		{
			int aColNo = 0;

			m_ChannelStateList.InsertColumn(aColNo++, _T("CID"), LVCFMT_LEFT, 50);
			m_ChannelStateList.InsertColumn(aColNo++, _T("Map"), LVCFMT_LEFT, 100);
			m_ChannelStateList.InsertColumn(aColNo++, _T("User Count (Cur/Max)"), LVCFMT_LEFT, 140);
			m_ChannelStateList.InsertColumn(aColNo++, _T("Attribute"), LVCFMT_LEFT, 60);
			m_ChannelStateList.InsertColumn(aColNo++, _T("Merit ID"), LVCFMT_LEFT, 60);
			m_ChannelStateList.InsertColumn(aColNo++, _T("Visibility"), LVCFMT_LEFT, 60);
			m_ChannelStateList.InsertColumn(aColNo++, _T("Limit Level"), LVCFMT_LEFT, 70);
			m_ChannelStateList.InsertColumn(aColNo++, _T("Middleware Show"), LVCFMT_LEFT, 110);

			m_ChannelStateList.MoveWindow(
				aRect.right - EV_UIS_MONITORFRAME_PAD_A - EV_UIS_CHANNELSTATELIST_WTH,
				aRect.top + EV_UIS_MONITORFRAME_PAD_Y + EV_UIS_SERVERSTATEGRID_HGT + EV_UIS_CHANNELSTATELIST_PAD_Y,
				EV_UIS_CHANNELSTATELIST_WTH,
				aRect.bottom - (aRect.top + EV_UIS_MONITORFRAME_PAD_Y + EV_UIS_MONITORFRAME_PAD_A + EV_UIS_SERVERSTATEGRID_HGT + EV_UIS_CHANNELSTATELIST_PAD_Y),
				TRUE
				);

			m_ChannelStateList.ShowWindow(false);
		}

		// Layout Manager
		{
			m_LayoutManager.Attach(this);
		}

		PostMessage(WM_NCACTIVATE, TRUE);
	}

}

void CBaseMonitorView::SetChannelStateColum(int nServerType)
{
	// Delete All Column
	LVCOLUMN lvc={0};
	lvc.mask=LVCF_FMT;
	int nColumnCount=0;
	while(m_ChannelStateList.GetColumn(nColumnCount,&lvc))
		nColumnCount++;

	for(int nIndex=0; nIndex<nColumnCount; nIndex++)
		m_ChannelStateList.DeleteColumn(0);


	int nIndex = 0;

	switch (nServerType)
	{
	case EV_SVT_VI:
		{
			m_ChannelStateList.InsertColumn(nIndex++, _T("CID"), LVCFMT_LEFT, 50);
			m_ChannelStateList.InsertColumn(nIndex++, _T("Map"), LVCFMT_LEFT, 100);
			m_ChannelStateList.InsertColumn(nIndex++, _T("User Count (Cur/Max)"), LVCFMT_LEFT, 140);
			m_ChannelStateList.InsertColumn(nIndex++, _T("Attribute"), LVCFMT_LEFT, 60);
			m_ChannelStateList.InsertColumn(nIndex++, _T("Merit ID"), LVCFMT_LEFT, 60);
			m_ChannelStateList.InsertColumn(nIndex++, _T("Visibility"), LVCFMT_LEFT, 60);
			m_ChannelStateList.InsertColumn(nIndex++, _T("Limit Level"), LVCFMT_LEFT, 70);
			m_ChannelStateList.InsertColumn(nIndex++, _T("DB Show"), LVCFMT_LEFT, 110);
			m_ChannelStateList.InsertColumn(nIndex++, _T("DB SID"), LVCFMT_LEFT, 110);
			m_ChannelStateList.InsertColumn(nIndex++, _T("DB ThreadID"), LVCFMT_LEFT, 110);
		}
		break;
	case EV_SVT_GA:
		{
			m_ChannelStateList.InsertColumn(nIndex++, _T("Farm DBID"), LVCFMT_LEFT, 75);
			m_ChannelStateList.InsertColumn(nIndex++, _T("User Count"), LVCFMT_LEFT, 80);
			m_ChannelStateList.InsertColumn(nIndex++, _T("Activate"), LVCFMT_LEFT, 70);
		}
		break;
	default:
		{
			m_ChannelStateList.ShowWindow(false);
			return;
		}
		break;
	}

	m_ChannelStateList.ShowWindow(true);
}

void CBaseMonitorView::Clear()
{
	ClearServerStateGrid();
	
}

void CBaseMonitorView::SetServerStateGrid(const SERVERINFO* pServerInfo)
{

	// Basic Information
	{
		USES_CONVERSION;

		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_SERVERTYPE], g_ServerTypeFullName[pServerInfo->m_ServerType]);
		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_SERVERID], I2T(pServerInfo->m_ServerID));
		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_NETLUNCHERID], I2T(pServerInfo->m_NetLuncherID));
		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_RSCSVRRVS], pServerInfo->m_ResourceRevision);
		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_EXEVERSION], pServerInfo->m_ExeVersion);

		if (EV_WORLDID_DEF != pServerInfo->m_WorldID[0]) {
			TCHAR aWorldID[MAX_PATH*2] = { _T('\0'), };
			for (int aIndex = 0 ; WORLDCOUNTMAX > aIndex ; ++aIndex) {
				if (EV_WORLDID_DEF == pServerInfo->m_WorldID[aIndex]) {
					break;
				}
				if (0 < aIndex) {
					_tcscat_s(aWorldID, _T(", "));
				}
				_tcscat_s(aWorldID, CVarArg<MAX_PATH>(_T("%d"), pServerInfo->m_WorldID[aIndex]));
			}
			m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_WORLDID], aWorldID);
		}
		else {
			m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_WORLDID], _T("N/A"));
		}

		IN_ADDR aInAddr;
		aInAddr.s_addr = pServerInfo->m_PublicIP;
		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_PUBLICIP], CA2CT(inet_ntoa(aInAddr)));
		
		aInAddr.s_addr = pServerInfo->m_IpAddr;
		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_IPADDRESS], CA2CT(inet_ntoa(aInAddr)));
	}

	// Current State
	{
		USES_CONVERSION;

		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_SERVERSTATE], (CHECK_LIMIT(pServerInfo->m_ServerState, EV_SVS_CNT)?(g_ServerStateName[pServerInfo->m_ServerState]):(_T("N/A"))));
	}

	// Specific State
	{
		USES_CONVERSION;

		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_VISIBILITY], _T("N/A"));
		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_USERCOUNT], _T("N/A"));
		m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_ROOMCOUNT], _T("N/A"));
	
		switch(pServerInfo->m_ServerType) {
		case EV_SVT_VI:
			{
				m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_USERCOUNT], I2T(pServerInfo->m_CurUserCount));
			}
			break;
		case EV_SVT_GA:
			{
				m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_VISIBILITY], CVarArg<MAX_PATH>(_T("%s"), (pServerInfo->m_Visibility)?(_T("Show")):(_T("Hide"))));
				m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_USERCOUNT], I2T(pServerInfo->m_CurUserCount));
				m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_ROOMCOUNT], I2T(pServerInfo->m_CurRoomCount));
				m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_MAXROOMCOUNT], I2T(pServerInfo->m_MaxRoomCount));
				m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_MAXFARMCOUNT], I2T(pServerInfo->m_MaxFarmCount));
				m_ServerStateGrid.SetItemValue(m_ServerStateItem[EV_SSI_TOTALFARMUSERCOUNT], I2T(pServerInfo->m_TotalFarmUserCount));
				
			}
			break;
		default:
			{
				
			}
			break;
		}
	}

	
}

void CBaseMonitorView::ClearServerStateGrid()
{
	if (::IsWindow(m_ServerStateGrid.GetSafeHwnd())) {
		for (int aIndex = 0 ; EV_SSI_CNT > aIndex ; ++aIndex) {
			m_ServerStateGrid.SetItemValue(m_ServerStateItem[aIndex], _T("N/A"));
		}
	}
}

void CBaseMonitorView::AddFarmStateList(UINT nFarmDBID, int nCurUserCount, bool bActivate)
{
	LV_ITEM aLvItem;
	aLvItem.mask		= LVIF_TEXT | LVIF_PARAM;
	aLvItem.cchTextMax	= 0;
	aLvItem.lParam		= 0;
	aLvItem.iItem		= m_ChannelStateList.GetItemCount();
	aLvItem.iSubItem	= 0;
	aLvItem.lParam		= nFarmDBID;

	TCHAR sBuffer[MAX_PATH] = { _T('\0'), };

	// Farm DBID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), nFarmDBID);
	aLvItem.pszText = sBuffer;

	int nRtnValue = m_ChannelStateList.InsertItem(&aLvItem);
	if (-1 == nRtnValue)
		return;

	// User Count
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), nCurUserCount);
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Activate
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), (bActivate)?(_T("True")):(_T("False")));
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	
	m_ChannelStateList.UnlockWindowUpdate();
	EndWaitCursor();
}


void CBaseMonitorView::SetFarmStateList(UINT nFarmDBID, int nCurUserCount, bool bActivate)
{
	for (int aIndex = 0 ; m_ChannelStateList.GetItemCount() > aIndex ; ++aIndex) 
	{
		if (nFarmDBID != static_cast<INT>(m_ChannelStateList.GetItemData(aIndex)))
			continue;
		
		TCHAR sBuffer[MAX_PATH] = { _T('\0'), };

		// User Count
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), nCurUserCount);
		m_ChannelStateList.SetItemText(aIndex, 1, sBuffer);

		// Show
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), (bActivate)?(_T("True")):(_T("False")));
		m_ChannelStateList.SetItemText(aIndex, 2, sBuffer);

		break;
	}
}

void CBaseMonitorView::AddChannelStateList(const struct CHANNELINFO* pChannelInfo)
{
	BeginWaitCursor();
	m_ChannelStateList.LockWindowUpdate();

	LV_ITEM aLvItem;
	aLvItem.mask		= LVIF_TEXT | LVIF_PARAM;
	aLvItem.cchTextMax	= 0;
	aLvItem.lParam		= 0;
	aLvItem.iItem		= m_ChannelStateList.GetItemCount();
	aLvItem.iSubItem	= 0;
	aLvItem.lParam		= pChannelInfo->m_ChannelID;	// 추후 빠른 정보 검색을 위해 파라메터로 지정

	TCHAR sBuffer[MAX_PATH] = { _T('\0'), };

	// Channel ID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pChannelInfo->m_ChannelID);
	aLvItem.pszText = sBuffer;

	int nRtnValue = m_ChannelStateList.InsertItem(&aLvItem);
	if (-1 == nRtnValue) return;

	// Map
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), pChannelInfo->m_wszMapName);
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// User Count (Cur/Max)
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d / %d"), pChannelInfo->m_CurUserCount, pChannelInfo->m_MaxUserCount);
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Attribute
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pChannelInfo->m_Attribute);
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Merit ID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pChannelInfo->m_MeritID);
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Visibility
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pChannelInfo->m_Visibility);
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Limit Level
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pChannelInfo->m_LimitLevel);
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Middleware Show
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), (pChannelInfo->m_IsMidShow)?(_T("True")):(_T("False")));
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Middleware SID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), (pChannelInfo->m_nServerID));
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Middleware ThreadID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), (pChannelInfo->m_cThreadID));
	m_ChannelStateList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	m_ChannelStateList.UnlockWindowUpdate();
	EndWaitCursor();

}

void CBaseMonitorView::SetChannelStateList(const struct CHANNELINFO* pChannelInfo)
{
	for (int aIndex = 0 ; m_ChannelStateList.GetItemCount() > aIndex ; ++aIndex) 
	{

		if (pChannelInfo->m_ChannelID != static_cast<INT>(m_ChannelStateList.GetItemData(aIndex))) 	
			continue;

		TCHAR sBuffer[MAX_PATH] = { _T('\0'), };

		// User Count (Cur/Max)
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d / %d"), pChannelInfo->m_CurUserCount, pChannelInfo->m_MaxUserCount);
		m_ChannelStateList.SetItemText(aIndex, EV_CSI_USERCOUNT, sBuffer);

		// Attribute
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pChannelInfo->m_Attribute);
		m_ChannelStateList.SetItemText(aIndex, EV_CSI_ATTRIBUTE, sBuffer);

		// Merit ID
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pChannelInfo->m_MeritID);
		m_ChannelStateList.SetItemText(aIndex, EV_CSI_MERITID, sBuffer);

		// Visibility
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pChannelInfo->m_Visibility);
		m_ChannelStateList.SetItemText(aIndex, EV_CSI_VISIBILITY, sBuffer);

		// Limit Level
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pChannelInfo->m_LimitLevel);
		m_ChannelStateList.SetItemText(aIndex, EV_CSI_LIMITLEVEL, sBuffer);

		// Middleware Show
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), (pChannelInfo->m_IsMidShow)?(_T("True")):(_T("False")));
		m_ChannelStateList.SetItemText(aIndex, EV_CSI_ISMIDSHOW, sBuffer);

		// Middleware SID
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), (pChannelInfo->m_nServerID));
		m_ChannelStateList.SetItemText(aIndex, EV_CSI_MIDSID, sBuffer);

		// Middleware ThreadID
		_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), (pChannelInfo->m_cThreadID));
		m_ChannelStateList.SetItemText(aIndex, EV_CSI_MIDTHREADID, sBuffer);

		break;
	}
}

void CBaseMonitorView::ClearChannelStateList()
{
	m_ChannelStateList.DeleteAllItems();
}

BOOL CBaseMonitorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	cs.style &= ~(WS_HSCROLL | WS_VSCROLL);

	return CFormView::PreCreateWindow(cs);
}


void CBaseMonitorView::SetPartitionView(CBasePartitionView* pView)
{
	m_pPartitionView = pView;

	m_SubMonitorFrameView->SetBaseInfo(m_pPartitionView);
}

CBasePartitionView* CBaseMonitorView::GetPartitionView()
{
	return m_pPartitionView;
}
