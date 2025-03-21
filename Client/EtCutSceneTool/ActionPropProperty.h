#pragma once
#include "IPropertyDelegate.h"
#include "IDnCutSceneDataReader.h"


class CActionPropProperty : public IPropertyDelegate
{
private:
	enum
	{
		PROP_LIST,
		PROP_ID,
		ACTION_LIST,
		SHOW,
		//ACTION_NAME,
		EVENT_ID,
		START_TIME,
		TIME_LENGTH,
		COUNT,
	};

	wxPGId m_aPGID[ COUNT ];
	int m_iMyEventInfoID;
	vector<int> m_vlPropID;


protected:
	void _UpdateProp( int iEventInfoID );
	void _GetPropertyValue( EventInfo* pEventInfo );

public:
	CActionPropProperty( wxWindow* pParent, int id );
	virtual ~CActionPropProperty(void);

	void Initialize( void );
	void OnShow( void );
	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	void ClearTempData( void );
	void CommandPerformed( ICommand* pCommand );
};
