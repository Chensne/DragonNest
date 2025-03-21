#pragma once
#include <wx/wx.h>
#include "EternityEngine.h"
#include "ipropertydelegate.h"



class CActorProperty : public IPropertyDelegate
{
private:
	enum
	{
		ACTOR_NAME,
		ACTOR_RES_NAME,			// 액터 변경을 위한 액터 리스트
		POSITION,
		//POS_X,
		//POS_Y,
		//POS_Z,
		FIT_YPOS_TO_MAP,
		ROTATION,
		INFLUENCE_LIGHTMAP,		// 라이트 맵 영향을 받는지 안받는지. (actor 는 기본적으로 받게 되어있음)
		
		SCALE_LOCK,				// 스케일 락이 걸려있으면 무조건 기본 스케일(1.0)으로 클라이언트에서 재생하도록 한다.

		// 보스몹으로 따로 셋팅할 것인지. 같은 액터들이 조무래기로 있는 경우 같은 액터 테이블 ID 를 사용하면서 겉으로 보기에 스케일 값이 다르므로 툴에서
		// 해당 스케일로 셋팅해주고 클라이언트에서도 실제로 돌릴 때 이 정보로 알 수 있도록 처리한다.
		// 스케일 락이 꺼져 있을 경우에만 선택 가능하다.
		SELECT_BOSS,
		COUNT,
	};

	wxPGId		m_aPGID[ COUNT ];

	// 업데이트 직전 데이터를 갖고 있는다.
	wxString		m_strPrevActorName;
	EtVector3		m_vPrevPos;
	float			m_fPrevRotation;
	bool			m_bPrevInfluenceLightmap;
	bool			m_bPrevScaleLock;

private:
	void _RebuildProperty( void );
	void _UpdateProp( void );


public:
	CActorProperty( wxWindow* pParent, int id );
	virtual ~CActorProperty(void);

	// from IPropertyDelegate
	void Initialize( void );
	void OnShow( void );
	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	void ClearTempData( void );
	void CommandPerformed( ICommand* pCommand );
	//
};
