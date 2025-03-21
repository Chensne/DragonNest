#pragma once

#include <wx/aui/aui.h>
#include <wx/dnd.h>

class cwxRenderPanel;
class cwxTimeLinePanel;
class cwxPropertyPanel;
class cwxWholeResTreePanel;
class cwxRegisteredResPanel;
class cwxCamEventListPanel;
class cwxFadeEventListPanel;
class cwxDOFEventListPanel;
class cwxPropEventListPanel;
class cwxSubtitleEventPanel;
class cwxSoundEventPanel;
class cwxParticleListPanel;
class cwxHistoryPanel;

//#ifdef PRE_ADD_FILTEREVENT
class cwxColorFilterEventListPanel;
//#endif // PRE_ADD_FILTEREVENT

// 툴 메인 프레임
class CRTCutSceneMakerFrame : public wxFrame, public wxFileDropTarget
{
private:
	enum
	{
		ID_RENDER_PANEL = wxID_HIGHEST + 1,
		ID_WHOLE_RES_TREE_PANEL,
		ID_TIMELINE_PANEL,
		ID_PROPERTY_PANEL,
		ID_REGISTERED_RES_PANEL,
		ID_CAMERA_EVENT_LIST_PANEL,
		ID_FADE_EVENT_LIST_PANEL,
		ID_DOF_EVENT_LIST_PANEL,
		ID_PROP_EVENT_LIST_PANEL,
		ID_SOUND_EVENT_LIST_PANEL,
		ID_SUBTITLE_EVENT_LIST_PANEL,
		ID_PARTICLE_EVENT_LIST_PANEL,
		ID_HISTORY_PANEL,

//#ifdef PRE_ADD_FILTEREVENT
		ID_COLORFILTER_PANEL,
//#endif // PRE_ADD_FILTEREVENT

		// 메뉴

		// File
		ID_MENU_LOAD,
		ID_MENU_SAVE,
		ID_MENU_SAVEAS,
		ID_MENU_QUIT,

		// Edit
		ID_MENU_UNDO,
		ID_MENU_REDO,
		ID_MENU_PLAY,
		ID_MENU_STOP,
		ID_MENU_PAUSE,
		ID_MENU_RESET,
		ID_MENU_BATCH_PROP_EDIT,		// 이벤트, 액션 일괄 편집.

		// View
		ID_MENU_TOGGLE_EDIT_CAM,		// 재생 모드일 때만 활성화.
		ID_MENU_TOGGLE_SHOW_SPOT_POS,
		ID_MENU_TOGGLE_RENDER_FOG,
		ID_MENU_TOGGLE_LETTERBOX,		
		ID_MENU_TOGGLE_WEAPON,          // 무기.

		// Window
		ID_MENU_TOGGLE_HISTORY_PANEL,
		ID_MENU_TOGGLE_TIMELINE_PANEL,

		
	};

private:
	wxAuiManager				m_AUIManager;

	// 메뉴바
	wxMenuBar*					m_pMenuBar;

	// 툴 바
	wxToolBar*					m_pToolBar;

	// 상태 바
	wxStatusBar*				m_pStatusBar;

	// 기본 패널
	cwxWholeResTreePanel*		m_pWholeResTreePanel;
	cwxRenderPanel*				m_pRenderPanel;
	cwxTimeLinePanel*			m_pTimeLinePanel;
	cwxPropertyPanel*			m_pPropertyPanel;
	cwxRegisteredResPanel*		m_pRegisteredResPanel;
	cwxCamEventListPanel*		m_pCamListPanel;
	cwxParticleListPanel*		m_pParticleListPanel;
	cwxFadeEventListPanel*		m_pFadeEventListPanel;
	cwxDOFEventListPanel*		m_pDOFEventListPanel;
	cwxPropEventListPanel*		m_pPropEventListPanel;
	cwxSoundEventPanel*			m_pSoundEventListPanel;
	cwxSubtitleEventPanel*		m_pSubtitleEventPanel;
	cwxHistoryPanel*			m_pHistoryPanel;

//#ifdef PRE_ADD_FILTEREVENT
	cwxColorFilterEventListPanel*m_pColorFilterEventPanel;
//#endif // PRE_ADD_FILTEREVENT

	wxMenu*						m_pWindowMenu;
	wxMenu*						m_pEditMenu;
	map<int, wxWindow*>			m_mapWindows;
	
	bool						m_bEditCameraOnPlay;
	bool						m_bShowSpotPos;
	bool						m_bRenderFog;
	bool						m_bShowLetterBox;
	bool						m_bWeapon; // bintitle.


private:
	void _SetupMenu( void );
	void _SetupToolBar( void );
	void _SetupAUI( void );
	void _SetupCmdListener( void );
	void _UpdateTitle( void );

public:
	CRTCutSceneMakerFrame( const wxString& Title, const wxPoint& Pos, const wxSize& Size );
	virtual ~CRTCutSceneMakerFrame(void);

	void ConnectMouseWheelEventToRenderPanel( wxWindow* pWheelOccurredWnd );

	void OnEraseBackGround( wxEraseEvent& EraseEvent );
	void OnMenuSave( wxCommandEvent& MenuEvent );
	void OnMenuSaveAs( wxCommandEvent& MenuEvent );
	void OnMenuLoad( wxCommandEvent& MenuEvent );
	void OnMenuExit( wxCommandEvent& MenuEvent );
	void OnCloseFrame( wxCloseEvent& CloseEvent );
	void OnActivate( wxActivateEvent& ActiveEvent );

	// Edit
	void OnMenuUndo( wxCommandEvent& MenuEvent );
	void OnMenuRedo( wxCommandEvent& MenuEvent );

	void OnMenuPlay( wxCommandEvent& MenuEvent );
	void OnMenuPause( wxCommandEvent& MenuEvent );
	void OnMenuStop( wxCommandEvent& MenuEvent );
	void OnMenuReset( wxCommandEvent& MenuEvent );
	
	void OnMenuBatchPropEdit( wxCommandEvent& MenuEvent );

	void OnUpdateWindowMenu( wxMenuEvent& MenuEvent );
	void OnMenuToggleWindow( wxCommandEvent& MenuEvent );
	
	void OnUpdateToggleCamMode( wxUpdateUIEvent& UpdateEvent );
	void OnMenuToggleCamMode( wxCommandEvent& MenuEvent );

	void OnUpdateToggleShowSpot( wxUpdateUIEvent& UpdateEvent );
	void OnMenuToggleShowSpot( wxCommandEvent& MenuEvent );

	void OnMenuToggleRenderFog( wxCommandEvent& MenuEvent );
	void OnUpdateToggleRenderFog( wxUpdateUIEvent& UpdateEvent );

	void OnMenuToggleLetterBox( wxCommandEvent& MenuEvent );
	void OnUpdateToggleLetterBox( wxUpdateUIEvent& UpdateEvent );

	void OnUpdateCanUndo( wxUpdateUIEvent& UpdateEvent );
	void OnUpdateCanRedo( wxUpdateUIEvent& UpdateEvent );

	virtual bool OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& FileNames );
	
	void OnMouseWheel( wxMouseEvent& WheelEvent );

	// bintitle.
	void OnMenuToggleWeapon( wxCommandEvent& UpdateEvent );
	void OnUpdateToggleWeapon( wxUpdateUIEvent& UpdateEvent );
	void OnKeyDown( wxKeyEvent & keyEvent );
	void OnKeyUp( wxKeyEvent & keyEvent );

	cwxRenderPanel* GetRenderPanel( void ) { return m_pRenderPanel; };

	void OnEdited( void );

	// from renderer
	void SetCursorPosText( const wxString& strPosText );

	DECLARE_EVENT_TABLE()
};
