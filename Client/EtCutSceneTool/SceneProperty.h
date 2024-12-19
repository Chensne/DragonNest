#pragma once
#include "ipropertydelegate.h"
#include "IDnCutSceneDataReader.h"


class CSceneProperty : public IPropertyDelegate
{
private:
	enum
	{
		SCENE_START_FADE_OUT_TIME,
		SCENE_END_FADE_IN_TIME,
		USE_SCENE_END_CLIPPING,
		SCENE_END_CLIP_TIME,
		USE_SIGNAL_SOUND,			// 액션에 있는 시그널 사운드를 사용할 것인지
		MAIN_CHARACTER,				// 컷신의 주인공 역할을 하는 메인 캐릭터
		FOGFAR_DELTA,				// 조정할 포그 far 가 있다면 입력.
		ACADEMIC_NOT_SUPPORTED,		// 아카데믹 캐릭터 지원하지 않음. (워리어, 아쳐, 소서리스, 클러릭 기본 캐를 제외한 신캐들 생길때마다 추가해야할듯)
		COUNT,
	};

	wxPGId						m_aPGID[ COUNT ];

private:
	void _UpdateProp( void );
	void _GetPropertyValue( SceneInfo* pInfo );

public:
	CSceneProperty( wxWindow* pParent, int id );
	virtual ~CSceneProperty(void);

	void Initialize( void );
	void OnShow( void );
	void OnPropertyChanged( wxPropertyGridEvent& PGEvent );
	void ClearTempData( void );
	void CommandPerformed( ICommand* pCommand );
};
