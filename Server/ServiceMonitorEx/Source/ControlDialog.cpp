// ControlDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "ControlDialog.h"

#include "SubControlProcOnOffView.h"
#include "SubControlWorldUserLimitView.h"
#include "SubControlViChnlShowHideView.h"
#include "SubControlViChnlMeritView.h"
#include "SubControlGaShowHideView.h"
#include "SubControlGaMeritView.h"
#include "SubControlProcMakeDump.h"


// CControlDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CControlDialog, CDialog)

CControlDialog::CControlDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CControlDialog::IDD, pParent)
{
	::memset(m_ControlView, 0, sizeof(m_ControlView));
	m_CurControlView = NULL;
	m_IsInitialized = FALSE;
	m_pSession = NULL;
}

CControlDialog::~CControlDialog()
{
	for (int aIndex = 0 ; EV_CTT_CNT > aIndex ; ++aIndex) {
		if (m_ControlView[aIndex]) {
			delete m_ControlView[aIndex];
			m_ControlView[aIndex] = NULL;
		}
	}
}

void CControlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTROLTAB, m_ControlTab);
}

BEGIN_MESSAGE_MAP(CControlDialog, CDialog)
//	ON_WM_NCHITTEST()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_CONTROLTAB, &CControlDialog::OnTcnSelChangeControlTab)
	ON_BN_CLICKED(IDC_CONTROLCANCEL, &CControlDialog::OnBnClickedControlCancel)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CControlDialog �޽��� ó�����Դϴ�.

BOOL CControlDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	if (m_IsInitialized)
		return FALSE;
	
	CenterWindow();

	CRect aRect;
	GetClientRect(&aRect);

	m_ControlTab.MoveWindow(
		aRect.left + EV_UIS_TAB_PAD_X,
		aRect.top + EV_UIS_TAB_PAD_TOP,
		aRect.right - (EV_UIS_TAB_PAD_X * 2),
		aRect.bottom - aRect.top - EV_UIS_TAB_PAD_TOP - EV_UIS_TAB_PAD_BTM,
		TRUE
		);


	// Control Tab View
	{
		for (int aIndex = 0 ; EV_CTT_CNT > aIndex ; ++aIndex) {
			m_ControlTab.InsertItem(aIndex, g_ControlTypeName[aIndex], -1);
		}

		CRect aRect1;
		m_ControlTab.GetClientRect(&aRect1);

		int aSubControlViewPadBottom = 0;
		for (int aIndex = 0 ; m_ControlTab.GetItemCount() > aIndex ; ++aIndex) {
			CRect aRect2;
			m_ControlTab.GetItemRect(aIndex, &aRect2);

			aSubControlViewPadBottom = max<LONG>(aRect2.bottom, aSubControlViewPadBottom);
		}

		m_ControlView[EV_CTT_PROCONOFF] = new CSubControlProcOnOffView();
		m_ControlView[EV_CTT_WORLDUSERLIMIT] = new CSubControlWorldUserLimitView();
		m_ControlView[EV_CTT_VICHNLSHOWHIDE] = new CSubControlViChnlShowHideView();
		m_ControlView[EV_CTT_VICHNLMERIT] = new CSubControlViChnlMeritView();
		m_ControlView[EV_CTT_GASHOWHIDE] = new CSubControlGaShowHideView();
		m_ControlView[EV_CTT_GAMERIT] = new CSubControlGaMeritView();
		m_ControlView[EV_CTT_PROCMAKEDUMP] = new CSubControlProcMakeDump();

		for (int aIndex = 0 ; EV_CTT_CNT > aIndex ; ++aIndex) {
			CDialog* aControlView = m_ControlView[aIndex];
			if (!aControlView) {
				ASSERT(0);
				continue;
			}
			
			BOOL aRetVal = aControlView->Create(static_cast<CSubControlBaseView*>(aControlView)->GetIdTemplate(), &m_ControlTab);
			if (!aRetVal) {
				ASSERT(0);
				continue;
			}
			aRetVal = aControlView->SetWindowPos(
				NULL, 
				EV_UIS_SUBCONTROLVIEW_PAD_A, 
				aSubControlViewPadBottom + EV_UIS_SUBCONTROLVIEW_PAD_A,
				aRect1.Width() - (EV_UIS_SUBCONTROLVIEW_PAD_A * 2), 
				aRect1.Height() - (aSubControlViewPadBottom + (EV_UIS_SUBCONTROLVIEW_PAD_A * 2)),
				SWP_HIDEWINDOW | SWP_NOZORDER
				);
			if (!aRetVal) {
				ASSERT(0);
				continue;
			}
		}

		m_CurControlView = m_ControlView[0];
		m_CurControlView->ShowWindow(SW_SHOW);
	}

	// Button
	{
		GetDlgItem(IDC_CONTROLCANCEL)->MoveWindow(
			aRect.right - (EV_UIS_CANCELBUTTON_WTH + EV_UIS_CANCELBUTTON_PAD_A),
			aRect.bottom - (EV_UIS_CANCELBUTTON_HGT + EV_UIS_CANCELBUTTON_PAD_A),
			EV_UIS_CANCELBUTTON_WTH,
			EV_UIS_CANCELBUTTON_HGT,
			TRUE
			);
	}

	// Layout Manager
	{
		m_LayoutManager.Attach(this);
		m_LayoutManager.TieChild(IDC_CONTROLTAB, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
		m_LayoutManager.TieChild(IDC_CONTROLCANCEL, OX_LMS_BOTTOM | OX_LMS_RIGHT, OX_LMT_SAME);
	}

	m_IsInitialized = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CControlDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	switch(pMsg->message) {
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam) {
			case VK_ESCAPE:
				CDialog::OnCancel();
				return TRUE;	// ESC Ű�� CDialog::OnCancel() ȣ�� (���� ��ȭ���� ���� CDialog::OnCancel() ȣ���� ���� �����)
//				break;
			}
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CControlDialog::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnOK();
}

void CControlDialog::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDialog::OnCancel();
}

/*
LRESULT CControlDialog::OnNcHitTest(CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	LRESULT nHitTest = CDialog::OnNcHitTest(point);   

	// HTCLIENT ����κ� Client ������ ó���� HTCAPTION �� ��ȯ (����κ��� ��� ��������)
	if (HTCLIENT == nHitTest) {
		return HTCAPTION;
	}

	return nHitTest; 
//	return CDialog::OnNcHitTest(point);
}
*/

void CControlDialog::DrawTitle(CDC* pDC)
{
	if(!pDC) return;

	Graphics aGraphics((*pDC));
	aGraphics.SetSmoothingMode(SmoothingModeHighQuality);

	CRect aRect;
	GetClientRect(&aRect);

	// Draw Title
	{
		// Main
		{
			// Gradient Line
			{
				LinearGradientBrush aBrush(
					Rect(
						EV_UIS_MAINTITLELINE_PAD_X, 
						EV_UIS_MAINTITLELINE_PAD_Y, 
						aRect.right - (EV_UIS_MAINTITLELINE_PAD_X * 2), 
						EV_UIS_MAINTITLELINE_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					EV_UIS_MAINTITLELINE_PAD_X, 
					EV_UIS_MAINTITLELINE_PAD_Y, 
					aRect.right - (EV_UIS_MAINTITLELINE_PAD_X * 2 + 1), 
					EV_UIS_MAINTITLELINE_HGT
					);
			}

			// Draw Text
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
//				_tcsncpy_s(aLogFont.lfFaceName, _T("Times New Roman"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
// 				VERIFY(aNewFont.CreatePointFont(120, _T("Arial"), pDC));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				CRect aRect(
					EV_UIS_MAINTITLELINE_PAD_X + EV_UIS_MAINTITLETEXT_PAD_X, 
					EV_UIS_MAINTITLELINE_PAD_Y, 
					aRect.right - EV_UIS_MAINTITLELINE_PAD_X - EV_UIS_MAINTITLETEXT_PAD_X, 
					aRect.top + EV_UIS_MAINTITLELINE_HGT + EV_UIS_MAINTITLELINE_PAD_Y
					);

				pDC->DrawText(_T("Control"), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();
			}
		}

		// Sub
		{
			// Gradient Line
			{
				LinearGradientBrush aBrush(
					Rect(
						EV_UIS_SUBTITLELINE_PAD_X, 
						EV_UIS_SUBTITLELINE_PAD_Y, 
						aRect.right - (EV_UIS_SUBTITLELINE_PAD_X * 2), 
						EV_UIS_SUBTITLELINE_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					EV_UIS_SUBTITLELINE_PAD_X, 
					EV_UIS_SUBTITLELINE_PAD_Y, 
					aRect.right - (EV_UIS_SUBTITLELINE_PAD_X * 2 + 1), 
					EV_UIS_SUBTITLELINE_HGT
					);
			}

			// Draw Text
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
//				_tcsncpy_s(aLogFont.lfFaceName, _T("Times New Roman"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
// 				VERIFY(aNewFont.CreatePointFont(120, _T("Arial"), pDC));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				CRect aRect(
					EV_UIS_SUBTITLELINE_PAD_X + EV_UIS_SUBTITLETEXT_PAD_X, 
					EV_UIS_SUBTITLELINE_PAD_Y, 
					aRect.right - EV_UIS_SUBTITLELINE_PAD_X - EV_UIS_SUBTITLETEXT_PAD_X, 
					aRect.top + EV_UIS_SUBTITLELINE_HGT + EV_UIS_SUBTITLELINE_PAD_Y
					);

				pDC->DrawText((0 <= m_ControlTab.GetCurSel())?(g_ControlTypeName[m_ControlTab.GetCurSel()]):(_T("N/A")), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();
			}
		}

		
	}


}

void CControlDialog::ActivateControlTab(EF_CONTROLTYPE pControlType)
{
	ASSERT(0 <= pControlType && EV_CTT_CNT > pControlType);

	// ���� �� ��Ʈ���� ���� ������ ����
	GetDlgItem(IDC_CONTROLTAB)->SendMessage(TCM_SETCURSEL, pControlType, 0);

	// �� ��Ʈ�ѿ� �������� ���� �޽����� ����
	NMHDR aNmHdr;
	aNmHdr.hwndFrom = GetDlgItem(IDC_CONTROLTAB)->GetSafeHwnd();
	aNmHdr.idFrom = IDC_CONTROLTAB;
	aNmHdr.code = TCN_SELCHANGE;
	::SendMessage(GetSafeHwnd(), WM_NOTIFY, aNmHdr.idFrom, reinterpret_cast<LPARAM>(&aNmHdr));
}

WORLDINFO* CControlDialog::AtWorld(INT nWorldID)
{
	TP_LISTWRLD_ITR aIt = m_ListWrld.find(nWorldID);
	if (m_ListWrld.end() != aIt) {
		return(static_cast<WORLDINFO*>(&aIt->second));
	}

	return NULL;
}

const WORLDINFO* CControlDialog::AtWorld(INT nWorldID) const
{
	return(static_cast<const WORLDINFO*>(const_cast<CControlDialog*>(this)->AtWorld(nWorldID)));
}

void CControlDialog::AddWorldList(const TP_LISTWRLDAUTO& pListWrld)
{
	if (pListWrld.empty()) {
//		ASSERT(0);
		return;
	}

	TP_LISTWRLDAUTO_CTR aIt = pListWrld.begin();
	for (; pListWrld.end() != aIt ; ++aIt) {
		const WORLDINFO* aWorldInfo = (&(*aIt));
		if (!aWorldInfo) return;
		if(!aWorldInfo->IsSet()) return;

		m_ListWrld.insert(TP_LISTWRLD::value_type(aWorldInfo->m_WorldID, (*aWorldInfo)));
	}
}

void CControlDialog::GetWorldList(TP_LISTWRLDAUTO& pListWrld)
{
	TP_LISTWRLD_CTR aIt = m_ListWrld.begin();
	for (; m_ListWrld.end() != aIt ; ++aIt) {
		const WORLDINFO* aWorldInfo = (&aIt->second);
		if (!aWorldInfo) return;
		if(!aWorldInfo->IsSet()) return;

		pListWrld.push_back((*aWorldInfo));
	}
}

SERVERINFO* CControlDialog::AtServer(INT pServerID)
{
	TP_LISTSERV_ITR aIt = m_ListServ.find(pServerID);
	if (m_ListServ.end() != aIt) {
		return(static_cast<SERVERINFO*>(&aIt->second));
	}

	return NULL;
}

const SERVERINFO* CControlDialog::AtServer(INT pServerID) const
{
	return(static_cast<const SERVERINFO*>(const_cast<CControlDialog*>(this)->AtServer(pServerID)));
}

void CControlDialog::AddServerList(const TP_LISTSERVAUTO& pListServ)
{
	if (pListServ.empty()) {
//		ASSERT(0);
		return;
	}

	TP_LISTSERVAUTO_CTR aIt = pListServ.begin();
	for (; pListServ.end() != aIt ; ++aIt) {
		const SERVERINFO* aServerInfo = (&(*aIt));
		if (!aServerInfo) return;
		if(!aServerInfo->IsSet()) return;

		m_ListServ.insert(TP_LISTSERV::value_type(aServerInfo->m_ServerID, (*aServerInfo)));
	}
}

void CControlDialog::GetServerList(TP_LISTSERVAUTO& pListServ)
{
	TP_LISTSERV_CTR aIt = m_ListServ.begin();
	for (; m_ListServ.end() != aIt ; ++aIt) {
		const SERVERINFO* aServerInfo = (&aIt->second);
		if (!aServerInfo) return;
		if(!aServerInfo->IsSet()) return;

		pListServ.push_back((*aServerInfo));
	}
}

CHANNELINFO* CControlDialog::AtChannel(INT pServerID, INT pChannelID)
{
	TP_LISTCHNL_ITR aIt = m_ListChnl.find(MAKE_SM_SVRCHN(pServerID, pChannelID));
	if (m_ListChnl.end() != aIt) {
		return(static_cast<CHANNELINFO*>(&aIt->second));
	}

	return NULL;
}

const CHANNELINFO* CControlDialog::AtChannel(INT pServerID, INT pChannelID) const
{
	return(static_cast<const CHANNELINFO*>(const_cast<CControlDialog*>(this)->AtChannel(pServerID, pChannelID)));
}

void CControlDialog::AddChannelList(const TP_LISTCHNLAUTO& pListChnl)
{
	if (pListChnl.empty()) {
//		ASSERT(0);
		return;
	}

	TP_LISTCHNLAUTO_CTR aIt = pListChnl.begin();
	for (; pListChnl.end() != aIt ; ++aIt) {
		const CHANNELINFO* aChannelInfo = (&(*aIt));
		if (!aChannelInfo) return;
		if(!aChannelInfo->IsSet()) return;

		m_ListChnl.insert(TP_LISTCHNL::value_type(MAKE_SM_SVRCHN(aChannelInfo->m_ServerID, aChannelInfo->m_ChannelID), (*aChannelInfo)));
	}
}

void CControlDialog::GetChannelList(TP_LISTCHNLAUTO& pListChnl)
{
	TP_LISTCHNL_CTR aIt = m_ListChnl.begin();
	for (; m_ListChnl.end() != aIt ; ++aIt) {
		const CHANNELINFO* aChannelInfo = (&aIt->second);
		if (!aChannelInfo) return;
		if(!aChannelInfo->IsSet()) return;

		pListChnl.push_back((*aChannelInfo));
	}
}

MERITINFO* CControlDialog::AtMerit(INT pMeritID)
{
	TP_LISTMRIT_ITR aIt = m_ListMrit.find(pMeritID);
	if (m_ListMrit.end() != aIt) {
		return(static_cast<MERITINFO*>(&aIt->second));
	}

	return NULL;
}

const MERITINFO* CControlDialog::AtMerit(INT pMeritID) const
{
	return(static_cast<const MERITINFO*>(const_cast<CControlDialog*>(this)->AtMerit(pMeritID)));
}

void CControlDialog::AddMeritList(const TP_LISTMRITAUTO& pListMrit)
{
	if (pListMrit.empty()) {
//		ASSERT(0);
		return;
	}

	TP_LISTMRITAUTO_CTR aIt = pListMrit.begin();
	for (; pListMrit.end() != aIt ; ++aIt) {
		const MERITINFO* aMeritInfo = (&(*aIt));
		if (!aMeritInfo) return;
		if(!aMeritInfo->IsSet()) return;

		m_ListMrit.insert(TP_LISTMRIT::value_type(aMeritInfo->m_MeritID, (*aMeritInfo)));
	}
}

void CControlDialog::GetMeritList(TP_LISTMRITAUTO& pListMrit)
{
	TP_LISTMRIT_CTR aIt = m_ListMrit.begin();
	for (; m_ListMrit.end() != aIt ; ++aIt) {
		const MERITINFO* aMeritInfo = (&aIt->second);
		if (!aMeritInfo) return;
		if(!aMeritInfo->IsSet()) return;

		pListMrit.push_back((*aMeritInfo));
	}
}

NETLUNCHERINFO* CControlDialog::AtNetLuncher(INT pNetLuncherID)
{
	TP_LISTNTLC_ITR aIt = m_ListNtlc.find(pNetLuncherID);
	if (m_ListNtlc.end() != aIt) {
		return(static_cast<NETLUNCHERINFO*>(&aIt->second));
	}

	return NULL;
}

const NETLUNCHERINFO* CControlDialog::AtNetLuncher(INT pNetLuncherID) const
{
	return(static_cast<const NETLUNCHERINFO*>(const_cast<CControlDialog*>(this)->AtNetLuncher(pNetLuncherID)));
}

void CControlDialog::AddNetLuncherList(const TP_LISTNTLCAUTO& pListNtlc)
{
	if (pListNtlc.empty()) 
		return;

	TP_LISTNTLCAUTO_CTR aIt = pListNtlc.begin();
	for (; pListNtlc.end() != aIt ; ++aIt) {
		const NETLUNCHERINFO* aNetLuncherInfo = (&(*aIt));
		if (!aNetLuncherInfo)
			continue;

		if (!aNetLuncherInfo->IsSet())
			continue;

		m_ListNtlc.insert(TP_LISTNTLC::value_type(aNetLuncherInfo->m_NetLuncherID, (*aNetLuncherInfo)));
	}
}

void CControlDialog::GetNetLuncherList(TP_LISTNTLCAUTO& pListNtlc)
{
	TP_LISTNTLC_CTR aIt = m_ListNtlc.begin();
	for (; m_ListNtlc.end() != aIt ; ++aIt) {
		const NETLUNCHERINFO* aNetLuncherInfo = (&aIt->second);
		if (!aNetLuncherInfo) continue;
		if (!aNetLuncherInfo->IsSet()) continue;

		pListNtlc.push_back((*aNetLuncherInfo));
	}
}

void CControlDialog::ClearAllList()
{
	m_ListWrld.clear();
	m_ListServ.clear();
	m_ListChnl.clear();
	m_ListMrit.clear();
	m_ListNtlc.clear();
}

void CControlDialog::UpdateDataAllView()
{
	for (int aIndex = 0 ; EV_CTT_CNT > aIndex ; ++aIndex) {
		CSubControlBaseView* aSubControlBaseView = static_cast<CSubControlBaseView*>(m_ControlView[aIndex]);
		if (aSubControlBaseView)
			aSubControlBaseView->UpdateData();
	}
}

void CControlDialog::SetSession(CNetSession* pSession)
{
	m_pSession = pSession;

	
	CSubControlProcOnOffView* pOnOffView = static_cast<CSubControlProcOnOffView*>(m_ControlView[EV_CTT_PROCONOFF]);
	pOnOffView->SetSession(m_pSession);

	CSubControlViChnlShowHideView* pViChnlShowHideView = static_cast<CSubControlViChnlShowHideView*>(m_ControlView[EV_CTT_VICHNLSHOWHIDE]);
	pViChnlShowHideView->SetSession(m_pSession);

	CSubControlGaShowHideView* pGaShowHideView = static_cast<CSubControlGaShowHideView*>(m_ControlView[EV_CTT_GASHOWHIDE]);
	pGaShowHideView->SetSession(m_pSession);

	CSubControlProcMakeDump* pMakeDumpView = static_cast<CSubControlProcMakeDump*>(m_ControlView[EV_CTT_PROCMAKEDUMP]);
	pMakeDumpView->SetSession(m_pSession);
	
}

void CControlDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.

	// Draw Title
	{
		DrawTitle(&dc);
	}
	
	
}

void CControlDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	if (m_IsInitialized) {
		// Draw Title
		{
			CClientDC aDC(this);

			DrawTitle(&aDC);
		}

		// Sub Control View(s)
		{
			CRect aRect1;
			m_ControlTab.GetClientRect(&aRect1);

			int aSubControlViewPadBottom = 0;
			for (int aIndex = 0 ; m_ControlTab.GetItemCount() > aIndex ; ++aIndex) {
				CRect aRect2;
				m_ControlTab.GetItemRect(aIndex, &aRect2);

				aSubControlViewPadBottom = max<LONG>(aRect2.bottom, aSubControlViewPadBottom);
			}

			for (int aIndex = 0 ; EV_CTT_CNT > aIndex ; ++aIndex) {
				CDialog* aControlView = m_ControlView[aIndex];
				if (!aControlView) {
					ASSERT(0);
					continue;
				}

				aControlView->MoveWindow(
					EV_UIS_SUBCONTROLVIEW_PAD_A, 
					aSubControlViewPadBottom + EV_UIS_SUBCONTROLVIEW_PAD_A,
					aRect1.Width() - (EV_UIS_SUBCONTROLVIEW_PAD_A * 2), 
					aRect1.Height() - (aSubControlViewPadBottom + (EV_UIS_SUBCONTROLVIEW_PAD_A * 2)),
					TRUE
					);
			}
		}

		m_LayoutManager.OnSize(cx,cy);
	}

	
}

void CControlDialog::OnTcnSelChangeControlTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UNUSED_ALWAYS(pNMHDR);

	if (m_CurControlView) {
		m_CurControlView->ShowWindow(SW_HIDE);
		m_CurControlView = NULL;
	}

	int aIndex = m_ControlTab.GetCurSel();
	if (0 <= aIndex && EV_CTT_CNT > aIndex) {
		CDialog* aControlView = m_ControlView[aIndex];
		if (aControlView) {
			m_CurControlView = aControlView;
		}
		else {
			ASSERT(0);
		}
	}
	if (m_CurControlView) {
		m_CurControlView->ShowWindow(SW_SHOW);
	}

	Invalidate(FALSE);

	*pResult = 0;
}

void CControlDialog::OnBnClickedControlCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

//	CDialog::OnCancel();	// ó���� �Ͽ�ȭ ��Ű�� ����
	CControlDialog::OnCancel();
}

void CControlDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CDialog::OnLButtonDown(nFlags, point);

	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));	// WM_NCHITTEST ��� ��� (���콺 ��ư �̺�Ʈ �ޱ� ����)
}

void CControlDialog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	// P.S.> ON_WM_NCHITTEST() �� ���� ���� �ʵ�

	WINDOWPLACEMENT aWindowPlacement;

	BOOL aRetVal = ::GetWindowPlacement(GetSafeHwnd(), &aWindowPlacement);
	if (aRetVal) {
		if (SW_MAXIMIZE == aWindowPlacement.showCmd ||
			SW_SHOWMAXIMIZED == aWindowPlacement.showCmd
			)
		{
			ShowWindow(SW_NORMAL);
		}
		else {
			ShowWindow(SW_MAXIMIZE);
		}
	}
	else {
		ASSERT(0);
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

