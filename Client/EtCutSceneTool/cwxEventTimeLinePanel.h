#pragma once


#include "cwxTimeLineCtrl.h"
#include "IDnCutSceneDataReader.h"

class ICommand;

class cwxEventTimeLinePanel : public wxPanel
{
private:
	enum
	{
		EVENT_TIMELINE_ID = 25000,

		POPUP_INSERT_CAM_EVENT,
		POPUP_INSERT_PARTICLE_EVENT,
		POPUP_INSERT_DOF,
		//POPUP_INSERT_SOUND_EVENT,
		POPUP_INSERT_FADE_EVENT,
		POPUP_INSERT_PROP_EVENT,
		POPUP_INSERT_SOUND_1_EVENT,
		POPUP_INSERT_SOUND_2_EVENT,
		POPUP_INSERT_SOUND_3_EVENT,
		POPUP_INSERT_SOUND_4_EVENT,
		POPUP_INSERT_SOUND_5_EVENT,
		POPUP_INSERT_SOUND_6_EVENT,
		POPUP_INSERT_SOUND_7_EVENT,
		POPUP_INSERT_SOUND_8_EVENT,
		POPUP_INSERT_SUBTITLE_EVENT,
		POPUP_REMOVE_EVENT,
		POPUP_SNAP_EVENT,
		POPUP_SELECT_PARTICLE,
		POPUP_UNSELECT_PARTICLE,
#ifdef PRE_ADD_FILTEREVENT
		POPUP_INSERT_COLORFILTER_EVENT
#endif // PRE_ADD_FILTEREVENT
	};

	wxBoxSizer*					m_pTopSizer;

	cwxTimeLineCtrl*			m_pTimeLine;

	wxArrayString				m_astrEventObjectNameByType;

	//DWORD						m_dwNowTimeOffset;		// 분, 시간 단위로 정해져있는 현재 시간 오프셋
	DWORD						m_dwNowTime;

	int							m_iOriEditMode;

	int							m_iXScrollPos;
	int							m_iYScrollPos;


private:
	void _PopupContextMenu( int iXPos, int iYPos );
	void _SnapEvent( int iEventType );
	EventInfo* _OnInsertEvent( const char* pEventName, int iEventType );

public:
	cwxEventTimeLinePanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxEventTimeLinePanel(void);

	bool Enable( bool bEnable = true );

	void OnTLObjectSelect( cwxTLObjectSelectEvent& TLSelectEvent );
	void OnTLObjectMove( cwxTLObjectMoveEvent& TLMoveEvent );
	void OnTLObjectResize( cwxTLObjectResizeEvent& TLResizeEvent );
	void OnTLAxisEvent( cwxTLAxisEvent& TLAxisEvent );
	void OnMouseEvent( wxMouseEvent& MouseEvent );
	void OnMenuInsertCamEvent( wxCommandEvent& MenuEvent );
	void OnMenuInsertParticleEvent( wxCommandEvent& MenuEvent );
	void OnMenuInsertDOFEvent( wxCommandEvent& MenuEvent );
	void OnMenuInsertSoundEvent( wxCommandEvent& MenuEvent );
	void OnMenuInsertFadeEvent( wxCommandEvent& MenuEvent );
	void OnMenuInsertPropEvent( wxCommandEvent& MenuEvent );
	void OnMenuInsertSubtitleEvent( wxCommandEvent& MenuEvent );
	void OnMenuRemoveEvent( wxCommandEvent& MenuEvent );
	void OnMenuSnapEvent( wxCommandEvent& MenuEvent );
	void OnMenuSelectParticleEvent( wxCommandEvent& MenuEvent );
	void OnMenuUnSelectParticleEvent( wxCommandEvent& MenuEvent );

//#ifdef PRE_ADD_FILTEREVENT
	void OnMenuInsertColorFilterEvent( wxCommandEvent& MenuEvent );
//#endif // PRE_ADD_FILTEREVENT	

	void OnMouseEventFromFrame( wxMouseEvent& MouseEvent );

	void SetTime( float fTime );

	void ReflectData( void );
	void ModifyObject( ICommand* pCommand );

	DWORD GetNowTime( void );

	cwxTimeLineCtrl* GetTimeLine( void ) { return m_pTimeLine; };

	void SelectObject( int iObjectID, DWORD dwStartTime );

	DECLARE_EVENT_TABLE()
};
