#pragma once
#include "ipropertydelegate.h"
#include "IDnCutSceneDataReader.h"

class CSubtitleEventProperty : public IPropertyDelegate
{
private:
	enum
	{
		ID,
		UISTRING_ID,
		//COLOR,
		TIME_LENGTH,
		START_TIME,
		COUNT,
	};

	wxPGId				m_aPGID[ COUNT ];
	int					m_iMyEventInfoID;

private:
	void _UpdateProp( int iEventInfoID );
	void _GetPropertyValue( EventInfo* pEventInfo, wxPropertyGridEvent& PGEvent );

public:
	CSubtitleEventProperty( wxWindow* pParent, int id );
	virtual ~CSubtitleEventProperty(void);

	void Initialize( void );
	void OnShow( void );
	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	void ClearTempData( void );
	void CommandPerformed( ICommand* pCommand );
};