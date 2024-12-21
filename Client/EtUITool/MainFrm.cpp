// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "EtUI.h"
#include "EtUITool.h"
#include "DockingPaneAdobeTheme.h"
#include "MainFrm.h"
#include "EtUIToolDoc.h"
#include "EtUIToolView.h"
#include "PaneDefine.h"
#include "LayoutView.h"
#include "DummyView.h"
#include "GlobalValue.h"
#include "PropertiesDlg.h"
#include "../../Common/EtResManager/EtResourceMng.h"
#include "../../Common/EtStringManager/EtUIXML.h"
#include "EtUIMan.h"
#include "EtUIControl.h"
#include "FontSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEALLTEMPLATE, &CMainFrame::OnUpdateFileSavealltemplate)
	ON_COMMAND(ID_FILE_SAVEALLTEMPLATE, &CMainFrame::OnFileSavealltemplate)
	ON_COMMAND(ID_UI_SAVE, &CMainFrame::OnFileSave)
	ON_COMMAND(ID_UI_SAVE_AS, &CMainFrame::OnFileSaveAs)
	ON_COMMAND(ID_UI_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_UI_NEW, &CMainFrame::OnFileNew)
	ON_COMMAND(ID_FILE_SETPROPERTYFOLDER, &CMainFrame::OnFileSetpropertyfolder)
	ON_COMMAND(ID_FORMAT_SETFONT, &CMainFrame::OnFormatSetfont)
	ON_COMMAND(ID_FILE_LOAD_XML, &CMainFrame::OnFileLoadXml)
	ON_COMMAND(ID_FILE_CREATE_INI_FILE, &CMainFrame::OnFileCreateIniFile)
	ON_UPDATE_COMMAND_UI(ID_FILE_CREATE_INI_FILE, &CMainFrame::OnUpdateFileCreateIniFile)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_VIEW_TEMPLATE_ON_LAYOUTVIEW, &CMainFrame::OnViewTemplateOnLayoutview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEMPLATE_ON_LAYOUTVIEW, &CMainFrame::OnUpdateViewTemplateOnLayoutview)
	ON_COMMAND(ID_FILE_SAVE_DIALOG_INFO, &CMainFrame::OnFileSaveDialogInfo)
	ON_COMMAND(ID_SAVE_CUSTOMUI, &CMainFrame::OnSaveCustomui)
	ON_COMMAND(ID_SAVE_ALLUI, &CMainFrame::OnSaveAllui)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

CString CMainFrame::m_szShaderFolder = _T("");
CString CMainFrame::m_szResourceFolder = _T("");
CString CMainFrame::m_szUITemplateFolder = _T("");

CMainFrame::CMainFrame()
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtMemDumpAllObjectsSince(0); 
	//CrtSetBreakAlloc(206865);
#endif //_DEBUG

	m_nCreateINIFile = 0;
	m_nTemplateOnLayoutView = 0;
	m_pResMng = new CEtResourceMng( false );

	char szStr1[2048] = {0};
	char szStr2[2048] = {0};
	char szStr3[2048] = {0};

	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ShaderFolder", szStr1, 2048 );
	if( strlen(szStr1) == 0 ) m_szShaderFolder = "\\\\eye-ad\\ToolData\\SharedEffect";
	else m_szShaderFolder = szStr1;

	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ResourceFolder", szStr2, 2048 );
	if( strlen(szStr2) == 0 ) m_szResourceFolder = "\\\\eye-ad\\ToolData\\Resource";
	else m_szResourceFolder = szStr2;

	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "UITemplateFolder", szStr3, 2048 );
	if( strlen(szStr3) == 0 ) m_szUITemplateFolder = "\\\\eye-ad\\ToolData\\UITemplate";
	else m_szUITemplateFolder = szStr3;

	GetCurrentDirectory( 2048, szStr1 );
	strcat_s( szStr1, "\\" );
	CEtResourceMng::GetInstance().RemoveResourcePath( szStr1 );

	if( strlen( szStr1 ) == 0 || strlen( szStr2 ) == 0 || strlen( szStr3 ) == 0 )
	{
		CPropertiesDlg Dlg;
		Dlg.m_bDisableCancel = true;
		Dlg.m_szShaderFolder = GetShaderFolder();
		Dlg.m_szResourceFolder = GetResourceFolder();
		Dlg.m_szUITemplateFolder = GetUITemplateFolder();
		if( Dlg.DoModal() == IDOK )
		{
			if( GetShaderFolder() != Dlg.m_szShaderFolder )
				ChangeShaderFolder( Dlg.m_szShaderFolder );
			if( GetResourceFolder() != Dlg.m_szResourceFolder )
				ChangeResourceFolder( Dlg.m_szResourceFolder );
			if( GetUITemplateFolder() != Dlg.m_szUITemplateFolder )
				ChangeUITemplateFolder( Dlg.m_szUITemplateFolder );
		}
	}
	else
	{
		CEtResourceMng::GetInstance().AddResourcePath( m_szShaderFolder, true );
		CEtResourceMng::GetInstance().AddResourcePath( m_szResourceFolder, true );
		CEtResourceMng::GetInstance().AddResourcePath( m_szUITemplateFolder, true );
	}
}

CMainFrame::~CMainFrame()
{
	SAFE_DELETE( m_pResMng );
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo( 0, ID_SEPARATOR, SBPS_STRETCH, 0 );
	m_wndStatusBar.SetPaneInfo( 1, ID_SEPARATOR, SBPS_NORMAL, 250 );

	if ( !InitCommandBars() )
	{
		return -1;
	}

	// Init Thema
	XTPPaintManager()->SetTheme(xtpThemeVisualStudio2008);//xtpThemeNativeWinXP );

	XTP_COMMANDBARS_ICONSINFO* pIconsInfo = XTPPaintManager()->GetIconsInfo();
	pIconsInfo->bUseDisabledIcons = TRUE;
	pIconsInfo->bOfficeStyleDisabledIcons = TRUE;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(_T("Menu Bar"), IDR_MAINFRAME); 

	CXTPToolBar *pCommandBar;

	//	XTPImageManager()->SetMaskColor(RGB(0, 255, 0));
	if (!(pCommandBar = (CXTPToolBar*)pCommandBars->Add(_T("Standard"), xtpBarTop)) ||
		!pCommandBar->LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	XTPDockingPaneManager()->InstallDockingPanes(this);
	//	XTPDockingPaneManager()->SetTheme( xtpPaneThemeNativeWinXP ); //NativeWinXP

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
	LoadCommandBars(_T("EtWorldPainterCommandBars"));

	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	if (layoutNormal.Load(_T("EtWorldPainterLayout")))
		m_paneManager.SetLayout(&layoutNormal);
#endif //_DEBUG

	for( int i=0; i<nCount; i++ ) {
		if( g_PaneList[i].bShow == FALSE ) {
			XTPDockingPaneManager()->ClosePane( g_PaneList[i].nPaneID );
		}
		else {
			XTPDockingPaneManager()->ShowPane( g_PaneList[i].nPaneID );
		}
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx = 1276 + 341;
	cs.cy = 949;
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
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
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers



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

void CMainFrame::SetPaneText( int nID, CString &szText )
{
    m_wndStatusBar.SetPaneText( nID, szText );
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

void CMainFrame::OnUpdateFileSavealltemplate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnFileSavealltemplate()
{
	CTemplatePaneView *pView;

	pView = ( CTemplatePaneView * )GetPaneWnd( TEMPLATE_PANE );
	if( pView )
	{
		pView->SaveAllTemplate();
	}
}

BOOL CMainFrame::IsPaneClosed( int nID )
{
	return XTPDockingPaneManager()->IsPaneClosed( nID );
}

void CMainFrame::OnFileSave()
{
	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
	pLayoutView->OnFileSave();
}

void CMainFrame::OnFileSaveAs()
{
	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
	pLayoutView->OnFileSaveAs();
}

void CMainFrame::OnFileOpen()
{
	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
	pLayoutView->OnFileOpen();
}

void CMainFrame::OnFileNew()
{
	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
	pLayoutView->OnFileNew();
}

void CMainFrame::OnFileSetpropertyfolder()
{
	CPropertiesDlg Dlg;
	Dlg.m_szShaderFolder = GetShaderFolder();
	Dlg.m_szResourceFolder = GetResourceFolder();
	Dlg.m_szUITemplateFolder = GetUITemplateFolder();
	if( Dlg.DoModal() == IDOK )
	{
		if( GetShaderFolder() != Dlg.m_szShaderFolder )
			ChangeShaderFolder( Dlg.m_szShaderFolder );
		if( GetResourceFolder() != Dlg.m_szResourceFolder )
			ChangeResourceFolder( Dlg.m_szResourceFolder );
		if( GetUITemplateFolder() != Dlg.m_szUITemplateFolder )
			ChangeUITemplateFolder( Dlg.m_szUITemplateFolder );
	}
}

void CMainFrame::OnFormatSetfont()
{
	// Note : 폰트 셋을 수정하기 위해서 호출
	//
	CFontSetDlg FontSetDlg;

	if( FontSetDlg.DoModal() == IDOK )
	{
	}
}

void CMainFrame::OnFileLoadXml()
{
	if( EtInterface::xml::IsValid() )
	{
		if( EtInterface::xml::GetXML()->LoadXML( "uistring.xml", CEtUIXML::idCategory1 ) )
		{
			MessageBox( "UI String XML Loading Succeeded!" );
		}
	}
}

void CMainFrame::OnFileCreateIniFile()
{
	m_nCreateINIFile = !m_nCreateINIFile;
}

void CMainFrame::OnUpdateFileCreateIniFile(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nCreateINIFile);
}

void CMainFrame::ChangeShaderFolder( const char *szFullPath )
{
	CEtResourceMng::GetInstance().RemoveResourcePath( m_szShaderFolder );
	m_szShaderFolder = szFullPath;
	CEtResourceMng::GetInstance().AddResourcePath( m_szShaderFolder, true );
}

void CMainFrame::ChangeResourceFolder( const char *szFullPath )
{
	CEtResourceMng::GetInstance().RemoveResourcePath( m_szResourceFolder );
	m_szResourceFolder = szFullPath;
	CEtResourceMng::GetInstance().AddResourcePath( m_szResourceFolder, true );
}

void CMainFrame::ChangeUITemplateFolder( const char *szFullPath )
{
	CEtResourceMng::GetInstance().RemoveResourcePath( m_szUITemplateFolder );
	m_szResourceFolder = szFullPath;
	CEtResourceMng::GetInstance().AddResourcePath( m_szUITemplateFolder, true );
}

void CMainFrame::OnViewTemplateOnLayoutview()
{
	m_nTemplateOnLayoutView = !m_nTemplateOnLayoutView;
	((CEtUIToolView*)(CGlobalValue::GetInstance().m_pCurView))->GetDummyView()->EnableTemplateOnLayoutView( m_nTemplateOnLayoutView ? true : false );
}

void CMainFrame::OnUpdateViewTemplateOnLayoutview(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nTemplateOnLayoutView);
}

void CMainFrame::OnFileSaveDialogInfo()
{
	TCHAR szFilter[] = _T( "Eternity UI Dialog Info File (*.txt)|*.txt|All Files (*.*)|*.*||" );
	CFileDialog FileDlg( FALSE, _T("txt"), _T("*.txt"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_NOCHANGEDIR, szFilter, this );

	if( FileDlg.DoModal() == IDOK )
	{
		FILE *fp(NULL);
		errno_t err;
		if( (err = fopen_s( &fp, FileDlg.m_ofn.lpstrFile, "wt" )) != 0 )
		{
			CDebugSet::ToLogFile( "CMainFrame::OnFileSaveDialogInfo, The file %s was not opened!", FileDlg.m_ofn.lpstrFile );
			return;
		}
		
		if( fp )
		{
			fprintf ( fp, "--------------------------------------------------------------\n" );
			fprintf ( fp, "\tID\t\t\tType\n" );
			fprintf ( fp, "--------------------------------------------------------------\n" );
			fprintf ( fp, "\n" );

			CLayoutView *pLayoutView;
			pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
			CEtUIDialog &layoutDialog = pLayoutView->GetDialog();
			std::vector<CEtUIControl*>&vecControl = layoutDialog.GetControlList();

			SUIControlProperty *pProperty(NULL);
			for( int i=0; i<(int)vecControl.size(); ++i )
			{
				pProperty = vecControl[i]->GetProperty();
				if( !pProperty )
				{
					CDebugSet::ToLogFile( "CMainFrame::OnFileSaveDialogInfo, pProperty is NULL!" );
					continue;
				}

				fprintf ( fp, "\t%s\t\t", pProperty->szUIName );
				fprintf ( fp, "%s\n", CONTROL_TYPE_2_STRING(pProperty->UIType) );
			}

			fclose(fp);
		}
		else
		{
			CDebugSet::ToLogFile( "CMainFrame::OnFileSaveDialogInfo, File pointer is NULL!" );
			return;
		}
	}
}

const char* CMainFrame::CONTROL_TYPE_2_STRING( UI_CONTROL_TYPE typeControl )
{
	switch( typeControl )
	{
	case UI_CONTROL_STATIC:				return "Static";
	case UI_CONTROL_BUTTON:				return "Button";
	case UI_CONTROL_CHECKBOX:			return "CheckBox";
	case UI_CONTROL_RADIOBUTTON:		return "RadioButton";
	case UI_CONTROL_COMBOBOX:			return "ComboBox";
	case UI_CONTROL_SLIDER:				return "Slider";
	case UI_CONTROL_EDITBOX:			return "EditBox";
	case UI_CONTROL_IMEEDITBOX:			return "IMEEditBox";
	case UI_CONTROL_LISTBOX:			return "ListBox";
	case UI_CONTROL_SCROLLBAR:			return "ScrollBar";
	case UI_CONTROL_PROGRESSBAR:		return "ProgressBar";
	case UI_CONTROL_CUSTOM:				return "Custom";
	case UI_CONTROL_TEXTBOX:			return "TextBox";
	case UI_CONTROL_TEXTURECONTROL:		return "TextureControl";
	case UI_CONTROL_HTMLTEXTBOX:		return "HTMLTextBox";
	case UI_CONTROL_TREECONTROL:		return "TreeControl";
	case UI_CONTROL_QUESTTREECONTROL:	return "QuestTreeControl";
	case UI_CONTROL_ANIMATION:			return "Animation";
	case UI_CONTROL_LINE_EDITBOX:		return "LineEditBox";
	case UI_CONTROL_LINE_IMEEDITBOX:	return "LineIMEEditBox";
	case UI_CONTROL_MOVIECONTROL:		return "MovieControl";
	case UI_CONTROL_LISTBOXEX:			return "ListBoxEx";
	default:							return "Invalid Control Type";
	}

	return NULL;
}
void CMainFrame::OnSaveCustomui()
{
	// TODO: Add your command handler code here
	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
	pLayoutView->SaveCustomUI();
}
void CMainFrame::OnSaveAllui()
{
	// TODO: Add your command handler code here
	CLayoutView *pLayoutView;
	pLayoutView = ( CLayoutView * )( ( CEtUIToolView * )( CGlobalValue::GetInstance().m_pCurView ) )->GetTabView( CDummyView::LAYOUT_VIEW );
	pLayoutView->SaveAllUI();
}