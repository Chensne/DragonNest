// SubControlGaShowHideView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "SubControlGaShowHideView.h"
#include "ControlDialog.h"
#include "NetConnection.h"

#define DNPRJSAFE	// DNPrjSafe.h ����
#include "CriticalSection.h"
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"

// CSubControlGaShowHideView ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSubControlGaShowHideView, CSubControlBaseView)

CSubControlGaShowHideView::CSubControlGaShowHideView(CWnd* pParent /*=NULL*/)
	: CSubControlBaseView(EV_CTT_GASHOWHIDE, CSubControlGaShowHideView::IDD, pParent)
{
	m_OldGridListSubProc = NULL;
	m_RequestType = EV_RQT_CNT;
	m_pSession = NULL;
}

CSubControlGaShowHideView::~CSubControlGaShowHideView()
{
}

void CSubControlGaShowHideView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GASHOWHIDELIST, m_GridList);
}


BEGIN_MESSAGE_MAP(CSubControlGaShowHideView, CSubControlBaseView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_GASHOWHIDEREQUEST, &CSubControlGaShowHideView::OnBnClickedGaShowHideRequest)
	ON_COMMAND_RANGE(IDC_GASHOWHIDERADIO_SHOW, IDC_GASHOWHIDERADIO_HIDE, &CSubControlGaShowHideView::OnBnClickedRadioGroup)
	ON_WM_DESTROY()
	ON_COMMAND(ID_POPUP_GASHOWHIDE_DELETEITEM, &CSubControlGaShowHideView::OnPopupGaShowHideDeleteItem)
END_MESSAGE_MAP()


void CSubControlGaShowHideView::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnOK();
}

void CSubControlGaShowHideView::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnCancel();
}

void CSubControlGaShowHideView::UpdateData()
{
	ClearGridList();

	m_ListServ.clear();

	CControlDialog* aControlDialog = static_cast<CControlDialog*>(GetParent()->GetParent());
	if (!aControlDialog) return;

	TP_LISTSERVAUTO aListServ;

	aControlDialog->GetServerList(aListServ);

	TP_LISTSERVAUTO_CTR aIt = aListServ.begin();
	for (; aListServ.end() != aIt ; ++aIt) {
		const SERVERINFO* aServerInfo = (&(*aIt));
		if (!aServerInfo) return;
		if(!aServerInfo->IsSet()) return;

		if (EV_SVT_GA != aServerInfo->m_ServerType) {
			continue;
		}

		AddGridList(aServerInfo);

		m_ListServ.insert(TP_LISTSERV::value_type(aServerInfo->m_ServerID, (*aServerInfo)));
	}
}

void CSubControlGaShowHideView::AddGridList(const SERVERINFO* pServerInfo)
{
	if (!pServerInfo) return;
	if(!pServerInfo->IsSet()) return;

	BeginWaitCursor();
	m_GridList.LockWindowUpdate();

	LV_ITEM aLvItem;
	aLvItem.mask		= LVIF_TEXT | LVIF_PARAM;
	aLvItem.cchTextMax	= 0;
	aLvItem.lParam		= 0;
	aLvItem.iItem		= m_GridList.GetItemCount();
	aLvItem.iSubItem	= 0;
	aLvItem.lParam		= pServerInfo->m_ServerID;	// ���� ���� ���� �˻��� ���� �Ķ���ͷ� ����

	TCHAR sBuffer[MAX_PATH*2] = { _T('\0'), };

	// Server ID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pServerInfo->m_ServerID);
	aLvItem.pszText = sBuffer;

	int nRtnValue = m_GridList.InsertItem(&aLvItem);
	if (-1 == nRtnValue) return;

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

	// Show / Hide
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), ((pServerInfo->m_Visibility)?(_T("Show")):(_T("Hide"))));
	m_GridList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	m_GridList.UnlockWindowUpdate();
	EndWaitCursor();

}

// CSubControlGaShowHideView �޽��� ó�����Դϴ�.

BOOL CSubControlGaShowHideView::OnInitDialog()
{
	CSubControlBaseView::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	CRect aRect;
	GetClientRect(&aRect);

	// Control ListView
	{
		int aColNo = 0;

		m_GridList.InsertColumn(aColNo++, _T("Server ID"), LVCFMT_LEFT, 80);
		m_GridList.InsertColumn(aColNo++, _T("World ID"), LVCFMT_LEFT, 80);
		m_GridList.InsertColumn(aColNo++, _T("Show / Hide"), LVCFMT_LEFT, 100);

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
		GetDlgItem(IDC_GASHOWHIDEGROUP)->MoveWindow(
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

		GetDlgItem(IDC_GASHOWHIDERADIO_SHOW)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_RADIO_PAD_LFT,
			aStartY,
			EV_UIS_RADIO_WTH,
			EV_UIS_RADIO_HGT,
			TRUE
			);
		aStartY += EV_UIS_RADIO_PAD_Y;

		GetDlgItem(IDC_GASHOWHIDERADIO_HIDE)->MoveWindow(
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
		GetDlgItem(IDC_GASHOWHIDEREQUEST)->MoveWindow(
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
		m_LayoutManager.TieChild(IDC_GASHOWHIDELIST, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT, OX_LMT_SAME);
		m_LayoutManager.TieChild(IDC_GASHOWHIDEGROUP, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
		m_LayoutManager.TieChild(IDC_GASHOWHIDEREQUEST, OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
	}

	// Grid List Subclassing
	{
		m_OldGridListSubProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(GetDlgItem(IDC_GASHOWHIDELIST)->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<LONG_PTR>(GridListSubProc)));
	}
	



	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CSubControlGaShowHideView::PreTranslateMessage(MSG* pMsg)
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

void CSubControlGaShowHideView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CSubControlBaseView::OnPaint()��(��) ȣ������ ���ʽÿ�.
}

void CSubControlGaShowHideView::OnSize(UINT nType, int cx, int cy)
{
	CSubControlBaseView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	m_LayoutManager.OnSize(cx,cy);
}

void CSubControlGaShowHideView::OnBnClickedGaShowHideRequest()
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
	if (m_ListServ.empty()) {
		::AfxMessageBox(_T("Server list is empty !"), MB_ICONINFORMATION | MB_OK);
		return;
	}

	if (SERVERMONITOR_LEVEL_SUPERADMIN > m_pSession->GetMonitorLevel()) {
		::AfxMessageBox(_T("You have no authority to request"), MB_ICONWARNING | MB_OK);
		return;
	}

	if (IDOK != ::AfxMessageBox(_T("Are you sure to request ?"), MB_ICONQUESTION | MB_OKCANCEL)) {
		return;
	}

	TSMGameControl aReq;
	aReq.bClose = (EV_RQT_SHOW == m_RequestType)?(false):(true);
	aReq.cCount = 0;

	TP_LISTSERV_CTR aIt = m_ListServ.begin();
	for (; m_ListServ.end() != aIt ; ++aIt) {
		const SERVERINFO* aServerInfo = (&(aIt->second));
		if (!aServerInfo) return;
		if(!aServerInfo->IsSet()) return;

		aReq.nSID[aReq.cCount++] = aServerInfo->m_ServerID;

		if (_countof(aReq.nSID) <= aReq.cCount) {
			::AfxMessageBox(CVarArg<MAX_PATH>(_T("Item count you requested is over %d.\n\novered item will be ignored."), _countof(aReq.nSID)), MB_ICONWARNING | MB_OK);
			break;
		}
	}
	
	if (0 < aReq.cCount) {
		m_pSession->SendData(MONITOR2MANAGER_GAMECONTROL, 0, reinterpret_cast<CHAR*>(&aReq), (static_cast<int>((sizeof(aReq) - (sizeof(aReq.nSID[0]) * (_countof(aReq.nSID) - aReq.cCount)))&ULONG_MAX)));
	}
	else {
		ASSERT(0);
	}

	GetParent()->GetParent()->ShowWindow(SW_HIDE);

}

void CSubControlGaShowHideView::OnBnClickedRadioGroup(UINT pCtrlID)
{
	switch(pCtrlID) {
	case IDC_GASHOWHIDERADIO_SHOW:
		{
			m_RequestType = EV_RQT_SHOW;
		}
		break;
	case IDC_GASHOWHIDERADIO_HIDE:
		{
			m_RequestType = EV_RQT_HIDE;
		}
		break;
	}

}

void CSubControlGaShowHideView::DoPopupMenu()
{
	CMenu aPopMenu;
	VERIFY(aPopMenu.LoadMenu(IDR_GASHOWHIDE_POPUP));

	// set default item
	MENUITEMINFO aItemInfo = { sizeof(MENUITEMINFO) };
	aItemInfo.fMask = MIIM_STATE;
	aItemInfo.fState = MFS_DEFAULT;
	::SetMenuItemInfo(aPopMenu.m_hMenu, ID_POPUP_GASHOWHIDE_DELETEITEM, FALSE, &aItemInfo);

	CPoint aPosMouse;
	GetCursorPos(&aPosMouse);

	// show menu
	SendMessage(WM_NCACTIVATE, FALSE);
	aPopMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPosMouse.x, aPosMouse.y, this);
}

LRESULT CALLBACK CSubControlGaShowHideView::GridListSubProc(HWND hWnd, UINT pMessage, WPARAM wParam, LPARAM lParam)
{
	CSubControlGaShowHideView* aParent = static_cast<CSubControlGaShowHideView*>(CWnd::FromHandlePermanent(::GetParent(hWnd)));
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

void CSubControlGaShowHideView::OnDestroy()
{
	CSubControlBaseView::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	if (m_OldGridListSubProc) {
		SetWindowLongPtr(GetDlgItem(IDC_GASHOWHIDELIST)->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<LONG_PTR>(m_OldGridListSubProc));
		m_OldGridListSubProc = NULL;
	}
}

void CSubControlGaShowHideView::OnPopupGaShowHideDeleteItem()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	DeleteCurSelItem();
}

void CSubControlGaShowHideView::DeleteCurSelItem()
{
	if (0 >= m_GridList.GetSelCount()) {
		return;
	}

	TP_LISTAUTO aList;

	CString aText = _T("Are you sure to delete this item(s) ?\n\n");
	for (int aIndex = 0 ; m_GridList.GetItemCount() > aIndex ; ++aIndex) {
		if (!m_GridList.IsSelected(aIndex)) {
			continue;
		}

		INT aServerID = static_cast<INT>(m_GridList.GetItemData(aIndex));
		ASSERT(0 < aServerID);

		aText += CVarArg<MAX_PATH>(_T("Server ID : %d\n"), aServerID);

		aList.push_back(TP_LISTELEM(aIndex, aServerID));
	}

	if (IDOK != ::AfxMessageBox(aText, MB_ICONQUESTION | MB_OKCANCEL)) {
		return;
	}

	std::sort(aList.begin(), aList.end(), CSortList());

	for (TP_LISTAUTO_CTR aIt = aList.begin() ; aList.end() != aIt ; ++aIt) {
		m_ListServ.erase(static_cast<INT>(aIt->second));

		m_GridList.DeleteItem(static_cast<INT>(aIt->first));
	}

}
