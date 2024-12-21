#pragma once
#include "ipropertydelegate.h"
#include "IDnCutSceneDataReader.h"


// �׼� �Ӽ�
class CActionProperty : public IPropertyDelegate
{
private:
	enum
	{
		ACTOR_NAME,
		ACTION_NAME,
		ID,
		USE_ANI_DISTANCE,
		FIT_Y_ANI_DISTANCE_TO_MAP,	
		ANIMATION_NAME_ENUM,
		SPEED,
		USE_START_ROTATION,
		ROTATION,
		USE_START_POSITION,
		POSITION,
		ACTION_TYPE,		// ���� ������� �ʴ´�.
		START_TIME,
		TIME_LENGTH,
		//ROTATION,
		//START_X_POS,
		//START_Y_POS,
		//START_Z_POS,
		//END_X_POS,
		//END_Y_POS,
		//END_Z_POS,
		COUNT,
	};

	wxPGId				m_aPGID[ COUNT ];

	ActionInfo*			m_pReflectedActionInfo;

private:
	void _RebuildProperty( void );
	void _GetPropertyValues( /*IN OUT*/ ActionInfo* pActionInfo );
	void _UpdateProp( int iActionInfoID );


public:
	CActionProperty( wxWindow* pParent, int id );
	virtual ~CActionProperty(void);

	void Initialize( void );
	void OnShow( void );
	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	void ClearTempData( void );
	void CommandPerformed( ICommand* pCommand );
};
