// SubMonitorWorldView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "SubMonitorWorldView.h"
#include "SubMonitorFrameView.h"
#include "ServerDrawFilter.h"
#include "BaseMonitorView.h"
#include "MainFrm.h"
#include "ConfigMngr.h"


#pragma comment(lib, "winmm.lib")


// SERVERDRAW

SERVERDRAW::SERVERDRAW(const SERVERINFO& pServerInfo) : SERVERINFO(pServerInfo)
{
	Reset(FALSE);
}

VOID SERVERDRAW::Reset(BOOL pIsAll)
{
	if (pIsAll) {
		TP_SUPER::Reset();
	}

	m_Area = 0;
	m_IsPick = FALSE;
	m_OldServerState = EV_SVS_NONE;
}

// SERVERTYPEPICK

SERVERTYPEPICK::SERVERTYPEPICK()
{
	Reset();
}

void SERVERTYPEPICK::Reset()
{
	m_Area = 0;
	m_IsPick = FALSE;
}


// CSubMonitorWorldView

IMPLEMENT_DYNCREATE(CSubMonitorWorldView, CScrollView)

CSubMonitorWorldView::CSubMonitorWorldView() : m_ServiceInfoUpdateNo(EV_SVCINFOUPD_DEF), m_WorldID(EV_WORLDID_DEF), m_LastPickServerID(EV_SERVERID_DEF), m_PageIndex(EV_PAGEIDX_DEF), m_TabIconTick(0), m_DoAlert(FALSE), m_WorldUserCount(0)
{
	m_IsInitialized = FALSE;

	::memset(m_ServerDrawFilter, 0, sizeof(m_ServerDrawFilter));
	m_ServerDrawFilter[EV_SVT_LO] = static_cast<CServerDrawBaseFilter*>(new CServerDrawDervFilter<EV_SVT_LO>());
	m_ServerDrawFilter[EV_SVT_DB] = static_cast<CServerDrawBaseFilter*>(new CServerDrawDervFilter<EV_SVT_DB>());
	m_ServerDrawFilter[EV_SVT_MA] = static_cast<CServerDrawBaseFilter*>(new CServerDrawDervFilter<EV_SVT_MA>());
	m_ServerDrawFilter[EV_SVT_VI] = static_cast<CServerDrawBaseFilter*>(new CServerDrawDervFilter<EV_SVT_VI>());
	m_ServerDrawFilter[EV_SVT_GA] = static_cast<CServerDrawBaseFilter*>(new CServerDrawDervFilter<EV_SVT_GA>());
	m_ServerDrawFilter[EV_SVT_LG] = static_cast<CServerDrawBaseFilter*>(new CServerDrawDervFilter<EV_SVT_LG>());
	m_ServerDrawFilter[EV_SVT_CA] = static_cast<CServerDrawBaseFilter*>(new CServerDrawDervFilter<EV_SVT_CA>());

	for (int aIndex = 0 ; EV_SVT_CNT > aIndex ; ++aIndex) {
		m_ServerTypePick[aIndex].Reset();
	}

	m_pServiceInfo = NULL;
}

CSubMonitorWorldView::~CSubMonitorWorldView()
{
	for (int aIndex = 0 ; EV_SVT_CNT > aIndex ; ++aIndex) {
		SAFE_DELETE(m_ServerDrawFilter[aIndex]);
	}
}


BEGIN_MESSAGE_MAP(CSubMonitorWorldView, CScrollView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_POPUP_PROCONOFF, &CSubMonitorWorldView::OnPopupProcOnOff)
	ON_COMMAND(ID_POPUP_WORLDUSERLIMIT, &CSubMonitorWorldView::OnPopupWorldUserLimit)
	ON_COMMAND(ID_POPUP_VICHNLSHOWHIDE, &CSubMonitorWorldView::OnPopupViChnlShowHide)
	ON_COMMAND(ID_POPUP_VICHNLMERIT, &CSubMonitorWorldView::OnPopupViChnlMerit)
	ON_COMMAND(ID_POPUP_GASHOWHIDE, &CSubMonitorWorldView::OnPopupGaShowHide)
	ON_COMMAND(ID_POPUP_GAMERIT, &CSubMonitorWorldView::OnPopupGaMerit)
	ON_COMMAND(ID_POPUP_PROCMAKEDUMP, &CSubMonitorWorldView::OnPopupProcMakeDump)
END_MESSAGE_MAP()


// CSubMonitorWorldView 그리기입니다.

void CSubMonitorWorldView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

}

void CSubMonitorWorldView::OnDraw(CDC* /*pDC*/)
{
	CDocument* pDoc = GetDocument();
	UNUSED_ALWAYS(pDoc);

	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CSubMonitorWorldView 진단입니다.

#ifdef _DEBUG
void CSubMonitorWorldView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CSubMonitorWorldView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


void CSubMonitorWorldView::Set(INT nWorldID, INT pPageIndex)
{
	ASSERT(EV_PAGEIDX_DEF != pPageIndex);

	m_WorldID = nWorldID;
	m_PageIndex = pPageIndex;

	SetTimer(EV_TMD_REFRESHSERVICEINFO, g_TimerInterval[EV_TMD_REFRESHSERVICEINFO - EV_TIMERID_DEF], NULL);
	SetTimer(EV_TMD_SHOWSERVERSTATE, g_TimerInterval[EV_TMD_SHOWSERVERSTATE - EV_TIMERID_DEF], NULL);
}

WORLDINFO* CSubMonitorWorldView::AtWorld(INT nWorldID)
{
	TP_LISTWRLD_ITR aIt = m_ListWrld.find(nWorldID);
	if (m_ListWrld.end() != aIt) {
		return(static_cast<WORLDINFO*>(&aIt->second));
	}

	return NULL;
}

const WORLDINFO* CSubMonitorWorldView::AtWorld(INT nWorldID) const
{
	return(static_cast<const WORLDINFO*>(const_cast<CSubMonitorWorldView*>(this)->AtWorld(nWorldID)));
}

void CSubMonitorWorldView::GetWorldList(TP_LISTWRLDAUTO& pListWrld)
{
	TP_LISTWRLD_CTR aIt = m_ListWrld.begin();
	for (; m_ListWrld.end() != aIt ; ++aIt) {
		const WORLDINFO* aWorldInfo = (&aIt->second);
		if (!aWorldInfo) return;
		if(!aWorldInfo->IsSet()) return;

		pListWrld.push_back((*aWorldInfo));
	}
}

SERVERDRAW* CSubMonitorWorldView::AtServer(INT pServerID)
{
	TP_LISTSERV_ITR aIt = m_ListServ.find(pServerID);
	if (m_ListServ.end() != aIt) {
		return(static_cast<SERVERDRAW*>(&aIt->second));
	}

	return NULL;
}

const SERVERDRAW* CSubMonitorWorldView::AtServer(INT pServerID) const
{
	return(static_cast<const SERVERDRAW*>(const_cast<CSubMonitorWorldView*>(this)->AtServer(pServerID)));
}

void CSubMonitorWorldView::GetServerList(TP_LISTSERVAUTO& pListServ, EF_SERVERTYPE pServerType, BOOL pIsPick)
{
	ASSERT(EV_SVT_NONE <= pServerType && EV_SVT_CNT > pServerType);

	TP_LISTSERV_CTR aIt = m_ListServ.begin();
	for (; m_ListServ.end() != aIt ; ++aIt) {
		const SERVERDRAW* aServerDraw = (&aIt->second);
		if (!aServerDraw)
			continue;

		if (!aServerDraw->IsSet())
			continue;

		if (EV_SVT_NONE != pServerType && aServerDraw->m_ServerType != pServerType)
			continue;
		

		if (pIsPick && !aServerDraw->m_IsPick)
			continue;
		
		pListServ.push_back((*aServerDraw));
	}
}

CHANNELINFO* CSubMonitorWorldView::AtChannel(INT pServerID, INT pChannelID)
{
	TP_LISTCHNL_ITR aIt = m_ListChnl.find(MAKE_SM_SVRCHN(pServerID, pChannelID));
	if (m_ListChnl.end() != aIt)
		return(static_cast<CHANNELINFO*>(&aIt->second));
	
	return NULL;
}

const CHANNELINFO* CSubMonitorWorldView::AtChannel(INT pServerID, INT pChannelID) const
{
	return(static_cast<const CHANNELINFO*>(const_cast<CSubMonitorWorldView*>(this)->AtChannel(pServerID, pChannelID)));
}

void CSubMonitorWorldView::GetChannelList(TP_LISTCHNLAUTO& pListChnl, INT pServerID)
{
	TP_LISTCHNL_CTR aIt = m_ListChnl.begin();
	for (; m_ListChnl.end() != aIt ; ++aIt) {
		const CHANNELINFO* aChannelInfo = (&aIt->second);
		if (!aChannelInfo) return;
		if(!aChannelInfo->IsSet()) return;

		if (EV_SERVERID_DEF != pServerID &&
			aChannelInfo->m_ServerID != pServerID)
		{
			continue;
		}

		pListChnl.push_back((*aChannelInfo));
	}
}

void CSubMonitorWorldView::RebuildServiceInfo()
{
	LONG64 aServiceInfoUpdateNo = 0;

	m_LastPickServerID = EV_SERVERID_DEF;

	CServiceInfo::TP_LISTWRLDAUTO aListWrld;
	CServiceInfo::TP_LISTSERVAUTO aListServ;
	CServiceInfo::TP_LISTCHNLAUTO aListChnl;
	{
		CServiceInfo::TP_LOCKAUTO AutoLock(m_pServiceInfo->GetLock());

		m_pServiceInfo->GetWorldList(EV_SMT_REAL, aListWrld, FALSE);
		m_pServiceInfo->GetServerList(EV_SMT_REAL, aListServ, GetWorldID(), FALSE);

		CServiceInfo::TP_LISTSERVAUTO_CTR aIt = aListServ.begin();
		for (; aListServ.end() != aIt ; ++aIt) {
			const SERVERINFO* aServerInfo = static_cast<const SERVERINFO*>(&(*aIt));
			if (!aServerInfo) return;
			if(!aServerInfo->IsSet()) return;

			m_pServiceInfo->GetChannelList(EV_SMT_REAL, aListChnl, aServerInfo->m_ServerID, FALSE);
		}

		aServiceInfoUpdateNo = m_pServiceInfo->GetServiceInfoUpdateNo();
	}

	if (aListServ.empty()) {	// 정보 관리의 기준은 서버임
		return;
	}

	ClearWorldInfoList();
	ClearServerInfoList();
	ClearChannelInfoList();

	{
		CServiceInfo::TP_LISTWRLDAUTO_CTR aIt = aListWrld.begin();
		for (; aListWrld.end() != aIt ; ++aIt) {
			AddWorldInfo(&(*aIt));
		}
	}
	{
		CServiceInfo::TP_LISTSERVAUTO_CTR aIt = aListServ.begin();
		for (; aListServ.end() != aIt ; ++aIt) {
			AddServerInfo(&(*aIt));
		}
	}
	{
		CServiceInfo::TP_LISTCHNLAUTO_CTR aIt = aListChnl.begin();
		for (; aListChnl.end() != aIt ; ++aIt) {
			AddChannelInfo(&(*aIt));
		}
	}
}

void CSubMonitorWorldView::RefreshServiceInfo()
{
	m_WorldUserCount = 0;

	CServiceInfo::TP_LISTWRLDAUTO aListWrld;
	CServiceInfo::TP_LISTSERVAUTO aListServ;
	CServiceInfo::TP_LISTCHNLAUTO aListChnl;
	{
		CServiceInfo::TP_LOCKAUTO AutoLock(m_pServiceInfo->GetLock());

		m_pServiceInfo->GetWorldList(EV_SMT_REAL, aListWrld, FALSE);
		m_pServiceInfo->GetServerList(EV_SMT_REAL, aListServ, GetWorldID(), FALSE);

		CServiceInfo::TP_LISTSERVAUTO_CTR aIt = aListServ.begin();
		for (; aListServ.end() != aIt ; ++aIt) {
			const SERVERINFO* aServerInfo = static_cast<const SERVERINFO*>(&(*aIt));
			if (!aServerInfo) return;
			if(!aServerInfo->IsSet()) return;

			switch(aServerInfo->m_ServerType)
			{
			case EV_SVT_VI:
			case EV_SVT_GA:
				{
					m_WorldUserCount += aServerInfo->m_CurUserCount;
				}
				break;
			}

			m_pServiceInfo->GetChannelList(EV_SMT_REAL, aListChnl, aServerInfo->m_ServerID, FALSE);
		}
	}

	if (aListServ.empty()) {	// 정보 관리의 기준은 서버임
		return;
	}

	{
		CServiceInfo::TP_LISTWRLDAUTO_CTR aIt = aListWrld.begin();
		for (; aListWrld.end() != aIt ; ++aIt) {
			SetWorldInfo(&(*aIt));
		}
	}
	{
		CServiceInfo::TP_LISTSERVAUTO_CTR aIt = aListServ.begin();
		for (; aListServ.end() != aIt ; ++aIt) {
			SetServerInfo(&(*aIt));
		}
	}
	{
		CServiceInfo::TP_LISTCHNLAUTO_CTR aIt = aListChnl.begin();
		for (; aListChnl.end() != aIt ; ++aIt) {
			SetChannelInfo(&(*aIt));
		}
	}
}

void CSubMonitorWorldView::PickServerAll(BOOL pDoPick)
{
	if (IsServerEmpty()) {
		return;
	}

	for (int aIndex = 0 ; EV_SVT_CNT > aIndex ; ++aIndex) {
		m_ServerTypePick[aIndex].m_IsPick = pDoPick;
	}

	TP_LISTSERV_ITR aIt = m_ListServ.begin();
	for (; m_ListServ.end() != aIt ; ++aIt) {
		SERVERDRAW* aServerDraw = &(aIt->second);
		if (!aServerDraw)
			continue;

		aServerDraw->m_IsPick = pDoPick;
	}

	Invalidate(TRUE);
}

void CSubMonitorWorldView::SetBaseInfo (CBasePartitionView* pMainView, CSubMonitorFrameView* pSubView, CServiceInfo* pInfo)
{
	m_pPartitionView = pMainView;
	m_pSubMoniterFrameView = pSubView;
	m_pServiceInfo = pInfo;
}

void CSubMonitorWorldView::AddWorldInfo(const WORLDINFO* pWorldInfo)
{
	if (!pWorldInfo)
		return;

	if (!pWorldInfo->IsSet())
		return;

	m_ListWrld.insert(TP_LISTWRLD::value_type(pWorldInfo->m_WorldID, (*pWorldInfo)));
}

void CSubMonitorWorldView::SetWorldInfo(const WORLDINFO* pWorldInfo)
{
	if (!pWorldInfo)
		return;

	if (!pWorldInfo->IsSet())
		return;

	TP_LISTWRLD_ITR aIt = m_ListWrld.find(pWorldInfo->m_WorldID);
	if (m_ListWrld.end() == aIt) {
		return;
	}
	WORLDINFO* aWorldInfo = &(aIt->second);
	if (!aWorldInfo) return;
	if(!aWorldInfo->IsSet()) return;

	aWorldInfo->SetChangeable(pWorldInfo);
}

void CSubMonitorWorldView::ClearWorldInfoList()
{
	m_ListWrld.clear();
}

void CSubMonitorWorldView::AddServerInfo(const SERVERINFO* pServerInfo)
{
	if (!pServerInfo) return;
	if(!pServerInfo->IsSet()) return;

	m_ListServ.insert(TP_LISTSERV::value_type(pServerInfo->m_ServerID, (*pServerInfo)));
}

void CSubMonitorWorldView::SetServerInfo(const SERVERINFO* pServerInfo)
{
	if (!pServerInfo) return;
	if(!pServerInfo->IsSet()) return;

	TP_LISTSERV_ITR aIt = m_ListServ.find(pServerInfo->m_ServerID);
	if (m_ListServ.end() == aIt) {
		return;
	}
	SERVERDRAW* aServerDraw = &(aIt->second);
	if (!aServerDraw)
		return;

	if(!aServerDraw->IsSet())
		return;

	aServerDraw->SetChangeable(pServerInfo);
}

void CSubMonitorWorldView::ClearServerInfoList()
{
	m_ListServ.clear();
}

void CSubMonitorWorldView::AddChannelInfo(const CHANNELINFO* pChannelInfo)
{
	if (!pChannelInfo) return;
	if(!pChannelInfo->IsSet()) return;

	m_ListChnl.insert(TP_LISTCHNL::value_type(MAKE_SM_SVRCHN(pChannelInfo->m_ServerID, pChannelInfo->m_ChannelID), (*pChannelInfo)));
}

void CSubMonitorWorldView::SetChannelInfo(const CHANNELINFO* pChannelInfo)
{
	if (!pChannelInfo) return;
	if(!pChannelInfo->IsSet()) return;

	TP_LISTCHNL_ITR aIt = m_ListChnl.find(MAKE_SM_SVRCHN(pChannelInfo->m_ServerID, pChannelInfo->m_ChannelID));
	if (m_ListChnl.end() == aIt) {
		return;
	}
	CHANNELINFO* aChannelInfo = &(aIt->second);
	if (!aChannelInfo) return;
	if(!aChannelInfo->IsSet()) return;

	aChannelInfo->SetChangeable(pChannelInfo);
}

void CSubMonitorWorldView::ClearChannelInfoList()
{
	m_ListChnl.clear();
}

void CSubMonitorWorldView::DrawView(CDC* pDC, const CRect& pRect, CPoint& pStartPoint, int* pHeight)
{
	if(!pDC) return;

	if (IsServerEmpty()) {
		for (int aIndex = 0 ; EV_SVT_CNT > aIndex ; ++aIndex) {
			m_ServerTypePick[aIndex].Reset();
		}

		// Draw Line
		{
			CRect aRect(pRect.left, (((pRect.bottom - pRect.top) - EV_UIS_NOSERVERLIST_HGT) / 2), pRect.right, ((((pRect.bottom - pRect.top) - EV_UIS_NOSERVERLIST_HGT) / 2) + EV_UIS_NOSERVERLIST_HGT));

			pDC->FillRect(&aRect, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DSHADOW)));
		}

		// Draw Text
		{
			LOGFONT aLogFont;
			::memset(&aLogFont, 0x00, sizeof(aLogFont));
			_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
			aLogFont.lfCharSet = DEFAULT_CHARSET;
			aLogFont.lfHeight = 45;
			aLogFont.lfWeight = FW_ULTRABOLD;

			CFont aNewFont;
			VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
			CFont* aOldFont = pDC->SelectObject(&aNewFont);

			CRect aRect(pRect.left, (((pRect.bottom - pRect.top) - EV_UIS_NOSERVERLIST_HGT) / 2), pRect.right, ((((pRect.bottom - pRect.top) - EV_UIS_NOSERVERLIST_HGT) / 2) + EV_UIS_NOSERVERLIST_HGT));
			
			COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
			int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

			pDC->DrawText(_T("No Server List"), &aRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			pDC->SetTextColor(aOldTxColor);
			pDC->SetBkMode(aOldBkMode);

			pDC->SelectObject(aOldFont);
			aNewFont.DeleteObject();
		}

		pStartPoint.x = EV_SERVERDRAW_START_X;
		pStartPoint.y += ((((pRect.bottom - pRect.top) - EV_UIS_NOSERVERLIST_HGT) / 2) + EV_UIS_NOSERVERLIST_HGT);
		if (pHeight) {
			(*pHeight) += ((((pRect.bottom - pRect.top) - EV_UIS_NOSERVERLIST_HGT) / 2) + EV_UIS_NOSERVERLIST_HGT);
		}

		return;
	}

	TP_LISTSVTP aListType;

	switch(GetWorldID()) {
	case EV_WORLDID_ALL:
		{
			aListType.push_back(EV_SVT_LO);
			aListType.push_back(EV_SVT_MA);
			aListType.push_back(EV_SVT_VI);
			aListType.push_back(EV_SVT_GA);
			aListType.push_back(EV_SVT_DB);
			aListType.push_back(EV_SVT_LG);
			aListType.push_back(EV_SVT_CA);
		}
		break;
	default:
		{
			aListType.push_back(EV_SVT_MA);
			aListType.push_back(EV_SVT_VI);
			aListType.push_back(EV_SVT_GA);
			aListType.push_back(EV_SVT_CA);
		}
		break;
	}
	ASSERT(!aListType.empty());

	if (EV_WORLDID_ALL == m_WorldID) {
		// Total Info
		{
			// Gradient Line
			{
				Graphics aGraphics((*pDC));
				aGraphics.SetSmoothingMode(SmoothingModeHighQuality);

				LinearGradientBrush aBrush(
					Rect(
					pStartPoint.x,
					pStartPoint.y,
					pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_START_X, 
					EV_SERVERDRAW_TYPENAME_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					pStartPoint.x,
					pStartPoint.y,
					pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_START_X, 
					EV_SERVERDRAW_TYPENAME_HGT
					);
			}

			// Draw Text
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				CRect aRect(pStartPoint.x + EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y, pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y + EV_SERVERDRAW_TYPENAME_HGT);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				pDC->DrawText(CString(_T("Total Information")), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();

				pStartPoint.x = EV_SERVERDRAW_START_X;
				pStartPoint.y += EV_SERVERDRAW_TYPENAME_HGT + EV_SERVERDRAW_GAP_Y;
				if (pHeight) {
					(*pHeight) += EV_SERVERDRAW_TYPENAME_HGT + EV_SERVERDRAW_GAP_Y;
				}
			}

			// Display Info
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				CRect aRect(pStartPoint.x + EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y, pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y + EV_SERVERDRAW_TYPENAME_HGT);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(0, 0, 0));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				USES_CONVERSION;

				CNetSession* pSession = m_pPartitionView->GetNetConnection()->GetSession();

				pDC->DrawText(CString(CVarArg<MAX_PATH>(_T("Service Manager IP Address : %S, Port : %d"), pSession->GetRemoteIpAddressString(), pSession->GetRemotePortNumberH())), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();

				pStartPoint.x = EV_SERVERDRAW_START_X;
				pStartPoint.y += EV_SERVERDRAW_WORLDINFO_HGT + EV_SERVERDRAW_GAP_Y;
				if (pHeight) {
					(*pHeight) += EV_SERVERDRAW_WORLDINFO_HGT + EV_SERVERDRAW_GAP_Y;
				}
			}
		}

		// Seperator
		{
			LOGBRUSH aLogBrush;
			aLogBrush.lbStyle = BS_SOLID;
			aLogBrush.lbColor = RGB(192,192,192);

			CPen aNewPen(PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT, 5, &aLogBrush);
			CPen* aOldPen = pDC->SelectObject(&aNewPen);

			pDC->MoveTo(pStartPoint.x, pStartPoint.y);
			pDC->LineTo(pRect.right - EV_SERVERDRAW_PAD_X, pStartPoint.y);

			pDC->SelectObject(aOldPen);
			aNewPen.DeleteObject();

			pStartPoint.x = EV_SERVERDRAW_START_X;
			pStartPoint.y += EV_SERVERDRAW_SEPERATOR_HGT + EV_SERVERDRAW_GAP_Y;
			if (pHeight) {
				(*pHeight) += EV_SERVERDRAW_SEPERATOR_HGT + EV_SERVERDRAW_GAP_Y;
			}
		}
	}
	else {
		// World Info
		{
			// Gradient Line
			{
				Graphics aGraphics((*pDC));
				aGraphics.SetSmoothingMode(SmoothingModeHighQuality);

				LinearGradientBrush aBrush(
					Rect(
					pStartPoint.x,
					pStartPoint.y,
					pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_START_X, 
					EV_SERVERDRAW_TYPENAME_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					pStartPoint.x,
					pStartPoint.y,
					pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_START_X, 
					EV_SERVERDRAW_TYPENAME_HGT
					);
			}

			// Draw Text
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				CRect aRect(pStartPoint.x + EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y, pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y + EV_SERVERDRAW_TYPENAME_HGT);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				pDC->DrawText(CString(_T("World Information")), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();

				pStartPoint.x = EV_SERVERDRAW_START_X;
				pStartPoint.y += EV_SERVERDRAW_TYPENAME_HGT + EV_SERVERDRAW_GAP_Y;
				if (pHeight) {
					(*pHeight) += EV_SERVERDRAW_TYPENAME_HGT + EV_SERVERDRAW_GAP_Y;
				}
			}

			// Display Info
			{
				const WORLDINFO* aWorldInfo = AtWorld(m_WorldID);
				
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				CRect aRect(pStartPoint.x + EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y, pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y + EV_SERVERDRAW_TYPENAME_HGT);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(0, 0, 0));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				pDC->DrawText(CString(CVarArg<MAX_PATH>(_T("%s (%d / %d)"), (_T('\0') != aWorldInfo->m_WorldName[0])?(aWorldInfo->m_WorldName):(_T("")), m_WorldUserCount, aWorldInfo->m_MaxUserLimit)), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();

				pStartPoint.x = EV_SERVERDRAW_START_X;
				pStartPoint.y += EV_SERVERDRAW_WORLDINFO_HGT + EV_SERVERDRAW_GAP_Y;
				if (pHeight) {
					(*pHeight) += EV_SERVERDRAW_WORLDINFO_HGT + EV_SERVERDRAW_GAP_Y;
				}
			}
		}

		// Seperator
		{
			LOGBRUSH aLogBrush;
			aLogBrush.lbStyle = BS_SOLID;
			aLogBrush.lbColor = RGB(192,192,192);

			CPen aNewPen(PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT, 5, &aLogBrush);
			CPen* aOldPen = pDC->SelectObject(&aNewPen);

			pDC->MoveTo(pStartPoint.x, pStartPoint.y);
			pDC->LineTo(pRect.right - EV_SERVERDRAW_PAD_X, pStartPoint.y);

			pDC->SelectObject(aOldPen);
			aNewPen.DeleteObject();

			pStartPoint.x = EV_SERVERDRAW_START_X;
			pStartPoint.y += EV_SERVERDRAW_SEPERATOR_HGT + EV_SERVERDRAW_GAP_Y;
			if (pHeight) {
				(*pHeight) += EV_SERVERDRAW_SEPERATOR_HGT + EV_SERVERDRAW_GAP_Y;
			}
		}
	}

	TP_LISTSVTP_CTR aIt1 = aListType.begin();
	for (int aIndex = 0 ; aListType.end() != aIt1 ; ++aIt1, ++aIndex) {
		const EF_SERVERTYPE aServerType = (*aIt1);
		ASSERT(EV_SVT_NONE < aServerType && EV_SVT_CNT > aServerType);

		// Seperator
		if (0 < aIndex) {
			LOGBRUSH aLogBrush;
			aLogBrush.lbStyle = BS_SOLID;
			aLogBrush.lbColor = RGB(192,192,192);

			CPen aNewPen(PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT, 5, &aLogBrush);
			CPen* aOldPen = pDC->SelectObject(&aNewPen);

			pDC->MoveTo(pStartPoint.x, pStartPoint.y);
			pDC->LineTo(pRect.right - EV_SERVERDRAW_PAD_X, pStartPoint.y);

			pDC->SelectObject(aOldPen);
			aNewPen.DeleteObject();

			pStartPoint.x = EV_SERVERDRAW_START_X;
			pStartPoint.y += EV_SERVERDRAW_SEPERATOR_HGT + EV_SERVERDRAW_GAP_Y;
			if (pHeight) {
				(*pHeight) += EV_SERVERDRAW_SEPERATOR_HGT + EV_SERVERDRAW_GAP_Y;
			}
		}

		int nTotalUserCount = 0;
		int nTotalRoomCount = 0;

		TP_LISTSERVAUTO aListServ;

		GetServerList(aListServ, aServerType, FALSE);	// 특정 서버타입의 목록 구함

		if (aServerType == EV_SVT_GA || aServerType == EV_SVT_VI || aServerType == EV_SVT_LO)
		{
			TP_LISTSERVAUTO_CTR aIt2 = aListServ.begin();
			for (; aListServ.end() != aIt2 ; ++aIt2) 
			{
				CServerDrawBaseFilter* aServerDrawFilter = m_ServerDrawFilter[aServerType];

				if (aServerDrawFilter) 
				{
					aServerDrawFilter->Reset();
					aServerDrawFilter->Set(&(*aIt2));

					// 전체 유저수 계산
					nTotalUserCount += aServerDrawFilter->GetServerDraw()->m_CurUserCount;

					// 전체 룸 갯수 계산
					nTotalRoomCount += aServerDrawFilter->GetServerDraw()->m_CurRoomCount;
				}
			}
		}

		// Server Type Name
		{
			// Gradient Line
			{
				Graphics aGraphics((*pDC));
				aGraphics.SetSmoothingMode(SmoothingModeHighQuality);

				LinearGradientBrush aBrush(
					Rect(
					pStartPoint.x,
					pStartPoint.y,
					pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_START_X, 
					EV_SERVERDRAW_TYPENAME_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					pStartPoint.x,
					pStartPoint.y,
					pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_START_X, 
					EV_SERVERDRAW_TYPENAME_HGT
					);

				if (pHeight) {
					// 마우스 피킹할 수 있도록 위치 저장

					m_ServerTypePick[aServerType].m_Area = CRect(
						pStartPoint.x + GetScrollPos(SB_HORZ),
						pStartPoint.y + GetScrollPos(SB_VERT),
						pStartPoint.x + (pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_START_X) + GetScrollPos(SB_HORZ),
						pStartPoint.y + EV_SERVERDRAW_TYPENAME_HGT + GetScrollPos(SB_VERT)
						);
				}
			}

			// Draw Text
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				CRect aRect(pStartPoint.x + EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y, pRect.right - EV_SERVERDRAW_PAD_X - EV_SERVERDRAW_TYPENAME_PAD_LFT, pStartPoint.y + EV_SERVERDRAW_TYPENAME_HGT);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				if (aServerType == EV_SVT_GA)
				{
					CVarArg<MAX_PATH> szTitleBar(_T("%s (%d / %d)"), g_ServerTypeFullName[aServerType], nTotalUserCount, nTotalRoomCount);
					pDC->DrawText(szTitleBar.GetMsg(), &aRect, DT_LEFT | DT_VCENTER);	
				}
				else if (aServerType == EV_SVT_VI || aServerType == EV_SVT_LO)
				{
					CVarArg<MAX_PATH> szTitleBar(_T("%s (%d)"), g_ServerTypeFullName[aServerType], nTotalUserCount);
					pDC->DrawText(szTitleBar.GetMsg(), &aRect, DT_LEFT | DT_VCENTER);	
				}
				else
				{
					CVarArg<MAX_PATH> szTitleBar(_T("%s"), g_ServerTypeFullName[aServerType]);
					pDC->DrawText(szTitleBar.GetMsg(), &aRect, DT_LEFT | DT_VCENTER);
				}


				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();

				pStartPoint.x = EV_SERVERDRAW_START_X;
				pStartPoint.y += EV_SERVERDRAW_TYPENAME_HGT + EV_SERVERDRAW_GAP_Y;
				if (pHeight) {
					(*pHeight) += EV_SERVERDRAW_TYPENAME_HGT + EV_SERVERDRAW_GAP_Y;
				}
			}
		}

		// Server Diagram
		{
			int aServerDrawHeight = 0;

			TP_LISTSERVAUTO_CTR aIt2 = aListServ.begin();
			for (; aListServ.end() != aIt2 ; ++aIt2) {
				CServerDrawBaseFilter* aServerDrawFilter = m_ServerDrawFilter[aServerType];

				if (aServerDrawFilter) {
					aServerDrawFilter->Reset();
					aServerDrawFilter->Set(&(*aIt2));

					const SERVERDRAW* pServerDraw = aServerDrawFilter->GetServerDraw();
					if (EV_WORLDID_ALL != m_WorldID && pServerDraw->m_WorldID[1])
						continue;

					aServerDrawHeight = aServerDrawFilter->GetHeight();

					if (EV_SERVERDRAW_START_X != pStartPoint.x &&
						aServerDrawFilter->GetWidth() >= (pRect.right - pStartPoint.x))
					{
						// 다음 줄에 표시

						pStartPoint.x = EV_SERVERDRAW_START_X;
						pStartPoint.y += aServerDrawHeight + EV_SERVERDRAW_GAP_Y;
						if (pHeight) {
							(*pHeight) += aServerDrawHeight + EV_SERVERDRAW_GAP_Y;
						}
					}

					aServerDrawFilter->Draw(pDC, pStartPoint.x, pStartPoint.y);

					if (pHeight) {
						// 마우스 피킹할 수 있도록 위치 저장

						SERVERDRAW* aServerDraw = AtServer(aServerDrawFilter->GetServerDraw()->m_ServerID);
						if (aServerDraw) {
							aServerDraw->m_Area = CRect(
								pStartPoint.x + GetScrollPos(SB_HORZ), 
								pStartPoint.y + GetScrollPos(SB_VERT), 
								pStartPoint.x + aServerDrawFilter->GetWidth() + GetScrollPos(SB_HORZ), 
								pStartPoint.y + aServerDrawFilter->GetHeight() + GetScrollPos(SB_VERT)
								);
						}
						else {
							ASSERT(0);
						}
					}

					pStartPoint.x += aServerDrawFilter->GetWidth() + EV_SERVERDRAW_GAP_X;
				}
			}

			pStartPoint.x = EV_SERVERDRAW_START_X;
			pStartPoint.y += aServerDrawHeight + EV_SERVERDRAW_GAP_Y;
			if (pHeight) {
				(*pHeight) += aServerDrawHeight + EV_SERVERDRAW_GAP_Y;
			}
		}
	}
}

void CSubMonitorWorldView::DoPopupMenu()
{
	CMenu aPopMenu;
	VERIFY(aPopMenu.LoadMenu(IDR_WORLDVIEW_POPUP));

	// set default item
	MENUITEMINFO aItemInfo = { sizeof(MENUITEMINFO) };
	aItemInfo.fMask = MIIM_STATE;
	aItemInfo.fState = MFS_DEFAULT;
	::SetMenuItemInfo(aPopMenu.m_hMenu, ID_POPUP_PROCONOFF, FALSE, &aItemInfo);

	// disable sub menu
	aPopMenu.EnableMenuItem(ID_POPUP_VICHNLMERIT, MF_DISABLED | MF_GRAYED);		// 부 메뉴 임시 비활성화 - 추후 기능 지원 시 제거되어야 함 !!!
	aPopMenu.EnableMenuItem(ID_POPUP_WORLDUSERLIMIT, MF_DISABLED | MF_GRAYED);	// 부 메뉴 임시 비활성화 - 추후 기능 지원 시 제거되어야 함 !!!
	aPopMenu.EnableMenuItem(ID_POPUP_GAMERIT, MF_DISABLED | MF_GRAYED);			// 부 메뉴 임시 비활성화 - 추후 기능 지원 시 제거되어야 함 !!!

	CPoint aPosMouse;
	GetCursorPos(&aPosMouse);

	// show menu
	SendMessage(WM_NCACTIVATE, FALSE);
	aPopMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPosMouse.x, aPosMouse.y, this);
}

void CSubMonitorWorldView::DisplayServerState(INT pServerID, BOOL pIsNew)
{
	// 현재 월드 뷰에 활성화된 페이지가 아니면 정보 출력 불가
	CSubMonitorFrameView* aSubMonitorFrameView = static_cast<CSubMonitorFrameView*>(GetParent()->GetParent());
	if (!aSubMonitorFrameView)
		return;

	if (aSubMonitorFrameView->GetActivePageIndex() != GetPageIndex()) {
		return;
	}

	if (!m_pPartitionView)
		return;

	CBaseMonitorView* aBaseMonitorView = m_pPartitionView->GetMoniterView();
	if(!aBaseMonitorView)
		return;

	if (EV_SERVERID_DEF == pServerID) {
		aBaseMonitorView->ClearServerStateGrid();
		aBaseMonitorView->ClearChannelStateList();
		aBaseMonitorView->SetLastDispServerID(EV_SERVERID_DEF);
		return;
	}

	const SERVERDRAW* aServerDraw = AtServer(pServerID);
	if (!aServerDraw) 
		return;

	aBaseMonitorView->SetServerStateGrid(aServerDraw);


	BOOL aIsSvrDff = FALSE;
	if (pServerID != aBaseMonitorView->GetLastDispServerID()) 
	{
		aIsSvrDff = TRUE;
		aBaseMonitorView->SetLastDispServerID(pServerID);
	}

	if (pIsNew || aIsSvrDff) 
	{
		aBaseMonitorView->ClearChannelStateList();
		aBaseMonitorView->SetChannelStateColum(aServerDraw->m_ServerType);
	}

	switch (aServerDraw->m_ServerType)
	{
	case EV_SVT_VI:
		{
			TP_LISTCHNLAUTO aListChnl;
			GetChannelList(aListChnl, pServerID);

			if (aListChnl.empty()) {
				return;
			}

			TP_LISTCHNLAUTO_CTR aIt = aListChnl.begin();
			for (; aListChnl.end() != aIt ; ++aIt) {
				const CHANNELINFO* aChannelInfo = &(*aIt);
				if (!aChannelInfo) return;
				if(!aChannelInfo->IsSet()) return;

				if (pIsNew || aIsSvrDff) {
					aBaseMonitorView->AddChannelStateList(aChannelInfo);
				}
				else {
					aBaseMonitorView->SetChannelStateList(aChannelInfo);
				}
			}
		}
		break;
	case EV_SVT_GA:
		{
			for (int i=0; i<aServerDraw->m_MaxFarmCount; i++)
			{
				if (pIsNew || aIsSvrDff) {
					aBaseMonitorView->AddFarmStateList(aServerDraw->m_FarmDBID[i], aServerDraw->m_CurFarmUserCount[i], aServerDraw->m_bActivate[i]);
				}
				else {
					aBaseMonitorView->SetFarmStateList(aServerDraw->m_FarmDBID[i], aServerDraw->m_CurFarmUserCount[i], aServerDraw->m_bActivate[i]);
				}
			}
		}
		break;
	default:
		return;
	}


}

void CSubMonitorWorldView::UpdateControlDialog(CControlDialog* pControlDialog)
{
	if (!pControlDialog)
		return;

	CControlDialog::TP_LISTWRLDAUTO aListWrld;
	CControlDialog::TP_LISTSERVAUTO aListServ;
	CControlDialog::TP_LISTCHNLAUTO aListChnl;
	CControlDialog::TP_LISTMRITAUTO aListMrit;
	CControlDialog::TP_LISTNTLCAUTO aListNtlc;

	// WORLD
	{
		GetWorldList(aListWrld);
	}

	{
		TP_LISTSERVAUTO aListServ2;

		GetServerList(aListServ2, EV_SVT_NONE, TRUE);

		TP_LISTSERVAUTO_CTR aIt = aListServ2.begin();
		for (; aListServ2.end() != aIt ; ++aIt) {
			const SERVERDRAW* aServerDraw = (&(*aIt));
			if (!aServerDraw)
				continue;

			if (!aServerDraw->IsSet())
				continue;
			

			// SERVER
			{
				aListServ.push_back((*aServerDraw));
			}

			// CHANNEL (VI)
			{
				GetChannelList(aListChnl, aServerDraw->m_ServerID);
			}
		}
	}

	{
		CServiceInfo::TP_LOCKAUTO AutoLock(m_pServiceInfo->GetLock());

		// MERIT (GA, VI)
		{
			m_pServiceInfo->GetMeritList(EV_SMT_REAL, aListMrit, FALSE);
		}

		// NET LUNCHER
		{
			m_pServiceInfo->GetNetLuncherList(EV_SMT_REAL, aListNtlc, FALSE);
		}
	}
	
	{
		pControlDialog->ClearAllList();

		pControlDialog->AddWorldList(aListWrld);
		pControlDialog->AddServerList(aListServ);
		pControlDialog->AddChannelList(aListChnl);
		pControlDialog->AddMeritList(aListMrit);
		pControlDialog->AddNetLuncherList(aListNtlc);

		pControlDialog->UpdateDataAllView();
	}
}

// CSubMonitorWorldView 메시지 처리기입니다.

BOOL CSubMonitorWorldView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	cs.style &= ~(WS_HSCROLL);
	cs.style |= (WS_VSCROLL);

	return CScrollView::PreCreateWindow(cs);
}

BOOL CSubMonitorWorldView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CScrollView::PreTranslateMessage(pMsg);
}

void CSubMonitorWorldView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CRect aRect;
	GetClientRect(&aRect);

	if (!m_IsInitialized) {
		m_IsInitialized = TRUE;

 		ShowScrollBar(SB_VERT, TRUE);
//		ShowScrollBar(SB_BOTH, TRUE);

		
	}
}

BOOL CSubMonitorWorldView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}

void CSubMonitorWorldView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	CClientDC aDC(this);

	CRect aRect;
	GetClientRect(&aRect);

	{
		Invalidate(TRUE);

		int aHeight = EV_SERVERDRAW_START_Y;

		CPoint aStartPoint(EV_SERVERDRAW_START_X, EV_SERVERDRAW_START_Y - GetScrollPos(SB_VERT));	// 각 표시의 출력 시작 위치를 계산할 때 사용

		CBitmap aNewBitmap;
		aNewBitmap.CreateCompatibleBitmap(&aDC, aRect.right, aRect.bottom);

		CDC aMemDC;
		aMemDC.CreateCompatibleDC(&aDC);

		CBitmap* aOldBitmap = aMemDC.SelectObject(&aNewBitmap);

		aMemDC.PatBlt(0, 0, aRect.right, aRect.bottom, WHITENESS);

		DrawView(&aMemDC, aRect, aStartPoint, &aHeight);

		aDC.BitBlt(0, 0, aRect.right, aRect.bottom, &aMemDC, 0, 0, SRCCOPY);

		aMemDC.SelectObject(aOldBitmap);

		SetScrollSizes(MM_LOENGLISH, CSize(aRect.right, aHeight + EV_SERVERDRAW_SCROLL_PAD_Y));	// 최대 간격 조절
		SetScrollRange(SB_VERT, 0, aHeight, TRUE);	// 실제 간격 지정, 스크롤 바 갱신
	}
}

void CSubMonitorWorldView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CScrollView::OnPaint()을(를) 호출하지 마십시오.

	CRect aRect;
	GetClientRect(&aRect);

//	if (!IsServerEmpty()) {	// 추후 교체되어야 함 ???
	{
		CPoint aStartPoint(EV_SERVERDRAW_START_X, EV_SERVERDRAW_START_Y - GetScrollPos(SB_VERT));	// 각 표시의 출력 시작 위치를 계산할 때 사용

		CBitmap aNewBitmap;
		aNewBitmap.CreateCompatibleBitmap(&dc, aRect.right, aRect.bottom);

		CDC aMemDC;
		aMemDC.CreateCompatibleDC(&dc);

		CBitmap* aOldBitmap = aMemDC.SelectObject(&aNewBitmap);

		aMemDC.PatBlt(0, 0, aRect.right, aRect.bottom, WHITENESS);

		DrawView(&aMemDC, aRect, aStartPoint);

		dc.BitBlt(0, 0, aRect.right, aRect.bottom, &aMemDC, 0, 0, SRCCOPY);

		aMemDC.SelectObject(aOldBitmap);
	}
}

void CSubMonitorWorldView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (!m_pServiceInfo)
		return;

	switch(nIDEvent) {
	case EV_TMD_REFRESHSERVICEINFO:
		{
			// World View
			{
				LONG64 aServiceInfoUpdateNo = m_pServiceInfo->GetServiceInfoUpdateNo();
				if (GetServiceInfoUpdateNo() != aServiceInfoUpdateNo) {
					// 목록 갱신

					SetServiceInfoUpdateNo(aServiceInfoUpdateNo);

					RebuildServiceInfo();

					CRect aRect;
					GetClientRect(&aRect);

					PostMessage(WM_SIZE, SIZE_RESTORED, MAKELONG(aRect.right, aRect.bottom));
				}
				else {
					// 정보 갱신

					RefreshServiceInfo();

					Invalidate(TRUE);
				}
			}

			// Tab Image
			{
				BOOL aIsServerTerminated = FALSE;

				TP_LISTSERV_ITR aIt = m_ListServ.begin();
				for (; m_ListServ.end() != aIt ; ++aIt) {
					SERVERDRAW* aServerDraw = &(aIt->second);
					ASSERT(NULL != aServerDraw);

					if (EV_SVS_TMNT == aServerDraw->m_ServerState) {
						aIsServerTerminated = TRUE;
					}
				}

				CSubMonitorFrameView* aSubMonitorFrameView = static_cast<CSubMonitorFrameView*>(GetParent()->GetParent());
				if (!aSubMonitorFrameView)
					return;


				if (aIsServerTerminated) {
					if (!m_DoAlert) {
						m_DoAlert = TRUE;
					}
				}
				else {
					if (m_DoAlert) {
						m_DoAlert = FALSE;

						aSubMonitorFrameView->SetPageImageIndex(GetPageIndex(), EV_IML_WORLDVIEW);
					}
				}

				if (m_DoAlert) {
					if (aSubMonitorFrameView->GetActivePageIndex() == GetPageIndex()) {
						m_DoAlert = FALSE;

						aSubMonitorFrameView->SetPageImageIndex(GetPageIndex(), EV_IML_WORLDVIEW);
					}
					else {
						if ((++m_TabIconTick) % 2) {
							aSubMonitorFrameView->SetPageImageIndex(GetPageIndex(), EV_IML_ALERT);
						}
						else {
							aSubMonitorFrameView->SetPageImageIndex(GetPageIndex(), EV_IML_WORLDVIEW);
						}
					}
				}
			}
		}
		break;
	case EV_TMD_SHOWSERVERSTATE:
		{
			LONG64 aServiceInfoUpdateNo = m_pServiceInfo->GetServiceInfoUpdateNo();
			if (GetServiceInfoUpdateNo() == aServiceInfoUpdateNo) {
				// 정보 갱신

				DisplayServerState(m_LastPickServerID, FALSE);
			}
		}
		break;
	default:
		break;
	}

	CScrollView::OnTimer(nIDEvent);
}

void CSubMonitorWorldView::OnDestroy()
{
	CScrollView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	KillTimer(EV_TMD_REFRESHSERVICEINFO);
	KillTimer(EV_TMD_SHOWSERVERSTATE);	
}

void CSubMonitorWorldView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	Invalidate(TRUE);

	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CSubMonitorWorldView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_LastPickServerID = EV_SERVERID_DEF;

	BOOL aDoUpdate = FALSE;

	if (!aDoUpdate &&
		!IsServerEmpty()
		)
	{
		for (int aIndex = 0 ; EV_SVT_CNT > aIndex ; ++aIndex) {
			if (MK_CONTROL != (MK_CONTROL & nFlags)) {
				// 기존 선택 서버 목록 해제

				m_ServerTypePick[aIndex].m_IsPick = FALSE;
			}

			if (CHECK_RANGE(point.x + GetScrollPos(SB_HORZ), m_ServerTypePick[aIndex].m_Area.left, m_ServerTypePick[aIndex].m_Area.right) &&
				CHECK_RANGE(point.y + GetScrollPos(SB_VERT), m_ServerTypePick[aIndex].m_Area.top, m_ServerTypePick[aIndex].m_Area.bottom)
				)
			{
				// 특정 서버 타입 전체 지정

				m_ServerTypePick[aIndex].m_IsPick = !(m_ServerTypePick[aIndex].m_IsPick);

				TP_LISTSERV_ITR aIt = m_ListServ.begin();
				for (; m_ListServ.end() != aIt ; ++aIt) {
					SERVERDRAW* aServerDraw = &(aIt->second);
					if (!aServerDraw)
						continue;
					if (!aServerDraw->IsSet())
						continue;

					if (MK_CONTROL != (MK_CONTROL & nFlags)) {
						// 기존 선택 서버 목록 해제

						aServerDraw->m_IsPick = FALSE;
					}

					if (static_cast<EF_SERVERTYPE>(aIndex) != aServerDraw->m_ServerType) {
						continue;
					}

					aServerDraw->m_IsPick = m_ServerTypePick[aIndex].m_IsPick;
				}

				aDoUpdate = TRUE;
			}
		}
	}

	if (!aDoUpdate &&
		!IsServerEmpty()
		)
	{
		TP_LISTSERV_ITR aIt = m_ListServ.begin();
		for (; m_ListServ.end() != aIt ; ++aIt) {
			SERVERDRAW* aServerDraw = &(aIt->second);
			if (!aServerDraw)
				continue;
			if (!aServerDraw->IsSet())
				continue;
			
			if (MK_CONTROL != (MK_CONTROL & nFlags)) {
				// 기존 선택 서버 목록 해제

				aServerDraw->m_IsPick = FALSE;
			}

			if (CHECK_RANGE(point.x + GetScrollPos(SB_HORZ), aServerDraw->m_Area.left, aServerDraw->m_Area.right) &&
				CHECK_RANGE(point.y + GetScrollPos(SB_VERT), aServerDraw->m_Area.top, aServerDraw->m_Area.bottom)
				)
			{
				// 신규 서버 선택 목록 지정

				aServerDraw->m_IsPick = !(aServerDraw->m_IsPick);

				m_LastPickServerID = (aServerDraw->m_IsPick)?(aServerDraw->m_ServerID):(EV_SERVERID_DEF);
//				m_LastPickServerID = aServerDraw->m_ServerID;
			}
		}

		aDoUpdate = TRUE;
	}

	if (aDoUpdate) {
		Invalidate(TRUE);
	}

	DisplayServerState(m_LastPickServerID, TRUE);	// EV_SERVERID_DEF 도 전달되어야 함
	
	CScrollView::OnLButtonDown(nFlags, point);
}

void CSubMonitorWorldView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	DoPopupMenu();

	CScrollView::OnRButtonDown(nFlags, point);
}

void CSubMonitorWorldView::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL CSubMonitorWorldView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	Invalidate(TRUE);

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSubMonitorWorldView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 마우스 커서가 특정 서버 표시 위에 있을 때 해당 서버의 정보를 속성 그리드에 보여줌

	CScrollView::OnMouseMove(nFlags, point);
}

BOOL CSubMonitorWorldView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	return TRUE;	// 전체화면 초기화 안함 (화면 깜박임 방지)
}

void CSubMonitorWorldView::OnPopupProcOnOff()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CControlDialog& ControlDialog  = m_pSubMoniterFrameView->GetControlDlg();

	if (::IsWindow(ControlDialog.GetSafeHwnd()))
	{
		ControlDialog.ShowWindow(TRUE);
		UpdateControlDialog(&ControlDialog);
		ControlDialog.ActivateControlTab(EV_CTT_PROCONOFF);
	}
}

void CSubMonitorWorldView::OnPopupWorldUserLimit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	CControlDialog& ControlDialog  = m_pSubMoniterFrameView->GetControlDlg();

	if (::IsWindow(ControlDialog.GetSafeHwnd()))
	{
		ControlDialog.ShowWindow(TRUE);
		UpdateControlDialog(&ControlDialog);
		ControlDialog.ActivateControlTab(EV_CTT_WORLDUSERLIMIT);
	}
}

void CSubMonitorWorldView::OnPopupViChnlShowHide()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CControlDialog& ControlDialog  = m_pSubMoniterFrameView->GetControlDlg();

	if (::IsWindow(ControlDialog.GetSafeHwnd()))
	{
		ControlDialog.ShowWindow(TRUE);
		UpdateControlDialog(&ControlDialog);
		ControlDialog.ActivateControlTab(EV_CTT_VICHNLSHOWHIDE);
	}
}

void CSubMonitorWorldView::OnPopupViChnlMerit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CControlDialog& ControlDialog  = m_pSubMoniterFrameView->GetControlDlg();

	if (::IsWindow(ControlDialog.GetSafeHwnd()))
	{
		ControlDialog.ShowWindow(TRUE);
		UpdateControlDialog(&ControlDialog);
		ControlDialog.ActivateControlTab(EV_CTT_VICHNLMERIT);
	}
}

void CSubMonitorWorldView::OnPopupGaShowHide()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CControlDialog& ControlDialog  = m_pSubMoniterFrameView->GetControlDlg();

	if (::IsWindow(ControlDialog.GetSafeHwnd()))
	{
		ControlDialog.ShowWindow(TRUE);
		UpdateControlDialog(&ControlDialog);
		ControlDialog.ActivateControlTab(EV_CTT_GASHOWHIDE);
	}
}

void CSubMonitorWorldView::OnPopupGaMerit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CControlDialog& ControlDialog  = m_pSubMoniterFrameView->GetControlDlg();

	if (::IsWindow(ControlDialog.GetSafeHwnd()))
	{
		ControlDialog.ShowWindow(TRUE);
		UpdateControlDialog(&ControlDialog);
		ControlDialog.ActivateControlTab(EV_CTT_GAMERIT);
	}
}

void CSubMonitorWorldView::OnPopupProcMakeDump()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CControlDialog& ControlDialog  = m_pSubMoniterFrameView->GetControlDlg();

	if (::IsWindow(ControlDialog.GetSafeHwnd()))
	{
		ControlDialog.ShowWindow(TRUE);
		UpdateControlDialog(&ControlDialog);
		ControlDialog.ActivateControlTab(EV_CTT_PROCMAKEDUMP);
	}
}
