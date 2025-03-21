// DnEditLauncherDlg.cpp : 구현 파일입니다.
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

// DnEditLauncherDlg 대화 상자입니다.

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
	//DDX_Control(pDX, IDC_STATIC_WEBPAGE, m_pcWebpage); // 삭제했음...

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



// DnEditLauncherDlg 메시지 처리기입니다.
BOOL DnEditLauncherDlg::OnInitDialog()
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CDialog::OnInitDialog();

	// 단축키 설정.
	m_hAccel = LoadAccelerators( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// 폰트 설정.
	m_Font.CreateFont( 11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("돋움") );
	
	// 다이얼로그 배경 이미지 변경.
	if( !InitBackGround() )
	{
		this->EndDialog(0);
		return false;
	}

	// 트리 컨트롤 초기화
	if( !InitTreeControl() )
	{
		this->EndDialog(0);
		return false;
	}

	return TRUE;
}

int DnEditLauncherDlg::InitTreeControl()
{
	// 상위 버튼 생성.
	DNCTRLMANAGER.m_hRootItem		= m_TreeSoft.InsertItem(_T("컨트롤 목록"), TVI_ROOT, TVI_LAST);
	DNCTRLMANAGER.m_hBtnItem		= m_TreeSoft.InsertItem(_T("BUTTONS"),		DNCTRLMANAGER.m_hRootItem, TVI_LAST);
	DNCTRLMANAGER.m_hTextItem		= m_TreeSoft.InsertItem(_T("STATIC_TEXT"),	DNCTRLMANAGER.m_hRootItem, TVI_LAST);
	DNCTRLMANAGER.m_hProgressItem	= m_TreeSoft.InsertItem(_T("PROGRESSBAR"),	DNCTRLMANAGER.m_hRootItem, TVI_LAST);
	DNCTRLMANAGER.m_hCheckBoxItem	= m_TreeSoft.InsertItem(_T("CHECK_BOX"),	DNCTRLMANAGER.m_hRootItem, TVI_LAST);
	
	// 펼친다.
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hRootItem,	TVE_EXPAND);
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hBtnItem,		TVE_EXPAND);
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hTextItem,	TVE_EXPAND);
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hProgressItem,TVE_EXPAND);
	m_TreeSoft.Expand(DNCTRLMANAGER.m_hCheckBoxItem,TVE_EXPAND);

	// 위치 설정.
	RECT treeRT;
	m_TreeSoft.GetWindowRect(&treeRT);
	m_TreeSoft.SetWindowPos(NULL, m_dwWidth+10, (m_dwHeight-m_dwHeight)+10, treeRT.right-treeRT.left , treeRT.bottom-treeRT.top, SWP_NOZORDER );
	
	
	// 트리 컨트롤의 주소 저장.
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
	// 마우스 왼쪽 클릭
	SetCapture();

	POINT pt;
	GetCursorPos(&pt);
	
	// 현재 선택된 컨트롤
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
	// 마우스 이동
	if( m_bLBtnClickedFlag )
	{
		POINT	MousePt;
		RECT	rect;
		
		GetClientRect(&rect);
		
		GetCursorPos(&MousePt);
		ScreenToClient(&MousePt);
		
		if( MousePt.x < 0 || MousePt.y < 0  || MousePt.x > rect.right || MousePt.y > rect.bottom )
		{
			// 윈도우 영역 밖;
			return;
		}
		
		RECT rtLocal, rtWorld;
		m_pLBtnClickedControl->GetClientRect(&rtLocal); // 로컬 좌표
		m_pLBtnClickedControl->GetWindowRect(&rtWorld); // 월드 좌표
		
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
	// 마우스 왼쪽 클릭 업
	if( GetCapture() == this && m_bLBtnClickedFlag && m_pLBtnClickedControl )
	{
		ReleaseCapture();
		m_bLBtnClickedFlag = false;
		
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);

		RECT rtLocal, rtWorld;
		m_pLBtnClickedControl->GetClientRect(&rtLocal); // 로컬 좌표
		m_pLBtnClickedControl->GetWindowRect(&rtWorld); // 월드 좌표

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
	// [마우스 우 클릭 다운]


	CDialog::OnRButtonDown(nFlags, point);
}

void DnEditLauncherDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// [마우스 우 클릭 업]

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
	// 트리 리스트에서 마우스 왼클릭.

	DWORD pos = ::GetMessagePos();
	CPoint point( LOWORD( pos ), HIWORD( pos ) );
	m_TreeSoft.ScreenToClient( &point );

	UINT nFlags = 0;
	HTREEITEM hItem = m_TreeSoft.HitTest( point, &nFlags );
	CString		str	= m_TreeSoft.GetItemText(hItem);
	m_szRButtonItem	= str;

	if(hItem)
	{
		// 체크박스를 클릭한 경우!
		if( nFlags & TVHT_ONITEMSTATEICON )
		{
			if( m_TreeSoft.GetCheck(hItem) )
			{
				UnCheckChildItems(hItem); // 기능이 필요없어서 막았음.
			}
			else
			{
				CheckChildItems(hItem);  // 기능이 필요없어서 막았음.
			}
		}
	}

	*pResult = 0;
}

void DnEditLauncherDlg::OnNMRclickTreeControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 트리 리스트에서 마우스 우클릭. [팝업 생성]

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

		CMenu* Dmenu = menu.GetSubMenu(0);			// 맨앞을 가져옴
		SetPossibleMenu(Dmenu, m_szRButtonItem);	// 사용하지 않는 메뉴 비활성화.
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
	 // 메뉴 -> 저장하기
	if( DNCTRLMANAGER.Save() == E_FAIL )
	{
		CString strErr;
		strErr.Format(CString("xml저장 오류"));
		AfxMessageBox(strErr);

		return;
	}
}

void DnEditLauncherDlg::OnBnClickedButtonLoad()
{
	// 메뉴 -> 불러오기
	if( DNCTRLMANAGER.Load() == E_FAIL )
	{
		CString strErr;
		strErr.Format(CString("xml로드 오류"));
		AfxMessageBox(strErr);

		return;
	}
}

void DnEditLauncherDlg::OnLoadImage()
{
	// 메뉴 -> 이미지 불러오기
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
			szMSG.Format(_T("파일경로 설정에 실패했습니다."));
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
	// 메뉴 -> 컨트롤 위치 초기화
	// 이상한데로 들어갔을때 빼기 위해서..
	CWnd* pCurrentSelectedItem = DNCTRLMANAGER.GetCtrl(m_szRButtonItem);
	
	if( pCurrentSelectedItem )
	{		
		pCurrentSelectedItem->SetWindowPos(NULL, 0,0,0,0, SWP_NOSIZE);
		Invalidate();
	}
}


void DnEditLauncherDlg::OnMenuTextmodify()
{
	// 메뉴 -> 텍스트 수정 클릭.
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
	// 메뉴 -> 스케일
	
	if( m_pRBtnClickedControl )
	{
		DNCTRLMANAGER.SetCurrentControl(m_pRBtnClickedControl);
	}

	CDnScaleDlg dlg;
	dlg.DoModal();
}

void DnEditLauncherDlg::OnMenuResourceReset()
{
	// 메뉴 ---> 이미지 초기화 클릭

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
		// 폰트 색 변경
		pDC->SetTextColor(RGB(0,0,0));
		CFont *OldFont = (CFont*)pDC->SelectObject(&m_Font);

		// 배경 투명하게
		//pDC->SetBkMode(TRANSPARENT);
		//return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

void DnEditLauncherDlg::OnFileMainSave()
{
	// 파일 -> 저장
	HRESULT result = DNCTRLMANAGER.Save();

	if( result == S_OK )
	{
		CString strMsg;
		strMsg.Format(CString("XML파일 저장 완료!!"));
		AfxMessageBox(strMsg);
	}
	else if( result == E_FAIL )
	{
		CString strMsg;
		strMsg.Format(CString("XML파일 저장 실패.."));
		AfxMessageBox(strMsg);
	}
}

void DnEditLauncherDlg::OnFileMainLoad()
{
	// 파일 -> 불러오기
	if( DNCTRLMANAGER.Load() == S_OK )
	{
		DNCTRLMANAGER.CreateControlData(this);
	}
	else
	{
		CString strErr;
		strErr.Format(CString("xml로드 오류"));
		AfxMessageBox(strErr);
	}
}

void DnEditLauncherDlg::OnFileMainQuit()
{
	// 파일 -> 나가기
	CString strMessage;
	strMessage.Format(L"툴을 종료 하시겠습니까?\n[[ 주의: 자동 저장 되지 않습니다!! 파일 저장은 하셨습니까?? ]]");

	if( AfxMessageBox(strMessage, MB_OKCANCEL) == IDOK )
	{
		this->EndDialog(0);
	}
}

void DnEditLauncherDlg::OnAbout()
{
	// 파일 -> 정보
	
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
	// 메뉴 --> 버튼 만들기
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

		// 리소스 벡터에 저장.
		DNCTRLMANAGER.m_vecCtrlData.push_back(tempData);
		
		// 트리 컨트롤에 인서트.
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
	// 메뉴 --> 텍스트 만들기
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

		// 리소스 벡터에 저장.
		DNCTRLMANAGER.m_vecCtrlData.push_back(tempData);

		// 트리 컨트롤에 인서트.
		//CString szBuff;
		//szBuff.Format(L"리소스 ID:%s , 이름:%s", tempData.szID, tempData.szName);
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
	// 메뉴 --> 프로그래시브바 만들기
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

		// 리소스 벡터에 저장.
		DNCTRLMANAGER.m_vecCtrlData.push_back(tempData);

		// 트리 컨트롤에 인서트.
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
	// 메뉴 --> 체크빅스 만들기
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

		// 리소스 벡터에 저장.
		DNCTRLMANAGER.m_vecCtrlData.push_back(tempData);

		// 트리 컨트롤에 인서트.
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
	// 메뉴 --> 컨트롤 속성 보기

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
			// 1. 리스트 컨트롤의 목록을 바꿔준다.
			m_TreeSoft.SetItemText(m_hTreeItem, DNCTRLMANAGER.m_DynamicControlData.m_szName);

			// 2. 컨트롤 매니저의 벡터에 값을 바꿔준다.
			(*it).m_szName	 = DNCTRLMANAGER.m_DynamicControlData.m_szName;
			(*it).m_nEnumValue = DNCTRLMANAGER.m_DynamicControlData.m_nEnumValue;
			
			// 3. 컨트롤 텍스트도 변경.
			pCtrl->SetWindowText((*it).m_szName);
		}
	}
}

void DnEditLauncherDlg::OnFileNewfile()
{
	// 메뉴 ---> 새로 만들기
	CString strMessage;
	strMessage.Format(L"파일을 새로 만드시겠습니까?");
	
	if( AfxMessageBox(strMessage, MB_OKCANCEL) == IDOK )
	{
		DNCTRLMANAGER.ResetControlData();
	}
}

void DnEditLauncherDlg::OnMenuDelete()
{
	// 메뉴 ---> 삭제
	CString strMessage;
	strMessage.Format(L"삭제하시겠습니까?");
	if( AfxMessageBox(strMessage, MB_OKCANCEL) == IDOK )
	{
		if( !DNCTRLMANAGER.DeleteCtrl(m_szRButtonItem, m_hTreeItem) )
		{
			strMessage.Format( L"삭제에 실패했습니다." );
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
				// 파일 세이브
				OnFileMainSave();
				return false;
			}
			else if(pMsg->wParam == 'l' ||pMsg->wParam == 'L')
			{
				// 파일 불러오기
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
	// 메뉴 --> SetRect
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	int nControlEnum = DNCTRLMANAGER.GetEnum(m_szRButtonItem);
	if(nControlEnum != -1)
		DNCTRLMANAGER.SetLockObject(nControlEnum);
}

void DnEditLauncherDlg::OnControlUnlock()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	int nControlEnum = DNCTRLMANAGER.GetEnum(m_szRButtonItem);
	if(nControlEnum != -1)
		DNCTRLMANAGER.SetUnLockObject(nControlEnum);
}
