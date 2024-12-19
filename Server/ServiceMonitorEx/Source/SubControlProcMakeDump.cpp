// SubControlProcMakeDump.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "SubControlProcMakeDump.h"
#include "ControlDialog.h"
#include "NetConnection.h"

#define DNPRJSAFE	// DNPrjSafe.h 제외
#include "CriticalSection.h"
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"

// CSubControlProcMakeDump 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSubControlProcMakeDump, CSubControlBaseView)

CSubControlProcMakeDump::CSubControlProcMakeDump(CWnd* pParent /*=NULL*/)
	: CSubControlBaseView(EV_CTT_PROCMAKEDUMP, CSubControlProcMakeDump::IDD, pParent)
{
	m_OldGridListSubProc = NULL;
	m_pSession = NULL;
}

CSubControlProcMakeDump::~CSubControlProcMakeDump()
{
}

void CSubControlProcMakeDump::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCMAKEDUMPLIST, m_GridList);
}


BEGIN_MESSAGE_MAP(CSubControlProcMakeDump, CSubControlBaseView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PROCMAKEDUMPREQUEST, &CSubControlProcMakeDump::OnBnClickedProcMakeDumpRequest)
	ON_WM_DESTROY()
	ON_COMMAND(ID_POPUP_PROCMAKEDUMP_DELETEITEM, &CSubControlProcMakeDump::OnPopupProcMakeDumpDeleteItem)
END_MESSAGE_MAP()


void CSubControlProcMakeDump::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

//	CDialog::OnOK();
}

void CSubControlProcMakeDump::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

//	CDialog::OnCancel();
}

void CSubControlProcMakeDump::UpdateData()
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

		AddGridList(aServerInfo);

		m_ListServ.insert(TP_LISTSERV::value_type(aServerInfo->m_ServerID, (*aServerInfo)));
	}
}

void CSubControlProcMakeDump::AddGridList(const SERVERINFO* pServerInfo)
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
	aLvItem.lParam		= pServerInfo->m_ServerID;	// 추후 빠른 정보 검색을 위해 파라메터로 지정

	TCHAR sBuffer[MAX_PATH*2] = { _T('\0'), };

	// Server ID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pServerInfo->m_ServerID);
	aLvItem.pszText = sBuffer;

	int nRtnValue = m_GridList.InsertItem(&aLvItem);
	if (-1 == nRtnValue) return;

	// Server Type
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"),  g_ServerTypeFullName[pServerInfo->m_ServerType]);
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

	m_GridList.UnlockWindowUpdate();
	EndWaitCursor();

}

// CSubControlProcMakeDump 메시지 처리기입니다.

BOOL CSubControlProcMakeDump::OnInitDialog()
{
	CSubControlBaseView::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CRect aRect;
	GetClientRect(&aRect);

	// Control ListView
	{
		int aColNo = 0;

		m_GridList.InsertColumn(aColNo++, _T("Server ID"), LVCFMT_LEFT, 80);
		m_GridList.InsertColumn(aColNo++, _T("Server Type"), LVCFMT_LEFT, 120);
		m_GridList.InsertColumn(aColNo++, _T("World ID"), LVCFMT_LEFT, 80);

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
		GetDlgItem(IDC_PROCMAKEDUMPGROUP)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X,
			aRect.top + EV_UIS_GROUP_PAD_TOP,
			aRect.right - (aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_GROUP_PAD_X),
			aRect.bottom - (aRect.top + EV_UIS_GROUP_PAD_TOP + EV_UIS_GROUP_PAD_Y),
			TRUE
			);
	}

	// Button
	{
		GetDlgItem(IDC_PROCMAKEDUMPREQUEST)->MoveWindow(
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
		m_LayoutManager.TieChild(IDC_PROCMAKEDUMPLIST, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT, OX_LMT_SAME);
		m_LayoutManager.TieChild(IDC_PROCMAKEDUMPGROUP, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
		m_LayoutManager.TieChild(IDC_PROCMAKEDUMPREQUEST, OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
	}

	// Grid List Subclassing
	{
		m_OldGridListSubProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(GetDlgItem(IDC_PROCMAKEDUMPLIST)->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<LONG_PTR>(GridListSubProc)));
	}
	



	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CSubControlProcMakeDump::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	switch(pMsg->message) {
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam) {
			case VK_ESCAPE:
				GetParent()->GetParent()->ShowWindow(SW_HIDE);
				return TRUE;	// ESC 키로 부모 탭 컨트롤의 부모 대화상자를 숨김
//				break;
			}
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSubControlProcMakeDump::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CSubControlBaseView::OnPaint()을(를) 호출하지 마십시오.
}

void CSubControlProcMakeDump::OnSize(UINT nType, int cx, int cy)
{
	CSubControlBaseView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_LayoutManager.OnSize(cx,cy);
}

void CSubControlProcMakeDump::OnBnClickedProcMakeDumpRequest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 네트워크 연결, 구성정보 구축 여부 체크
	if (! m_pSession) return;
	if (!m_pSession->IsConnect()) {
		::AfxMessageBox(_T("Session is not connected !"), MB_ICONINFORMATION | MB_OK);
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

	TSMMakeDump aReq;
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
		m_pSession->SendData(MONITOR2MANAGER_MAKEDUMP, 0, reinterpret_cast<CHAR*>(&aReq), (static_cast<int>((sizeof(aReq) - (sizeof(aReq.nSID[0]) * (_countof(aReq.nSID) - aReq.cCount)))&ULONG_MAX)));
	}
	else {
		ASSERT(0);
	}

	GetParent()->GetParent()->ShowWindow(SW_HIDE);

}

void CSubControlProcMakeDump::DoPopupMenu()
{
	CMenu aPopMenu;
	VERIFY(aPopMenu.LoadMenu(IDR_PROCMAKEDUMP_POPUP));

	// set default item
	MENUITEMINFO aItemInfo = { sizeof(MENUITEMINFO) };
	aItemInfo.fMask = MIIM_STATE;
	aItemInfo.fState = MFS_DEFAULT;
	::SetMenuItemInfo(aPopMenu.m_hMenu, ID_POPUP_PROCMAKEDUMP_DELETEITEM, FALSE, &aItemInfo);

	CPoint aPosMouse;
	GetCursorPos(&aPosMouse);

	// show menu
	SendMessage(WM_NCACTIVATE, FALSE);
	aPopMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPosMouse.x, aPosMouse.y, this);
}

LRESULT CALLBACK CSubControlProcMakeDump::GridListSubProc(HWND hWnd, UINT pMessage, WPARAM wParam, LPARAM lParam)
{
	CSubControlProcMakeDump* aParent = static_cast<CSubControlProcMakeDump*>(CWnd::FromHandlePermanent(::GetParent(hWnd)));
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

void CSubControlProcMakeDump::OnDestroy()
{
	CSubControlBaseView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (m_OldGridListSubProc) {
		SetWindowLongPtr(GetDlgItem(IDC_PROCMAKEDUMPLIST)->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<LONG_PTR>(m_OldGridListSubProc));
		m_OldGridListSubProc = NULL;
	}
}

void CSubControlProcMakeDump::OnPopupProcMakeDumpDeleteItem()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	DeleteCurSelItem();
}

void CSubControlProcMakeDump::DeleteCurSelItem()
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
