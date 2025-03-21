#pragma once

#include "IPropertyDelegate.h"
#include "IDnCutSceneDataReader.h"



class CCameraEventProperty : public IPropertyDelegate
{
private:
	enum
	{
		CAMERA_NAME,
		ID,
		IMPORT_FILE_PATH,
		TRACE_TYPE,
		ACTOR_LIST,
		START_TIME,
		SPEED,	
		TIME_LENGTH,		// 스피드에 따라 종속적
		START_OFFSET,
		ACTOR_CATEGORY,
		COUNT,
	};

	wxPGId					m_aPGID[ COUNT ];
	int						m_iMyEventInfoID;
	vector<wxString>		m_vlRenderingActors;		// 현재 프로퍼티 그리드에 랜더링 되고 있는 액터리스트
	
private:
	void _UpdateProp( int iEventInfoID );
	void _UpdateRegActor( void );
	void _GetPropertyValue( EventInfo* pEventInfo );


public:
	CCameraEventProperty( wxWindow* pParent, int id );
	virtual ~CCameraEventProperty(void);

	void Initialize( void );
	void OnShow( void );
	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	void ClearTempData( void );
	void CommandPerformed( ICommand* pCommand );
};
