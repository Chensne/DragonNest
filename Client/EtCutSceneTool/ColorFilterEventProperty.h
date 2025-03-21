#pragma once
#include "ipropertydelegate.h"
#include "IDnCutSceneDataReader.h"


class CColorFilterEventProperty : public IPropertyDelegate
{
private:
	enum
	{
		START_TIME,
		TIME_LENGTH,
		MONOCHROME,
		COLOR_R,
		COLOR_G,
		COLOR_B,
		COLOR_VOL,
		COUNT,
	};

	wxPGId					m_aPGID[ COUNT ];
	int						m_iMyEventInfoID;

public:
	CColorFilterEventProperty( wxWindow* pParent, int id );
	virtual ~CColorFilterEventProperty(void);

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
