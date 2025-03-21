#pragma once
#include "ipropertydelegate.h"
#include "IDnCutSceneDataReader.h"


class CFadeEventProperty : public IPropertyDelegate
{
private:
	enum
	{
		FADE_KIND,
		START_TIME,
		COLOR_RED,
		COLOR_GREEN,
		COLOR_BLUE,
		LENGTH,
		COUNT,
	};

	wxPGId					m_aPGID[ COUNT ];
	int						m_iMyEventInfoID;

public:
	CFadeEventProperty( wxWindow* pParent, int id );
	virtual ~CFadeEventProperty(void);

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
