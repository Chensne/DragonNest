// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "EtActionTool.h"

#include "MainFrm.h"

#include "DockingPaneAdobeTheme.h"

#include "PaneDefine.h"
#include "InputDevice.h"
#include "MiniSliderEx.h"
#include "GlobalValue.h"
#include "ActionElement.h"
#include "ModifyEventDlg.h"
#include "SignalManager.h"
#include "ActionObject.h"
#include "PropertiesDlg.h"
#include "EtResourceMng.h"
#include "RenderBase.h"
#include "UserMessage.h"
#include "EventPaneView.h"
#include "SelectWeaponDlg.h"
#include "CopyAndPasteSignalDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CInputDevice g_InputDevice;
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CXTPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CXTPFrameWnd)
	ON_WM_CREATE()
	ON_XTP_CREATECONTROL()
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)

	ON_COMMAND(ID_PLAY_REWIND, &CMainFrame::OnPlayRewind)
	ON_COMMAND(ID_PLAY_PREV, &CMainFrame::OnPlayPrev)
	ON_COMMAND(ID_PLAY_PLAY, &CMainFrame::OnPlayPlay)
	ON_COMMAND(ID_PLAY_NEXT, &CMainFrame::OnPlayNext)
	ON_COMMAND(ID_PLAY_FOWARD, &CMainFrame::OnPlayFoward)
	ON_COMMAND(ID_PLAY_FRAME, &CMainFrame::OnPlayFrame)
	ON_COMMAND(ID_PLAY_LOOP, &CMainFrame::OnPlayLoop)
	ON_MESSAGE( UM_MINISLIDER_UPDATE_POS, OnPlaySliderUpdatePos )

	ON_UPDATE_COMMAND_UI(ID_PLAY_REWIND, &CMainFrame::OnUpdatePlayRewind)
	ON_UPDATE_COMMAND_UI(ID_PLAY_PREV, &CMainFrame::OnUpdatePlayPrev)
	ON_UPDATE_COMMAND_UI(ID_PLAY_PLAY, &CMainFrame::OnUpdatePlayPlay)
	ON_UPDATE_COMMAND_UI(ID_PLAY_NEXT, &CMainFrame::OnUpdatePlayNext)
	ON_UPDATE_COMMAND_UI(ID_PLAY_FOWARD, &CMainFrame::OnUpdatePlayFoward)
	ON_UPDATE_COMMAND_UI(ID_PLAY_FRAME, &CMainFrame::OnUpdatePlayFrame)
	ON_UPDATE_COMMAND_UI(ID_PLAY_SLIDER, &CMainFrame::OnUpdatePlaySlider)
	ON_UPDATE_COMMAND_UI(ID_PLAY_LOOP, &CMainFrame::OnUpdatePlayLoop)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(ID_SIGNAL_EDIT, &CMainFrame::OnSignalEdit)
	ON_COMMAND(ID_PROPERTIES, &CMainFrame::OnProperties)
	ON_COMMAND(ID_TOGGLE_GRID, &CMainFrame::OnToggleGrid)
	ON_COMMAND(ID_LESSPRIORITY, &CMainFrame::OnLesspriority)
	ON_UPDATE_COMMAND_UI(ID_LESSPRIORITY, &CMainFrame::OnUpdateLesspriority)
	ON_COMMAND(ID_PLAY_RELOAD, &CMainFrame::OnPlayReload)
	ON_UPDATE_COMMAND_UI(ID_PLAY_RELOAD, &CMainFrame::OnUpdatePlayReload)
	ON_COMMAND(ID_EDIT_COPY, &CMainFrame::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CMainFrame::OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CMainFrame::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CMainFrame::OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_REMOVE, &CMainFrame::OnEditRemove)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REMOVE, &CMainFrame::OnUpdateEditRemove)
	ON_COMMAND(ID_CONFIG_SELECT_WEAPON, &CMainFrame::OnConfigSelectWeapon)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_SELECT_WEAPON, &CMainFrame::OnUpdateConfigSelectWeapon)
	ON_COMMAND(ID_CAPTURE, &CMainFrame::OnCapture)
	ON_COMMAND(ID_HIT_SIGNAL_SHOW, &CMainFrame::OnHitSignalShow)
	ON_UPDATE_COMMAND_UI(ID_HIT_SIGNAL_SHOW, &CMainFrame::OnUpdateHitSignalShow)
	ON_COMMAND(ID_COPY_PASTE_SIGNAL_MENU, &CMainFrame::OnCopyPasteSignal)
	ON_UPDATE_COMMAND_UI(ID_COPY_PASTE_SIGNAL_MENU, &CMainFrame::OnUpdateCopyPasteSignal)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	m_bLoopFlag = false;
	m_pResMng = new CEtResourceMng( false );
}

CMainFrame::~CMainFrame()
{
	SAFE_DELETE( m_pResMng );
}

int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	switch( lpCreateControl->nID ) {
		case ID_PLAY_FRAME:
			{
				CXTPControlEdit* pControl = (CXTPControlEdit* )CXTPControlEdit::CreateObject();
				pControl->SetEditStyle( ES_WANTRETURN | ES_NUMBER | ES_RIGHT );
				pControl->SetWidth(50);
				CString szStr;
				szStr.Format( "%d", CGlobalValue::GetInstance().GetFPS() );
				pControl->SetEditText( szStr );
				lpCreateControl->pControl = pControl;
				return TRUE;
			}
			break;
		case ID_PLAY_SLIDER:
			{
				m_PlaySlider.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 10), this, ID_PLAY_SLIDER );
				m_PlaySlider.SetRange( 1, 120 );
				m_PlaySlider.SetPos( CGlobalValue::GetInstance().GetFPS() );
				m_PlaySlider.SetSendRootParent( 2 );

				CXTPControlCustom* pControl = CXTPControlCustom::CreateControlCustom(&m_PlaySlider);

				pControl->SetVerticalPositionOption(xtpVerticalButton);
				pControl->SetFlags(xtpFlagManualUpdate);
				lpCreateControl->pControl = pControl;
				return TRUE;
			}
			break;
	}

	return FALSE;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	if (!InitCommandBars()) return -1;

	// RichEdit 컨트롤을 사용하기 위한 세팅 
	AfxInitRichEdit();

	// Init Thema
	XTPPaintManager()->SetTheme( xtpThemeNativeWinXP );

	XTP_COMMANDBARS_ICONSINFO* pIconsInfo = XTPPaintManager()->GetIconsInfo();
	pIconsInfo->bUseDisabledIcons = TRUE;
	pIconsInfo->bOfficeStyleDisabledIcons = TRUE;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(_T("Menu Bar"), IDR_MAINFRAME); 

	CXTPToolBar *pCommandBar, *pPlayBar, *pTempBar;

	if (!(pCommandBar = (CXTPToolBar*)pCommandBars->Add(_T("Standard"), xtpBarTop)) ||
		!pCommandBar->LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	if (!(pPlayBar = (CXTPToolBar*)pCommandBars->Add(_T("PlayControl"), xtpBarTop)) ||
		!pPlayBar->LoadToolBar(IDR_PLAY_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	if (!(pTempBar = (CXTPToolBar*)pCommandBars->Add(_T("Temp"), xtpBarTop)) ||
		!pTempBar->LoadToolBar(IDR_TEMP_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	pCommandBars->Remove( pTempBar );
	
	DockRightOf( pPlayBar, pCommandBar );


	XTPDockingPaneManager()->InstallDockingPanes(this);

	m_paneManager.SetCustomTheme(new CDockingPaneAdobeTheme);
	m_paneManager.UseSplitterTracker(FALSE);
	m_paneManager.SetDockingContext(new CDockingPaneAdobeContext);
	m_paneManager.SetDefaultPaneOptions(xtpPaneNoCloseable);

	int nCount = sizeof(g_PaneList) / sizeof(PaneCreateStruct);
	int *nIconList = new int[nCount];
	for( int i=0; i<nCount; i++ ) {
		CXTPDockingPane *pDockPane = NULL;
		nIconList[i] = g_PaneList[i].nPaneID;
		if( g_PaneList[i].nDockPaneID != -1 ) pDockPane = g_PaneList[g_PaneList[i].nDockPaneID].pThis;

		if( (int)g_PaneList[i].Direction == -1 ) {
			g_PaneList[i].pThis = CreatePane( g_PaneList[i].nPaneID, g_PaneList[i].pClass, CString(g_PaneList[i].szName), g_PaneList[i].Direction, pDockPane, &CRect( g_PaneList[i].nLeft, g_PaneList[i].nTop, g_PaneList[i].nRight, g_PaneList[i].nBottom ) );
			XTPDockingPaneManager()->FloatPane( g_PaneList[i].pThis, CRect( g_PaneList[i].nLeft, g_PaneList[i].nTop, g_PaneList[i].nRight, g_PaneList[i].nBottom ) );
		}
		else g_PaneList[i].pThis = CreatePane( g_PaneList[i].nPaneID, g_PaneList[i].pClass, CString(g_PaneList[i].szName), g_PaneList[i].Direction, pDockPane, &CRect( g_PaneList[i].nLeft, g_PaneList[i].nTop, g_PaneList[i].nRight, g_PaneList[i].nBottom ) );
		if( g_PaneList[i].nAttachPaneID != -1 ) {
			XTPDockingPaneManager()->AttachPane( g_PaneList[i].pThis, g_PaneList[g_PaneList[i].nAttachPaneID].pThis );
			XTPDockingPaneManager()->ShowPane( g_PaneList[g_PaneList[i].nAttachPaneID].pThis );
		}
	}
	// 일단 아이콘 막아놓는다.
	delete []nIconList;

#ifndef _DEBUG
	LoadCommandBars(_T("EtActionToolCommandBars"));

	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	if (layoutNormal.Load(_T("EtActionToolLayout")))
		m_paneManager.SetLayout(&layoutNormal);
#endif //_DEBUG

	for( int i=0; i<nCount; i++ ) {
		if( g_PaneList[i].bShow == FALSE )
			XTPDockingPaneManager()->ClosePane( g_PaneList[i].pThis );
	}

	CInputDevice::GetInstance().Initialize( m_hWnd, CInputDevice::MOUSE|CInputDevice::KEYBOARD );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CXTPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CXTPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CXTPFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기



CXTPDockingPane* CMainFrame::CreatePane( int nID, CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour, CRect *pRect ) 
{
	CRect Rect;
	if( pRect == NULL ) Rect = CRect( 0, 0, 200, 200 );
	else Rect = *pRect;
	CXTPDockingPane* pwndPane = m_paneManager.CreatePane(
		nID, Rect, direction, pNeighbour);

	CString strTitle;
	strTitle.Format(strFormat, nID);
	pwndPane->SetTitle(strTitle);

	CFrameWnd* pFrame = new CFrameWnd;

	CCreateContext context;
	context.m_pNewViewClass = pNewViewClass;
	context.m_pCurrentDoc = NULL;

	DWORD dwStyle;
	dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	pFrame->Create(NULL, NULL, dwStyle, CRect(0, 0, 0, 0), this, NULL, 0, &context);
	pFrame->ModifyStyleEx( WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_OVERLAPPEDWINDOW, 0 );
	pFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);


	m_mapPanes.SetAt(nID, pFrame);

	return pwndPane;
}

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		// get a pointer to the docking pane being shown.
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			CWnd* pWnd = NULL;
			if (m_mapPanes.Lookup(pPane->GetID(), pWnd))
			{
				pPane->Attach(pWnd);
			}
		}	
		return TRUE; // handled
	}

	if (wParam == XTP_DPN_CLOSEPANE)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		CWnd* pWnd =NULL;

		if (!m_mapPanes.Lookup(pPane->GetID(), pWnd))
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

CWnd *CMainFrame::GetPaneFrame( int nID )
{
	if( XTPDockingPaneManager() == NULL ) return NULL;
	if( XTPDockingPaneManager()->GetSite() == NULL ) return NULL;
	CXTPDockingPane* pPane = XTPDockingPaneManager()->FindPane( nID );
	if( pPane == NULL ) return NULL;

	CWnd *pWnd = NULL;
	if (!m_mapPanes.Lookup(pPane->GetID(), pWnd) ) return NULL;

	CFrameWnd* pFrame = DYNAMIC_DOWNCAST(CFrameWnd, pWnd);
	return (CWnd*)pFrame->GetWindow( GW_CHILD );
	//	return pWnd;
}

int CMainFrame::GetFocusPaneID()
{
	int nCount = sizeof(g_PaneList) / sizeof(PaneCreateStruct);
	for( int i=0; i<nCount; i++ ) {
		CXTPDockingPane* pPane = XTPDockingPaneManager()->FindPane( g_PaneList[i].nPaneID );
		if( pPane->IsFocus() ) return g_PaneList[i].nPaneID;
	}
	return -1;
}

void CMainFrame::OnPlayRewind()
{
	CActionElement *pElement = (CActionElement*)CGlobalValue::GetInstance().GetPlayObject();
	if( !pElement ) return;

	pElement->SetCurFrame( 0.f );
}

void CMainFrame::OnPlayPrev()
{
	CActionElement *pElement = (CActionElement*)CGlobalValue::GetInstance().GetPlayObject();
	if( !pElement ) return;

	if( !pElement->IsPause() ) pElement->Pause();
	float fFrame = pElement->GetCurFrame() - 1.f;
	if( fFrame < 0.f ) fFrame = 0.f;
	pElement->SetCurFrame( fFrame );
}

void CMainFrame::OnPlayPlay()
{
	CActionElement *pElement = (CActionElement*)CGlobalValue::GetInstance().GetPlayObject();
	if( !pElement ) return;

	if( pElement->IsPause() ) {
		pElement->Resume();
		if( pElement->IsStop() ) pElement->Play( m_bLoopFlag ? INT_MAX : 1, pElement->GetCurFrame() );
	}
	else if( pElement->IsStop() ) pElement->Play( m_bLoopFlag ? INT_MAX : 1 );
	else pElement->Pause();
}

void CMainFrame::OnPlayNext()
{
 	CActionElement *pElement = (CActionElement*)CGlobalValue::GetInstance().GetPlayObject();
	if( !pElement ) return;

	if( !pElement->IsPause() ) pElement->Pause();
	float fFrame = pElement->GetCurFrame() + 1.f;
	if( fFrame >= (float)pElement->GetLength() ) fFrame = (float)pElement->GetLength() ;
	pElement->SetCurFrame( fFrame );
}

void CMainFrame::OnPlayFoward()
{
	CActionElement *pElement = (CActionElement*)CGlobalValue::GetInstance().GetPlayObject();
	if( !pElement ) return;

	pElement->SetCurFrame( (float)pElement->GetLength() );
}

void CMainFrame::OnPlayFrame()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPCommandBar *pPlayBar = pCommandBars->GetAt( 2 );
	CXTPControl *pControl = pPlayBar->GetControls()->FindControl( xtpControlEdit, ID_PLAY_FRAME, TRUE, FALSE );
	if( pControl ) {
		CString szStr = ((CXTPControlEdit*)pControl)->GetEditText();
		int nValue = atoi(szStr);
		if( nValue < m_PlaySlider.GetRangeMin() ) nValue = m_PlaySlider.GetRangeMin();
		if( nValue > m_PlaySlider.GetRangeMax() ) nValue = m_PlaySlider.GetRangeMax();
		szStr.Format( "%d", nValue );
		((CXTPControlEdit*)pControl)->SetEditText( szStr );

		CGlobalValue::GetInstance().SetFPS( nValue );
		m_PlaySlider.SetPos( nValue );
	}
}

void CMainFrame::OnPlayLoop()
{
	CActionElement *pElement = (CActionElement*)CGlobalValue::GetInstance().GetPlayObject();
	if( !pElement ) return;

	m_bLoopFlag = !m_bLoopFlag;
	pElement->SetLoop( m_bLoopFlag );
}



void CMainFrame::OnUpdatePlayRewind(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CGlobalValue::GetInstance().IsCanPlayControl() );
}

void CMainFrame::OnUpdatePlayPrev(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CGlobalValue::GetInstance().IsCanPlayControl() );
}

void CMainFrame::OnUpdatePlayPlay(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CGlobalValue::GetInstance().IsCanPlayControl() );

	CActionElement *pElement = (CActionElement*)CGlobalValue::GetInstance().GetPlayObject();
	if( !pElement ) return;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPCommandBar *pPlayBar = pCommandBars->GetAt( 2 );
	CXTPControl *pControl = pPlayBar->GetControls()->FindControl( xtpControlError, ID_PLAY_PLAY, TRUE, FALSE );
	if( pControl ) {
		int nIconID = 0;
		if( pElement->IsPause() || pElement->IsStop() ) nIconID = ID_PLAY_PLAY;
		else nIconID = ID_PLAY_RESUME;
		if( pControl->GetIconId() != nIconID )
			pControl->SetIconId(nIconID);
	}
}

void CMainFrame::OnUpdatePlayNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CGlobalValue::GetInstance().IsCanPlayControl() );
}

void CMainFrame::OnUpdatePlayFoward(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CGlobalValue::GetInstance().IsCanPlayControl() );
}

void CMainFrame::OnUpdatePlayFrame(CCmdUI *pCmdUI)
{
}

void CMainFrame::OnUpdatePlaySlider(CCmdUI *pCmdUI)
{
}

void CMainFrame::OnUpdatePlayLoop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( CGlobalValue::GetInstance().IsCanPlayControl() );

	CActionElement *pElement = (CActionElement*)CGlobalValue::GetInstance().GetPlayObject();
	if( !pElement ) return;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPCommandBar *pPlayBar = pCommandBars->GetAt( 2 );
	CXTPControl *pControl = pPlayBar->GetControls()->FindControl( xtpControlError, ID_PLAY_LOOP, TRUE, FALSE );
	if( pControl ) {
		int nIconID = 0;
		if( m_bLoopFlag ) nIconID = ID_PLAY_LOOP;
		else nIconID = ID_PLAY_NOLOOP;
		if( pControl->GetIconId() != nIconID )
			pControl->SetIconId(nIconID);
	}
}


LRESULT CMainFrame::OnPlaySliderUpdatePos( WPARAM wParam, LPARAM lParam )
{
	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPCommandBar *pPlayBar = pCommandBars->GetAt( 2 );
	CXTPControl *pControl = pPlayBar->GetControls()->FindControl( xtpControlEdit, ID_PLAY_FRAME, TRUE, FALSE );
	if( pControl ) {
		CString szStr;
		szStr.Format( "%d", (int)lParam );
		((CXTPControlEdit*)pControl)->SetEditText( szStr );
		CGlobalValue::GetInstance().SetFPS( (int)lParam );
	}

	return S_OK;
}

void CMainFrame::OnDestroy()
{
	CXTPFrameWnd::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CMainFrame::OnClose()
{
#ifndef _DEBUG
	SaveCommandBars(_T("EtActionToolCommandBars"));
	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	m_paneManager.GetLayout(&layoutNormal);
	layoutNormal.Save(_T("EtActionToolLayout"));
#endif //_DEBUG

	CXTPFrameWnd::OnClose();
}

void CMainFrame::OnSignalEdit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( CSignalManager::GetInstance().IsCanCheckOut() == false ) {
		MessageBox( "다른사람이 설정을 변경중입니다.", "에러", MB_OK );
		return;
	}

	if( CSignalManager::GetInstance().CheckOut() == false ) {
		MessageBox( "체크아웃 실패!!", "에러", MB_OK );
		return;
	}
	CSignalManager::GetInstance().Refresh();

	CModifyEventDlg Dlg;
	Dlg.SetCurrentView( CWizardCommon::SELECT_EDIT_TYPE );

	CString szSkinName;
	if( CGlobalValue::GetInstance().IsOpenAction() ) {
		CActionObject *pObject = (CActionObject*)CGlobalValue::GetInstance().GetRootObject();
		szSkinName = pObject->GetSkinName();
	
		CGlobalValue::GetInstance().SaveAction();
	}

	if( Dlg.DoModal() == IDOK ) {

		CSignalManager::GetInstance().Save();

		if( CGlobalValue::GetInstance().IsOpenAction() ) {
			CGlobalValue::GetInstance().OpenAction( szSkinName );
		}
	}

	if( CSignalManager::GetInstance().CheckIn() == false ) {
		MessageBox( "체크인 실패!!", "에러", MB_OK );
		return;
	}
}

void CMainFrame::OnProperties()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CPropertiesDlg Dlg;
	Dlg.DoModal();
}

void CMainFrame::OnToggleGrid()
{
	// TODO: Add your command handler code here
	CRenderBase::GetInstance().ShowGrid( !CRenderBase::GetInstance().IsShowGrid() );
	CRenderBase::GetInstance().ShowAxis( !CRenderBase::GetInstance().IsShowAxis() );

	CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
	if( pWnd ) pWnd->SendMessage( UM_REFRESH_PANE );
}


void CMainFrame::OnLesspriority()
{
	// TODO: Add your command handler code here
	CRenderBase::GetInstance().ChangePriority( !CRenderBase::GetInstance().IsLessPriority() );
}

void CMainFrame::OnUpdateLesspriority(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck( CRenderBase::GetInstance().IsLessPriority() );
}

void CMainFrame::OnPlayReload()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CActionElement *pElement = (CActionElement*)CGlobalValue::GetInstance().GetPlayObject();
	if( !pElement ) return;

	if( !pElement->IsPause() ) {
		pElement->Pause();
	}
	pElement->ReloadSCR();
}

void CMainFrame::OnUpdatePlayReload(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->Enable( CGlobalValue::GetInstance().IsCanPlayControl() );
}

void CMainFrame::OnEditCopy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWnd *pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) {
		CEventPaneView *pPaneView = dynamic_cast<CEventPaneView*>(pWnd);
		if( pPaneView ) pPaneView->OnEventpanecontextCopysignal();
	}
}

void CMainFrame::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	CWnd *pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) {
		CSequenceView *pSequenceView = dynamic_cast<CSequenceView*>(pWnd);
		if( pSequenceView ) {
			pCmdUI->Enable( pSequenceView->IsSelectedSignal() );
		}
	}
}

void CMainFrame::OnEditPaste()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWnd *pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) {
		CEventPaneView *pPaneView = dynamic_cast<CEventPaneView*>(pWnd);
		if( pPaneView ) {
			POINT pt;
			::GetCursorPos(&pt);
			pPaneView->ScreenToClient(&pt);
			pPaneView->SetContextMousePos(pt);
			pPaneView->OnEventpanecontextPaste();
		}
	}
}

void CMainFrame::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	BOOL bCopyBuffer = FALSE;
	CWnd *pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) {
		CEventPaneView *pPaneView = dynamic_cast<CEventPaneView*>(pWnd);
		if( pPaneView ) {
			POINT pt;
			int nPos, nYOrder;
			
			pPaneView->EnableUpdateAdditionalGridHeightCount( false );

			pt = pPaneView->GetSelectEmptyMousePos();
			BOOL bSelectEmpty = pPaneView->IsSelectEmpty();
			BOOL bCalcItemPos = pPaneView->CalcSignalPos( pt.x, pt.y, nPos, nYOrder );
			pCmdUI->Enable( bSelectEmpty && bCalcItemPos );

			pPaneView->EnableUpdateAdditionalGridHeightCount( true );
		}
	}
}

void CMainFrame::OnEditRemove()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWnd *pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) {
		CEventPaneView *pPaneView = dynamic_cast<CEventPaneView*>(pWnd);
		if( pPaneView ) pPaneView->OnEventpanecontextRemovesignal();
	}
}

void CMainFrame::OnUpdateEditRemove(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	CWnd *pWnd = GetPaneWnd( EVENT_PANE );
	if( pWnd ) {
		CSequenceView *pSequenceView = dynamic_cast<CSequenceView*>(pWnd);
		if( pSequenceView ) {
			pCmdUI->Enable( pSequenceView->IsSelectedSignal() );
		}
	}
}
void CMainFrame::OnConfigSelectWeapon()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CSelectWeaponDlg Dlg;
	Dlg.DoModal();
}

void CMainFrame::OnUpdateConfigSelectWeapon(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->Enable( CGlobalValue::GetInstance().IsCanPlayControl() );
}

void CMainFrame::OnCapture()
{
	// TODO: Add your command handler code here
	EternityEngine::CaptureScreen( "ActionTool" );
}

void CMainFrame::OnHitSignalShow()
{
	CRenderBase::GetInstance().ChangeHitSignalShow( !CRenderBase::GetInstance().IsHitSignalShow() );
}

void CMainFrame::OnUpdateHitSignalShow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( CRenderBase::GetInstance().IsHitSignalShow() );
}

void CMainFrame::OnCopyPasteSignal()
{
	CCopyAndPasteSignalDlg Dlg;
	Dlg.DoModal();
}

void CMainFrame::OnUpdateCopyPasteSignal(CCmdUI *pCmdUI)
{

}