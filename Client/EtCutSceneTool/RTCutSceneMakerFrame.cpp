#include "Stdafx.h"

#include <wx/wx.h>
#include "RTCutSceneMakerFrame.h"
#include "cwxRenderPanel.h"
#include "cwxTimeLinePanel.h"
#include "cwxPropertyPanel.h"
#include "cwxWholeResTreePanel.h"
#include "cwxRegisteredResPanel.h"
#include "cwxCamEventListPanel.h"
#include "cwxParticleListPanel.h"
#include "cwxFadeEventListPanel.h"
#include "cwxDOFEventListPanel.h"
#include "cwxPropEventListPanel.h"
#include "cwxSoundEventPanel.h"
#include "cwxSubtitleEventPanel.h"
#include "cwxHistoryPanel.h"

//#ifdef PRE_ADD_FILTEREVENT
#include "cwxColorFilterEventListPanel.h"
//#endif // PRE_ADD_FILTEREVENT

#include "ToolData.h"
#include "CmdProcessor.h"
#include "CommandSet.h"
#include "RTCutSceneRenderer.h"
#include "cwxBatchPropertyEditorDlg.h"
#include "cwxRTCutSceneStatusBar.h"


#define RT_CUTSCENE_TOOL_TITLE wxT("DN Real Time CutScene Tool v0.5")




BEGIN_EVENT_TABLE( CRTCutSceneMakerFrame, wxFrame )
	EVT_ERASE_BACKGROUND( OnEraseBackGround )
	
	// ���� �޴�
	EVT_MENU( ID_MENU_SAVE, CRTCutSceneMakerFrame::OnMenuSave )
	EVT_MENU( ID_MENU_SAVEAS, CRTCutSceneMakerFrame::OnMenuSaveAs )
	EVT_MENU( ID_MENU_LOAD, CRTCutSceneMakerFrame::OnMenuLoad )
	EVT_MENU( ID_MENU_QUIT, CRTCutSceneMakerFrame::OnMenuExit )
	EVT_ACTIVATE( CRTCutSceneMakerFrame::OnActivate )

	// ���� �޴�
	EVT_MENU( ID_MENU_UNDO, CRTCutSceneMakerFrame::OnMenuUndo )
	EVT_MENU( ID_MENU_REDO, CRTCutSceneMakerFrame::OnMenuRedo )
	EVT_UPDATE_UI( ID_MENU_UNDO, CRTCutSceneMakerFrame::OnUpdateCanUndo )
	EVT_UPDATE_UI( ID_MENU_REDO, CRTCutSceneMakerFrame::OnUpdateCanRedo )

	// ������ ����
	EVT_MENU( ID_MENU_PLAY, CRTCutSceneMakerFrame::OnMenuPlay )
	EVT_MENU( ID_MENU_STOP, CRTCutSceneMakerFrame::OnMenuStop )
	EVT_MENU( ID_MENU_PAUSE, CRTCutSceneMakerFrame::OnMenuPause )
	EVT_MENU( ID_MENU_RESET, CRTCutSceneMakerFrame::OnMenuReset )
	EVT_MENU( ID_MENU_BATCH_PROP_EDIT, CRTCutSceneMakerFrame::OnMenuBatchPropEdit )

	// View �޴�
	EVT_UPDATE_UI( ID_MENU_TOGGLE_EDIT_CAM, CRTCutSceneMakerFrame::OnUpdateToggleCamMode )
	EVT_MENU( ID_MENU_TOGGLE_EDIT_CAM, CRTCutSceneMakerFrame::OnMenuToggleCamMode )
	
	EVT_UPDATE_UI( ID_MENU_TOGGLE_SHOW_SPOT_POS, CRTCutSceneMakerFrame::OnUpdateToggleShowSpot )
	EVT_MENU( ID_MENU_TOGGLE_SHOW_SPOT_POS, CRTCutSceneMakerFrame::OnMenuToggleShowSpot )

	EVT_MENU( ID_MENU_TOGGLE_RENDER_FOG, CRTCutSceneMakerFrame::OnMenuToggleRenderFog )
	EVT_UPDATE_UI( ID_MENU_TOGGLE_RENDER_FOG, CRTCutSceneMakerFrame::OnUpdateToggleRenderFog )

	EVT_MENU( ID_MENU_TOGGLE_LETTERBOX, CRTCutSceneMakerFrame::OnMenuToggleLetterBox )
	EVT_UPDATE_UI( ID_MENU_TOGGLE_LETTERBOX, CRTCutSceneMakerFrame::OnUpdateToggleLetterBox )
	
	// ������
	EVT_MENU_OPEN( CRTCutSceneMakerFrame::OnUpdateWindowMenu )
	EVT_MENU_RANGE( ID_MENU_TOGGLE_HISTORY_PANEL, ID_MENU_TOGGLE_TIMELINE_PANEL, CRTCutSceneMakerFrame::OnMenuToggleWindow )

	// ���콺 �� �̺�Ʈ
	EVT_MOUSEWHEEL( CRTCutSceneMakerFrame::OnMouseWheel )

	// bintitle. Ű�Է� �̺�Ʈ.
	EVT_MENU( ID_MENU_TOGGLE_WEAPON, CRTCutSceneMakerFrame::OnMenuToggleWeapon )
	EVT_UPDATE_UI( ID_MENU_TOGGLE_WEAPON, CRTCutSceneMakerFrame::OnUpdateToggleWeapon )
	EVT_KEY_DOWN( CRTCutSceneMakerFrame::OnKeyDown )
	EVT_KEY_UP( CRTCutSceneMakerFrame::OnKeyUp )

	// ���� �ڵ鷯
	EVT_CLOSE( CRTCutSceneMakerFrame::OnCloseFrame )
END_EVENT_TABLE()




CRTCutSceneMakerFrame::CRTCutSceneMakerFrame( const wxString& Title, const wxPoint& Pos, const wxSize& Size ) :
											  wxFrame( NULL, -1, Title, Pos, Size ),
											  m_pTimeLinePanel( NULL ),
											  m_pPropertyPanel( NULL ),
											  m_pWholeResTreePanel( NULL ),
											  m_pRenderPanel( NULL ),
											  m_pRegisteredResPanel( NULL ),
											  m_pHistoryPanel( NULL ),
											  m_pWindowMenu( NULL ),
											  m_pEditMenu( NULL ),
											  m_bEditCameraOnPlay( false ),
											  m_bShowSpotPos( false ),
											  m_pToolBar( NULL ),
											  m_bRenderFog( true ),			// Renderer ���� ���� ������ �ִµ� true �� ���־����Ƿ� ���⼭�� true�� ����
											  m_bShowLetterBox( true ),
											  m_bWeapon( false ) // bintitle.	
{
	// �ݵ�� ���� ���� ���� �������� ������ ���� ����ϰ� �гε��� �����ؾ���
	// �ٸ� �гε鿡�� wheel �̺�Ʈ�� ���� ���������� connect ��Ű�� �����̴�.
	TOOL_DATA.SetMainFrame( this );
	TOOL_DATA.Initialize();

	m_AUIManager.SetManagedWindow( this );

	_SetupMenu();
	_SetupToolBar();
	_SetupAUI();
	_SetupCmdListener();

	//CreateStatusBar();
	m_pStatusBar = new cwxRTCutSceneStatusBar( this );
	SetStatusBar( m_pStatusBar );

	SetStatusText( wxT("Ready") );

	TOOL_DATA.SetEditMode( CToolData::EM_EDIT );

	_UpdateTitle();

	DragAcceptFiles( true );
}



CRTCutSceneMakerFrame::~CRTCutSceneMakerFrame(void)
{
	m_AUIManager.UnInit();
}



void CRTCutSceneMakerFrame::_UpdateTitle( void )
{
	bool bIsOpen = TOOL_DATA.IsFileOpen();

	if( bIsOpen )
	{
		const wxChar* pOpenedFileName = TOOL_DATA.GetOpenedFileName();
		wxString strTitle = wxString(RT_CUTSCENE_TOOL_TITLE) + wxT(" - ") + pOpenedFileName;

		if( false == TOOL_DATA.IsWritable() )
			strTitle += wxT("(Read Only)");

		SetTitle( strTitle );
	}
	else
	{
		SetTitle( wxString(RT_CUTSCENE_TOOL_TITLE)+wxT(" - ")+wxT("Untitled") );
	}

	//const wxChar* pOpenedFileName = TOOL_DATA.GetOpenedFileName();

	//if( pOpenedFileName && strcmp(pOpenedFileName, "") != 0 )
	//	SetTitle( wxString(RT_CUTSCENE_TOOL_TITLE)+wxT(" - ")+pOpenedFileName );
	//else
	//	SetTitle( wxString(RT_CUTSCENE_TOOL_TITLE)+wxT(" - ")+wxT("Untitled") );
}



// AUI ����
void CRTCutSceneMakerFrame::_SetupAUI( void )
{
	// �� �ٸ� AUIManager�� �߰�
	m_AUIManager.AddPane( m_pToolBar, wxAuiPaneInfo().
						  Name(wxT("tb1")).Caption(wxT("ToolBar")).
						  ToolbarPane().Top().
						  LeftDockable(false).RightDockable(false));

	m_pRenderPanel = new cwxRenderPanel( this, ID_RENDER_PANEL );
	m_AUIManager.AddPane( m_pRenderPanel,
						  wxAuiPaneInfo().Name(wxT("Render")).CenterPane() );

	m_pWholeResTreePanel = new cwxWholeResTreePanel( this, ID_WHOLE_RES_TREE_PANEL );
	m_AUIManager.AddPane( m_pWholeResTreePanel, 
						  wxAuiPaneInfo().Name(wxT("Whole Resources")).Left().CloseButton(false).Caption( wxT("Whole Resources") ).BestSize(250, 800) );

	m_pHistoryPanel = new cwxHistoryPanel( this, ID_HISTORY_PANEL );
	m_AUIManager.AddPane( m_pHistoryPanel,
						  wxAuiPaneInfo().Name(wxT("History Panel")).Left().Bottom().CloseButton(true).Caption(wxT("Work History")).BestSize(200, 200).Hide() );
	m_mapWindows.insert( make_pair(ID_MENU_TOGGLE_HISTORY_PANEL, m_pHistoryPanel) );

	m_pTimeLinePanel = new cwxTimeLinePanel( this, ID_TIMELINE_PANEL );
	m_AUIManager.AddPane( m_pTimeLinePanel, 
						  wxAuiPaneInfo().Name(wxT("Sequence Viewer")).Bottom().CloseButton(true).Caption(wxT("Sequence Viewer")).BestSize(800, 400) );
	m_mapWindows.insert( make_pair(ID_MENU_TOGGLE_TIMELINE_PANEL, m_pTimeLinePanel) );

	// ��ϵ� ���ҽ��� Ű ��Ʈ �г��� AUI ��Ʈ������ ���´�.
	wxAuiNotebook* pAuiNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
													 wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_TOP | wxAUI_NB_SCROLL_BUTTONS );

	m_pRegisteredResPanel = new cwxRegisteredResPanel( this, ID_REGISTERED_RES_PANEL );
	pAuiNotebook->AddPage( m_pRegisteredResPanel, wxT("Registered Resources"), true );
	//m_AUIManager.AddPane( m_pRegisteredResPanel, 
	//					  wxAuiPaneInfo().Name(wxT("Registered Resources")).Right().CloseButton(false).Caption(wxT("Registered Resources")).BestSize(300, 200) );
	
	m_pCamListPanel = new cwxCamEventListPanel( this, ID_CAMERA_EVENT_LIST_PANEL );
	pAuiNotebook->AddPage( m_pCamListPanel, wxT("Camera List") );

	m_pFadeEventListPanel = new cwxFadeEventListPanel( this, ID_FADE_EVENT_LIST_PANEL );
	pAuiNotebook->AddPage( m_pFadeEventListPanel, wxT("Fade List") );

	m_pDOFEventListPanel = new cwxDOFEventListPanel( this, ID_DOF_EVENT_LIST_PANEL );
	pAuiNotebook->AddPage( m_pDOFEventListPanel, wxT("DOF List") );

	m_pPropEventListPanel = new cwxPropEventListPanel( this, ID_PROP_EVENT_LIST_PANEL );
	pAuiNotebook->AddPage( m_pPropEventListPanel, wxT("Prop List") );

	m_pSoundEventListPanel = new cwxSoundEventPanel( this, ID_SOUND_EVENT_LIST_PANEL );
	pAuiNotebook->AddPage( m_pSoundEventListPanel, wxT("Sound List") );

	m_pSubtitleEventPanel = new cwxSubtitleEventPanel( this, ID_SUBTITLE_EVENT_LIST_PANEL );
	pAuiNotebook->AddPage( m_pSubtitleEventPanel, wxT("Subtitle List") );

	m_pParticleListPanel = new cwxParticleListPanel( this, ID_PARTICLE_EVENT_LIST_PANEL );
	pAuiNotebook->AddPage( m_pParticleListPanel, wxT("Particle List") );

//#ifdef PRE_ADD_FILTEREVENT
	m_pColorFilterEventPanel = new cwxColorFilterEventListPanel( this, ID_COLORFILTER_PANEL );
	pAuiNotebook->AddPage( m_pColorFilterEventPanel, wxT("ColorFilter List") );
//#endif // PRE_ADD_FILTEREVENT

	m_AUIManager.AddPane( pAuiNotebook, 
						  wxAuiPaneInfo().Name(wxT("Key Set")).Right().CloseButton(false).BestSize(300, 200) );
	
	m_pPropertyPanel = new cwxPropertyPanel( this, ID_PROPERTY_PANEL );
	m_AUIManager.AddPane( m_pPropertyPanel,
						  wxAuiPaneInfo().Name(wxT("Property Panel")).Right().CloseButton(false).Caption(wxT("Property")).BestSize(200, 200) );

	m_pRenderPanel->SetTimeLinePanel( m_pTimeLinePanel );

	m_AUIManager.SetFlags( m_AUIManager.GetFlags() ^ (wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_TRANSPARENT_DRAG) );

	wxAuiDockArt* pArtProvider = m_AUIManager.GetArtProvider();
	pArtProvider->SetMetric( wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_VERTICAL );

	Maximize();

	m_AUIManager.Update();

	// ���� �׽�Ʈ
	//char* pTest = NULL;
	//strcpy( pTest, pTest );
}


void CRTCutSceneMakerFrame::_SetupCmdListener( void )
{
	TOOL_DATA.AddCommandListener( m_pRenderPanel );
	TOOL_DATA.AddCommandListener( m_pRegisteredResPanel );
	TOOL_DATA.AddCommandListener( m_pWholeResTreePanel );
	TOOL_DATA.AddCommandListener( m_pPropertyPanel );
	TOOL_DATA.AddCommandListener( m_pTimeLinePanel );
	TOOL_DATA.AddCommandListener( m_pHistoryPanel );
	TOOL_DATA.AddCommandListener( m_pCamListPanel );
	TOOL_DATA.AddCommandListener( m_pFadeEventListPanel );
	TOOL_DATA.AddCommandListener( m_pDOFEventListPanel );
	TOOL_DATA.AddCommandListener( m_pPropEventListPanel );
	TOOL_DATA.AddCommandListener( m_pSoundEventListPanel );
	TOOL_DATA.AddCommandListener( m_pSubtitleEventPanel );
	TOOL_DATA.AddCommandListener( m_pParticleListPanel );

//PRE_ADD_FILTEREVENT
	TOOL_DATA.AddCommandListener( m_pColorFilterEventPanel );
	
}


void CRTCutSceneMakerFrame::ConnectMouseWheelEventToRenderPanel( wxWindow* pWheelOccurredWnd )
{
	pWheelOccurredWnd->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler(CRTCutSceneMakerFrame::OnMouseWheel), NULL, this );
}



// �޴� ����
void CRTCutSceneMakerFrame::_SetupMenu( void )
{
	m_pMenuBar = new wxMenuBar;

	// File �޴�
	wxMenu* pMenu = new wxMenu;
	pMenu->Append( ID_MENU_LOAD, wxT("Load\tCtrl-O"), wxT("Load working file..") );
	pMenu->Append( ID_MENU_SAVE, wxT("Save\tCtrl-S"), wxT("Save this work") );
	pMenu->Append( ID_MENU_SAVEAS, wxT("Save As..\tCtrl-Shift-S"), wxT("Save this work as another file") );
	pMenu->AppendSeparator();
	pMenu->Append( ID_MENU_QUIT, wxT("Exit\tCtrl-Q"), wxT("Quit Real Time Cut Scene Maker") );

	m_pMenuBar->Append( pMenu, wxT("&File") );

	// Edit �޴�
	pMenu = new wxMenu;
	pMenu->Append( ID_MENU_PLAY, wxT("Play\t/"), wxT("Play Scene") );
	pMenu->Append( ID_MENU_PAUSE, wxT("Pause\t'"), wxT("Pause Scene") );
	pMenu->Append( ID_MENU_STOP, wxT("Stop\t;"), wxT("Stop Scene") );
	pMenu->AppendSeparator();
	pMenu->Append( ID_MENU_UNDO, wxT("&Undo\tCtrl-Z"), wxT("Undo last work did") );
	pMenu->Append( ID_MENU_REDO, wxT("&Redo\tCtrl-Y"), wxT("Redo last work did") );
	pMenu->AppendSeparator();
	pMenu->Append( ID_MENU_BATCH_PROP_EDIT, wxT("&Batch Property Edit..\tCtrl-B"), wxT("Edit property batched") );
	m_pEditMenu = pMenu;

	m_pMenuBar->Append( pMenu, wxT("&Edit") );

	// View �޴�
	pMenu = new wxMenu;
	pMenu->AppendCheckItem( ID_MENU_TOGGLE_EDIT_CAM, wxT("Toggle &Edit Cam\tCtrl-G"), wxT("Toggle Edit Camera") );
	pMenu->AppendCheckItem( ID_MENU_TOGGLE_SHOW_SPOT_POS, wxT("Toggle Show &Spot Pos\tAlt-S"), wxT("Toggle Spot Pos") );
	wxMenuItem* pItem = pMenu->AppendCheckItem( ID_MENU_TOGGLE_RENDER_FOG, wxT("Toggle Render &Fog\tAlt-F"), wxT("Toggle Render Fog") );
	//pItem->Check( m_bRenderFog );
	m_pMenuBar->Append( pMenu, wxT("&View") );
	pMenu->AppendCheckItem( ID_MENU_TOGGLE_LETTERBOX, wxT("Toggle &LetterBox\tCtrl-L"), wxT("Toggle LetterBox") );
	pMenu->AppendCheckItem( ID_MENU_TOGGLE_WEAPON, wxT("&Weapon\tAlt-W"), wxT("Toggle Weapon") ); // Weapon - bintitle.
	
	// Window �޴�
	pMenu = new wxMenu;
	pMenu->AppendCheckItem( ID_MENU_TOGGLE_HISTORY_PANEL, wxT("&History Panel\tCtrl-H"), wxT("Toggle History Panel") );
	pMenu->AppendCheckItem( ID_MENU_TOGGLE_TIMELINE_PANEL, wxT("&TimeLine Panel\tCtrl-T"), wxT("Toggle TimeLine Panel") );
	m_pWindowMenu = pMenu;
	m_pMenuBar->Append( pMenu, wxT("&Window") );

	SetMenuBar( m_pMenuBar );
}



// �� �� ����
void CRTCutSceneMakerFrame::_SetupToolBar( void )
{
	//CreateToolBar( wxTB_FLAT | wxTB_HORIZONTAL | wxTB_DOCKABLE/* | wxTB_TEXT */);

	m_pToolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
										 wxTB_FLAT | wxTB_NODIVIDER);
	
	//wxToolBarBase* m_pToolBar = GetToolBar();
	m_pToolBar->SetToolBitmapSize( wxSize(16, 16) );

	m_pToolBar->AddTool( ID_MENU_LOAD, wxT("Load"), wxBITMAP(open), wxT("Load work file(Ctrl-O)") );
	m_pToolBar->AddTool( ID_MENU_SAVE, wxT("Save"), wxBITMAP(save), wxT("Save this work(Ctrl-S)") );
	m_pToolBar->AddTool( ID_MENU_SAVEAS, wxT("Save As"), wxBITMAP(save), wxT("Save this work as another file(Ctrl-Shift-S)") );

	m_pToolBar->AddSeparator();

	m_pToolBar->AddTool( ID_MENU_UNDO, wxT("Undo"), wxBITMAP(undo), wxT("Cancel last work (Ctrl-Z)") );
	m_pToolBar->AddTool( ID_MENU_REDO, wxT("Redo"), wxBITMAP(redo), wxT("Replay previous work (Ctrl-Y)") );

	m_pToolBar->AddSeparator();

	m_pToolBar->AddTool( ID_MENU_PLAY, wxT("Play"), wxBITMAP(play), wxT("Play scene") );
	m_pToolBar->AddTool( ID_MENU_PAUSE, wxT("Pause"), wxBITMAP(pause), wxT("Pause scene") );
	m_pToolBar->AddTool( ID_MENU_STOP, wxT("Stop"), wxBITMAP(stop), wxT("Stop scene") );
	m_pToolBar->AddTool( ID_MENU_RESET, wxT("Reset"), wxBITMAP(reset), wxT("Reset scene") );
	
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool( ID_MENU_TOGGLE_EDIT_CAM, wxT("Edit Cam"), wxBITMAP(camera), wxT("Toggle Edit Camera in play mode"), wxITEM_CHECK );
	m_pToolBar->AddTool( ID_MENU_TOGGLE_SHOW_SPOT_POS, wxT("Show Pos"), wxBITMAP(camera), wxT("Toggle spot pos in render view"), wxITEM_CHECK );
	m_pToolBar->AddTool( ID_MENU_TOGGLE_RENDER_FOG, wxT("Fog"), wxBITMAP(camera), wxT("Toggle rendering fog in render view"), wxITEM_CHECK );
	m_pToolBar->AddTool( ID_MENU_TOGGLE_LETTERBOX, wxT("LetterBox"), wxBITMAP(letterbox), wxT("Toggle letterbox"), wxITEM_CHECK );
	
	m_pToolBar->AddTool( ID_MENU_TOGGLE_WEAPON, wxT("Weapon_1"), wxBITMAP(weapon_1), wxT("Toggle Weapon"), wxITEM_CHECK ); // bintitle.

	m_pToolBar->EnableTool( ID_MENU_STOP, false );
	m_pToolBar->EnableTool( ID_MENU_PAUSE, false );

	m_pToolBar->Realize();
}




void CRTCutSceneMakerFrame::OnEraseBackGround( wxEraseEvent& EraseEvent )
{
	// �ƹ��͵� ���Ѵ�.
	
}




void CRTCutSceneMakerFrame::OnMenuLoad( wxCommandEvent& MenuEvent )
{
	// ���� ���� �����Ǿ��°�? �����Ǿ��ٸ� ���� ������ ������ �����.
	if( TOOL_DATA.IsFileEdited() )
	{
		wxString strMessage;
		const wxChar* pFileName = TOOL_DATA.GetOpenedFileName();
		strMessage.Printf( wxT("%s File is editing now.\nSave working file?"), pFileName );
		int iResult = wxMessageBox( strMessage, wxT("RTCutScene Tool"), 
									wxYES_NO|wxCANCEL|wxCENTRE|wxICON_QUESTION, 
									this );

		switch( iResult )
		{
			case wxYES:
				{
					// �����ϰ� �ε��Ѵ�.
					wxString strFilePath = TOOL_DATA.GetOpenedFilePath();
					TOOL_DATA.SaveToFile( strFilePath.c_str() );
				}
				break;

			case wxCANCEL:
				// ��Ҹ� �ε����� ���� �״�� ����
				return;
		}
	}

	// ��� ��ũ��Ʈ�� �״�� �б⸸ �ϸ� �ȴ�.
	wxFileDialog* pOpenDialog = new wxFileDialog( this, wxT("Open the RTCutScene Work File"), wxT(""), wxT(""), wxT("DN Real time cutscene file (*.dmv)|*.dmv"), 
												  wxOPEN/* | wxOVERWRITE_PROMPT*/, wxDefaultPosition );

	if( pOpenDialog->ShowModal() == wxID_OK )
	{
		// �׽�Ʈ��.
		bool bLoadSuccess = TOOL_DATA.LoadFromFile( pOpenDialog->GetPath().GetData() );

		// ���� ���¿� �������� ��쿡��.
		if( bLoadSuccess )
		{
			// TODO: TOOL_DATA �� ���� �ִ� �����鵵 �ʱ�ȭ ���־�� ��!
			TOOL_DATA.ClearToolTempData();

			CUpdateViewCmd UpdateView( &TOOL_DATA );
			TOOL_DATA.RunCommand( &UpdateView );
			TOOL_DATA.SetEdited( false );

			_UpdateTitle();

			TOOL_DATA.GetRenderer()->OnPostLoadFile();
			TOOL_DATA.SyncWithDependencyResources();
		}
	}

	// ����� ���̾�α� ����.
	pOpenDialog->Destroy();

	//// �׽�Ʈ��.
	//TOOL_DATA.LoadFromFile( wxT("test.lua") );

	//// TODO: TOOL_DATA �� ���� �ִ� �����鵵 �ʱ�ȭ ���־�� ��!
	//TOOL_DATA.ClearToolTempData();

	//CUpdateViewCmd UpdateView( &TOOL_DATA );
	//TOOL_DATA.RunCommand( &UpdateView );
}




void CRTCutSceneMakerFrame::OnMenuSave( wxCommandEvent& MenuEvent )
{
	// ���� �����̶�� ���̾�α׸� ��� �����Ѵ�.
	if( false == TOOL_DATA.IsFileOpen() )
	{
		wxFileDialog* pSaveDialog = new wxFileDialog( this, wxT("Save the RTCutScene Work File"), wxT(""), wxT(""), wxT("DN Real time cutscene file (*.dmv)|*.dmv"), 
													  wxSAVE | wxOVERWRITE_PROMPT, wxDefaultPosition );

		if( pSaveDialog->ShowModal() == wxID_OK )
		{
			TOOL_DATA.SaveToFile( pSaveDialog->GetPath().GetData() );
			_UpdateTitle();
		}

		// ����� ���̾�α� ����.
		pSaveDialog->Destroy();
	}
	else
	{
		// read only �̸� save as �� �������..
		if( false == TOOL_DATA.IsWritable() )
			OnMenuSaveAs( MenuEvent );

		// �۾����� ���� �̸� �״�� ��ٷ� ����
		if( TOOL_DATA.IsFileEdited() )
		{
			wxString strOpenedFilePath = TOOL_DATA.GetOpenedFilePath();
			TOOL_DATA.SaveToFile( strOpenedFilePath );
			_UpdateTitle();
		}
	}

	// �׽�Ʈ��.
	//TOOL_DATA.SaveToFile( wxT("test.lua") );
}




void CRTCutSceneMakerFrame::OnMenuSaveAs( wxCommandEvent& MenuEvent )
{
	//if( false == TOOL_DATA.IsFileOpen() )
	//{
		wxFileDialog* pSaveDialog = new wxFileDialog( this, wxT("Save the RTCutScene Work File"), wxT(""), wxT(""), wxT("DN Real time cutscene file (*.dmv)|*.dmv"), 
													  wxSAVE | wxOVERWRITE_PROMPT, wxDefaultPosition );

		if( pSaveDialog->ShowModal() == wxID_OK )
		{
			TOOL_DATA.SaveToFile( pSaveDialog->GetPath().GetData() );
		}

		// ����� ���̾�α� ����.
		pSaveDialog->Destroy();
	//}
}




void CRTCutSceneMakerFrame::OnMenuExit( wxCommandEvent& MenuEvent )
{
	// �� ����
	Close( true );
}



void CRTCutSceneMakerFrame::OnCloseFrame( wxCloseEvent& CloseEvent )
{
	// ���࿡ �����Ͱ� ������ ���¶�� ������ �ųİ� �����.
	if( TOOL_DATA.IsFileEdited() )
	{
		wxString strMessage;
		const wxChar* pFileName = NULL;

		if( TOOL_DATA.IsFileOpen() )
			pFileName = TOOL_DATA.GetOpenedFileName();
		else
			pFileName = wxT("Untitled");

		strMessage.Printf( wxT("%s File has modified.\nDo you save this file?"), pFileName );
		int iResult = wxMessageBox( strMessage, wxT("DN RTCutScene Tool"), 
									wxYES_NO|wxCANCEL|wxCENTRE|wxICON_QUESTION, 
									this );

		switch( iResult )
		{
			case wxYES:
				{
					// ���࿡ ���� ������ �ȵǾ��ִ� ���¶�� ���� �̸� ���ؼ� ������ �� �ְ� ���ش�.
					if( TOOL_DATA.IsFileOpen() )
					{
						// �����ϰ� �����Ѵ�.
					 	const wxString strFilePath = TOOL_DATA.GetOpenedFilePath();
						TOOL_DATA.SaveToFile( strFilePath.c_str() );
						this->Destroy();
					}
					else
					{
						wxFileDialog* pSaveDialog = new wxFileDialog( this, wxT("Save the RTCutScene Work File"), wxT(""), wxT(""), wxT("DN Real time cutscene file (*.dmv)|*.dmv"), 
																	  wxSAVE | wxOVERWRITE_PROMPT, wxDefaultPosition );

						if( pSaveDialog->ShowModal() == wxID_OK )
						{
							TOOL_DATA.SaveToFile( pSaveDialog->GetPath().GetData() );
						}

						// ����� ���̾�α� ����.
						pSaveDialog->Destroy();
					}
				}
				break;

			case wxNO:
				this->Destroy();
				break;

			case wxCANCEL:
				if( !CloseEvent.CanVeto() )			// �� ���� ��ɿ� ���� �źα��� ����� �� �ִ°�~~~
					this->Destroy();
				else
					CloseEvent.Veto();				// �źα� ���~ ���� ����~~
				break;
		}
	}
	else
		this->Destroy();
}



void CRTCutSceneMakerFrame::OnUpdateWindowMenu( wxMenuEvent& MenuEvent )
{
	if( MenuEvent.GetMenu() == m_pWindowMenu )
	{
		int iNumMenu = (int)m_pWindowMenu->GetMenuItemCount() + ID_MENU_TOGGLE_HISTORY_PANEL;
		for( int iMenu = ID_MENU_TOGGLE_HISTORY_PANEL; iMenu < iNumMenu; ++iMenu )
		{			
			m_pWindowMenu->Check( iMenu, m_AUIManager.GetPane(m_mapWindows[iMenu]).IsShown() );
		}
	}
}



void CRTCutSceneMakerFrame::OnMenuToggleWindow( wxCommandEvent& MenuEvent )
{
	wxWindow* pWindow = m_mapWindows[ MenuEvent.GetId() ];
	m_AUIManager.GetPane( pWindow ).Show( MenuEvent.IsChecked() );
	
	m_AUIManager.Update();
}



void CRTCutSceneMakerFrame::OnUpdateToggleCamMode( wxUpdateUIEvent& UpdateEvent )
{
	if( TOOL_DATA.GetEditMode() == CToolData::EM_PLAY )
	{
		UpdateEvent.Enable( true );
		UpdateEvent.Check( m_bEditCameraOnPlay );
	}
	else
	{
		UpdateEvent.Check( false );
		UpdateEvent.Enable( false );
		m_bEditCameraOnPlay = false;
	}
}


void CRTCutSceneMakerFrame::OnMenuToggleCamMode( wxCommandEvent& MenuEvent )
{
	// �÷����� ���� ī�޶�� ���!
	m_bEditCameraOnPlay = !m_bEditCameraOnPlay;

	m_pRenderPanel->ToggleCamMode( m_bEditCameraOnPlay );
}




void CRTCutSceneMakerFrame::OnUpdateToggleShowSpot( wxUpdateUIEvent& UpdateEvent )
{
	UpdateEvent.Check( m_bShowSpotPos );
}



void CRTCutSceneMakerFrame::OnMenuToggleShowSpot( wxCommandEvent& MenuEvent )
{
	m_bShowSpotPos = !m_bShowSpotPos;

	m_pRenderPanel->ToggleShowSpotPos( m_bShowSpotPos );
}



void CRTCutSceneMakerFrame::OnMenuToggleRenderFog( wxCommandEvent& MenuEvent )
{
	m_bRenderFog = !m_bRenderFog;

	m_pRenderPanel->ToggleRenderFog( m_bRenderFog );
}


void CRTCutSceneMakerFrame::OnMenuToggleLetterBox( wxCommandEvent& MenuEvent )
{
	m_bShowLetterBox = !m_bShowLetterBox;
	
	m_pRenderPanel->ToggleLetterBox( m_bShowLetterBox );
}



void CRTCutSceneMakerFrame::OnUpdateToggleRenderFog( wxUpdateUIEvent& UpdateEvent )
{
	UpdateEvent.Check( m_bRenderFog );
}


void CRTCutSceneMakerFrame::OnUpdateToggleLetterBox( wxUpdateUIEvent& UpdateEvent )
{
	UpdateEvent.Check( m_bShowLetterBox );
}


void CRTCutSceneMakerFrame::OnMenuUndo( wxCommandEvent& MenuEvent )
{
	if( TOOL_DATA.GetEditMode() == CToolData::EM_EDIT )
		TOOL_DATA.UndoCommand();
}


void CRTCutSceneMakerFrame::OnMenuRedo( wxCommandEvent& MenuEvent )
{
	if( TOOL_DATA.GetEditMode() == CToolData::EM_EDIT )
		TOOL_DATA.RedoCommand();
}



void CRTCutSceneMakerFrame::OnMenuPlay( wxCommandEvent& MenuEvent )
{
	// �Ӽ� â Disable
	TOOL_DATA.SetEditMode( CToolData::EM_PLAY );
	m_pPropertyPanel->Enable( false );
	m_pWholeResTreePanel->Enable( false );
	m_pRegisteredResPanel->Enable( false );
	m_pHistoryPanel->Enable( false );
	m_pTimeLinePanel->Enable( false );

	//wxToolBarBase* pToolBar = GetToolBar();

	m_pToolBar->EnableTool( ID_MENU_PLAY, false );
	m_pToolBar->EnableTool( ID_MENU_PAUSE, true );
	m_pToolBar->EnableTool( ID_MENU_STOP, true );

	m_pEditMenu->Enable( ID_MENU_PLAY, false );
	m_pEditMenu->Enable( ID_MENU_STOP, true );

	m_pRenderPanel->StartPlay();
}



void CRTCutSceneMakerFrame::OnMenuStop( wxCommandEvent& MenuEvent )
{
	TOOL_DATA.SetEditMode( CToolData::EM_EDIT );
	m_pPropertyPanel->Enable( true );
	m_pWholeResTreePanel->Enable( true );
	m_pRegisteredResPanel->Enable( true );
	m_pHistoryPanel->Enable( true );
	m_pTimeLinePanel->Enable( true );
	//m_pTimeLinePanel->Refresh();

	//wxToolBarBase* pToolBar = GetToolBar();

	m_pToolBar->EnableTool( ID_MENU_PLAY, true );
	m_pToolBar->EnableTool( ID_MENU_PAUSE, false );
	m_pToolBar->EnableTool( ID_MENU_STOP, false );

	m_pEditMenu->Enable( ID_MENU_PLAY, true );
	m_pEditMenu->Enable( ID_MENU_STOP, false );

	m_pRenderPanel->StopPlay();
}



void CRTCutSceneMakerFrame::OnMenuPause( wxCommandEvent& MenuEvent )
{
	m_pRenderPanel->PausePlay();

	//wxToolBarBase* pToolBar = GetToolBar();
	m_pEditMenu->Enable( ID_MENU_PLAY, true );
	m_pToolBar->EnableTool( ID_MENU_PLAY, true );
}



void CRTCutSceneMakerFrame::OnMenuReset( wxCommandEvent& MenuEvent )
{
	m_pRenderPanel->ResetPlay();
}


void CRTCutSceneMakerFrame::OnMenuBatchPropEdit( wxCommandEvent& MenuEvent )
{
	// �ϰ� ���� ���̾�α� �����.
	cwxBatchPropertyEditorDlg* pBatchPropertyEditorDlg = new cwxBatchPropertyEditorDlg( this, -1, wxT("Batch Property Editor"), 
																						wxDefaultPosition, wxSize(640, 480), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

	int iResult = pBatchPropertyEditorDlg->ShowModal();
	if( wxID_OK == iResult )
	{
		
	}
}


void CRTCutSceneMakerFrame::OnUpdateCanUndo( wxUpdateUIEvent& UpdateEvent )
{
	if( TOOL_DATA.GetEditMode() == CToolData::EM_EDIT )
	{
		CCmdProcessor* pCmdProcessor = TOOL_DATA.GetCmdProcessor();
		UpdateEvent.Enable( pCmdProcessor->CanUndo() );
	}
	else
		UpdateEvent.Enable( false );
}


void CRTCutSceneMakerFrame::OnUpdateCanRedo( wxUpdateUIEvent& UpdateEvent )
{
	if( TOOL_DATA.GetEditMode() == CToolData::EM_EDIT )
	{
		CCmdProcessor* pCmdProcessor = TOOL_DATA.GetCmdProcessor();
		UpdateEvent.Enable( pCmdProcessor->CanRedo() );
	}
	else
		UpdateEvent.Enable( false );
}

bool CRTCutSceneMakerFrame::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& FileNames )
{
	// �� ���� ���ϸ� ����!
	wxString strFileName = FileNames.front();
	
	strFileName.MakeLower();
	if( wxNOT_FOUND != strFileName.find( wxT(".dmv") ) )
	{
		// ���� ���� �����Ǿ��°�? �����Ǿ��ٸ� ���� ������ ������ �����.
		if( TOOL_DATA.IsFileEdited() )
		{
			wxString strMessage;
			const wxChar* pFileName = TOOL_DATA.GetOpenedFileName();
			strMessage.Printf( wxT("%s File is editing now.\nSave working file?"), pFileName );
			int iResult = wxMessageBox( strMessage, wxT("RTCutScene Tool"), 
				wxYES_NO|wxCANCEL|wxCENTRE|wxICON_QUESTION, 
				this );

			switch( iResult )
			{
			case wxYES:
				{
					// �����ϰ� �ε��Ѵ�.
					wxString strFilePath = TOOL_DATA.GetOpenedFilePath();
					TOOL_DATA.SaveToFile( strFilePath.c_str() );
				}
				break;

			case wxCANCEL:
				// ��Ҹ� �ε����� ���� �״�� ����
				return true;
			}
		}

		bool bLoadSuccess = TOOL_DATA.LoadFromFile( strFileName.c_str() );

		// ���� ���¿� �������� ��쿡��.
		if( bLoadSuccess )
		{
			// TODO: TOOL_DATA �� ���� �ִ� �����鵵 �ʱ�ȭ ���־�� ��!
			TOOL_DATA.ClearToolTempData();

			CUpdateViewCmd UpdateView( &TOOL_DATA );
			TOOL_DATA.RunCommand( &UpdateView );
			TOOL_DATA.SetEdited( false );

			_UpdateTitle();

			TOOL_DATA.GetRenderer()->OnPostLoadFile();
			TOOL_DATA.SyncWithDependencyResources();
		}
	}
	else
	{
		wxMessageBox( wxT("Not EtCutSceneTool File!"), wxT("RTCutScene Tool"), wxOK|wxCENTRE|wxICON_ERROR, this );
	}

	return true;
}


void CRTCutSceneMakerFrame::OnActivate( wxActivateEvent& ActiveEvent )
{
	if( m_pRenderPanel )
		m_pRenderPanel->OnActivate( ActiveEvent );
}


// �׽�Ʈ�� �� �޽��� ���� ����. �ٸ޽����� ������ �гη� �Ȱ��Ƿ� ��ٷ� ������� �ش�.
void CRTCutSceneMakerFrame::OnMouseWheel( wxMouseEvent& WheelEvent )
{
	// �� �̺�Ʈ�� �˾Ƽ� �� ã�ư� �� �ְ� �����.
	if( m_pRenderPanel->IsMouseInWindow() )
		m_pRenderPanel->OnMouseEvent( WheelEvent );
	else
	if( m_pTimeLinePanel->IsMouseInWindow() )
		m_pTimeLinePanel->OnMouseEvent( WheelEvent );
}


// bintitle.
void CRTCutSceneMakerFrame::OnKeyDown( wxKeyEvent & keyEvent )
{
	//if( m_pRenderPanel->IsMouseInWindow() )
		m_pRenderPanel->OnKeyDown( keyEvent );	
}

void CRTCutSceneMakerFrame::OnKeyUp( wxKeyEvent & keyEvent )
{
	//if( m_pRenderPanel->IsMouseInWindow() )
	m_pRenderPanel->OnKeyUp( keyEvent );	
}


void CRTCutSceneMakerFrame::OnMenuToggleWeapon( wxCommandEvent& UpdateEvent )
{
	m_bWeapon ^= true;
	m_pRenderPanel->ToggleWeapon( m_bWeapon );
}


void CRTCutSceneMakerFrame::OnUpdateToggleWeapon( wxUpdateUIEvent& UpdateEvent )
{
	UpdateEvent.Check( m_bWeapon );
}


void CRTCutSceneMakerFrame::OnEdited( void )
{
	_UpdateTitle();

	if( TOOL_DATA.IsFileEdited() )
		SetTitle( GetTitle()+wxT("*") );
}


void CRTCutSceneMakerFrame::SetCursorPosText( const wxString& strPosText )
{
	static_cast<cwxRTCutSceneStatusBar*>(m_pStatusBar)->SetPositionText( strPosText );
}