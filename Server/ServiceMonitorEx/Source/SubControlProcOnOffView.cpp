// SubControlProcOnOffView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "SubControlProcOnOffView.h"
#include "ControlDialog.h"
#include "NetConnection.h"

#define DNPRJSAFE	// DNPrjSafe.h ����
#include "CriticalSection.h"
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"


// CSubControlProcOnOffView ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSubControlProcOnOffView, CSubControlBaseView)

CSubControlProcOnOffView::CSubControlProcOnOffView(CWnd* pParent /*=NULL*/)
	: CSubControlBaseView(EV_CTT_PROCONOFF, CSubControlProcOnOffView::IDD, pParent)
{
	m_OldGridListSubProc = NULL;
	m_RequestType = EV_RQT_CNT;
	m_pSession = NULL;
}

CSubControlProcOnOffView::~CSubControlProcOnOffView()
{
}

void CSubControlProcOnOffView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCONOFFLIST, m_GridList);
}


BEGIN_MESSAGE_MAP(CSubControlProcOnOffView, CSubControlBaseView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PROCONOFFREQUEST, &CSubControlProcOnOffView::OnBnClickedProcOnOffRequest)
	ON_COMMAND_RANGE(IDC_PROCONOFFRADIO_STARTALL, IDC_PROCONOFFRADIO_STOPFRCSEL, &CSubControlProcOnOffView::OnBnClickedRadioGroup)
	ON_WM_DESTROY()
	ON_COMMAND(ID_POPUP_PROCONOFF_DELETEITEM, &CSubControlProcOnOffView::OnPopupProcOnOffDeleteItem)
END_MESSAGE_MAP()


void CSubControlProcOnOffView::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnOK();
}

void CSubControlProcOnOffView::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

//	CDialog::OnCancel();
}

void CSubControlProcOnOffView::UpdateData()
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

void CSubControlProcOnOffView::AddGridList(const SERVERINFO* pServerInfo)
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

	// Server State
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), (CHECK_LIMIT(pServerInfo->m_ServerState, EV_SVS_CNT)?(g_ServerStateName[pServerInfo->m_ServerState]):(_T("N/A"))));
	m_GridList.SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	m_GridList.UnlockWindowUpdate();
	EndWaitCursor();

}

// CSubControlProcOnOffView �޽��� ó�����Դϴ�.

BOOL CSubControlProcOnOffView::OnInitDialog()
{
	CSubControlBaseView::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	CRect aRect;
	GetClientRect(&aRect);

	// Control ListView
	{
		int aColNo = 0;

		m_GridList.InsertColumn(aColNo++, _T("Server ID"), LVCFMT_LEFT, 80);
		m_GridList.InsertColumn(aColNo++, _T("Server Type"), LVCFMT_LEFT, 120);
		m_GridList.InsertColumn(aColNo++, _T("World ID"), LVCFMT_LEFT, 80);
		m_GridList.InsertColumn(aColNo++, _T("Server State"), LVCFMT_LEFT, 90);

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
		GetDlgItem(IDC_PROCONOFFGROUP)->MoveWindow(
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

		/*
		GetDlgItem(IDC_PROCONOFFRADIO_STARTALL)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_RADIO_PAD_LFT,
			aStartY,
			EV_UIS_RADIO_WTH,
			EV_UIS_RADIO_HGT,
			TRUE
			);
		aStartY += EV_UIS_RADIO_PAD_Y;
		*/

		GetDlgItem(IDC_PROCONOFFRADIO_STARTSEL)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_RADIO_PAD_LFT,
			aStartY,
			EV_UIS_RADIO_WTH,
			EV_UIS_RADIO_HGT,
			TRUE
			);
		aStartY += EV_UIS_RADIO_PAD_Y;

		/*
		GetDlgItem(IDC_PROCONOFFRADIO_STOPSAFEALL)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_RADIO_PAD_LFT,
			aStartY,
			EV_UIS_RADIO_WTH,
			EV_UIS_RADIO_HGT,
			TRUE
			);
		aStartY += EV_UIS_RADIO_PAD_Y;
		*/

		GetDlgItem(IDC_PROCONOFFRADIO_STOPSAFESEL)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_RADIO_PAD_LFT,
			aStartY,
			EV_UIS_RADIO_WTH,
			EV_UIS_RADIO_HGT,
			TRUE
			);
		aStartY += EV_UIS_RADIO_PAD_Y;

		/*
		GetDlgItem(IDC_PROCONOFFRADIO_STOPFRCALL)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_RADIO_PAD_LFT,
			aStartY,
			EV_UIS_RADIO_WTH,
			EV_UIS_RADIO_HGT,
			TRUE
			);
		aStartY += EV_UIS_RADIO_PAD_Y;

		GetDlgItem(IDC_PROCONOFFRADIO_STOPFRCSEL)->MoveWindow(
			aRect.left + EV_UIS_LIST_PAD_X + EV_UIS_LIST_WTH + EV_UIS_GROUP_GAP_X + EV_UIS_RADIO_PAD_LFT,
			aStartY,
			EV_UIS_RADIO_WTH,
			EV_UIS_RADIO_HGT,
			TRUE
			);
		aStartY += EV_UIS_RADIO_PAD_Y;
		*/
	}

	// Button
	{
		GetDlgItem(IDC_PROCONOFFREQUEST)->MoveWindow(
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
		m_LayoutManager.TieChild(IDC_PROCONOFFLIST, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT, OX_LMT_SAME);
		m_LayoutManager.TieChild(IDC_PROCONOFFGROUP, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
		m_LayoutManager.TieChild(IDC_PROCONOFFREQUEST, OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
	}
	
	// Grid List Subclassing
	{
		m_OldGridListSubProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(GetDlgItem(IDC_PROCONOFFLIST)->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<LONG_PTR>(GridListSubProc)));
	}
	
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CSubControlProcOnOffView::PreTranslateMessage(MSG* pMsg)
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

void CSubControlProcOnOffView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CSubControlBaseView::OnPaint()��(��) ȣ������ ���ʽÿ�.
}

void CSubControlProcOnOffView::OnSize(UINT nType, int cx, int cy)
{
	CSubControlBaseView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	m_LayoutManager.OnSize(cx,cy);
}

void CSubControlProcOnOffView::OnBnClickedProcOnOffRequest()
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
	switch(m_RequestType) {
	case EV_RQT_STARTSEL:
	case EV_RQT_STOPSAFESEL:
	case EV_RQT_STOPFRCSEL:
		{
			if (m_ListServ.empty()) {
				::AfxMessageBox(_T("Server list is empty !"), MB_ICONINFORMATION | MB_OK);
				return;
			}
		}
		break;
	default:
		break;
	}

	//����üũ ���⼭ ó��
	switch(m_RequestType)
	{
	case EV_RQT_STARTSEL:
	case EV_RQT_STARTALL:
		{
			if (SERVERMONITOR_LEVEL_ADMIN > m_pSession->GetMonitorLevel()) 
			{
				::AfxMessageBox(_T("You have no authority to request"), MB_ICONWARNING | MB_OK);
				return;
			}
		}
		break;
	case EV_RQT_STOPSAFEALL:
	case EV_RQT_STOPSAFESEL:
	case EV_RQT_STOPFRCALL:
	case EV_RQT_STOPFRCSEL:
		{
			if (SERVERMONITOR_LEVEL_SUPERADMIN > m_pSession->GetMonitorLevel()) 
			{
				::AfxMessageBox(_T("You have no authority to request"), MB_ICONWARNING | MB_OK);
				return;
			}
		}
		break;
	}

	if (IDOK != ::AfxMessageBox(_T("Are you sure to request ?"), MB_ICONQUESTION | MB_OKCANCEL)) {
		return;
	}

	switch(m_RequestType) {
	case EV_RQT_STARTALL:			// Start (All)
		{
			TSMServiceStart aReq;
			aReq.nStartType = _SERVICE_START_ALL;
			aReq.cCount = 0;
			m_pSession->SendData(MONITOR2MANAGER_STARTSERVICE, 0, reinterpret_cast<CHAR*>(&aReq), (static_cast<int>((sizeof(aReq) - (sizeof(aReq.nIDs[0]) * (_countof(aReq.nIDs) - aReq.cCount)))&ULONG_MAX)));
		}
		break;
	case EV_RQT_STARTSEL:			// Start (Select)
		{
			TSMServiceStart aReq;
			aReq.nStartType = _SERVICE_START_EACH;
			aReq.cCount = 0;

			TP_LISTSERV_CTR aIt = m_ListServ.begin();
			for (; m_ListServ.end() != aIt ; ++aIt) {
				const SERVERINFO* aServerInfo = (&(aIt->second));
				if (!aServerInfo) return;
				if(!aServerInfo->IsSet()) return;

				aReq.nIDs[aReq.cCount++] = aServerInfo->m_ServerID;

				if (_countof(aReq.nIDs) <= aReq.cCount) {
					::AfxMessageBox(CVarArg<MAX_PATH>(_T("Item count you requested is over %d.\n\novered item will be ignored."), _countof(aReq.nIDs)), MB_ICONWARNING | MB_OK);
					break;
				}
			}

			if (0 < aReq.cCount) {
				m_pSession->SendData(MONITOR2MANAGER_STARTSERVICE, 0, reinterpret_cast<CHAR*>(&aReq), (static_cast<int>((sizeof(aReq) - (sizeof(aReq.nIDs[0]) * (_countof(aReq.nIDs) - aReq.cCount)))&ULONG_MAX)));
			}
			else {
				ASSERT(0);
			}
		}
		break;
	case EV_RQT_STOPSAFEALL:		// Stop Safe (All)
		{
			TSMServiceStop aReq;
			aReq.nStopType = _SERVICE_STOP_ALL;
			aReq.cCount = 0;
			m_pSession->SendData(MONITOR2MANAGER_STOPSERVICE, 0, reinterpret_cast<CHAR*>(&aReq), (static_cast<int>((sizeof(aReq) - (sizeof(aReq.nIDs[0]) * (_countof(aReq.nIDs) - aReq.cCount)))&ULONG_MAX)));
		}
		break;
	case EV_RQT_STOPSAFESEL:		// Stop Safe (Select)
		{
			TSMServiceStop aReq;
			aReq.nStopType = _SERVICE_STOP_EACH;
			aReq.cCount = 0;

			TP_LISTSERV_CTR aIt = m_ListServ.begin();
			for (; m_ListServ.end() != aIt ; ++aIt) {
				const SERVERINFO* aServerInfo = (&(aIt->second));
				if (!aServerInfo) return;
				if(!aServerInfo->IsSet()) return;

				if (aServerInfo->m_ServerState != EV_SVS_CNNT)
				{
					::AfxMessageBox(CVarArg<MAX_PATH>(_T("Terminate Process Busy[SID:%d]"), aServerInfo->m_ServerID), MB_ICONINFORMATION);
					continue;
				}


				aReq.nIDs[aReq.cCount++] = aServerInfo->m_ServerID;

				if (_countof(aReq.nIDs) <= aReq.cCount) {
					::AfxMessageBox(CVarArg<MAX_PATH>(_T("Item count you requested is over %d.\n\novered item will be ignored."), _countof(aReq.nIDs)), MB_ICONWARNING | MB_OK);
					break;
				}
			}

			if (0 < aReq.cCount)
				m_pSession->SendData(MONITOR2MANAGER_STOPSERVICE, 0, reinterpret_cast<CHAR*>(&aReq), (static_cast<int>((sizeof(aReq) - (sizeof(aReq.nIDs[0]) * (_countof(aReq.nIDs) - aReq.cCount)))&ULONG_MAX)));
			
		}
		break;
	case EV_RQT_STOPFRCALL:			// Stop Force (All)
		{
			TSMServiceStop aReq;
			aReq.nStopType = _SERVICE_STOP_ALL_FORCE;
			aReq.cCount = 0;
			m_pSession->SendData(MONITOR2MANAGER_STOPSERVICE, 0, reinterpret_cast<CHAR*>(&aReq), (static_cast<int>((sizeof(aReq) - (sizeof(aReq.nIDs[0]) * (_countof(aReq.nIDs) - aReq.cCount)))&ULONG_MAX)));
		}
		break;
	case EV_RQT_STOPFRCSEL:			// Stop Force (Select)
		{
			TSMServiceStop aReq;
			aReq.nStopType = _SERVICE_STOP_EACH_FORCE;
			aReq.cCount = 0;

			TP_LISTSERV_CTR aIt = m_ListServ.begin();
			for (; m_ListServ.end() != aIt ; ++aIt) {
				const SERVERINFO* aServerInfo = (&(aIt->second));
				if (!aServerInfo) return;
				if(!aServerInfo->IsSet()) return;

				aReq.nIDs[aReq.cCount++] = aServerInfo->m_ServerID;

				if (_countof(aReq.nIDs) <= aReq.cCount) {
					::AfxMessageBox(CVarArg<MAX_PATH>(_T("Item count you requested is over %d.\n\novered item will be ignored."), _countof(aReq.nIDs)), MB_ICONWARNING | MB_OK);
					break;
				}
			}

			if (0 < aReq.cCount) {
				m_pSession->SendData(MONITOR2MANAGER_STOPSERVICE, 0, reinterpret_cast<CHAR*>(&aReq), (static_cast<int>((sizeof(aReq) - (sizeof(aReq.nIDs[0]) * (_countof(aReq.nIDs) - aReq.cCount)))&ULONG_MAX)));
			}
			else {
				ASSERT(0);
			}
		}
		break;
	default:
		BASE_BREAK;
	}

	GetParent()->GetParent()->ShowWindow(SW_HIDE);

}

void CSubControlProcOnOffView::OnBnClickedRadioGroup(UINT pCtrlID)
{
	switch(pCtrlID) {
	case IDC_PROCONOFFRADIO_STARTALL:
		{
			m_RequestType = EV_RQT_STARTALL;
			m_GridList.EnableWindow(FALSE);
		}
		break;
	case IDC_PROCONOFFRADIO_STARTSEL:
		{
			m_RequestType = EV_RQT_STARTSEL;
			m_GridList.EnableWindow(TRUE);
		}
		break;
	case IDC_PROCONOFFRADIO_STOPSAFEALL:
		{
			m_RequestType = EV_RQT_STOPSAFEALL;
			m_GridList.EnableWindow(FALSE);
		}
		break;
	case IDC_PROCONOFFRADIO_STOPSAFESEL:
		{
			m_RequestType = EV_RQT_STOPSAFESEL;
			m_GridList.EnableWindow(TRUE);
		}
		break;
	case IDC_PROCONOFFRADIO_STOPFRCALL:
		{
			m_RequestType = EV_RQT_STOPFRCALL;
			m_GridList.EnableWindow(FALSE);
		}
		break;
	case IDC_PROCONOFFRADIO_STOPFRCSEL:
		{
			m_RequestType = EV_RQT_STOPFRCSEL;
			m_GridList.EnableWindow(TRUE);
		}
		break;
	}

}

void CSubControlProcOnOffView::DoPopupMenu()
{
	CMenu aPopMenu;
	VERIFY(aPopMenu.LoadMenu(IDR_PROCONOFF_POPUP));

	// set default item
	MENUITEMINFO aItemInfo = { sizeof(MENUITEMINFO) };
	aItemInfo.fMask = MIIM_STATE;
	aItemInfo.fState = MFS_DEFAULT;
	::SetMenuItemInfo(aPopMenu.m_hMenu, ID_POPUP_PROCONOFF_DELETEITEM, FALSE, &aItemInfo);

	CPoint aPosMouse;
	GetCursorPos(&aPosMouse);

	// show menu
	SendMessage(WM_NCACTIVATE, FALSE);
	aPopMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPosMouse.x, aPosMouse.y, this);
}

LRESULT CALLBACK CSubControlProcOnOffView::GridListSubProc(HWND hWnd, UINT pMessage, WPARAM wParam, LPARAM lParam)
{
	CSubControlProcOnOffView* aParent = static_cast<CSubControlProcOnOffView*>(CWnd::FromHandlePermanent(::GetParent(hWnd)));
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

void CSubControlProcOnOffView::OnDestroy()
{
	CSubControlBaseView::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	if (m_OldGridListSubProc) {
		SetWindowLongPtr(GetDlgItem(IDC_PROCONOFFLIST)->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<LONG_PTR>(m_OldGridListSubProc));
		m_OldGridListSubProc = NULL;
	}
}

void CSubControlProcOnOffView::OnPopupProcOnOffDeleteItem()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	DeleteCurSelItem();
}

void CSubControlProcOnOffView::DeleteCurSelItem()
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
