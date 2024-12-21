#pragma once

#include "IPropertyDelegate.h"
#include "IDnCutSceneDataReader.h"


class CParticleEventProperty : public IPropertyDelegate
{
private:
	enum
	{
		PARTICLE_NAME,
		IMPORT_FILE_PATH,
		ID,
		POSITION,
		START_TIME,
		//SPEED,
		TIME_LENGTH,
		COUNT,
	};

	wxPGId						m_aPGID[ COUNT ];
	int							m_iMyEventInfoID;

public:
	CParticleEventProperty( wxWindow* pParent, int iId );
	virtual ~CParticleEventProperty(void);

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
