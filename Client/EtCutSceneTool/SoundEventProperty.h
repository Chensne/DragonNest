#pragma once

#include "IPropertyDelegate.h"
#include "IDnCutSceneDataReader.h"


class CSoundEventProperty : public IPropertyDelegate
{
private:
	enum
	{
		SOUND_NAME,
		IMPORT_FILE_PATH,
		ID,
		VOLUME,
		//POSITION,
		START_TIME,
		TIME_LENGTH,
		COUNT,
	};

	wxPGId						m_aPGID[ COUNT ];
	int							m_iMyEventInfoID;

public:
	CSoundEventProperty( wxWindow* pParent, int iId );
	virtual ~CSoundEventProperty(void);

private:
	void _UpdateProp( int iEventInfoID );
	void _GetPropertyValue( EventInfo* pEventInfo );

public:
	void Initialize( void );
	void OnShow( void );
	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	void CommandPerformed( ICommand* pCommand );
	void ClearTempData( void );
};
