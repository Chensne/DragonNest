#pragma once
#include "DnSlotButton.h"

// ����ó��ư�̾�����, ���߿� Ȯ���� ���� LifeSkillButton���� �ٲ����.
// MIInventoryItem::Gesture, MIInventoryItem::SecondarySkill �� Ÿ���� ǥ���� �� �ְ�, �������� ���Ǵ� ���´�.(�ΰ��� ��� LifeSkill�̶� ��Ī��.)
// Ŀ������Ʈ���̸��� ���� �״�� ����ó��ư�̸��� ����ϴ� ������ ��.
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

	int		m_nItemCondition;	// ��밡�� ����
	bool	m_bCash;
#ifdef PRE_MOD_SELECT_CHAR
	bool	m_bForceRender;		// ĳ���� ����â���� ���
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