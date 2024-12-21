#pragma once
#include "ipropertydelegate.h"
#include "IDnCutSceneDataReader.h"

class CDofEventProperty : public IPropertyDelegate
{
private:
	enum
	{
		ENABLE,
		USE_NOW_DATA_AS_START,
		NEAR_START_FROM,
		NEAR_END_FROM,
		FAR_START_FROM,
		FAR_END_FROM,
		FOCUS_DIST_FROM,
		NEAR_BLUR_SIZE_FROM,
		FAR_BLUR_SIZE_FROM,
		NEAR_START_DEST,
		NEAR_END_DEST,
		FAR_START_DEST,
		FAR_END_DEST,
		FOCUS_DIST_DEST,
		NEAR_BLUR_SIZE_DEST,
		FAR_BLUR_SIZE_DEST,
		ID,
		START_TIME,
		LENGTH,
		COUNT,
	};

	wxPGId						m_aPGID[ COUNT ];
	int							m_iMyEventInfoID;

public:
	CDofEventProperty( wxWindow* pParent, int id );
	virtual ~CDofEventProperty(void);

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
