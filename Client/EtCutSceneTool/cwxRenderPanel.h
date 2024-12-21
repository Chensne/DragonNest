#pragma once

#include "ICommandListener.h"

class IRenderBase;
class cwxTimeLineCtrl;
class cwxTimeLinePanel;


// 3D 화면이 렌더링 되는 패널입니다. 패널 상속받아서 사용.
class cwxRenderPanel : public wxPanel,
					   public ICommandListener
{
private:
	enum
	{
		POPUP_SELECTED_ACTOR_MOVE = 3000,
		POPUP_SELECTED_ACTOR_ROTATE,
		POPUP_SELECTED_PARTICLE_MOVE,
		POPUP_SELECTED_PARTICLE_ROTATE,
		POPUP_SELECTED_PARTICLE_UNSELECT,
	};

	enum
	{
		RP_EM_ACTOR_MOVE,
		RP_EM_ACTOR_ROTATE,
		RP_EM_PARTICLE_MOVE,
		RP_EM_PARTICLE_ROTATE,
		RP_EM_COUNT,
	};

	IRenderBase*			m_pRenderer;
	LOCAL_TIME				m_PrevFrameTime;
	//LOCAL_TIME				m_PlayStartTime;
	LOCAL_TIME				m_LocalTime;
	cwxTimeLineCtrl*		m_pActionTimeLineCtrl;

	// 타임 라인 패널
	cwxTimeLinePanel*		m_pTimeLinePanel;

	bool					m_bPauseScene;

	// 렌더 패널 자체의 편집 모드
	int						m_iNowMode;

	wxString				m_strRotateActorName;
	wxPoint					m_PrevMousePoint;
	bool					m_bActorMove;


private:
	void _PopupContextMenu( int iXPos, int iYPos );
	void _ProcessSelectedObjectEdit( wxMouseEvent& MouseEvent );


public:
	cwxRenderPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxRenderPanel(void);

	// 리사이즈 이벤트
	void OnSize( wxSizeEvent& SizeEvent );

	// 프레임 업데이트 및 렌더링을 위한 아이들 이벤트
	void OnIdle( wxIdleEvent& IdleEvent );

	// 활성화 비활성화 여부
	void OnActivate( wxActivateEvent& ActiveEvent );

	// 마우스 이벤트
	void OnMouseEvent( wxMouseEvent& MouseEvent );
	
	// bintitle.
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);

	// 컨텍스트 메뉴 이벤트
	void OnSelectedActorMove( wxCommandEvent& MenuEvent );
	void OnSelectedActorRotate( wxCommandEvent& MenuEvent );

	void OnSelectedParticleMove( wxCommandEvent& MenuEvent );
	void OnSelectedParticleRotate( wxCommandEvent& MenuEvent );
	void OnSelectedParticleUnSelect( wxCommandEvent& MenuEvent );

	void ToggleCamMode( bool bEditCameraOnPlay );
	void ToggleShowSpotPos( bool bShowSpotPos );
	void ToggleRenderFog( bool bRenderFog );
	void ToggleLetterBox( bool bToggleLetterBox );
	void ToggleWeapon( bool bToggleWeapon ); // bintitle.

	void SetTimeLinePanel( cwxTimeLinePanel* pTimeLinePanel );

	void StartPlay( void );
	void StopPlay( void );
	void PausePlay( void );
	void ResetPlay( void );

	void SeeThis( const wxChar* pActorName );

	// from ICommandListener
	void CommandPerformed( ICommand* pCommand );

	DECLARE_EVENT_TABLE()
};
