// DnEditLauncherDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DnResourceEditer.h"
#include "DnEditLauncherDlg.h"
#include "DnControlManager.h"
#include "DnTextModifyDlg.h"
#include "DnScaleDlg.h"
#include "DnDynamicCtrl.h"
#include "DnAboutDlg.h"
#include "DnSetRectDlg.h"

// DnEditLauncherDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DnEditLauncherDlg, CDialog)

DnEditLauncherDlg::DnEditLauncherDlg(CWnd* pParent /*=NULL*/)
:CDialog(DnEditLauncherDlg::IDD, pParent) , 
 m_bLBtnClickedFlag(false), m_pLBtnClickedControl(NULL)
{
}

DnEditLauncherDlg::~DnEditLauncherDlg()
{
}

void DnEditLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CONTROL,		m_TreeSoft);
	DDX_Control(pDX, IDC_BUTTON_QUIT,		m_BtnQuit);
	DDX_Control(pDX, IDC_BUTTON_GAMESTART,	m_btnStartGame);
	DDX_Control(pDX, IDC_CHECK_DIRECTSTART, m_cbDirectStart);

	DDX_Control(pDX, IDC_BUTTON_OPTION, m_btnOption);
	DDX_Control(pDX, IDC_BUTTON_WINCLOSE, m_btnWinClose);
	DDX_Control(pDX, IDC_BUTTON_WINMINIMUM, m_btnWinMini);
	DDX_Control(pDX, IDC_BUTTON_START_PATCH, m_btnStartPatch);
	DDX_Control(pDX, IDC_BUTTON_SELECT_PARTITION, m_btnSelectPartition);
	DDX_Control(pDX, IDC_STATIC_LAUNCHER_VER, m_stVersion);
	DDX_Control(pDX, IDC_STATIC_FILECOUNT, m_stFileCnt);
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_stFileName);
	DDX_Control(pDX, IDC_STATIC_TIMELEFT, m_stTimeLeft);
	DDX_Control(pDX, IDC_STATIC_FILESIZE, m_stFileSize);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_PER, m_stDownPercent);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_STATE, m_stDownState);
	DDX_Control(pDX, IDC_STATIC_DIRECTSTART_TEXT, m_stDirectStart);
	DDX_Control(pDX, IDC_STATIC_SELECT_PARTITION, m_stSelectPartition);
	DDX_Control(pDX, IDC_STATIC_SELECT_PARTITION_NAME, m_stPartitionName);
	DDX_Control(pDX, IDC_STATIC_SELECT_PARTITION_GUIDE, m_stPartitionGuide);

	DDX_Control(pDX, IDC_PROGRESS_FILECOUNT, m_prgFileCount);
	DDX_Control(pDX, IDC_PROGRESS_DOWNLOAD, m_prgDownLoad);
	//DDX_Control(pDX, IDC_STATIC_WEBPAGE, m_pcWebpage); // ��������...

	DDX_Control(pDX, IDC_BUTTON_PROGRESS_DOWNLOAD, m_btnProgressDownload);
	DDX_Control(pDX, IDC_BUTTON_PROGRESS_COUNT, m_btnProgressCount);
	DDX_Control(pDX, IDC_BUTTON_WEBPAGE, m_btnWebpage);
	
}


BEGIN_MESSAGE_MAP(DnEditLauncherDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONTROL, &DnEditLauncherDlg::OnTvnSelchangedTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE_CONTROL, &DnEditLauncherDlg::OnNMClickTree1)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CONTROL, &DnEditLauncherDlg::OnNMRclickTreeControl)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_QUIT, OnUpdateCmdUI)
	ON_COMMAND(ID_LOAD_IMAGE, &DnEditLauncherDlg::OnLoadImage)
	//ON_BN_CLICKED(IDC_BUTTON_SAVE, &DnEditLauncherDlg::OnBnClickedButtonSave)
	//ON_BN_CLICKED(IDC_BUTTON_LOAD, &DnEditLauncherDlg::OnBnClickedButtonLoad)
	ON_COMMAND(ID_MENU_TEXTMODIFY, &DnEditLauncherDlg::OnMenuTextmodify)
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_FILE_MAIN_SAVE, &DnEditLauncherDlg::OnFileMainSave)
	ON_COMMAND(ID_FILE_MAIN_LOAD, &DnEditLauncherDlg::OnFileMainLoad)
	ON_COMMAND(ID_FILE_MAIN_QUIT, &DnEditLauncherDlg::OnFileMainQuit)
	ON_COMMAND(ID_ABOUT, &DnEditLauncherDlg::OnAbout)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENU_POS_DEFAULT, &DnEditLauncherDlg::OnMenuPosDefault)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_MENU_SCALE, &DnEditLauncherDlg::OnMenuScale)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MENU_RESOURCE_RESET, &DnEditLauncherDlg::OnMenuResourceReset)
	ON_COMMAND(ID_CREATE_BTN, &DnEditLauncherDlg::OnCreateBtn)

	//ON_COMMAND(ID_DYNAMIC_BTN, &DnEditLauncherDlg::OnFileMainQuit)
	
	ON_COMMAND(ID_CREATE_STATICTEXT, &DnEditLauncherDlg::OnCreateStatictext)
	ON_COMMAND(ID_CREATE_PROGRESS, &DnEditLauncherDlg::OnCreateProgress)
	ON_COMMAND(ID_CREATE_CHECKBOX, &DnEditLauncherDlg::OnCreateCheckbox)
	ON_COMMAND(ID_MENU_CTRLMODIFY, &DnEditLauncherDlg::OnMenuCtrlmodify)
	
	ON_COMMAND(ID_FILE_NEWFILE, &DnEditLauncherDlg::OnFileNewfile)
	ON_COMMAND(ID_MENU_DELETE, &DnEditLauncherDlg::OnMenuDelete)
	
	ON_COMMAND(ID_MENU_SETRECT, &DnEditLauncherDlg::OnMenuSetrect)
	ON_COMMAND(ID_CONTROL_LOCK, &DnEditLauncherDlg::OnControlLock)
	ON_COMMAND(ID_CONTROL_UNLOCK, &DnEditLauncherDlg::OnControlUnlock)
END_MESSAGE_MAP()



// DnEditLauncherDlg �޽��� ó�����Դϴ�.
BOOL DnEditLauncherDlg::OnInitDialog()
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CDialog::OnInitDialog();

	// ����Ű ����.
	m_hAccel = LoadAccelerators( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// ��Ʈ ����.
	m_Font.CreateFont( 11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("����") );
	
	// ���̾�α� ��� �̹��� ����.
	if( !InitBackGround() )
	{
		this->EndDialog(0);
		return false;
	}

	// Ʈ�� ��Ʈ�� �ʱ�ȭ
	if( !InitTreeControl() )
	{
		this->EndDialog(0);
		return false;
	}

	return TRUE;
}

int DnEditLauncherDlg::InitTreeControl()
{
	// ���� ��ư ����.
	DNCTRLMANAGER.m_hRootItem		= m_TreeSoft.InsertItem(_T("��Ʈ�� ���"), TVI_ROOT, TVI_LAST);
	DNCTRLMANAGER.m_hBtnItem		= m_TreeSoft.InsertItem(_T("BUTTONS"),		DNCTRLMANAGER.m_hRootItem, TVI_LAST);
	DNCTRLMANAGER.m_hTextItem		= m_TreeSoft.InsertItem(_T("STATIC_TEXT"),	DNCTRLMANAGER.m_hRootItem, TVI_LAST);
	DNCTRLMANAGER.m_hProgressItem	= m_TreeSoft.InsertItem(_T("PROGRESSBAR"),	DNCTRLMANAGER.m_hRootItem, TVI_LAST);
	DNCTRLMANAGER.m_hCheckBoxItem	= m_TreeSoft.InsertItem(_T("CHECK_BOX"),	DNCTRLMANAGER.m_hRootItem, TVI_LAST);
	
	// ��ģ��.
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hRootItem,	TVE_EXPAND);
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hBtnItem,		TVE_EXPAND);
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hTextItem,	TVE_EXPAND);
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hProgressItem,TVE_EXPAND);
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hCheckBoxItem,TVE_EXPAND);

	// ��ġ ����.
	RECT treeRT;
	m_TreeSoft.GetWindowRect(&treeRT);
	m_TreeSoft.SetWindowPos(NULL, m_dwWidth+10, (m_dwHeight-m_dwHeight)+10, treeRT.right-treeRT.left , treeRT.bottom-treeRT.top, SWP_NOZORDER );
	
	
	// Ʈ�� ��Ʈ���� �ּ� ����.
	DNCTRLMANAGER.SetTreeListPointer(&m_TreeSoft);

	return true;
}


int DnEditLauncherDlg::InitBackGround()
{
	// make bitmap
	CString szFilePath;
	HBITMAP hBmp = DNCTRLMANAGER.MakeBitmap(szFilePath);
	if( hBmp == NULL )
	{
		return false;
	}

	// set window size
	BITMAP bmp = { 0 };
	GetObject( hBmp, sizeof(BITMAP), &bmp );
	m_dwWidth	= bmp.bmWidth;		// bitmap width
	m_dwHeight	= bmp.bmHeight;		// bitmap height
	
	// build memory dc for background
	CDC* dc = GetDC();
	m_dcBkGrnd = CreateCompatibleDC( dc->m_hDC );
	ReleaseDC( dc );

	// select background image
	SelectObject( m_dcBkGrnd, hBmp );
	
	// set window size the same as image size
	SetWindowPos( this, 0, 0, m_dwWidth + _WINDOW_OFFSET , m_dwHeight + 50, SWP_NOZORDER | SWP_NOMOVE);
	
	return true;
}

BOOL DnEditLauncherDlg::OnEraseBkgnd(CDC* pDC)
{
	RECT rt;
	GetClientRect(&rt);
	pDC->FillSolidRect(&rt, RGB(204,204,255));


	BitBlt( pDC->m_hDC, 0, 0, m_dwWidth, m_dwHeight, m_dcBkGrnd, 0 , 0 ,SRCCOPY );
	return TRUE;
}

void DnEditLauncherDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void DnEditLauncherDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// ���콺 ���� Ŭ��
	SetCapture();

	POINT pt;
	GetCursorPos(&pt);
	
	// ���� ���õ� ��Ʈ��
	int nEnum = -1;
	m_pLBtnClickedControl = DNCTRLMANAGER.CheckCollision(pt, nEnum);
	if( m_pLBtnClickedControl != NULL )
	{
		if(DNCTRLMANAGER.IsLockObject(nEnum))
		{
			return;
		}

		m_bLBtnClickedFlag	= true;
		m_LBtnClickedPos	= point;
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void DnEditLauncherDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// ���콺 �̵�
	if( m_bLBtnClickedFlag )
	{
		POINT	MousePt;
		RECT	rect;
		
		GetClientRect(&rect);
		
		GetCursorPos(&MousePt);
		ScreenToClient(&MousePt);
		
		if( MousePt.x < 0 || MousePt.y < 0  || MousePt.x > rect.right || MousePt.y > rect.bottom )
		{
			// ������ ���� ��;
			return;
		}
		
		RECT rtLocal, rtWorld;
		m_pLBtnClickedControl->GetClientRect(&rtLocal); // ���� ��ǥ
		m_pLBtnClickedControl->GetWindowRect(&rtWorld); // ���� ��ǥ
		
		int nhalf_X = rtLocal.right  / 2;
		int nhalf_Y = rtLocal.bottom / 2;
		m_pLBtnClickedControl->SetWindowPos(NULL, point.x - nhalf_X, point.y - nhalf_Y, 0,0,SWP_NOSIZE | SWP_NOZORDER);
		
		RECT rt;
		m_pLBtnClickedControl->GetClientRect(&rt);
		m_pLBtnClickedControl->InvalidateRect(&rt);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void DnEditLauncherDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// ���콺 ���� Ŭ�� ��
	if( GetCapture() == this && m_bLBtnClickedFlag && m_pLBtnClickedControl )
	{
		ReleaseCapture();
		m_bLBtnClickedFlag = false;
		
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);

		RECT rtLocal, rtWorld;
		m_pLBtnClickedControl->GetClientRect(&rtLocal); // ���� ��ǥ
		m_pLBtnClickedControl->GetWindowRect(&rtWorld); // ���� ��ǥ

		int nhalf_X = rtLocal.right  / 2;
		int nhalf_Y = rtLocal.bottom / 2;

		m_pLBtnClickedControl->SetWindowPos(NULL, point.x - nhalf_X, point.y - nhalf_Y, 0,0,SWP_NOSIZE | SWP_NOZORDER);
		m_pLBtnClickedControl = NULL;

		Invalidate();
	}

	CDialog::OnLButtonUp(nFlags, point);
}


void DnEditLauncherDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// [���콺 �� Ŭ�� �ٿ�]


	CDialog::OnRButtonDown(nFlags, point);
}

void DnEditLauncherDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// [���콺 �� Ŭ�� ��]

	CDialog::OnRButtonUp(nFlags, point);
}



void DnEditLauncherDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	*pResult = 0;
}


void DnEditLauncherDlg::CheckChildItems( HTREEITEM hItem )
{
	HTREEITEM hChildItem = m_TreeSoft.GetChildItem(hItem);

	if(hChildItem)
	{
		while(hChildItem != NULL)
		{
			m_TreeSoft.SetCheck(hChildItem, true);

			//CWnd* pCtrl = DNCTRLMANAGER.GetCtrl( m_TreeSoft.GetItemText(hChildItem) );
		/*	if(pCtrl)
			{
				pCtrl->ShowWindow( !(pCtrl->IsWindowVisible()) );
				
				DNCTRLMANAGER.SetCheckBox(m_TreeSoft.GetItemText(hChildItem), 1);
			}

			if(m_TreeSoft.ItemHasChildren(hChildItem))
			{
				CheckChildItems(hChildItem);
			}*/

			hChildItem = m_TreeSoft.GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	else
	{
		CWnd* pCtrl = DNCTRLMANAGER.GetCtrl( m_TreeSoft.GetItemText(hItem) );
		if(pCtrl)
		{
			pCtrl->ShowWindow( !(pCtrl->IsWindowVisible()) );
			DNCTRLMANAGER.SetCheckBox(m_TreeSoft.GetItemText(hItem), 1);
		}
	}

}

void DnEditLauncherDlg::UnCheckChildItems( HTREEITEM hItem )
{
	HTREEITEM hChildItem = m_TreeSoft.GetChildItem(hItem);

	if(hChildItem)
	{
		while(hChildItem != NULL)
		{
			m_TreeSoft.SetCheck(hChildItem, false);

		//	CWnd* pCtrl = DNCTRLMANAGER.GetCtrl( m_TreeSoft.GetItemText(hChildItem) );
			//if(pCtrl)
			//{
			//	pCtrl->ShowWindow( !(pCtrl->IsWindowVisible()) );
			//	DNCTRLMANAGER.SetCheckBox(m_TreeSoft.GetItemText(hChildItem), 0);
			//}

			//if(m_TreeSoft.ItemHasChildren(hChildItem))
			//{
			//	UnCheckChildItems(hChildItem);
			//}

			hChildItem = m_TreeSoft.GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	else
	{
		CWnd* pCtrl = DNCTRLMANAGER.GetCtrl( m_TreeSoft.GetItemText(hItem) );
		if(pCtrl)
		{
			pCtrl->ShowWindow( !(pCtrl->IsWindowVisible()) );
			DNCTRLMANAGER.SetCheckBox(m_TreeSoft.GetItemText(hItem), 0);
		}
	}
	
}

void DnEditLauncherDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// Ʈ�� ����Ʈ���� ���콺 ��Ŭ��.

	DWORD pos = ::GetMessagePos();
	CPoint point( LOWORD( pos ), HIWORD( pos ) );
	m_TreeSoft.ScreenToClient( &point );

	UINT nFlags = 0;
	HTREEITEM hItem = m_TreeSoft.HitTest( point, &nFlags );
	CString		str	= m_TreeSoft.GetItemText(hItem);
	m_szRButtonItem	= str;

	if(hItem)
	{
		// üũ�ڽ��� Ŭ���� ���!
		if( nFlags & TVHT_ONITEMSTATEICON )
		{
			if( m_TreeSoft.GetCheck(hItem) )
			{
				UnCheckChildItems(hItem); // ����� �ʿ��� ������.
			}
			else
			{
				CheckChildItems(hItem);  // ����� �ʿ��� ������.
			}
		}
	}

	*pResult = 0;
}

void DnEditLauncherDlg::OnNMRclickTreeControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// Ʈ�� ����Ʈ���� ���콺 ��Ŭ��. [�˾� ����]

	CPoint	pt;
	UINT	nFlags = 0;
	
	GetCursorPos(&pt);
	m_TreeSoft.ScreenToClient( &pt );
	
	HTREEITEM	hItem	= m_TreeSoft.HitTest( pt, &nFlags );
	CString		str		= m_TreeSoft.GetItemText(hItem);
	m_szRButtonItem		= str;
	
	m_pRBtnClickedControl = DNCTRLMANAGER.GetCtrl(str);
	m_hTreeItem = hItem;

	if( str.GetLength() > 0 )
	{
		CPoint p;
		GetCursorPos(&p);

		CMenu menu;
		menu.LoadMenu(IDR_MENU_MODIFY);

		CMenu* Dmenu = menu.GetSubMenu(0);			// �Ǿ��� ������
		SetPossibleMenu(Dmenu, m_szRButtonItem);	// ������� �ʴ� �޴� ��Ȱ��ȭ.
		Dmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, p.x, p.y, this);
	}

	*pResult = 0;
}

void DnEditLauncherDlg::SetPossibleMenu( CMenu* pMenu, CString currentItemName )
{
	_CTRL_IT it;

	if( DNCTRLMANAGER.GetVecIterator(currentItemName, it) == false )
	{
		return;
	}

	if( (*it).m_WndPT == NULL )
	{
		return;
	}

	switch((*it).m_nType)
	{
	case _BUTTON:
		break;

	case _TEXT:
		pMenu->EnableMenuItem(ID_LOAD_IMAGE, MF_DISABLED);
		break;

	case _PROGRESS_BAR:
		pMenu->EnableMenuItem(ID_LOAD_IMAGE, MF_DISABLED);
		break;

	case _CHECK_BOX:
		pMenu->EnableMenuItem(ID_LOAD_IMAGE, MF_DISABLED);
		break;
	}	
}


void DnEditLauncherDlg::OnBnClickedButtonSave()
{
	 // �޴� -> �����ϱ�
	if( DNCTRLMANAGER.Save() == E_FAIL )
	{
		CString strErr;
		strErr.Format(CString("xml���� ����"));
		AfxMessageBox(strErr);

		return;
	}
}

void DnEditLauncherDlg::OnBnClickedButtonLoad()
{
	// �޴� -> �ҷ�����
	if( DNCTRLMANAGER.Load() == E_FAIL )
	{
		CString strErr;
		strErr.Format(CString("xml�ε� ����"));
		AfxMessageBox(strErr);

		return;
	}
}

void DnEditLauncherDlg::OnLoadImage()
{
	// �޴� -> �̹��� �ҷ�����
	CWnd* pCurrentSelectedItem = DNCTRLMANAGER.GetCtrl(m_szRButtonItem);
	if( pCurrentSelectedItem )
	{
		CString szFilePath;
		HBITMAP hBmp = DNCTRLMANAGER.MakeBitmap(szFilePath);
		if( hBmp == NULL )
		{
			return ;
		}

		if( !DNCTRLMANAGER.SetResourceFilePath(m_szRButtonItem, szFilePath) )
		{
			CString szMSG;
			szMSG.Format(_T("���ϰ�� ������ �����߽��ϴ�."));
			AfxMessageBox(szMSG);
		}

		BITMAP bmp = { 0 };
		GetObject( hBmp, sizeof(BITMAP), &bmp );

		CButton* pBtn = static_cast<CButton*> (pCurrentSelectedItem);
		pBtn->SetBitmap(hBmp);
		pBtn->SetWindowPos(NULL, 0,0,bmp.bmWidth,bmp.bmHeight,SWP_NOMOVE);
	}
}

void DnEditLauncherDlg::OnMenuPosDefault()
{
	// �޴� -> ��Ʈ�� ��ġ �ʱ�ȭ
	// �̻��ѵ��� ������ ���� ���ؼ�..
	CWnd* pCurrentSelectedItem = DNCTRLMANAGER.GetCtrl(m_szRButtonItem);
	
	if( pCurrentSelectedItem )
	{		
		pCurrentSelectedItem->SetWindowPos(NULL, 0,0,0,0, SWP_NOSIZE);
		Invalidate();
	}
}


void DnEditLauncherDlg::OnMenuTextmodify()
{
	// �޴� -> �ؽ�Ʈ ���� Ŭ��.
	CDnTextModifyDlg* pEditDialog = new CDnTextModifyDlg();

	if(pEditDialog)
	{
		if( pEditDialog->DoModal() == IDOK )
		{
			CString str;
			CWnd* pCurrentSelectedItem = DNCTRLMANAGER.GetCtrl(m_szRButtonItem);
			if(pCurrentSelectedItem)
			{
				pCurrentSelectedItem->SetWindowText(DNCTRLMANAGER.GetEditString());
				DNCTRLMANAGER.SetStaticText(m_szRButtonItem, DNCTRLMANAGER.GetEditString());
				
				m_TreeSoft.SetItemText(m_hTreeItem, DNCTRLMANAGER.GetEditString());
				
				Invalidate();
			}
		}
	}

	SAFE_DELETE(pEditDialog);
}

void DnEditLauncherDlg::OnMenuScale()
{
	// �޴� -> ������
	
	if( m_pRBtnClickedControl )
	{
		DNCTRLMANAGER.SetCurrentControl(m_pRBtnClickedControl);
	}

	CDnScaleDlg dlg;
	dlg.DoModal();
}

void DnEditLauncherDlg::OnMenuResourceReset()
{
	// �޴� ---> �̹��� �ʱ�ȭ Ŭ��

	if( m_pRBtnClickedControl )
	{
		CButton* pBtn = static_cast<CButton*> (m_pRBtnClickedControl);

		pBtn->SetBitmap(NULL);
		pBtn->Invalidate();

		DNCTRLMANAGER.ReleaseResourceFilePath(m_szRButtonItem);
	}
}

HBRUSH DnEditLauncherDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	CRect rect;
	UINT nID = pWnd->GetDlgCtrlID();

	if(nCtlColor == CTLCOLOR_STATIC)
	{
		// ��Ʈ �� ����
		pDC->SetTextColor(RGB(0,0,0));
		CFont *OldFont = (CFont*)pDC->SelectObject(&m_Font);

		// ��� �����ϰ�
		//pDC->SetBkMode(TRANSPARENT);
		//return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

void DnEditLauncherDlg::OnFileMainSave()
{
	// ���� -> ����
	HRESULT result = DNCTRLMANAGER.Save();

	if( result == S_OK )
	{
		CString strMsg;
		strMsg.Format(CString("XML���� ���� �Ϸ�!!"));
		AfxMessageBox(strMsg);
	}
	else if( result == E_FAIL )
	{
		CString strMsg;
		strMsg.Format(CString("XML���� ���� ����.."));
		AfxMessageBox(strMsg);
	}
}

void DnEditLauncherDlg::OnFileMainLoad()
{
	// ���� -> �ҷ�����
	if( DNCTRLMANAGER.Load() == S_OK )
	{
		DNCTRLMANAGER.CreateControlData(this);
	}
	else
	{
		CString strErr;
		strErr.Format(CString("xml�ε� ����"));
		AfxMessageBox(strErr);
	}
}

void DnEditLauncherDlg::OnFileMainQuit()
{
	// ���� -> ������
	CString strMessage;
	strMessage.Format(L"���� ���� �Ͻðڽ��ϱ�?\n[[ ����: �ڵ� ���� ���� �ʽ��ϴ�!! ���� ������ �ϼ̽��ϱ�?? ]]");

	if( AfxMessageBox(strMessage, MB_OKCANCEL) == IDOK )
	{
		this->EndDialog(0);
	}
}

void DnEditLauncherDlg::OnAbout()
{
	// ���� -> ����
	
	CDnAboutDlg dlg;
	dlg.DoModal();
}


BOOL DnEditLauncherDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
/*
	if( wParam == (IDC_BUTTON_OPTION) )
	{
		GetDlgItem(IDC_BUTTON_OPTION)->EnableWindow(true);
	}
*/
	return CDialog::OnCommand(wParam, lParam);
}

void DnEditLauncherDlg::OnUpdateCmdUI(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void DnEditLauncherDlg::OnDestroy()
{
	CDialog::OnDestroy();
	
	DNCTRLMANAGER.ResetControlData();
	m_Font.DeleteObject();

	DeleteDC(m_dcBkGrnd);
}

void DnEditLauncherDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void DnEditLauncherDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

void DnEditLauncherDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	//m_RBtnPopupMenu.LoadMenu(IDR_MENU_MODIFY);
	//m_RBtnPopupMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void DnEditLauncherDlg::OnCreateBtn()
{
	// �޴� --> ��ư �����
	CDnDynamicCtrl dlg(_BUTTON);
	
	if( dlg.DoModal() == IDOK )
	{
		stNewControlData tempData = DNCTRLMANAGER.m_DynamicControlData;
		SetRect(&tempData.m_RectPos, 0,0,200,40);
		
		CButton *pButton = new CButton;
		pButton->Create((tempData.m_szName), WS_VISIBLE | WS_CHILD | LBS_NOTIFY | BS_BITMAP , tempData.m_RectPos, this, 0);
		pButton->EnableWindow(false);
		pButton->ShowWindow(SW_SHOW);
		tempData.m_WndPT = pButton;
		tempData.m_nType = _BUTTON;

		// ���ҽ� ���Ϳ� ����.
		DNCTRLMANAGER.m_vecCtrlData.push_back(tempData);
		
		// Ʈ�� ��Ʈ�ѿ� �μ�Ʈ.
		HTREEITEM hItem = DNCTRLMANAGER.GetParentItem(tempData.m_nType);
		HTREEITEM CurrentItem = m_TreeSoft.InsertItem(tempData.m_szName, hItem, TVI_LAST);
		if(m_TreeSoft)
		{
			m_TreeSoft.SetCheck(CurrentItem, true);
		}

		m_TreeSoft.Expand(DNCTRLMANAGER.m_hBtnItem, TVE_EXPAND);
		m_TreeSoft.SortChildren(DNCTRLMANAGER.m_hBtnItem);
		m_TreeSoft.Invalidate(true);	
	}
}

void DnEditLauncherDlg::OnCreateStatictext()
{
	// �޴� --> �ؽ�Ʈ �����
	CDnDynamicCtrl dlg(_TEXT);

	if( dlg.DoModal() == IDOK )
	{
		stNewControlData tempData = DNCTRLMANAGER.m_DynamicControlData;
		SetRect(&tempData.m_RectPos, 0,0,100,11);

		
		CStatic* pStatic = new CStatic;
		pStatic->Create((tempData.m_szName), WS_VISIBLE | SS_LEFTNOWORDWRAP, tempData.m_RectPos, this, 1);
		pStatic->ShowWindow(SW_SHOW);

		tempData.m_WndPT = pStatic;
		tempData.m_nType = _TEXT;

		// ���ҽ� ���Ϳ� ����.
		DNCTRLMANAGER.m_vecCtrlData.push_back(tempData);

		// Ʈ�� ��Ʈ�ѿ� �μ�Ʈ.
		//CString szBuff;
		//szBuff.Format(L"���ҽ� ID:%s , �̸�:%s", tempData.szID, tempData.szName);
		HTREEITEM hItem = DNCTRLMANAGER.GetParentItem(tempData.m_nType);
		HTREEITEM CurrentItem = m_TreeSoft.InsertItem(tempData.m_szName, hItem, TVI_LAST);
		if(m_TreeSoft)
		{
			m_TreeSoft.SetCheck(CurrentItem, true);
		}

		m_TreeSoft.Expand(DNCTRLMANAGER.m_hTextItem, TVE_EXPAND);
		m_TreeSoft.SortChildren(DNCTRLMANAGER.m_hTextItem);
		m_TreeSoft.Invalidate(true);
	}
}

void DnEditLauncherDlg::OnCreateProgress()
{
	// �޴� --> ���α׷��ú�� �����
	CDnDynamicCtrl dlg(_PROGRESS_BAR);

	if( dlg.DoModal() == IDOK )
	{
		stNewControlData tempData = DNCTRLMANAGER.m_DynamicControlData;

		SetRect(&tempData.m_RectPos, 0,0,400,9);

		CButton *pButton = new CButton;
		pButton->Create((tempData.m_szName), WS_VISIBLE | WS_CHILD | LBS_NOTIFY | BS_BITMAP , tempData.m_RectPos, this, 0);
		pButton->EnableWindow(false);
		pButton->ShowWindow(SW_SHOW);
		tempData.m_nType = _PROGRESS_BAR;
		tempData.m_WndPT = pButton;

		// ���ҽ� ���Ϳ� ����.
		DNCTRLMANAGER.m_vecCtrlData.push_back(tempData);

		// Ʈ�� ��Ʈ�ѿ� �μ�Ʈ.
		HTREEITEM hItem = DNCTRLMANAGER.GetParentItem(tempData.m_nType);
		HTREEITEM CurrentItem = m_TreeSoft.InsertItem(tempData.m_szName, hItem , TVI_LAST);
		if(m_TreeSoft)
		{
			m_TreeSoft.SetCheck(CurrentItem, true);
		}

		m_TreeSoft.Expand(DNCTRLMANAGER.m_hProgressItem, TVE_EXPAND);
		m_TreeSoft.SortChildren(DNCTRLMANAGER.m_hProgressItem);
		m_TreeSoft.Invalidate(true);
	}
}

void DnEditLauncherDlg::OnCreateCheckbox()
{
	// �޴� --> üũ�� �����
	CDnDynamicCtrl dlg(_CHECK_BOX);

	if( dlg.DoModal() == IDOK )
	{
		stNewControlData tempData = DNCTRLMANAGER.m_DynamicControlData;

		SetRect(&tempData.m_RectPos, 0,0,13,13);

		CButton *pCheckBox = new CButton;
		pCheckBox->Create(_T(""), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_LEFTTEXT , tempData.m_RectPos, this, 4857);
		pCheckBox->EnableWindow(false);
		pCheckBox->ShowWindow(SW_SHOW);
		tempData.m_nType = _CHECK_BOX;
		tempData.m_WndPT = pCheckBox;

		// ���ҽ� ���Ϳ� ����.
		DNCTRLMANAGER.m_vecCtrlData.push_back(tempData);

		// Ʈ�� ��Ʈ�ѿ� �μ�Ʈ.
		HTREEITEM hItem = DNCTRLMANAGER.GetParentItem(tempData.m_nType);
		HTREEITEM CurrentItem = m_TreeSoft.InsertItem(tempData.m_szName, hItem, TVI_LAST);
		if(m_TreeSoft)
		{
			m_TreeSoft.SetCheck(CurrentItem, true);
		}

		m_TreeSoft.Expand(DNCTRLMANAGER.m_hCheckBoxItem, TVE_EXPAND);
		m_TreeSoft.SortChildren(DNCTRLMANAGER.m_hCheckBoxItem);
		m_TreeSoft.Invalidate(true);
	}
}

void DnEditLauncherDlg::OnMenuCtrlmodify()
{
	// �޴� --> ��Ʈ�� �Ӽ� ����

	CWnd* pCtrl = DNCTRLMANAGER.GetCtrl(m_szRButtonItem);
	if(pCtrl == NULL)
	{
		return;
	}

	_CTRL_IT it;
	if( DNCTRLMANAGER.GetVecIterator(m_szRButtonItem, it) == false )
	{
		return;
	}

	if( !(*it).m_szName.IsEmpty() )
	{
		CString szEnum;
		szEnum.Format(_T("%d"), (*it).m_nEnumValue);

		CDnDynamicCtrl dlg(szEnum, (*it).m_szName);
		if( dlg.DoModal() == IDOK )
		{
			// 1. ����Ʈ ��Ʈ���� ����� �ٲ��ش�.
			m_TreeSoft.SetItemText(m_hTreeItem, DNCTRLMANAGER.m_DynamicControlData.m_szName);

			// 2. ��Ʈ�� �Ŵ����� ���Ϳ� ���� �ٲ��ش�.
			(*it).m_szName	 = DNCTRLMANAGER.m_DynamicControlData.m_szName;
			(*it).m_nEnumValue = DNCTRLMANAGER.m_DynamicControlData.m_nEnumValue;
			
			// 3. ��Ʈ�� �ؽ�Ʈ�� ����.
			pCtrl->SetWindowText((*it).m_szName);
		}
	}
}

void DnEditLauncherDlg::OnFileNewfile()
{
	// �޴� ---> ���� �����
	CString strMessage;
	strMessage.Format(L"������ ���� ����ðڽ��ϱ�?");
	
	if( AfxMessageBox(strMessage, MB_OKCANCEL) == IDOK )
	{
		DNCTRLMANAGER.ResetControlData();
	}
}

void DnEditLauncherDlg::OnMenuDelete()
{
	// �޴� ---> ����
	CString strMessage;
	strMessage.Format(L"�����Ͻðڽ��ϱ�?");
	if( AfxMessageBox(strMessage, MB_OKCANCEL) == IDOK )
	{
		if( !DNCTRLMANAGER.DeleteCtrl(m_szRButtonItem, m_hTreeItem) )
		{
			strMessage.Format( L"������ �����߽��ϴ�." );
			AfxMessageBox( strMessage );
		}
	}
}

BOOL DnEditLauncherDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		bool bControl = ((GetKeyState(VK_CONTROL) & 0x8000) != 0);

		if( bControl )
		{
			if(pMsg->wParam == 's' ||pMsg->wParam == 'S')
			{
				// ���� ���̺�
				OnFileMainSave();
				return false;
			}
			else if(pMsg->wParam == 'l' ||pMsg->wParam == 'L')
			{
				// ���� �ҷ�����
				OnFileMainLoad();
				return false;
			}
		}
	}

	if(WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		if(m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		{
			return true;
		}
	}

	if( pMsg->wParam == VK_RETURN )
	{
		return false;
	}
	
	if( pMsg->wParam == VK_ESCAPE )
	{
		//OnFileMainQuit();
		return false;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void DnEditLauncherDlg::OnMenuSetrect()
{
	// �޴� --> SetRect
	CDnSetRectDlg dlg;
	if( dlg.DoModal() == IDOK )
	{
		CWnd* pWindow = DNCTRLMANAGER.GetCtrl(m_szRButtonItem);
		if(pWindow)
		{
			pWindow->MoveWindow(&DNCTRLMANAGER.m_RectData, true);
			pWindow->Invalidate(true);
		}
	}
}

void DnEditLauncherDlg::OnControlLock()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	int nControlEnum = DNCTRLMANAGER.GetEnum(m_szRButtonItem);
	if(nControlEnum != -1)
		DNCTRLMANAGER.SetLockObject(nControlEnum);
}

void DnEditLauncherDlg::OnControlUnlock()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	int nControlEnum = DNCTRLMANAGER.GetEnum(m_szRButtonItem);
	if(nControlEnum != -1)
		DNCTRLMANAGER.SetUnLockObject(nControlEnum);
}
