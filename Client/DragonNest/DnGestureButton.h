#pragma once
#include "DnSlotButton.h"

// 제스처버튼이었으나, 나중에 확장을 위해 LifeSkillButton으로 바뀌었다.
// MIInventoryItem::Gesture, MIInventoryItem::SecondarySkill 두 타입을 표현할 수 있고, 공용으로 사용되는 형태다.(두개를 묶어서 LifeSkill이라 통칭함.)
// 커스텀컨트롤이름은 예전 그대로 제스처버튼이름을 사용하니 주의할 것.
class CDnLifeSkillButton : public CDnSlotButton
{
public:
	CDnLifeSkillButton( CEtUIDialog *pParent );
	virtual ~CDnLifeSkillButton(void);

protected:

	EtTextureHandle m_hShortCutIcon;
	SUICoord m_ShortCutUV;
	SUICoord m_ShortCutCoord;

	EtTextureHandle m_hGestureIcon;
	EtTextureHandle m_hGestureIconList[GESTURE_TEXTURE_COUNT];
	EtTextureHandle m_hSecondarySkillIconList[SECONDARYSKILL_TEXTURE_COUNT];

	int		m_nItemCondition;	// 사용가능 유무
	bool	m_bCash;
#ifdef PRE_MOD_SELECT_CHAR
	bool	m_bForceRender;		// 캐릭터 생성창에서 사용
#endif // PRE_MOD_SELECT_CHAR

public:
	void SetGestureInfo( int nGestureID );
	void SetSecondarySkillInfo( int nSecondarySkillID );
	bool IsUsable();
#ifdef PRE_MOD_SELECT_CHAR
	void SetGestureIcon( int nIconID );
#endif // PRE_MOD_SELECT_CHAR

	virtual void SetHotKey( int nKey );

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual void RenderDrag( float fElapsedTime );
	virtual void Process( float fElapsedTime );
};