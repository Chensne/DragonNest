#pragma once
#include "ipropertydelegate.h"
#include "IDnCutSceneDataReader.h"


class CImageEventProperty : public IPropertyDelegate
{
private:
	enum
	{
		FADE_KIND,
		START_TIME,
		IMAGE_FILE_NAME,
		XPOS,
		YPOS,
		LENGTH,
		COUNT,
	};

	wxPGId					m_aPGID[ COUNT ];
	int						m_iMyEventInfoID;

public:
	CImageEventProperty( wxWindow* pParent, int id );
	virtual ~CImageEventProperty(void);

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
