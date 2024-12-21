// SubControlViChnlShowHideView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "SubControlViChnlShowHideView.h"
#include "ControlDialog.h"
#include "NetConnection.h"

#define DNPRJSAFE	// DNPrjSafe.h ����
#include "CriticalSection.h"
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"

// CSubControlViChnlShowHideView ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSubControlViChnlShowHideView, CSubControlBaseView)

CSubControlViChnlShowHideView::CSubControlViChnlShowHideView(CWnd* pParent /*=NULL*/)
	: CSubControlBaseView(EV_CTT_VICHNLSHOWHIDE, CSubControlViChnlShowHideView::IDD, pParent)
{
	m_OldGridListSubProc = NULL;
	m_RequestType = EV_RQT_CNT;
	m_pSession = NULL;
}

CSubControlViChnlShowHideView::~CSubControlViChnlShowHideView()
{
}

void CSubControlViChnlShowHideView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VICHNLSHOWHIDELIST, m_GridList);
}


BEGIN_MESSAGE_MAP(CSubControlViChnlShowHideView, CSubControlBaseView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_VICHNLSHOWHIDEREQUEST, &CSubControlViChnlShowHideView::OnBnClickedViChnlShowHideRequest)
	ON_COMMAND_RANGE(IDC_VICHNLSHOWHIDERADIO_SHOW, IDC_VICHNLSHOWHIDERADIO_HIDE, &CSubControlViChnlShowHideView::OnBnClickedRadioGroup)
	ON_WM_DESTROY()
	ON_COMMAND(ID_POPUP_VICHNLSHOWHIDE_DELETEITEM, &CSubControlViChnlShowHideView::OnPopupVichnlShowHideDeleteItem)
END_MESSAGE_MAP()


void CSubControlViChnlShowHideView::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnOK();
}

void CSubControlViChnlShowHideView::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnCancel();
}

void CSubControlViChnlShowHideView::UpdateData()
{
	ClearGridList();

	m_ListServ.clear();
	m_ListChnl.clear();

	CControlDialog* aControlDialog = static_cast<CControlDialog*>(GetParent()->GetParent());
	if (!aControlDialog) return;

	TP_LISTCHNLAUTO aListChnl;

	aControlDialog->GetChannelList(aListChnl);

	TP_LISTCHNLAUTO_CTR aIt = aListChnl.begin();
	for (; aListChnl.end() != aIt ; ++aIt) {
		const CHANNELINFO* aChannelInfo = (&(*aIt));
		if (!aChannelInfo) return;
		if(!aChannelInfo->IsSet()) return;

		const SERVERINFO* aServerInfo = aControlDialog->AtServer(aChannelInfo->m_ServerID);
		if (!aServerInfo) return;
		if(!aServerInfo->IsSet()) return;

		AddGridList(aServerInfo, aChannelInfo);

		m_ListServ.insert(TP_LISTSERV::value_type(aServerInfo->m_ServerID, (*aServerInfo)));
		m_ListChnl.insert(TP_LISTCHNL::value_type(MAKE_SM_SVRCHN(aChannelInfo->m_ServerID, aChannelInfo->m_ChannelID), (*aChannelInfo)));
	}
}

void CSubControlViChnlShowHideView::AddGridList(const SERVERINFO* pServerInfo, const struct CHANNELINFO* pChannelInfo)
{
	if (!pServerInfo) return;
	ASSERT(NULL != pChannelInfo);
	if(!pServerInfo->IsSet()) return;
	if(!pChannelInfo->IsSet()) return;

	BeginWaitCursor();
	m_GridList.LockWindowUpdate();

	LV_ITEM aLvItem;
//	aLvItem.mask		= LVIF_TEXT | LVIF_IMAGE;
	aLvItem.mask		= LVIF_TEXT | LVIF_PARAM;
	aLvItem.cchTextMax	= 0;
	aLvItem.lParam		= 0;
	aLvItem.iItem		= m_GridList.GetItemCount();
	aLvItem.iSubItem	= 0;
//	aLvItem.iImage		= 1;
	aLvItem.lParam		= pChannelInfo->m_ChannelID;	// ���� ���� ���� �˻��� ���� �Ķ���ͷ� ����

	TCHAR sBuffer[MAX_PATH*2] = { _T('\0'), };

	// Server ID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pServerInfo->m_ServerID);
	aLvItem.pszText = sBuffer;

	int nRtnValue = m_GridList.InsertItem(&aLvItem);
	if (-1 == nRtnValue) return;

	// Channel ID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"),  pChannelInfo->m_ChannelID);
	m_GridList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// World ID
	{
		if (EV_WORLDID_DEF != pServerInfo->m_WorldID[0]) {
			sBuffer[0] = _T('\0');
			for (int aIndex = 0 ; WORLDCOUNTMAX > aIndex ; ++aIndex) {
				if (EV_WORLDID_DEF == pServerInfo->m_WorldID[aIndex]) {
					break;
				}
				if (0 < aIndex) {
					_tcscat_s(sBuffer, _T(", "));
				}
				_tcscat_s(sBuffer, CVarArg<MAX_PATH>(_T("%d"), pServerInfo->m_WorldID[aIndex]));
			}
		}
		else {
			_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), _T("N/A"));
		}
		m_GridList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);
	}

	// Map Index
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"),  pChannelInfo->m_MapIndex);
	m_GridList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Show / Hide
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"),  ((pChannelInfo->m_Visibility)?(_T("Show")):(_T("Hide"))));
	m_GridList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	m_GridList.UnlockWindowUpdate();
	EndWaitCursor();

}


// CSubControlViChnlShowHideView �޽��� ó�����Դϴ�.

BOOL CSubControlViChnlShowHideView::OnInitDialog()
{
	CSubControlBaseView::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	CRect aRect;
	GetClientRect(&aRect);

	// Control ListView
	{
		int aColNo = 0;

		m_GridList.InsertColumn(aColNo++, _T("Server ID"), LVCFMT_LEFT, 80);
		m_GridList.InsertColumn(aColNo++, _T("Channel ID"), LVCFMT_LEFT, 80);
		m_GridList.InsertColumn(aColNo++, _T("World ID"), LVCFMT_LEFT, 80);
		m_GridList.InsertColumn(aColNo++, _T("Map Index"), LVCFMT_LEFT, 80);
		m_GridList.InsertColumn(aColNo++, _T("Show / Hide"), LVCFMT_LEFT, 100);

//		m_GridList.SetImageList(&m_ImageList, LVSIL_SMALL);
//		m_GridList.SetEqualWidth();
//		m_GridList.SetTextFont(&aFont);		// ��Ʈ ����
//		m_GridList.SetTextFont();			// ��Ʈ ����
//		m_GridList.SetTextColor(color);		// ���� ����� ����
//		m_GridList.SetTextBkColor(color);	// ���� ���� ����
//		m_GridList.SetBkColor(color);		// ���� ����

		m_GridList.MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X,
			aRect.top + EV_UIS_LIST_PAD_TOP,
			EV_UIS_LIST_WTH,
			aRect.bottom - (aRect.top + EV_UIS_LIST_PAD_TOP + EV_UIS_LIST_PAD_Y),
			TRUE
			);
	}

	// Group Box
	{
		GetDlgItem(IDC_VICHNLSHOWHIDEGROUP)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X,
			aRect.top + EV_UIS_GROUP_PAD_TOP,
			aRect.right - (aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_GROUP_PAD_X),
			aRect.bottom - (aRect.top + EV_UIS_GROUP_PAD_TOP + EV_UIS_GROUP_PAD_Y),
			TRUE
			);
	}

	// Radio Button(s)
	{
		int aStartY = aRect.top + EV_UIS_RADIO_PAD_TOP;

		GetDlgItem(IDC_VICHNLSHOWHIDERADIO_SHOW)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_RADIO_PAD_LFT,
			aStartY,
			EV_UIS_RADIO_WTH,
			EV_UIS_RADIO_HGT,
			TRUE
			);
		aStartY += EV_UIS_RADIO_PAD_Y;

		GetDlgItem(IDC_VICHNLSHOWHIDERADIO_HIDE)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_RADIO_PAD_LFT,
			aStartY,
			EV_UIS_RADIO_WTH,
			EV_UIS_RADIO_HGT,
			TRUE
			);
		aStartY += EV_UIS_RADIO_PAD_Y;
	}

	// Button
	{
		GetDlgItem(IDC_VICHNLSHOWHIDEREQUEST)->MoveWindow(
			aRect.left + (EV_UIS_LIST_WTH + (EV_UIS_LIST_PAD_X * 2) + EV_UIS_REQUESTBUTTON_PAD_A),
			aRect.bottom - (EV_UIS_REQUESTBUTTON_HGT + (EV_UIS_REQUESTBUTTON_PAD_A * 2)),
			aRect.right - (aRect.left + EV_UIS_LIST_WTH + (EV_UIS_LIST_PAD_X * 2) + EV_UIS_REQUESTBUTTON_PAD_A * 3),
			EV_UIS_REQUESTBUTTON_HGT,
			TRUE
			);
	}

	// Layout Manager
	{
		m_LayoutManager.Attach(this);
		m_LayoutManager.TieChild(IDC_VICHNLSHOWHIDELIST, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT, OX_LMT_SAME);
		m_LayoutManager.TieChild(IDC_VICHNLSHOWHIDEGROUP, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
		m_LayoutManager.TieChild(IDC_VICHNLSHOWHIDEREQUEST, OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
	}

	// Grid List Subclassing
	{
		m_OldGridListSubProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(GetDlgItem(IDC_VICHNLSHOWHIDELIST)->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<LONG_PTR>(GridListSubProc)));
	}
	



	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CSubControlViChnlShowHideView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	switch(pMsg->message) {
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam) {
			case VK_ESCAPE:
				GetParent()->GetParent()->ShowWindow(SW_HIDE);
				return TRUE;	// ESC Ű�� �θ� �� ��Ʈ���� �θ� ��ȭ���ڸ� ����
//				break;
			}
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSubControlViChnlShowHideView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CSubControlBaseView::OnPaint()��(��) ȣ������ ���ʽÿ�.
}

void CSubControlViChnlShowHideView::OnSize(UINT nType, int cx, int cy)
{
	CSubControlBaseView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	m_LayoutManager.OnSize(cx,cy);
}

void CSubControlViChnlShowHideView::OnBnClickedViChnlShowHideRequest()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// ��Ʈ��ũ ����, �������� ���� ���� üũ
	if (! m_pSession) return;
	if (!m_pSession->IsConnect()) {
		::AfxMessageBox(_T("Session is not connected !"), MB_ICONINFORMATION | MB_OK);
		return;
	}
	if (!CHECK_LIMIT(m_RequestType, EV_RQT_CNT)) {
		::AfxMessageBox(_T("Request is not selected !"), MB_ICONINFORMATION | MB_OK);
		return;
	}
	if (m_ListChnl.empty()) {
		::AfxMessageBox(_T("Channel list is empty !"), MB_ICONINFORMATION | MB_OK);
		return;
	}

	if (m_GridList.GetSelCount() <= 0) 
	{
		::AfxMessageBox(_T("Channel is not selected !"), MB_ICONINFORMATION | MB_OK);
		return;
	}

	if (SERVERMONITOR_LEVEL_SUPERADMIN > m_pSession->GetMonitorLevel()) {
		::AfxMessageBox(_T("You have no authority to request"), MB_ICONWARNING | MB_OK);
		return;
	}

	if (IDOK != ::AfxMessageBox(_T("Are you sure to request ?"), MB_ICONQUESTION | MB_OKCANCEL)) {
		return;
	}


	//---------------------------------------------------------------------------------------------
	// SEND

	TSMChannelControl Packet;

	Packet.bVisibility = (EV_RQT_SHOW == m_RequestType)?(true):(false);
	Packet.cCount = 0;

	for (int i=0; i < m_GridList.GetItemCount(); i++)
	{
		if (!m_GridList.IsSelected(i))
			continue;

		INT nChannelID = static_cast<INT>(m_GridList.GetItemData(i));
		if (nChannelID <0)
			continue;

		// ���� ������ ��������� ��¿�� ����.
		const CHANNELINFO* pChannelInfo = NULL;
		TP_LISTCHNL_CTR channel_iter = m_ListChnl.begin();
		for (; channel_iter != m_ListChnl.end(); channel_iter++) 
		{
			pChannelInfo = &(channel_iter->second);
			if (!pChannelInfo || !pChannelInfo->IsSet())
				continue;

			if (pChannelInfo->m_ChannelID == nChannelID)
				break;
		}

		// ����üũ
		if (pChannelInfo->m_ChannelID != nChannelID)
			continue;

		TP_LISTSERV_CTR server_iter = m_ListServ.find(pChannelInfo->m_ServerID);
		if (server_iter == m_ListServ.end()) {
			::AfxMessageBox(CVarArg<MAX_PATH>(_T("Channel ID : %d / Server ID : %d Information not found !"), pChannelInfo->m_ChannelID, pChannelInfo->m_ServerID), MB_ICONWARNING | MB_OK);
			continue;
		}
		const SERVERINFO* pServerInfo = &(server_iter->second);
		if (!pServerInfo || !pServerInfo->IsSet()) {
			::AfxMessageBox(CVarArg<MAX_PATH>(_T("Channel ID : %d / Server ID : %d Information not found !"), pChannelInfo->m_ChannelID, pChannelInfo->m_ServerID), MB_ICONWARNING | MB_OK);
			continue;
		}

		if (_countof(Packet.Control) <= Packet.cCount) 
		{
			::AfxMessageBox(CVarArg<MAX_PATH>(_T("Item count you requested is over %d.\n\novered item will be ignored."), _countof(Packet.Control)), MB_ICONWARNING | MB_OK);
			break;
		}

		Packet.Control[Packet.cCount].nWorldID = pChannelInfo->m_WorldID;
		Packet.Control[Packet.cCount].nChannelID = pChannelInfo->m_ChannelID;
		Packet.cCount++;
	}

	if (Packet.cCount > 0)
		m_pSession->SendData(MONITOR2MANAGER_CHANNELCONTROL, 0, reinterpret_cast<CHAR*>(&Packet), (static_cast<int>((sizeof(Packet) - (sizeof(Packet.Control[0]) * (_countof(Packet.Control) - Packet.cCount)))&ULONG_MAX)));


	GetParent()->GetParent()->ShowWindow(SW_HIDE);
}


void CSubControlViChnlShowHideView::OnBnClickedRadioGroup(UINT pCtrlID)
{
	switch(pCtrlID) {
	case IDC_VICHNLSHOWHIDERADIO_SHOW:
		{
			m_RequestType = EV_RQT_SHOW;
		}
		break;
	case IDC_VICHNLSHOWHIDERADIO_HIDE:
		{
			m_RequestType = EV_RQT_HIDE;
		}
		break;
	}

}

void CSubControlViChnlShowHideView::DoPopupMenu()
{
	CMenu aPopMenu;
	VERIFY(aPopMenu.LoadMenu(IDR_VICHNLSHOWHIDE_POPUP));

	// set default item
	MENUITEMINFO aItemInfo = { sizeof(MENUITEMINFO) };
	aItemInfo.fMask = MIIM_STATE;
	aItemInfo.fState = MFS_DEFAULT;
	::SetMenuItemInfo(aPopMenu.m_hMenu, ID_POPUP_VICHNLSHOWHIDE_DELETEITEM, FALSE, &aItemInfo);

	CPoint aPosMouse;
	GetCursorPos(&aPosMouse);

	// show menu
	SendMessage(WM_NCACTIVATE, FALSE);
	aPopMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPosMouse.x, aPosMouse.y, this);
}

LRESULT CALLBACK CSubControlViChnlShowHideView::GridListSubProc(HWND hWnd, UINT pMessage, WPARAM wParam, LPARAM lParam)
{
	CSubControlViChnlShowHideView* aParent = static_cast<CSubControlViChnlShowHideView*>(CWnd::FromHandlePermanent(::GetParent(hWnd)));
	ASSERT(NULL != aParent);

	COXGridList* aThis = static_cast<COXGridList*>(CWnd::FromHandlePermanent(hWnd));
	ASSERT(NULL != aThis);

	switch(pMessage) {
	case WM_RBUTTONDOWN:
		{
			aParent->DoPopupMenu();
		}
		break;
	case WM_KEYDOWN:
		{
			switch(wParam) {
			case VK_DELETE:
				{
					if (::IsWindowEnabled(hWnd)) {
						aParent->DeleteCurSelItem();
					}
				}
				break;
			}
		}
		break;
	}

	return CallWindowProc(aParent->m_OldGridListSubProc, hWnd, pMessage, wParam, lParam);
}

void CSubControlViChnlShowHideView::OnDestroy()
{
	CSubControlBaseView::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	if (m_OldGridListSubProc) {
		SetWindowLongPtr(GetDlgItem(IDC_VICHNLSHOWHIDELIST)->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<LONG_PTR>(m_OldGridListSubProc));
		m_OldGridListSubProc = NULL;
	}
}

void CSubControlViChnlShowHideView::OnPopupVichnlShowHideDeleteItem()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	DeleteCurSelItem();
}

void CSubControlViChnlShowHideView::DeleteCurSelItem()
{
	if (0 >= m_GridList.GetSelCount()) {
		return;
	}

	TP_LISTAUTO aList;
	TCHAR aItemText1[64] = { _T('\0'), };	// SERVER ID
	TCHAR aItemText2[64] = { _T('\0'), };	// CHANNEL ID
	TCHAR aItemText3[64] = { _T('\0'), };	// WORLD ID

	CString aText = _T("Are you sure to delete this item(s) ?\n\n");
	for (int aIndex = 0 ; m_GridList.GetItemCount() > aIndex ; ++aIndex) {
		if (!m_GridList.IsSelected(aIndex)) {
			continue;
		}

		m_GridList.GetItemText(aIndex, 0, aItemText1, _countof(aItemText1));
		m_GridList.GetItemText(aIndex, 1, aItemText2, _countof(aItemText2));
		m_GridList.GetItemText(aIndex, 2, aItemText3, _countof(aItemText3));

		INT aServerID = static_cast<INT>(_ttoi(aItemText1));
		ASSERT(0 < aServerID);

		INT aChannelID = static_cast<INT>(_ttoi(aItemText2));
		ASSERT(0 < aChannelID);

		aText += CVarArg<MAX_PATH>(_T("Server ID : %d, Channel ID : %d, World ID : %s\n"), aServerID, aChannelID, aItemText3);

		aList.push_back(TP_LISTELEM(aIndex, MAKE_SM_SVRCHN(aServerID, aChannelID)));
	}

	if (IDOK != ::AfxMessageBox(aText, MB_ICONQUESTION | MB_OKCANCEL)) {
		return;
	}

	std::sort(aList.begin(), aList.end(), CSortList());

	for (TP_LISTAUTO_CTR aIt = aList.begin() ; aList.end() != aIt ; ++aIt) {
		m_ListServ.erase(static_cast<DWORD64>(aIt->second));

		m_GridList.DeleteItem(static_cast<INT>(aIt->first));
	}
}
