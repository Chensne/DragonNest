#pragma once
#include "IPropertyDelegate.h"
#include "IDnCutSceneDataReader.h"


class CKeyProperty : public IPropertyDelegate
{
private:
	enum
	{
		ACTOR_NAME,
		KEY_NAME,
		ID,
		KEY_TYPE,
		ROTATION,
		ROTATION_FRONT,
		FIT_YPOS_TO_MAP,
		NUM_MOVEKEY,
		MOVEKEY_LIST,
		MOVEKEY_1,
		MOVEKEY_2,
		MOVEKEY_3,
		MOVEKEY_4,
		MOVEKEY_5,
		MOVEKEY_6,
		MOVEKEY_7,
		MOVEKEY_8,
		MOVEKEY_9,
		MOVEKEY_10,
		MOVEKEY_11,
		MOVEKEY_12,
		MOVEKEY_13,
		MOVEKEY_14,
		MOVEKEY_15,
		MOVEKEY_16,
		MOVEKEY_17,
		MOVEKEY_18,
		MOVEKEY_19,
		MOVEKEY_20,
		MOVEKEY_21,
		MOVEKEY_22,
		MOVEKEY_23,
		MOVEKEY_24,
		MOVEKEY_25,
		MOVEKEY_26,
		MOVEKEY_27,
		MOVEKEY_28,
		MOVEKEY_29,
		MOVEKEY_30,
		MOVEKEY_LENGTH_1,
		MOVEKEY_LENGTH_2,
		MOVEKEY_LENGTH_3,
		MOVEKEY_LENGTH_4,
		MOVEKEY_LENGTH_5,
		MOVEKEY_LENGTH_6,
		MOVEKEY_LENGTH_7,
		MOVEKEY_LENGTH_8,
		MOVEKEY_LENGTH_9,
		MOVEKEY_LENGTH_10,
		MOVEKEY_LENGTH_11,
		MOVEKEY_LENGTH_12,
		MOVEKEY_LENGTH_13,
		MOVEKEY_LENGTH_14,
		MOVEKEY_LENGTH_15,
		MOVEKEY_LENGTH_16,
		MOVEKEY_LENGTH_17,
		MOVEKEY_LENGTH_18,
		MOVEKEY_LENGTH_19,
		MOVEKEY_LENGTH_20,
		MOVEKEY_LENGTH_21,
		MOVEKEY_LENGTH_22,
		MOVEKEY_LENGTH_23,
		MOVEKEY_LENGTH_24,
		MOVEKEY_LENGTH_25,
		MOVEKEY_LENGTH_26,
		MOVEKEY_LENGTH_27,
		MOVEKEY_LENGTH_28,
		MOVEKEY_LENGTH_29,
		MOVEKEY_LENGTH_30,
		START_POS,
		DEST_POS,
		//START_POS_X,
		//START_POS_Y,
		//START_POS_Z,
		//DEST_POS_X,
		//DEST_POS_Y,
		//DEST_POS_Z,
		//ROTATION,
		//SCALE,
		START_TIME,
		TIME_LENGTH,
		COUNT,
	};

	wxPGId				m_aPGID[ COUNT ];
	KeyInfo*			m_pReflectedKeyInfo;
	//bool				m_bChangeToMultiMove;

private:
	void _RebuildProperty( void );
	void _GetPropertyValues( KeyInfo* pKeyInfo );
	void _UpdateProp( int iObjectID );
	void _UpdateMoveType( int iKeyType );
	void _GetVectorProperty( int iPropID, /*IN OUT*/ EtVector3& vVector );

public:
	CKeyProperty( wxWindow* pParent, int id );
	virtual ~CKeyProperty(void);

	void Initialize( void );
	void OnShow( void );
	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	void ClearTempData( void );
	void CommandPerformed( ICommand* pCommand );
};
