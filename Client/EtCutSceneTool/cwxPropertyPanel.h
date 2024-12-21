#pragma once

#include "ICommandListener.h"
#include <wx/propgrid/propgrid.h>
#include <wx/listctrl.h>
#include "EtMathWrapperD3D.h"


class IPropertyDelegate;


// 속성을 보여주는 프로퍼티 패널
class cwxPropertyPanel : public wxPanel,
						 public ICommandListener
{
public:
	enum
	{
		ACTOR_PROP_ID,
		ACTION_PROP_ID,
		KEY_PROP_ID,
		CAMERA_EVENT_PROP_ID,
		PARTICLE_EVENT_PROP_ID,
		SOUND_EVENT_PROP_ID,
		FADE_EVENT_PROP_ID,
		SCENE_PROP_ID,
		PROP_EVENT_ID,
		DOF_EVENT_PROP_ID,
		SUBTITLE_EVENT_PROP_ID,
		IMAGE_EVENT_PROP_ID,
		//MOVE_PROP_ID,
		//ROTATE_PROP_ID,
//#ifdef PRE_ADD_FILTEREVENT
		COLORFILTER_EVENT_PROP_ID,
//#endif // PRE_ADD_FILTEREVENT
		PROP_COUNT,
	};

private:
	wxBoxSizer*					m_pTopSizer;
	IPropertyDelegate*			m_pNowPropGrid;
	IPropertyDelegate*			m_apPropGrids[ PROP_COUNT ];

	int							m_iFocusingWndID;


private:


public:
	cwxPropertyPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxPropertyPanel(void);

	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	//void OnUpdateProperty( wxUpdateUIEvent& UpdateEvent );

	// RegisteredResPanel event
	void OnSelChangeRegResList( int iSelection );

	// TimeLinePanel event
	void OnTLActionSelect( void );

	// from ICommandListener
	void CommandPerformed( ICommand* pCommand );

	// Enable overloading
	bool Enable( bool bEnable = true );

	void ShowThisProp( int iPropToShow );

	// 툴에서 파일을 로딩 했을 때 각 property 에서 사용하고 있는 임시데이터 리셋.
	void ClearTempData( void );

	// 상태바에서 현재 위치 assign 버튼 눌린 경우.
	void AssignPosition( const EtVector3& vPos );

	DECLARE_EVENT_TABLE()
};
